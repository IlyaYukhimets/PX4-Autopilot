
#include "ekf.h"

void Ekf::controlVisualPitchRollFusion(const imuSample &imu_delayed)
{
	if (!_visual_pitch_roll_buffer || !_params.visual_pitch_roll_control)
	{
		return;
	}
	visualPitchRoll visual_sample;
	if (!_visual_pitch_roll_buffer->pop_first_older_than(_time_delayed_us, &visual_sample))
	{
		return;
	}
	if (_control_status.flags.gps || !_control_status.flags.in_air)
		return;

	fuseVisualPitchRoll(visual_sample, _aid_src_visual_pitch_roll);
}
