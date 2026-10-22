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

#అన్తర్భావ "autosave.h"

#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <uORB/topics/actuator_armed.h>
#అన్తర్భావ <uORB/Subscription.hpp>

#అన్తర్భావ "param.h"
#అన్తర్భావ "atomic_transaction.h"

ఉపయోగించి నామాకాశ సమయం_literals;

ParamAutosave::ParamAutosave()
	: Scheduled_Work_అంశం(MODULE_NAME, px4::wq_configurations::lp_default)
{
}

శూన్య ParamAutosave::request()
{
	ఒకవేళ (_scheduled.ఎక్కించు() || _disabled) {
		తిరిగి;
	}

	// wait at least 300ms before saving, because:
	// - tasks often call param_set() for multiple params, so this avoids unnecessary save calls
	// - the logger stores changed params. He gets notified on a param change via uORB and then
	//   looks at all unsaved params.
	hrt_abstime delay = 300_ms;

	స్థిర constexpr స్థిరరాశి hrt_abstime రేటు_హద్దు = 2_s; // rate-limit saving to 2 seconds
	స్థిరరాశి hrt_abstime చివరి_save_elapsed = hrt_elapsed_సమయం(&చివరి_timestamp);

	ఒకవేళ (చివరి_save_elapsed < రేటు_హద్దు && రేటు_హద్దు > చివరి_save_elapsed + delay) {
		delay = రేటు_హద్దు - చివరి_save_elapsed;
	}

	_scheduled.store(నిజం);
	కాలపట్టిక_Delayed(delay);
}

శూన్య ParamAutosave::enable(తార్కిక enable)
{
	AtomicTransaction transaction;
	_disabled = !enable;

	ఒకవేళ (!enable && _scheduled.ఎక్కించు()) {
		_scheduled.store(అబద్ధం);
		px4::Scheduled_Work_అంశం::కాలపట్టిక_Clear();
	}
}

శూన్య ParamAutosave::నిరంతరం()
{
	తార్కిక disabled = అబద్ధం;

	ఒకవేళ (!param_get_default_file()) {
		// In case we save to FLASH, defer param writes until disarmed,
		// as writing to FLASH can stall the entire CPU (in rare cases around 300ms on STM32F7)
		uORB::Subscription_దత్తాంశం<actuator_armed_s> armed_sub{ORB_ID(actuator_armed)};

		ఒకవేళ (armed_sub.get().armed) {
			కాలపట్టిక_Delayed(1_s);
			తిరిగి;
		}
	}

	{
		స్థిరరాశి AtomicTransaction transaction;
		చివరి_timestamp = hrt_absolute_సమయం();
		// Note that the order is important here: we first clear _scheduled, then save the parameters, as during export,
		// more parameter changes could happen.
		_scheduled.store(అబద్ధం);
		disabled = _disabled;
	}

	ఒకవేళ (disabled) {
		తిరిగి;
	}

	PX4_DEBUG("Autosaving params");
	పూర్ణాంకం ret = param_save_default(అబద్ధం);

	ఒకవేళ (ret != PX4_OK) {
		// re-request to be saved in the future, try 3 times at most
		ఒకవేళ (retry_లెక్క_2 < 3) {
			retry_లెక్క_2++;
			PX4_INFO("param auto save unavailable (%i), retrying..", ret);
			request();

		} లేకపోతే {
			PX4_ERR("param auto save failed (%i)", ret);
			retry_లెక్క_2 = 0;
		}

	} లేకపోతే {
		retry_లెక్క_2 = 0;
	}
}
