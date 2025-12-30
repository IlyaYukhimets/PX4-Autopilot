/****************************************************************************
 *
 *   Copyright (c) 2018 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "vtx_control.h"

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>

#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/input_rc.h>


int VTX_CONTROL::print_status()
{
	PX4_INFO("Running");
	// TODO: print additional runtime information about the state of the module

	return 0;
}

int VTX_CONTROL::custom_command(int argc, char *argv[])
{
	/*
	if (!is_running()) {
		print_usage("not running");
		return 1;
	}

	// additional custom commands can be handled like this:
	if (!strcmp(argv[0], "do-something")) {
		get_instance()->do_something();
		return 0;
	}
	 */

	return print_usage("unknown command");
}


int VTX_CONTROL::task_spawn(int argc, char *argv[])
{
	_task_id = px4_task_spawn_cmd("VTX_CONTROL",
				      SCHED_DEFAULT,
				      SCHED_PRIORITY_DEFAULT,
				      1536,
				      (px4_main_t)&run_trampoline,
				      (char *const *)argv);

	if (_task_id < 0) {
		_task_id = -1;
		return -errno;
	}

	return 0;
}

VTX_CONTROL *VTX_CONTROL::instantiate(int argc, char *argv[])
{
	//int example_param = 0;
	//bool example_flag = false;
	bool error_flag = false;

	int myoptind = 1;
	int ch;
	const char *myoptarg = nullptr;

	// parse CLI arguments
	while ((ch = px4_getopt(argc, argv, "p:f", &myoptind, &myoptarg)) != EOF) {
		switch (ch) {
		case 'p':
			//example_param = (int)strtol(myoptarg, nullptr, 10);
			break;

		case 'f':
			//example_flag = true;
			break;

		case '?':
			error_flag = true;
			break;

		default:
			PX4_WARN("unrecognized flag");
			error_flag = true;
			break;
		}
	}

	if (error_flag) {
		return nullptr;
	}

	VTX_CONTROL *instance = new VTX_CONTROL();

	if (instance == nullptr) {
		PX4_ERR("alloc failed");
	}

	return instance;
}

VTX_CONTROL::VTX_CONTROL(/*int example_param, bool example_flag*/) : ModuleParams(nullptr)
{
}

void VTX_CONTROL::run()
{
	uint16_t camera_enable = 0;
	uint32_t en_param_for_tests = 0;
	uint32_t min_thr_param = 0;
	uint32_t max_thr_param = 0;
	uint32_t rc_ch_param = 0;

	// Example: run the loop synchronized to the sensor_combined topic publication
	int rc_sub = orb_subscribe(ORB_ID(input_rc));

	px4_pollfd_struct_t fds[1];
	fds[0].fd = rc_sub;
	fds[0].events = POLLIN;

	// initialize parameters
	parameters_update(true);

	//PX4_ERR("enter to run()");

	en_param_for_tests = _vtx_test_param.get();
	min_thr_param = _vtx_min_thr_param.get();
	max_thr_param = _vtx_max_thr_param.get();
	rc_ch_param = _vtx_rc_ch_param.get();

	if (en_param_for_tests == 123)
	{
		if(!px4_arch_gpioread(GPIO_CAMERA_EN_PIN))
		{
			px4_arch_gpiowrite(GPIO_CAMERA_EN_PIN, true);
		}
	}
	else if (en_param_for_tests == 0)
	{
		while (!should_exit())
		{
			//PX4_ERR("enter to while()");
			// wait for up to 100ms for data
			int pret = px4_poll(fds, (sizeof(fds) / sizeof(fds[0])), 100);

			// if (en_param_for_tests == 0)
			// {
				if (pret < 0)
				{
					// this is undesirable but not much we can do
					PX4_ERR("poll error %d, %d", pret, errno);
					px4_usleep(50000);
					continue;
				}
				else if ((fds[0].revents & POLLIN) /*&& (en_param_for_tests == 0)*/)
				{
					struct input_rc_s rc_input;
					orb_copy(ORB_ID(input_rc), rc_sub, &rc_input);

					camera_enable = rc_input.values[rc_ch_param - 1]; // in the code radio channel number starts from 0, and in the QGC that param starts from 1

					//PX4_ERR("RC10 %d", rc_input.values[9]);

					if ((camera_enable > min_thr_param) && ((camera_enable < max_thr_param)))
					{
						if(!px4_arch_gpioread(GPIO_CAMERA_EN_PIN))
						{
							px4_arch_gpiowrite(GPIO_CAMERA_EN_PIN, true);
						}
						//PX4_ERR("true");
					}
					else if(px4_arch_gpioread(GPIO_CAMERA_EN_PIN))
					{
						px4_arch_gpiowrite(GPIO_CAMERA_EN_PIN, false);
						//PX4_ERR("false");
					}
				}
	//		}

			parameters_update();
		}
	}

	orb_unsubscribe(rc_sub);
}

void VTX_CONTROL::parameters_update(bool force)
{
	// check for parameter updates
	if (_parameter_update_sub.updated() || force) {
		// clear update
		parameter_update_s update;
		_parameter_update_sub.copy(&update);

		// update parameters from storage
		updateParams();
	}
}

int VTX_CONTROL::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Section that describes the provided module functionality.

This is a template for a module running as a task in the background with start/stop/status functionality.

### Implementation
Section describing the high-level implementation of this module.

### Examples
CLI usage example:
$ module start -f -p 42

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("module", "template");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAM_FLAG('f', "Optional example flag", true);
	PRINT_MODULE_USAGE_PARAM_INT('p', 0, 0, 1000, "Optional example parameter", true);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}

int vtx_control_main(int argc, char *argv[])
{
	return VTX_CONTROL::main(argc, argv);
}
