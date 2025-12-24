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
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSSs
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file atxxxx.cpp
 * @author Daniele Pettenuzzo
 * @author Beat Küng <beat-kueng@gmx.net>
 *
 * Driver for the ATXXXX chip (e.g. MAX7456) on the omnibus f4 fcu connected via SPI.
 */

#include "atxxxx.h"
#include "symbols.h"
#include "symbols_default_at7456.h"

#ifdef BOTTLE_OSD

#define BOTTLE_BUF_SIZE (uint8_t)(13 + 1)

#define BOTTLE_ROW_BUF_SIZE (uint8_t)(13 + 1)
#define BOTTLE_GPS_BUF_SIZE (uint8_t)(13 + 1)
#define BOTTLE_COURSE_BUF_SIZE (uint8_t)(13 + 1)
#define BOTTLE_AIRSPEED_BUF_SIZE (uint8_t)(13 + 1)
#define BOTTLE_ANT_BUF_SIZE (uint8_t)(13 + 1)

#define BOTTLE_BAT_V_INFO_ROW 		(14)
#define BOTTLE_ALTITUDE_INFO_ROW 	(14)
#define BOTTLE_FLIGHTMODE_INFO_ROW 	(1)
#define BOTTLE_HEADING_INFO_ROW 	(15)
#define BOTTLE_FLIGHTTIME_INFO_ROW 	(2)
#define BOTTLE_ANT_INFO_ROW 		(10)

#define BOTTLE_GPS_INFO_ROW		(OSD_NUM_ROWS_PAL - 3)
#define BOTTLE_AIRSPEED_INFO_ROW 	(OSD_NUM_ROWS_PAL - 14)
#define GPS_LAT_POSITION		(OSD_NUM_ROWS_PAL - 2)
#define GPS_LON_POSITION		(OSD_NUM_ROWS_PAL - 1)
#define GPS_SAT_POSITION		(OSD_NUM_ROWS_PAL - 15)

#endif

using namespace time_literals;

static constexpr uint32_t OSD_UPDATE_RATE{50_ms};	// 20 Hz

OSDatxxxx::OSDatxxxx(const I2CSPIDriverConfig &config) :
	SPI(config),
	ModuleParams(nullptr),
	I2CSPIDriver(config)
{
}

int
OSDatxxxx::init()
{
	/* do SPI init (and probe) first */
	int ret = SPI::init();

	if (ret != PX4_OK) {
		return ret;
	}

	ret = reset();

	if (ret != PX4_OK) {
		return ret;
	}

	ret = init_osd();

	if (ret != PX4_OK) {
		return ret;
	}

	// clear the screen
	int num_rows = (_param_osd_atxxxx_cfg.get() == 1 ? OSD_NUM_ROWS_NTSC : OSD_NUM_ROWS_PAL);

	for (int y = 0; y < num_rows; y++)
	{
		for (int x = 0; x < OSD_CHARS_PER_ROW; x++)
		{
			add_character_to_screen(OSD_SYMBOL_SPACE, x, y);
		}
	}

	if (ret == PX4_OK) {
		start();
	}


	return ret;
}

int
OSDatxxxx::start()
{
	ScheduleOnInterval(OSD_UPDATE_RATE, 10000);

	return PX4_OK;
}

int
OSDatxxxx::probe()
{
	uint8_t data = 0;
	int ret = PX4_OK;

	ret |= writeRegister(0x00, 0x01); //disable video output
	ret |= readRegister(0x00, &data, 1);

	if (data != 1 || ret != PX4_OK) {
		PX4_ERR("probe failed (%i %i)", ret, data);
	}

	return ret;
}

int
OSDatxxxx::init_osd()
{
	int ret = PX4_OK;
	uint8_t data = OSD_ZERO_BYTE;
	uint8_t i = 0;

	if (_param_osd_atxxxx_cfg.get() == 2) {
		data |= OSD_PAL_TX_MODE;
	}
	else
	{
		data |= OSD_NTSC_TX_MODE;
	}
	//data |= 0x04;


	ret |= writeRegister(0x00, data);
	ret |= writeRegister(0x01, 0x4C);
	ret |= writeRegister(0x04, 0x40); // 8 bit interface
	ret |= writeRegister(0x02, 0x2C); // HOR offset
	ret |= writeRegister(0x03, 0x0A); // VER offset
	//ret |= writeRegister(0x0C, 0x2B); // OSDM
	//ret |= writeRegister(0x6C, 0x00); // OSDBL

	for (i = 0; i < 16; i++)
	{
		ret |= writeRegister(0x10 + i, 0x0F); // RB0..15
	}

	enable_screen();

	return ret;
}

int
OSDatxxxx::readRegister(unsigned reg, uint8_t *data, unsigned count)
{
	uint8_t cmd[5] {}; // read up to 4 bytes

	cmd[0] = DIR_READ(reg);

	int ret = transfer(&cmd[0], &cmd[0], count + 1);

	if (ret != PX4_OK) {
		DEVICE_LOG("spi::transfer returned %d", ret);
		return ret;
	}

	memcpy(&data[0], &cmd[1], count);

	return ret;
}

int
OSDatxxxx::writeRegister(unsigned reg, uint8_t data)
{
	uint8_t cmd[2] {}; // write 1 byte

	cmd[0] = DIR_WRITE(reg);
	cmd[1] = data;

	int ret = transfer(&cmd[0], nullptr, 2);

	if (OK != ret) {
		DEVICE_LOG("spi::transfer returned %d", ret);
		return ret;
	}

	return ret;
}

int
OSDatxxxx::add_character_to_screen(char c, uint8_t pos_x, uint8_t pos_y)
{
	uint16_t position = (OSD_CHARS_PER_ROW * pos_y) + pos_x;
	uint8_t position_lsb = 0;
	int ret = PX4_ERROR;

	if (position > 0xFF) {

#ifndef BOTTLE_OSD
		position_lsb = static_cast<uint8_t>(position) - 0xFF;
#else
		position_lsb = static_cast<uint8_t>(position & 0xFF)/* - 0xFF*/;
#endif
		ret = writeRegister(0x05, 0x01); //DMAH

	} else {
		position_lsb = static_cast<uint8_t>(position);
		ret = writeRegister(0x05, 0x00); //DMAH
	}

	if (ret != 0) {
		return ret;
	}

	ret = writeRegister(0x06, position_lsb); //DMAL

	if (ret != 0) {
		return ret;
	}

	ret = writeRegister(0x07, c);

	return ret;
}

void
OSDatxxxx::add_string_to_screen_centered(const char *str, uint8_t pos_y, int max_length)
{
	int len = strlen(str);

	if (len > max_length) {
		len = max_length;
	}

	int pos = (OSD_CHARS_PER_ROW - max_length) / 2;
	int before = (max_length - len) / 2;

	for (int i = 0; i < before; ++i) {
		add_character_to_screen(' ', pos++, pos_y);
	}

	for (int i = 0; i < len; ++i) {
		add_character_to_screen(str[i], pos++, pos_y);
	}

	while (pos < (OSD_CHARS_PER_ROW + max_length) / 2) {
		add_character_to_screen(' ', pos++, pos_y);
	}
}

void
OSDatxxxx::clear_line(uint8_t pos_x, uint8_t pos_y, int length)
{
	for (int i = 0; i < length; ++i)
	{
		add_character_to_screen(OSD_SYMBOL_SPACE, pos_x + i, pos_y);
	}
}

int OSDatxxxx::add_battery_info(uint8_t pos_x, uint8_t pos_y)
{
	int ret = PX4_OK;
#ifndef BOTTLE_OSD
	char buf[10];

	// TODO: show battery symbol based on battery fill level
	snprintf(buf, sizeof(buf), "%c%5.2f", OSD_SYMBOL_BATT_3, (double)_battery_voltage_filtered_v);
	buf[sizeof(buf) - 1] = '\0';

	for (int i = 0; buf[i] != '\0'; i++) {
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	ret |= add_character_to_screen('V', pos_x + 5, pos_y);

	pos_y++;
	pos_x++;

	snprintf(buf, sizeof(buf), "%5d", (int)_battery_discharge_mah);
	buf[sizeof(buf) - 1] = '\0';

	for (int i = 0; buf[i] != '\0'; i++) {
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	ret |= add_character_to_screen(OSD_SYMBOL_MAH, pos_x + 5, pos_y);

#else
	char buf[BOTTLE_ROW_BUF_SIZE];
	uint8_t null_position = 0;
	// TODO: show battery symbol based on battery fill level

	snprintf(buf, sizeof(buf), "%2.1fV", (double)_battery_voltage_filtered_v);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_ROW_BUF_SIZE, null_position);

	for (int i = 0; buf[i] != '\0'; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	snprintf(buf, sizeof(buf), "%fmA", (double)_battery_discharge_mah);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_ROW_BUF_SIZE, null_position);

	for (int i = 0; buf[i] != '\0'; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y + 1);
	}

	// snprintf(buf, sizeof(buf), "I:%3.2fA", (double)_battery_current_filtered_a);

 	// null_position = ascii_to_at7456_symbol_convert(buf);
	// fill_buf_with_spaces(buf, BOTTLE_ROW_BUF_SIZE, null_position);

	// for (int i = 0; buf[i] != '\0'; i++)
	// {
	// 	ret |= add_character_to_screen(buf[i], pos_x + i, pos_y + 1);
	// }


#endif
	return ret;
}

int OSDatxxxx::add_voltage_info(uint8_t pos_x, uint8_t pos_y)
{
	int ret = PX4_OK;

	char buf[BOTTLE_ROW_BUF_SIZE];
	uint8_t null_position = 0;
	// TODO: show battery symbol based on battery fill level

	snprintf(buf, sizeof(buf), "%2.1fV", (double)_battery_voltage_filtered_v);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_ROW_BUF_SIZE, null_position);

	//memcpy(buf_temp, buf, sizeof(buf));

	for (int i = 0; i < BOTTLE_ROW_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	return ret;
}

int OSDatxxxx::add_charge_info(uint8_t pos_x, uint8_t pos_y)
{
	int ret = PX4_OK;

	char buf[BOTTLE_ROW_BUF_SIZE];
	uint8_t null_position = 0;
	// TODO: show battery symbol based on battery fill level

	snprintf(buf, sizeof(buf), "%dmA", (int)_battery_discharge_mah);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_ROW_BUF_SIZE, null_position);

	for (int i = 0; i < BOTTLE_ROW_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	return ret;
}

int
OSDatxxxx::add_altitude(uint8_t pos_x, uint8_t pos_y)
{
#ifndef BOTTLE_OSD

	char buf[16];
	int ret = PX4_OK;

	snprintf(buf, sizeof(buf), "%c%5.2fm", OSD_SYMBOL_ARROW_NORTH, (double)_local_position_z);
	buf[sizeof(buf) - 1] = '\0';

	for (int i = 0; buf[i] != '\0'; i++) {
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

#else
	char buf[BOTTLE_ROW_BUF_SIZE];
	int ret = PX4_OK;
	uint8_t null_position = 0;
	//int temp_alt = 0;

	if (_param_osd_test_alt.get() > 0)
	{
		_local_position_z = (float)_param_osd_test_alt.get();
		//temp_alt = (int)_local_position_z;
	}

	memset(buf, 0, sizeof(buf));

	//snprintf(buf, sizeof(buf), "%5.2fm", (double)_local_position_z);

	snprintf(buf, sizeof(buf), "%3d", (int)_local_position_z);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_ROW_BUF_SIZE, null_position);

	for (int i = 0; i < 3; i++) // it is a MEGA WTF (i < 3)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

#endif

	return ret;
}

#ifdef BOTTLE_OSD

void
OSDatxxxx::fill_buf_with_spaces(char *p_buf, uint8_t buf_size, uint8_t null_pos)
{
	for(int i = null_pos; i < buf_size; i++)
	{
		p_buf[i] = OSD_SYMBOL_SPACE;
	}

	p_buf[buf_size - 1] = '\0';
}

int
OSDatxxxx::add_heading(uint8_t pos_x, uint8_t pos_y)
{
	char buf[BOTTLE_COURSE_BUF_SIZE];
	int ret = PX4_OK;
	uint8_t null_position = 0;

	memset(buf, 0, sizeof(buf));

	snprintf(buf, sizeof(buf), "%3d", (int)_local_position_heading);

 	null_position = ascii_to_at7456_symbol_convert(buf);

	fill_buf_with_spaces(buf, BOTTLE_COURSE_BUF_SIZE, null_position);

	for (int i = 0; i < 3; i++) // it is a MEGA WTF (i < 3)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	return ret;
}

int
OSDatxxxx::add_ant(uint8_t pos_x, uint8_t pos_y)
{
	char buf[BOTTLE_ANT_BUF_SIZE];
	int ret = PX4_OK;
	uint8_t null_position = 0;

	memset(buf, 0, sizeof(buf));

	snprintf(buf, sizeof(buf), "%3d", (int)_antenna_used);

 	null_position = ascii_to_at7456_symbol_convert(buf);

	fill_buf_with_spaces(buf, BOTTLE_ANT_BUF_SIZE, null_position);

	for (int i = 0; i < 3; i++) // it is a MEGA WTF (i < 3)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	return ret;
}

int
OSDatxxxx::add_airspeed(uint8_t pos_x, uint8_t pos_y)
{
	char buf[BOTTLE_AIRSPEED_BUF_SIZE];
	int ret = PX4_OK;
	uint8_t null_position = 0;

	memset(buf, 0, sizeof(buf));

	snprintf(buf, sizeof(buf), "%1.1fm/s", (double)_airspeed);

 	null_position = ascii_to_at7456_symbol_convert(buf);

	fill_buf_with_spaces(buf, BOTTLE_AIRSPEED_BUF_SIZE, null_position);

	for (int i = 0; i < BOTTLE_AIRSPEED_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	return ret;
}


int
OSDatxxxx::add_gps(uint8_t pos_x, uint8_t pos_y)
{
	char buf[BOTTLE_GPS_BUF_SIZE];
	int ret = PX4_OK;
	uint8_t null_position = 0;

	const char *s_gps_fix = "";

	memset(buf, 0, sizeof(buf));

	switch(_gps_fix_type)
	{
		case 1:
			s_gps_fix = "NO";
			break;
		case 2:
			s_gps_fix = "2D";
			break;
		case 3:
			s_gps_fix = "3D";
			break;

		default:
			s_gps_fix = "NO";
			break;
	}

	//clear_line(0, BOTTLE_GPS_INFO_ROW, BOTTLE_ROW_BUF_SIZE);

	snprintf(buf, sizeof(buf), "GPS:FIX%s", s_gps_fix);

	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_GPS_BUF_SIZE, null_position);

	for (int i = 0; i < BOTTLE_GPS_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

	snprintf(buf, sizeof(buf), "LAT:%2.5f", (double)_global_position_latitude);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_GPS_BUF_SIZE, null_position);

	for (int i = 0; i < BOTTLE_GPS_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y + 1);
	}

	snprintf(buf, sizeof(buf), "LON:%2.5f", (double)_global_position_lontitude);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_GPS_BUF_SIZE, null_position);

	for (int i = 0; i < BOTTLE_GPS_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y + 2);
	}

	//_gps_satellites_used = 2;

	snprintf(buf, sizeof(buf), "SAT:%d", (int)_gps_satellites_used);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_GPS_BUF_SIZE, null_position);

	for (int i = 0; i < BOTTLE_GPS_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, GPS_SAT_POSITION);
	}

	return ret;
}

#endif


int
OSDatxxxx::add_flighttime(float flight_time, uint8_t pos_x, uint8_t pos_y)
{

#ifndef BOTTLE_OSD

	char buf[10];
	int ret = PX4_OK;

	snprintf(buf, sizeof(buf), "%c%5.1f", OSD_SYMBOL_FLIGHT_TIME, (double)flight_time);
	buf[sizeof(buf) - 1] = '\0';

	for (int i = 0; buf[i] != '\0'; i++) {
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

#else
	uint8_t null_position = 0;
	uint8_t min = 0;
	uint8_t sec = 0;
	char buf[BOTTLE_ROW_BUF_SIZE];
	int ret = PX4_OK;

	min = flight_time / 60;
	sec = flight_time - min * 60;

	memset(buf, 0, sizeof(buf));

	if((min < 10) && (sec < 10))
	{
		snprintf(buf, sizeof(buf), "0%d:0%d", (int)min, (int)sec);
	}

	if((min >= 10) && (sec >= 10))
	{
		snprintf(buf, sizeof(buf), "%2d:%2d", (int)min, (int)sec);
	}

	if((min >= 10) && (sec < 10))
	{
		snprintf(buf, sizeof(buf), "%2d:0%d", (int)min, (int)sec);
	}

	if((min < 10) && (sec >= 10))
	{
		snprintf(buf, sizeof(buf), "0%d:%2d", (int)min, (int)sec);
	}

 	null_position = ascii_to_at7456_symbol_convert(buf);

	fill_buf_with_spaces(buf, BOTTLE_BUF_SIZE, null_position);

	for (int i = 0; i < BOTTLE_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}

#endif

	return ret;
}

int
OSDatxxxx::enable_screen()
{
	uint8_t data = 0;
	int ret = PX4_OK;

	ret |= readRegister(0x00, &data, 1);
	ret |= writeRegister(0x00, data | 0x48);

	return ret;
}

int
OSDatxxxx::disable_screen()
{
	uint8_t data = 0;
	int ret = PX4_OK;

	ret |= readRegister(0x00, &data, 1);
	ret |= writeRegister(0x00, data & 0xF7);

	return ret;
}

int
OSDatxxxx::update_topics()
{
	/* update battery subscription */
	if (_battery_sub.updated()) {
		battery_status_s battery{};
		_battery_sub.copy(&battery);

		if (battery.connected) {
			_battery_voltage_filtered_v = battery.voltage_filtered_v;
			_battery_discharge_mah = battery.discharged_mah;
			_battery_current_filtered_a = battery.current_filtered_a;
			_battery_valid = true;

		} else {
			_battery_valid = false;
		}
	}

#ifndef BOTTLE_OSD

	/* update vehicle local position subscription */
	if (_local_position_sub.updated()) {
		vehicle_local_position_s local_position{};
		_local_position_sub.copy(&local_position);

		_local_position_valid = local_position.z_valid;

		if (_local_position_valid) {
			_local_position_z = -local_position.z;
		}

		_local_position_heading = local_position.heading;
	}
#else

	/* update vehicle local position subscription */
	if (_local_position_sub.updated() || _mount_orientation_sub.updated() || _sensor_gps_sub.updated())
	{
		vehicle_local_position_s local_position{};
		vehicle_global_position_s global_position{};
		mount_orientation_s mount_orientation{};
		sensor_gps_s gps_position{};
		airspeed_s airspeed{};

		_local_position_sub.copy(&local_position);
		_vehicle_global_position_sub.copy(&global_position);
		_sensor_gps_sub.copy(&gps_position);
		_mount_orientation_sub.copy(&mount_orientation);
		_airspeed_sub.copy(&airspeed);

		_local_position_valid = local_position.z_valid;

		if (_local_position_valid)
		{
			_local_position_z = -local_position.z;
		}

		_local_position_heading = math::degrees(matrix::wrap_2pi(local_position.heading + mount_orientation.attitude_euler_angle[2]));
		_gps_fix_type = gps_position.fix_type;
		_gps_satellites_used = gps_position.satellites_used;
		_airspeed = gps_position.vel_m_s;
		_global_position_latitude = global_position.lat;
		_global_position_lontitude = global_position.lon;

	}

	/* update rc_input subscription */
	if (_rc_sub.updated()) {
		struct input_rc_s rc{};
		uint32_t osd_ant_ch = 0;

		_rc_sub.copy(&rc);
		osd_ant_ch = (_param_osd_ant_ch.get()) - 1;


		// first antenna used
		if((rc.values[osd_ant_ch] >= 1000) && (rc.values[osd_ant_ch]< 1099))
		{
			_antenna_used = 740;
		}
		// second antenna used
		else if((rc.values[osd_ant_ch] >= 1100) && (rc.values[osd_ant_ch] < 1199))
		{
			_antenna_used = 433;
		}
		else
		{
			_antenna_used = 0;
		}
	}

#endif

	/* update vehicle status subscription */
	if (_vehicle_status_sub.updated()) {
		vehicle_status_s vehicle_status{};
		_vehicle_status_sub.copy(&vehicle_status);

		if (vehicle_status.arming_state == vehicle_status_s::ARMING_STATE_ARMED &&
		    _arming_state != vehicle_status_s::ARMING_STATE_ARMED) {
			// arming
			_arming_timestamp = hrt_absolute_time();

		} else if (vehicle_status.arming_state != vehicle_status_s::ARMING_STATE_ARMED &&
			   _arming_state == vehicle_status_s::ARMING_STATE_ARMED) {
			// disarming
		}

		_arming_state = vehicle_status.arming_state;
		_nav_state = vehicle_status.nav_state;
	}

	return PX4_OK;
}

const char *
OSDatxxxx::get_flight_mode(uint8_t nav_state)
{
	const char *flight_mode = "UNKNOWN";

	switch (nav_state) {
	case vehicle_status_s::NAVIGATION_STATE_MANUAL:
		flight_mode = "MAN";
		break;

	case vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		flight_mode = "ALT";
		break;

	case vehicle_status_s::NAVIGATION_STATE_POSCTL:
		flight_mode = "POSITION";
		break;

	case vehicle_status_s::NAVIGATION_STATE_AUTO_RTL:
		flight_mode = "RETURN";
		break;

	case vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
		flight_mode = "MISSION";
		break;

	case vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER:
	case vehicle_status_s::NAVIGATION_STATE_DESCEND:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
		flight_mode = "AUTO";
		break;

	// case vehicle_status_s::NAVIGATION_STATE_AUTO_LANDENGFAIL:
	// 	flight_mode = "FAILURE";
	// 	break;

	case vehicle_status_s::NAVIGATION_STATE_ACRO:
		flight_mode = "ACRO";
		break;

	case vehicle_status_s::NAVIGATION_STATE_TERMINATION:
		flight_mode = "TERMINATE";
		break;

	case vehicle_status_s::NAVIGATION_STATE_OFFBOARD:
		flight_mode = "OFFB";
		break;

	case vehicle_status_s::NAVIGATION_STATE_STAB:
		flight_mode = "STAB";
		break;
	}

	return flight_mode;
}

void
OSDatxxxx::add_flight_mode(uint8_t pos_x, uint8_t pos_y)
{
	int ret = PX4_OK;
	const char * p_flight_mode = "";
	char buf[BOTTLE_ROW_BUF_SIZE];
	uint8_t null_position = 0;

	memset(buf, 0, sizeof(buf));

	p_flight_mode = get_flight_mode(_nav_state);

	// TODO: show battery symbol based on battery fill level
	snprintf(buf, sizeof(buf), "%s", p_flight_mode);

 	null_position = ascii_to_at7456_symbol_convert(buf);
	fill_buf_with_spaces(buf, BOTTLE_ROW_BUF_SIZE, null_position);

	for (int i = 0; i < BOTTLE_ROW_BUF_SIZE; i++)
	{
		ret |= add_character_to_screen(buf[i], pos_x + i, pos_y);
	}
}

int
OSDatxxxx::update_screen()
{
	int ret = PX4_OK;

#ifndef BOTTLE_OSD

	if (_battery_valid) {
		ret |= add_battery_info(0, OSD_NUM_ROWS_PAL-1);
	 } else {
	 	clear_line(1, 1, 10);
	 	clear_line(1, 2, 10);
	 }

	if (_local_position_valid) {
		ret |= add_altitude(1, 11);

	} else {
		clear_line(1, 3, 10);
	}

	const char *flight_mode = "";

	if (_arming_state == vehicle_status_s::ARMING_STATE_ARMED) {
		float flight_time_sec = static_cast<float>((hrt_absolute_time() - _arming_timestamp) / (1e6f));
		ret |= add_flighttime(flight_time_sec, 1, 14);

	} else {
		flight_mode = get_flight_mode(_nav_state);
	}

	add_string_to_screen_centered(flight_mode, 12, 10);

#else
	add_flight_mode(_param_osd_mode_x.get(), _param_osd_mode_y.get());

	if (_arming_state == vehicle_status_s::ARMING_STATE_ARMED)
	{
		float flight_time_sec = static_cast<float>((hrt_absolute_time() - _arming_timestamp) / (1e6f));
		ret |= add_flighttime(flight_time_sec, _param_osd_ftime_x.get(), _param_osd_ftime_y.get());
	}
	else
	{
		ret |= add_flighttime(0.0f, _param_osd_ftime_x.get(), _param_osd_ftime_y.get());
	}

	ret |= add_voltage_info(_param_osd_voltage_x.get(), _param_osd_voltage_y.get());
	ret |= add_charge_info(_param_osd_charge_x.get(), _param_osd_charge_y.get());
	ret |= add_airspeed(_param_osd_speed_g_x.get(), _param_osd_speed_g_y.get());
	ret |= add_altitude(_param_osd_alt_x.get(), _param_osd_alt_y.get());
	ret |= add_heading(_param_osd_heading_x.get(), _param_osd_heading_y.get());

	add_ant(_param_osd_ant_x.get(), _param_osd_ant_y.get());

#endif

	return ret;
}

int
OSDatxxxx::reset()
{
	int ret = writeRegister(0x00, 0x02);
	usleep(100);

	return ret;
}

void
OSDatxxxx::RunImpl()
{
	if (should_exit()) {
		exit_and_cleanup();
		return;
	}

	update_topics();

	update_screen();

}

void
OSDatxxxx::print_usage()
{
	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
OSD driver for the ATXXXX chip that is mounted on the OmnibusF4SD board for example.

It can be enabled with the OSD_ATXXXX_CFG parameter.
)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("atxxxx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(false, true);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

int
atxxxx_main(int argc, char *argv[])
{
	using ThisDriver = OSDatxxxx;
	BusCLIArguments cli{false, true};
	cli.spi_mode = SPIDEV_MODE0;
	cli.default_spi_frequency = OSD_SPI_BUS_SPEED;

	const char *verb = cli.parseDefaultArguments(argc, argv);

	if (!verb) {
		ThisDriver::print_usage();
		return -1;
	}

	BusInstanceIterator iterator(MODULE_NAME, cli, DRV_OSD_DEVTYPE_ATXXXX);

	if (!strcmp(verb, "start")) {
		return ThisDriver::module_start(cli, iterator);
	}

	if (!strcmp(verb, "stop")) {
		return ThisDriver::module_stop(iterator);
	}

	if (!strcmp(verb, "status")) {
		return ThisDriver::module_status(iterator);
	}

	ThisDriver::print_usage();
	return -1;
}

uint8_t
OSDatxxxx::ascii_to_at7456_symbol_convert(char *p_array)
{
	//int ret = PX4_ERROR;
	uint8_t counter = 0;

	while(p_array[counter] != '\0')
	{
		// We have a numbers - 1 to 9
		if((p_array[counter] >= '1') && ((p_array[counter] <= '9')))
		{
			p_array[counter] -= OSD_NUMBER_1_TO_9_ASCII_OFFSET;
		}
		// We have a 0 number
		else if(p_array[counter] == '0')
		{
			p_array[counter] = OSD_SYMBOL_0;
		}
		// We have a BIG letters
		else if((p_array[counter] >= 'A') && ((p_array[counter] <= 'Z')))
		{
			p_array[counter] -= OSD_LETTER_A_TO_Z_BIG_ASCII_OFFSET;
		}
		// We have a SMALL letters
		else if((p_array[counter] >= 'a') && ((p_array[counter] <= 'z')))
		{
			p_array[counter] -= OSD_LETTER_A_TO_Z_SMALL_ASCII_OFFSET;
		}
		// We have a dash
		else if(p_array[counter] == '-')
		{
			p_array[counter] = OSD_SYMBOL_DASH;
		}
		// We have a dot
		else if(p_array[counter] == '.')
		{
			p_array[counter] = OSD_SYMBOL_DOT;
		}
		// We have a space
		else if(p_array[counter] == ' ')
		{
			p_array[counter] = OSD_SYMBOL_SPACE;
		}
		// We have a slash
		else if(p_array[counter] == '/')
		{
			p_array[counter] = OSD_SYMBOL_SLASH;
		}
		// We have a colon
		else if(p_array[counter] == ':')
		{
			p_array[counter] = OSD_SYMBOL_COLON;
		}


		counter++;
	}

	return counter;
}

void
OSDatxxxx::at7456_test_row(void)
{
	char buf[50];

	_battery_voltage_filtered_v = 25.2f;
	_battery_discharge_mah = 10;

	snprintf(buf, sizeof(buf), "ABCDEFGHIJKLMNOPQRST123456789");

	ascii_to_at7456_symbol_convert(buf);

	for(int pos_y = 0; pos_y != (OSD_NUM_ROWS_PAL); pos_y++)
	{
		for (int pos_x = 0; buf[pos_x] != '\0'; pos_x++)
		{
			add_character_to_screen(buf[pos_x], pos_x, pos_y);
		}
	}
}


