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

#ಅನ್ತರ್ಭಾವ "autosave.h"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/actuator_armed.h>
#ಅನ್ತರ್ಭಾವ <uORB/Subscription.hpp>

#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "atomic_transaction.h"

ಬಳಸಿ ನಾಮಾಕಾಶ ಸಮಯ_literals;

ParamAutosave::ParamAutosave()
	: Scheduled_Work_ಅಂಶ(MODULE_NAME, px4::wq_configurations::lp_default)
{
}

ಶೂನ್ಯ ParamAutosave::request()
{
	ಆದರೆ (_scheduled.ತುಂಬು() || _disabled) {
		ಹಿಂತಿರುಗಿ;
	}

	// wait at least 300ms before saving, because:
	// - tasks often call param_set() for multiple params, so this avoids unnecessary save calls
	// - the logger stores changed params. He gets notified on a param change via uORB and then
	//   looks at all unsaved params.
	hrt_abstime delay = 300_ms;

	ಸ್ಥಿರ constexpr ಸ್ಥಿರಾಂಕ hrt_abstime ದರ_ಎಲ್ಲೆ = 2_s; // rate-limit saving to 2 seconds
	ಸ್ಥಿರಾಂಕ hrt_abstime ಕೊನೆಯ_save_elapsed = hrt_elapsed_ಸಮಯ(&ಕೊನೆಯ_timestamp);

	ಆದರೆ (ಕೊನೆಯ_save_elapsed < ದರ_ಎಲ್ಲೆ && ದರ_ಎಲ್ಲೆ > ಕೊನೆಯ_save_elapsed + delay) {
		delay = ದರ_ಎಲ್ಲೆ - ಕೊನೆಯ_save_elapsed;
	}

	_scheduled.store(ನಿಜ);
	ವೇಳಾಪಟ್ಟಿ_Delayed(delay);
}

ಶೂನ್ಯ ParamAutosave::enable(ತಾರ್ಕಿಕ enable)
{
	AtomicTransaction transaction;
	_disabled = !enable;

	ಆದರೆ (!enable && _scheduled.ತುಂಬು()) {
		_scheduled.store(ಸುಳ್ಳು);
		px4::Scheduled_Work_ಅಂಶ::ವೇಳಾಪಟ್ಟಿ_Clear();
	}
}

ಶೂನ್ಯ ParamAutosave::ಸತತ()
{
	ತಾರ್ಕಿಕ disabled = ಸುಳ್ಳು;

	ಆದರೆ (!param_get_default_file()) {
		// In case we save to FLASH, defer param writes until disarmed,
		// as writing to FLASH can stall the entire CPU (in rare cases around 300ms on STM32F7)
		uORB::Subscription_ದತ್ತಾಂಶ<actuator_armed_s> armed_sub{ORB_ID(actuator_armed)};

		ಆದರೆ (armed_sub.get().armed) {
			ವೇಳಾಪಟ್ಟಿ_Delayed(1_s);
			ಹಿಂತಿರುಗಿ;
		}
	}

	{
		ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;
		ಕೊನೆಯ_timestamp = hrt_absolute_ಸಮಯ();
		// Note that the order is important here: we first clear _scheduled, then save the parameters, as during export,
		// more parameter changes could happen.
		_scheduled.store(ಸುಳ್ಳು);
		disabled = _disabled;
	}

	ಆದರೆ (disabled) {
		ಹಿಂತಿರುಗಿ;
	}

	PX4_DEBUG("Autosaving params");
	ಪೂರ್ಣಾಂಕ ret = param_save_default(ಸುಳ್ಳು);

	ಆದರೆ (ret != PX4_OK) {
		// re-request to be saved in the future, try 3 times at most
		ಆದರೆ (retry_ಎಣಿಕೆ_2 < 3) {
			retry_ಎಣಿಕೆ_2++;
			PX4_INFO("param auto save unavailable (%i), retrying..", ret);
			request();

		} ಇಲ್ಲದಿದ್ದರೆ {
			PX4_ERR("param auto save failed (%i)", ret);
			retry_ಎಣಿಕೆ_2 = 0;
		}

	} ಇಲ್ಲದಿದ್ದರೆ {
		retry_ಎಣಿಕೆ_2 = 0;
	}
}
