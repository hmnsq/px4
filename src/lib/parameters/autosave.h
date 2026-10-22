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

#సూచన once

#అన్తర్భావ <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#అన్తర్భావ <px4_platform_common/atomic.h>
#అన్తర్భావ <drivers/drv_hrt.h>

class ParamAutosave : బహిరంగ px4::Scheduled_Work_అంశం
{
బహిరంగ:

	ParamAutosave();

	/**
	 * Automatically save the parameters after a timeout and at limited rate.
	 */
	శూన్య request();

	శూన్య enable(bool enable);

	bool enabled() స్థిరరాశి { తిరిగి !_disabled; }

	hrt_abstime చివరి_Autosave() స్థిరరాశి { తిరిగి చివరి_timestamp; }

	శూన్య నిరంతరం() override;

అంతరంగ:
	hrt_abstime చివరి_timestamp{0};
	px4::atomic_bool _scheduled{false};
	పూర్ణాంకం retry_లెక్క_2{0};
	bool _disabled{false};
};
