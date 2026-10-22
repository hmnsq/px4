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

/**
 * @file WeatherVane.hpp
 * @author Ivo Drescher
 * @author Roman Bapst <roman@auterion.com>
 *
 * Weathervane controller.
 *
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/module_params.h>
#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/Subscription.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/vehicle_attitude_setpoint.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/vehicle_local_position.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/vehicle_control_mode.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/vehicle_status.h>

ଶ୍ରେଣୀ WeatherVane : ସର୍ବସାଧାରଣ ModuleParams
{
ସର୍ବସାଧାରଣ:
	WeatherVane(ModuleParams *parent);

	~WeatherVane() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଶୂନ୍ୟ setNavigatorForceDisabled(ତାର୍କିକ navigator_force_disabled) { _navigator_force_disabled = navigator_force_disabled; };

	ତାର୍କିକ isActive() {ଫେରନ୍ତୁ _is_active;}

	ଶୂନ୍ୟ update();

	ଦଶମିକ getWeathervaneYawrate();

ବ୍ୟକ୍ତିଗତ:
	uORB::Subscription _vehicle_attitude_setpoint_sub{ORB_ID(vehicle_attitude_setpoint)};
	uORB::Subscription _vehicle_local_position_sub{ORB_ID(vehicle_local_position)};
	uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

	ତାର୍କିକ _is_active{ମିଥ୍ୟା};

	// local copies of status such that we don't need to copy uORB messages all the time
	ତାର୍କିକ _flag_control_manual_enabled{ମିଥ୍ୟା};
	ତାର୍କିକ _flag_control_position_enabled{ମିଥ୍ୟା};
	ତାର୍କିକ _navigator_force_disabled{ମିଥ୍ୟା};

	DEFINE_PARAMETERS(
		(ParamBool<px4::params::WV_EN>) _param_wv_en,
		(ParamFloat<px4::params::WV_ROLL_MIN>) _param_wv_roll_min,
		(ParamFloat<px4::params::WV_GAIN>) _param_wv_gain,
		(ParamFloat<px4::params::WV_YRATE_MAX>) _param_wv_yrate_max
	)

};
