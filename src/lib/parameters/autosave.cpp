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

#شامل "autosave.h"

#شامل <px4_platform_common/log.h>
#شامل <uORB/topics/actuator_armed.h>
#شامل <uORB/Subscription.hpp>

#شامل "param.h"
#شامل "atomic_transaction.h"

استعمال نامخلا وقت_literals;

ParamAutosave::ParamAutosave()
	: Scheduled_Work_شے(MODULE_NAME, px4::wq_configurations::lp_default)
{
}

خالی ParamAutosave::request()
{
	اگر (_scheduled.بھرو() || _disabled) {
		واپس;
	}

	// wait at least 300ms before saving, because:
	// - tasks often call param_set() for multiple params, so this avoids unnecessary save calls
	// - the logger stores changed params. He gets notified on a param change via uORB and then
	//   looks at all unsaved params.
	hrt_abstime delay = 300_ms;

	ساکن constexpr ثابت hrt_abstime شرح_حد_بندی = 2_s; // rate-limit saving to 2 seconds
	ثابت hrt_abstime آخری_save_elapsed = hrt_elapsed_وقت(&آخری_timestamp);

	اگر (آخری_save_elapsed < شرح_حد_بندی && شرح_حد_بندی > آخری_save_elapsed + delay) {
		delay = شرح_حد_بندی - آخری_save_elapsed;
	}

	_scheduled.store(سچ);
	جدول_وقت_Delayed(delay);
}

خالی ParamAutosave::enable(منطقی enable)
{
	AtomicTransaction transaction;
	_disabled = !enable;

	اگر (!enable && _scheduled.بھرو()) {
		_scheduled.store(جھوٹ);
		px4::Scheduled_Work_شے::جدول_وقت_Clear();
	}
}

خالی ParamAutosave::تسلسل()
{
	منطقی disabled = جھوٹ;

	اگر (!param_get_default_file()) {
		// In case we save to FLASH, defer param writes until disarmed,
		// as writing to FLASH can stall the entire CPU (in rare cases around 300ms on STM32F7)
		uORB::Subscription_ڈیٹا<actuator_armed_s> armed_sub{ORB_ID(actuator_armed)};

		اگر (armed_sub.get().armed) {
			جدول_وقت_Delayed(1_s);
			واپس;
		}
	}

	{
		ثابت AtomicTransaction transaction;
		آخری_timestamp = hrt_absolute_وقت();
		// Note that the order is important here: we first clear _scheduled, then save the parameters, as during export,
		// more parameter changes could happen.
		_scheduled.store(جھوٹ);
		disabled = _disabled;
	}

	اگر (disabled) {
		واپس;
	}

	PX4_DEBUG("Autosaving params");
	صحیح ret = param_save_default(جھوٹ);

	اگر (ret != PX4_OK) {
		// re-request to be saved in the future, try 3 times at most
		اگر (retry_گنتی_2 < 3) {
			retry_گنتی_2++;
			PX4_INFO("param auto save unavailable (%i), retrying..", ret);
			request();

		} ورنہ {
			PX4_ERR("param auto save failed (%i)", ret);
			retry_گنتی_2 = 0;
		}

	} ورنہ {
		retry_گنتی_2 = 0;
	}
}
