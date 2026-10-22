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

#pragma once

#അന്തര്ഭാവ <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#അന്തര്ഭാവ <px4_platform_common/atomic.h>
#അന്തര്ഭാവ <drivers/drv_hrt.h>

class ParamAutosave : പൊതു px4::ScheduledWorkItem
{
പൊതു:

	ParamAutosave();

	/**
	 * Automatically save the parameters after a timeout and at limited rate.
	 */
	ശൂന്യം request();

	ശൂന്യം enable(bool enable);

	bool enabled() സ്ഥിരം { തിരികെ !_disabled; }

	hrt_abstime lastAutosave() സ്ഥിരം { തിരികെ _last_timestamp; }

	ശൂന്യം Run() override;

സ്വകാര്യ:
	hrt_abstime _last_timestamp{0};
	px4::atomic_bool _scheduled{false};
	പൂർണ്ണസംഖ്യ retry_എണ്ണം_2{0};
	bool _disabled{false};
};
