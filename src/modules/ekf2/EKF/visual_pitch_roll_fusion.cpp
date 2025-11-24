
#include "ekf.h"

#include <mathlib/mathlib.h>
#include <float.h>
#include "python/ekf_derivation/generated/compute_pitch_roll_innov_innov_var_and_Hp.h"
#include "python/ekf_derivation/generated/compute_roll_innov_innov_var_and_h.h"
#include "utils.hpp"


void Ekf::fuseVisualPitchRoll(const visualPitchRoll & pitch_roll_, estimator_aid_source2d_s & aid_src)
{
	if (pitch_roll_.confidence < _params.visual_pitch_roll_min_confidence)
	{
		return;
	}

	aid_src.timestamp_sample = pitch_roll_.time_us;

	const float noise_f = (pitch_roll_.confidence - _params.visual_pitch_roll_min_confidence) / (1.f - _params.visual_pitch_roll_min_confidence);

	const float NOISE_STD = math::lerp(_params.visual_pitch_roll_noise_max, _params.visual_pitch_roll_noise_min, noise_f) * M_DEG_TO_RAD_F;
	const float R = sq(NOISE_STD);

	const char* numerical_error_covariance_reset_string = "numerical error - covariance reset";
	Vector2f pitch_roll(pitch_roll_.pitch, pitch_roll_.roll);
	Vector2f pitch_roll_innov;
	Vector2f innov_var;

	// Observation jacobian and Kalman gain vectors

	VectorState H;
	sym::ComputePitchRollInnovInnovVarAndHp(_state.vector(), P, pitch_roll, R, FLT_EPSILON, &pitch_roll_innov, &innov_var, &H);

	innov_var.copyTo(aid_src.innovation_variance);

	if (aid_src.innovation_variance[0] < R) {
		// the innovation variance contribution from the state covariances is negative which means the covariance matrix is badly conditioned
		//	_fault_status.flags.bad_visual_pitch = true;

		// we need to re-initialise covariances and abort this fusion step
		resetQuatCov();
		ECL_ERR("visual pitch %s", numerical_error_covariance_reset_string);
		return;
	}
	//_fault_status.flags.bad_visual_pitch = false;

	if (aid_src.innovation_variance[1] < R) {
		// the innovation variance contribution from the state covariances is negtive which means the covariance matrix is badly conditioned
		// _fault_status.flags.bad_visual_roll = true;

		// we need to re-initialise covariances and abort this fusion step
		resetQuatCov();
		ECL_ERR("visual roll %s", numerical_error_covariance_reset_string);
		return;
	}

	//_fault_status.flags.bad_visual_roll = false;

	for (int i = 0; i < 2; i++) {
		aid_src.observation[i] = pitch_roll(i);
		aid_src.observation_variance[i] = R;
		aid_src.innovation[i] = pitch_roll_innov(i);
	}

//	aid_src.fusion_enabled = _params.visual_pitch_roll_control;

	const float innov_gate = math::max(_params.visual_pitch_roll_innov_gate, 1.f);
	setEstimatorAidStatusTestRatio(aid_src, innov_gate);

	// Perform an innovation consistency check and report the result
	//_innov_check_fail_status.flags.reject_visual_pitch = (aid_src.test_ratio[0] > 1.f);
	//_innov_check_fail_status.flags.reject_visual_roll = (aid_src.test_ratio[1] > 1.f);

	// if any axis fails, abort the mag fusion
	if (aid_src.innovation_rejected) {
		return;
	}

	bool fused[2] {false, false};

	// update the states and covariance using sequential fusion of the visual pitch and roll
	for (uint8_t index = 0; index < 2; index++) {
		// Calculate Kalman gains and observation jacobians
		if (index == 0)
		{
			// everything was already computed above
		} else if (index == 1) {
			// recalculate innovation variance because state covariances have changed due to previous fusion (linearise using the same initial state for all axes)
			sym::ComputeRollInnovInnovVarAndH(_state.vector(), P, pitch_roll_.roll, R, FLT_EPSILON, &aid_src.innovation[index], &aid_src.innovation_variance[index], &H);

			// recalculate innovation using the updated state
			{
				const Quatf & q = _state.quat_nominal;
				float sinRcosP = 2.0f * (q(0) * q(1) + q(2) * q(3));
				float cosRcosP = 1.0f - 2.0f * (q(1) * q(1) + q(2) * q(2));
				float roll_pred = atan2f(sinRcosP, cosRcosP);
				aid_src.innovation[index] = roll_pred - pitch_roll_.roll;
			}

			if (aid_src.innovation_variance[index] < R) {
				// the innovation variance contribution from the state covariances is negative which means the covariance matrix is badly conditioned
				//_fault_status.flags.bad_visual_roll = true;

				// we need to re-initialise covariances and abort this fusion step
				resetQuatCov();
				ECL_ERR("visual roll %s", numerical_error_covariance_reset_string);
				return;
			}
		}

		VectorState Kfusion = P * H / aid_src.innovation_variance[index];

	//	if (!update_all_states) {
	//		for (unsigned row = 0; row <= 15; row++) {
	//			Kfusion(row) = 0.f;
	//		}

	//		for (unsigned row = 22; row <= 23; row++) {
	//			Kfusion(row) = 0.f;
	//		}
	//	}

		if (measurementUpdate(Kfusion, H, aid_src.innovation_variance[index], aid_src.innovation[index])) {
			fused[index] = true;
	//		limitDeclination();

		} else {
			fused[index] = false;
		}
	}

	//_fault_status.flags.bad_visual_pitch = !fused[0];
	//_fault_status.flags.bad_visual_roll = !fused[1];

	if (fused[0] && fused[1]) {
		aid_src.fused = true;
		aid_src.time_last_fuse = _time_delayed_us;

//		printf("visualPitchRoll fused, confidence = %.2f, innov_var[0] = %.4f, innov_var[1] = %.4f\n", (double)pitch_roll_.confidence, (double)aid_src.innovation_variance[0], (double)aid_src.innovation_variance[1]);
		return;
	}

	aid_src.fused = false;
	return;
}

