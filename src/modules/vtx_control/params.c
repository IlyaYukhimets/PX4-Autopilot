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
* VTX module enable param
*
* Param for enable VTX module for tests
*
* @value 0 Disabled
* @value 123 Enable VTX module for tests
*
* @reboot_required true
* @group VTX
*
*/
PARAM_DEFINE_INT32(VTX_TEST_PAR, 0);

/**
* VTX module radio channel param
*
* Param for set radio channel for activate VTX module
*
* @min 1
* @max 18
*
* @reboot_required true
* @group VTX
*
*/
PARAM_DEFINE_INT32(VTX_RC_CH_PAR, 10);

/**
* VTX module threshold min param
*
* Param for set MIN threshold of VTX enable function
*
* @min 0
* @max 3000
*
* @reboot_required true
* @group VTX
*
*/
PARAM_DEFINE_INT32(VTX_MIN_THR_PAR, 800);

/**
* VTX module threshold max param
*
* Param for set MAX threshold of VTX enable function
*
* @min 0
* @max 3000
*
* @reboot_required true
* @group VTX
*
*/
PARAM_DEFINE_INT32(VTX_MAX_THR_PAR, 1200);

