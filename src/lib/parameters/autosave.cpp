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

#અન્તર્ભાવ "autosave.h"

#અન્તર્ભાવ <px4_platform_common/log.h>
#અન્તર્ભાવ <uORB/topics/actuator_armed.h>
#અન્તર્ભાવ <uORB/Subscription.hpp>

#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "atomic_transaction.h"

using નામાકાશ time_literals;

ParamAutosave::ParamAutosave()
	: ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::lp_default)
{
}

શૂન્ય ParamAutosave::request()
{
	જો (_scheduled.load() || _disabled) {
		પાછા;
	}

	// wait at least 300ms before saving, because:
	// - tasks often call param_set() for multiple params, so this avoids unnecessary save calls
	// - the logger stores changed params. He gets notified on a param change via uORB and then
	//   looks at all unsaved params.
	hrt_abstime delay = 300_ms;

	સ્થિર constexpr અચલ hrt_abstime rate_limit = 2_s; // rate-limit saving to 2 seconds
	અચલ hrt_abstime last_save_elapsed = hrt_elapsed_time(&_last_timestamp);

	જો (last_save_elapsed < rate_limit && rate_limit > last_save_elapsed + delay) {
		delay = rate_limit - last_save_elapsed;
	}

	_scheduled.store(સાચું);
	ScheduleDelayed(delay);
}

શૂન્ય ParamAutosave::enable(તાર્કિક enable)
{
	AtomicTransaction transaction;
	_disabled = !enable;

	જો (!enable && _scheduled.load()) {
		_scheduled.store(ખોટું);
		px4::ScheduledWorkItem::ScheduleClear();
	}
}

શૂન્ય ParamAutosave::Run()
{
	તાર્કિક disabled = ખોટું;

	જો (!param_get_default_file()) {
		// In case we save to FLASH, defer param writes until disarmed,
		// as writing to FLASH can stall the entire CPU (in rare cases around 300ms on STM32F7)
		uORB::SubscriptionData<actuator_armed_s> armed_sub{ORB_ID(actuator_armed)};

		જો (armed_sub.get().armed) {
			ScheduleDelayed(1_s);
			પાછા;
		}
	}

	{
		અચલ AtomicTransaction transaction;
		_last_timestamp = hrt_absolute_time();
		// Note that the order is important here: we first clear _scheduled, then save the parameters, as during export,
		// more parameter changes could happen.
		_scheduled.store(ખોટું);
		disabled = _disabled;
	}

	જો (disabled) {
		પાછા;
	}

	PX4_DEBUG("Autosaving params");
	પૂર્ણાંક ret = param_save_default(ખોટું);

	જો (ret != PX4_OK) {
		// re-request to be saved in the future, try 3 times at most
		જો (retry_ગણતરી_2 < 3) {
			retry_ગણતરી_2++;
			PX4_INFO("param auto save unavailable (%i), retrying..", ret);
			request();

		} નહીંતર {
			PX4_ERR("param auto save failed (%i)", ret);
			retry_ગણતરી_2 = 0;
		}

	} નહીંતર {
		retry_ગણતરી_2 = 0;
	}
}
