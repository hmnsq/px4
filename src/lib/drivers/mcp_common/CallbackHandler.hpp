/****************************************************************************
 *
 *   Copyright (C) 2025 PX4 Development Team. All rights reserved.
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

#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/gpio_in.h>
#ଅନ୍ତର୍ଭାଵ <uORB/SubscriptionCallback.hpp>

ଶ୍ରେଣୀ CallbackHandler : ସର୍ବସାଧାରଣ uORB::SubscriptionCallback
{
ସର୍ବସାଧାରଣ:
	uint32_t dev_id;
	uint16_t ନିବେଶ{0};

	CallbackHandler(orb_id_t id) : uORB::SubscriptionCallback(id) {}
	ଆଭାସୀ ~CallbackHandler() {}

	ଶୂନ୍ୟ call(unsigned generation) override
	{
		px4::ସନ୍ଦେଶ::GpioIn new_ନିବେଶ;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < new_ନିବେଶ.MAX_INSTANCES; i++) {
			ChangeInstance(i);

			ଯଦି (update(&new_ନିବେଶ) && new_ନିବେଶ.device_id == dev_id) {
				ନିବେଶ = new_ନିବେଶ.state;
				ଭାଙ୍ଗ;
			}
		}
	}
};
