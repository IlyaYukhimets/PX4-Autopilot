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

#pragma once

/**
 * @file atxxxx.h
 * @author Daniele Pettenuzzo
 *
 * Driver for the ATXXXX chip on the omnibus fcu connected via SPI.
 */
#include <drivers/device/spi.h>
#include <drivers/drv_hrt.h>
#include <parameters/param.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/i2c_spi_buses.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_status.h>

#define BOTTLE_OSD

#ifdef BOTTLE_OSD

#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/mount_orientation.h>
#include <uORB/topics/airspeed.h>
#include <uORB/topics/input_rc.h>
#endif

#define OSD_SPI_BUS_SPEED (2000000L) /*  2 MHz  */

#define DIR_READ(a) ((a) | (1 << 7))
#define DIR_WRITE(a) ((a) & 0x7f)

#define OSD_CHARS_PER_ROW	30
#define OSD_NUM_ROWS_PAL	16
#define OSD_NUM_ROWS_NTSC	13
#define OSD_ZERO_BYTE 		0x00
#define OSD_PAL_TX_MODE 	0x40
#define OSD_NTSC_TX_MODE	0x00



extern "C" __EXPORT int atxxxx_main(int argc, char *argv[]);

class OSDatxxxx : public device::SPI, public ModuleParams, public I2CSPIDriver<OSDatxxxx>
{
public:
	OSDatxxxx(const I2CSPIDriverConfig &config);
	virtual ~OSDatxxxx() = default;

	static void print_usage();

	int init() override;

	void RunImpl();

protected:
	int probe() override;

private:
	int start();

	int reset();

	int init_osd();

	int readRegister(unsigned reg, uint8_t *data, unsigned count);
	int writeRegister(unsigned reg, uint8_t data);

	int add_character_to_screen(char c, uint8_t pos_x, uint8_t pos_y);
	void add_string_to_screen_centered(const char *str, uint8_t pos_y, int max_length);
	void clear_line(uint8_t pos_x, uint8_t pos_y, int length);

	int add_battery_info(uint8_t pos_x, uint8_t pos_y);
	int add_voltage_info(uint8_t pos_x, uint8_t pos_y);
	int add_charge_info(uint8_t pos_x, uint8_t pos_y);

	int add_altitude(uint8_t pos_x, uint8_t pos_y);
	int add_flighttime(float flight_time, uint8_t pos_x, uint8_t pos_y);

	static const char *get_flight_mode(uint8_t nav_state);

	int enable_screen();
	int disable_screen();

	int update_topics();
	int update_screen();

	uORB::Subscription _battery_sub{ORB_ID(battery_status)};
	uORB::Subscription _local_position_sub{ORB_ID(vehicle_local_position)};
	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

#ifdef BOTTLE_OSD

	uORB::Subscription _vehicle_global_position_sub{ORB_ID(vehicle_global_position)};
	uORB::Subscription _sensor_gps_sub{ORB_ID(sensor_gps)};
	uORB::Subscription _mount_orientation_sub{ORB_ID(mount_orientation)};
	uORB::Subscription _airspeed_sub{ORB_ID(airspeed)};
	uORB::Subscription _rc_sub{ORB_ID(input_rc)};

#endif

	// LOCAL & GLOBAL
#ifdef BOTTLE_OSD
	float _global_position_latitude{0.f};
	float _global_position_lontitude{0.f};
	uint8_t _gps_fix_type{0};
	uint8_t _gps_satellites_used{0};
	float _local_position_heading{0.f};
	float _airspeed{0.f};
	uint16_t _antenna_used{0};
#endif
	// battery
	float _battery_voltage_filtered_v{0.f};
#ifdef BOTTLE_OSD
	float _battery_current_filtered_a{0.f};
#endif
	float _battery_discharge_mah{0.f};
	bool _battery_valid{false};

	// altitude
	float _local_position_z{0.f};
	bool _local_position_valid{false};

	// flight time
	uint8_t _arming_state{0};
	uint64_t _arming_timestamp{0};

	// flight mode
	uint8_t _nav_state{0};

#ifdef BOTTLE_OSD
	uint8_t ascii_to_at7456_symbol_convert(char *p_array);
	void at7456_test_row(void);
	void add_flight_mode(uint8_t pos_x, uint8_t pos_y);
	int add_airspeed(uint8_t pos_x, uint8_t pos_y);
	int add_heading(uint8_t pos_x, uint8_t pos_y);
	int add_gps(uint8_t pos_x, uint8_t pos_y);
	int add_ant(uint8_t pos_x, uint8_t pos_y);
	void fill_buf_with_spaces(char *p_buf, uint8_t buf_size, uint8_t null_pos);
#endif

	DEFINE_PARAMETERS(
		(ParamInt<px4::params::OSD_ATXXXX_CFG>) _param_osd_atxxxx_cfg,
		(ParamInt<px4::params::OSD_HEADING_Y>) _param_osd_heading_y,
		(ParamInt<px4::params::OSD_HEADING_X>) _param_osd_heading_x,
		(ParamInt<px4::params::OSD_VOLTAGE_Y>) _param_osd_voltage_y,
		(ParamInt<px4::params::OSD_VOLTAGE_X>) _param_osd_voltage_x,
		(ParamInt<px4::params::OSD_CHARGE_Y>) _param_osd_charge_y,
		(ParamInt<px4::params::OSD_CHARGE_X>) _param_osd_charge_x,
		(ParamInt<px4::params::OSD_MODE_Y>) _param_osd_mode_y,
		(ParamInt<px4::params::OSD_MODE_X>) _param_osd_mode_x,
		(ParamInt<px4::params::OSD_ALT_Y>) _param_osd_alt_y,
		(ParamInt<px4::params::OSD_ALT_X>) _param_osd_alt_x,
		(ParamInt<px4::params::OSD_FTIME_Y>) _param_osd_ftime_y,
		(ParamInt<px4::params::OSD_FTIME_X>) _param_osd_ftime_x,
		(ParamInt<px4::params::OSD_ANT_Y>) _param_osd_ant_y,
		(ParamInt<px4::params::OSD_ANT_X>) _param_osd_ant_x,
		(ParamInt<px4::params::OSD_ANT_CH>) _param_osd_ant_ch,
		(ParamInt<px4::params::OSD_TEST_ALT>) _param_osd_test_alt,
		(ParamInt<px4::params::OSD_SPEED_G_X>) _param_osd_speed_g_x,
		(ParamInt<px4::params::OSD_SPEED_G_Y>) _param_osd_speed_g_y
	)
};
