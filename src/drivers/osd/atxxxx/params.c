/****************************************************************************
 *
 *   Copyright (c) 2018-2019 PX4 Development Team. All rights reserved.
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

/**
* Enable/Disable the ATXXX OSD Chip
*
* Configure the ATXXXX OSD Chip (mounted on the OmnibusF4SD board) and
* select the transmission standard.
*
* @value 0 Disabled
* @value 1 NTSC
* @value 2 PAL
*
* @reboot_required true
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_ATXXXX_CFG, 2);

/**
* Set the X-point of Heading information
*
* Configure the ATXXXX OSD Chip and
* set the X-point of information.
*
* @min 0
* @max 30
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_HEADING_X, 25);

/**
* Set the Y-point of Heading information
*
* Configure the ATXXXX OSD Chip and
* set the Y-point of information.
*
* @min 0
* @max 15
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_HEADING_Y, 15);

/**
* Set the Y-point of Voltage information
*
* Configure the ATXXXX OSD Chip and
* set the Y-point of information.
*
* @min 0
* @max 15
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_VOLTAGE_Y, 14);

/**
* Set the X-point of Voltage information
*
* Configure the ATXXXX OSD Chip and
* set the X-point of information.
*
* @min 0
* @max 30
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_VOLTAGE_X, 0);

/**
* Set the Y-point of Charge information
*
* Configure the ATXXXX OSD Chip and
* set the Y-point of information.
*
* @min 0
* @max 15
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_CHARGE_Y, 15);

/**
* Set the X-point of Charge information
*
* Configure the ATXXXX OSD Chip and
* set the X-point of information.
*
* @min 0
* @max 30
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_CHARGE_X, 0);

/**
* Set the Y-point of Mode information
*
* Configure the ATXXXX OSD Chip and
* set the Y-point of information.
*
* @min 0
* @max 15
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_MODE_Y, 1);

/**
* Set the X-point of Mode information
*
* Configure the ATXXXX OSD Chip and
* set the X-point of information.
*
* @min 0
* @max 30
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_MODE_X, 0);

/**
* Set the Y-point of Altitude information
*
* Configure the ATXXXX OSD Chip and
* set the Y-point of information.
*
* @min 0
* @max 15
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_ALT_Y, 15);

/**
* Set the X-point of Altitude information
*
* Configure the ATXXXX OSD Chip and
* set the X-point of information.
*
* @min 0
* @max 30
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_ALT_X, 25);

/**
* Set the Y-point of Flight time information
*
* Configure the ATXXXX OSD Chip and
* set the Y-point of information.
*
* @min 0
* @max 15
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_FTIME_Y, 2);

/**
* Set the X-point of Flight time information
*
* Configure the ATXXXX OSD Chip and
* set the X-point of information.
*
* @min 0
* @max 30
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_FTIME_X, 0);

/**
* Set the Y-point of used antenna
*
* Configure the ATXXXX OSD Chip and
* set the Y-point of information.
*
* @min 0
* @max 15
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_ANT_Y, 3);

/**
* Set the X-point of used antenna
*
* Configure the ATXXXX OSD Chip and
* set the X-point of information.
*
* @min 0
* @max 30
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_ANT_X, 0);

/**
* OSD module used antenna channel param
*
* Param for set radio channel for monitoring used antenna
*
* @min 5
* @max 16
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_ANT_CH, 16);

/**
* OSD module used antenna channel param ground speed
*
* If test value is not null than this value will be set on the OSD
*
* @min 0
* @max 16
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_SPEED_G_X, 0);

/**
* OSD module used antenna channel param ground speed
*
* If test value is not null than this value will be set on the OSD
*
* @min 0
* @max 16
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_SPEED_G_Y, 0);

/**
* Set the test value for altitude
*
* If test value is not null than this value will be set on the OSD
*
* @min 0
* @max 999
*
* @group OSD
*
*/
PARAM_DEFINE_INT32(OSD_TEST_ALT, 0);
