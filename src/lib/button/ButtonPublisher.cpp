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
 * @file ButtonPublisher.cpp
 *
 * Library for button functionality.
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <button/ButtonPublisher.hpp>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

ButtonPublisher::ButtonPublisher()
{
	_safety_button_pub.advertise();
}

ਖਾਲੀ ButtonPublisher::safetyButtonTriggerEvent()
{


	button_event_s safety_button{};
	safety_button.triggered = ਸੱਚ;
	safety_button.timestamp = hrt_absolute_ਸਮਾਂ();

	_safety_button_pub.publish(safety_button);
}

ਖਾਲੀ ButtonPublisher::pairingButtonTriggerEvent()
{
	vehicle_command_s vcmd{};
	vcmd.command = vehicle_command_s::VEHICLE_CMD_ਸ਼ੁਰੂ_RX_PAIR;
	vcmd.param1 = 10.f; // GCS pairing request handled by a companion.
	vcmd.timestamp = hrt_absolute_ਸਮਾਂ();
	_vehicle_command_pub.publish(vcmd);
	PX4_DEBUG("Sending GCS pairing request");

	led_control_s led_control{};
	led_control.led_ਨਕਾਬ = 0xff;
	led_control.ਬਹੁਲਕ = led_control_s::ਬਹੁਲਕ_BLINK_FAST;
	led_control.color = led_control_s::COLOR_WHITE;
	led_control.ਅੰਕ_blinks = 20;
	led_control.ਤਰਜੀਹ = 2;
	led_control.timestamp = hrt_absolute_ਸਮਾਂ();
	_led_control_pub.publish(led_control);

	tune_control_s tune_control{};
	tune_control.tune_id = tune_control_s::TUNE_ID_NOTIFY_ਸਕਾਰਾਤਮਕ;
	tune_control.volume = tune_control_s::VOLUME_LEVEL_DEFAULT;
	tune_control.timestamp = hrt_absolute_ਸਮਾਂ();
	_tune_control_pub.publish(tune_control);
}
