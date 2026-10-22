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
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/gpio_in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/SubscriptionCallback.hpp>

ਵਰਗ CallbackHandler : ਜਨਤਕ uORB::SubscriptionCallback
{
ਜਨਤਕ:
	uint32_t dev_id;
	uint16_t ਨਿਵੇਸ਼{0};

	CallbackHandler(orb_id_t id) : uORB::SubscriptionCallback(id) {}
	ਆਭਾਸੀ ~CallbackHandler() {}

	ਖਾਲੀ call(ਬਿਨਾਨਿਸ਼ਾਨੀ generation) override
	{
		px4::ਸੁਨੇਹਾ_2::Gpio_ਵਿੱਚ new_ਨਿਵੇਸ਼;

		ਲਈ (ਪੂਰਨ i = 0; i < new_ਨਿਵੇਸ਼.MAX_INSTANCES; i++) {
			ChangeInstance(i);

			ਜੇ (update(&new_ਨਿਵੇਸ਼) && new_ਨਿਵੇਸ਼.device_id == dev_id) {
				ਨਿਵੇਸ਼ = new_ਨਿਵੇਸ਼.state;
				ਰੋਕੋ;
			}
		}
	}
};
