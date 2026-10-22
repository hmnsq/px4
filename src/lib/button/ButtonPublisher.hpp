/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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
 * @file ButtonPublisher.hpp
 *
 * Library for button functionality.
 *
 */

#pragma once

#அந்தர்பாவ <drivers/drv_tone_alarm.h>
#அந்தர்பாவ <drivers/drv_hrt.h>
#அந்தர்பாவ <uORB/Publication.hpp>
#அந்தர்பாவ <uORB/PublicationMulti.hpp>
#அந்தர்பாவ <uORB/Subscription.hpp>
#அந்தர்பாவ <uORB/topics/actuator_armed.h>
#அந்தர்பாவ <uORB/topics/button_event.h>
#அந்தர்பாவ <uORB/topics/vehicle_command.h>
#அந்தர்பாவ <uORB/topics/led_control.h>
#அந்தர்பாவ <uORB/topics/tune_control.h>

வகுப்பு ButtonPublisher
{
பொது:
	ButtonPublisher();
	~ButtonPublisher() = இயல்பு;

	/**
	 * Function for publishing safety button trigger event
	 */
	வெறுமை safetyButtonTriggerEvent();

	/**
	 * Function for publishing pairing button trigger event
	 */
	வெறுமை pairingButtonTriggerEvent();

	நிலையான constexpr uint8_t PAIRING_BUTTON_EVENT_மொத்தம்{3};

தனிப்பட்ட:
	uORB::Publication<button_event_s>		_safety_button_pub{ORB_ID(safety_button)};
	uORB::Publication<vehicle_command_s>	_vehicle_command_pub{ORB_ID(vehicle_command)};
	uORB::Publication<led_control_s> 		_led_control_pub{ORB_ID(led_control)};
	uORB::Publication<tune_control_s> 		_tune_control_pub{ORB_ID(tune_control)};
};
