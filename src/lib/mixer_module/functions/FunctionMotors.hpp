/****************************************************************************
 *
 *   Copyright (c) 2021-2022 PX4 Development Team. All rights reserved.
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

#ضمن "FunctionProviderBase.hpp"

#ضمن <uORB/topics/actuator_motors.h>

/**
 * Functions: Motor1 ... MotorMax
 */
صنف FunctionMotors : عام FunctionProviderBase
{
عام:
	static_assert(actuator_motors_s::NUM_CONTROLS == (صحيح)OutputFunction::MotorMax - (صحيح)OutputFunction::Motor1 + 1,
		      "Unexpected num motors");

	static_assert(actuator_motors_s::ACTUATOR_FUNCTION_MOTOR1 == (صحيح)OutputFunction::Motor1, "Unexpected motor idx");

	FunctionMotors(ثابت Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_motors)),
		_thrust_factor(context.thrust_factor)
	{
		لكل (صحيح i = 0; i < actuator_motors_s::NUM_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	ساكن FunctionProviderBase *allocate(ثابت Context &context) { أرجع جديد FunctionMotors(context); }

	فراغ update() override
	{
		𐤀𐤌 (_topic.update(&_data)) {
			updateValues(_data.reversible_flags, _thrust_factor, _data.control, actuator_motors_s::NUM_CONTROLS);
		}
	}

	عائم value(OutputFunction func) override { أرجع _data.control[(صحيح)func - (صحيح)OutputFunction::Motor1]; }

#𐤀𐤌 defined(CONFIG_DRIVERS_UAVCANNODE)
	منطقي allowPrearmControl() ثابت override { أرجع true; }
#وإلا
	منطقي allowPrearmControl() ثابت override { أرجع خطأ; }
#endif

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { أرجع &_topic; }

	منطقي getLatestSampleTimestamp(hrt_abstime &t) ثابت override { t = _data.timestamp_sample; أرجع t != 0; }

	ساكن مضمن فراغ updateValues(uint32_t reversible, عائم thrust_factor, عائم *values, صحيح num_values)
	{
		𐤀𐤌 (thrust_factor > FLT_EPSILON && thrust_factor <= 1.f) {
			// thrust factor
			//  rel_thrust = factor * x^2 + (1-factor) * x,
			ثابت عائم a = thrust_factor;
			ثابت عائم b = (1.f - thrust_factor);

			// don't recompute for all values (ax^2+bx+c=0)
			ثابت عائم tmp1 = b / (2.f * a);
			ثابت عائم tmp2 = b * b / (4.f * a * a);

			لكل (صحيح i = 0; i < num_values; ++i) {

				ثابت عائم control = values[i];

				𐤀𐤌 (PX4_ISFINITE(control)) {
					𐤀𐤌 (control > FLT_EPSILON) {
						values[i] = -tmp1 + sqrtf(tmp2 + (control / a));

					} وإلا 𐤀𐤌 (control < -FLT_EPSILON) {
						values[i] =  tmp1 - sqrtf(tmp2 - (control / a));

					}
				}
			}
		}

		لكل (صحيح i = 0; i < num_values; ++i) {
			𐤀𐤌 ((reversible & (1u << i)) == 0) {
				𐤀𐤌 (values[i] < -FLT_EPSILON) {
					values[i] = NAN;

				} وإلا {
					// remap from [0, 1] to [-1, 1]
					values[i] = values[i] * 2.f - 1.f;
				}
			}
		}
	}

	منطقي reversible(OutputFunction func) ثابت override { أرجع _data.reversible_flags & (1u << ((صحيح)func - (صحيح)OutputFunction::Motor1)); }

خاص:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_motors_s _data{};
	ثابت عائم &_thrust_factor;
};
