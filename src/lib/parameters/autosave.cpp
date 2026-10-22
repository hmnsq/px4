/****************************************************************************
 *
 *   Copyright (c) 2023 PX4 Development Team. All rights reserved.
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

#ਅਨ੍ਤਰ੍ਭਾਵ "autosave.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/actuator_armed.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Subscription.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "atomic_transaction.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

ParamAutosave::ParamAutosave()
	: Scheduled_Work_ਵਸਤੂ(MODULE_NAME, px4::wq_configurations::lp_default)
{
}

ਖਾਲੀ ParamAutosave::request()
{
	ਜੇ (_scheduled.ਲੱਦੋ() || _disabled) {
		ਵਾਪਸ;
	}

	// wait at least 300ms before saving, because:
	// - tasks often call param_set() for multiple params, so this avoids unnecessary save calls
	// - the logger stores changed params. He gets notified on a param change via uORB and then
	//   looks at all unsaved params.
	hrt_abstime delay = 300_ms;

	ਸਥਿਰ constexpr ਅਚਲ hrt_abstime ਦਰ_ਹੱਦਬੰਦੀ = 2_s; // rate-limit saving to 2 seconds
	ਅਚਲ hrt_abstime ਆਖਰੀ_save_elapsed = hrt_elapsed_ਸਮਾਂ(&ਆਖਰੀ_timestamp);

	ਜੇ (ਆਖਰੀ_save_elapsed < ਦਰ_ਹੱਦਬੰਦੀ && ਦਰ_ਹੱਦਬੰਦੀ > ਆਖਰੀ_save_elapsed + delay) {
		delay = ਦਰ_ਹੱਦਬੰਦੀ - ਆਖਰੀ_save_elapsed;
	}

	_scheduled.store(ਸੱਚ);
	ਸਮਾਂ_ਸਾਰਣੀ_Delayed(delay);
}

ਖਾਲੀ ParamAutosave::enable(ਤਾਰਕਿਕ enable)
{
	AtomicTransaction transaction;
	_disabled = !enable;

	ਜੇ (!enable && _scheduled.ਲੱਦੋ()) {
		_scheduled.store(ਝੂਠ);
		px4::Scheduled_Work_ਵਸਤੂ::ਸਮਾਂ_ਸਾਰਣੀ_Clear();
	}
}

ਖਾਲੀ ParamAutosave::ਲੜੀਵਾਰ()
{
	ਤਾਰਕਿਕ disabled = ਝੂਠ;

	ਜੇ (!param_get_default_file()) {
		// In case we save to FLASH, defer param writes until disarmed,
		// as writing to FLASH can stall the entire CPU (in rare cases around 300ms on STM32F7)
		uORB::Subscription_ਅੰਕੜੇ<actuator_armed_s> armed_sub{ORB_ID(actuator_armed)};

		ਜੇ (armed_sub.get().armed) {
			ਸਮਾਂ_ਸਾਰਣੀ_Delayed(1_s);
			ਵਾਪਸ;
		}
	}

	{
		ਅਚਲ AtomicTransaction transaction;
		ਆਖਰੀ_timestamp = hrt_absolute_ਸਮਾਂ();
		// Note that the order is important here: we first clear _scheduled, then save the parameters, as during export,
		// more parameter changes could happen.
		_scheduled.store(ਝੂਠ);
		disabled = _disabled;
	}

	ਜੇ (disabled) {
		ਵਾਪਸ;
	}

	PX4_DEBUG("Autosaving params");
	ਪੂਰਨ ret = param_save_default(ਝੂਠ);

	ਜੇ (ret != PX4_OK) {
		// re-request to be saved in the future, try 3 times at most
		ਜੇ (retry_ਗਿਣਤੀ_2 < 3) {
			retry_ਗਿਣਤੀ_2++;
			PX4_INFO("param auto save unavailable (%i), retrying..", ret);
			request();

		} ਨਹੀਂਤਾਂ {
			PX4_ERR("param auto save failed (%i)", ret);
			retry_ਗਿਣਤੀ_2 = 0;
		}

	} ਨਹੀਂਤਾਂ {
		retry_ਗਿਣਤੀ_2 = 0;
	}
}
