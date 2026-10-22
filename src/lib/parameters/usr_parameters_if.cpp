/****************************************************************************
 *
 *   Copyright (c) 2021 Technology Innovation Institute. All rights reserved.
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

/**
 * @file usr_parameters_if.cpp
 *
 * Protected build user space interface to global parameter store.
 */
#تعریف_کرو PARAM_IMPLEMENTATION
#شامل "param.h"
#شامل "parameters_ioctl.h"
#شامل <parameters/px4_parameters.hpp>
#شامل <sys/boardctl.h>
#شامل <px4_platform_common/defines.h>
#شامل <px4_platform_common/log.h>

#شامل "parameters_common.cpp"

خالی
param_notify_changes()
{
	boardctl(PARAMIOCNOTIFY, تہی);
}

param_t param_ڈھونڈو(ثابت حرف *name)
{
	paramiocfind_t ڈیٹا = {name, سچ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

param_t param_ڈھونڈو_no_notification(ثابت حرف *name)
{
	paramiocfind_t ڈیٹا = {name, جھوٹ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

غیرعلامتی param_گنتی_مستعمل()
{
	paramioccountused_t ڈیٹا = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

param_t param_for_مستعمل_اشاریہ(غیرعلامتی اشاریہ)
{
	paramiocforusedindex_t ڈیٹا = {اشاریہ, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

صحیح param_get_مستعمل_اشاریہ(param_t param)
{
	paramiocgetusedindex_t ڈیٹا = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

منطقی
param_قدر_unsaved(param_t param)
{
	paramiocunsaved_t ڈیٹا = {param, جھوٹ};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

صحیح
param_get(param_t param, خالی *val)
{
	paramiocget_t ڈیٹا = {param, جھوٹ, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

صحیح
param_get_default_قدر(param_t param, خالی *default_val)
{
	paramiocget_t ڈیٹا = {param, سچ, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

خالی
param_control_autosave(منطقی enable)
{
	paramiocautosave_t ڈیٹا = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
}

صحیح param_سیٹ(param_t param, ثابت خالی *val)
{
	paramiocset_t ڈیٹا = {param, سچ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

صحیح param_سیٹ_no_notification(param_t param, ثابت خالی *val)
{
	paramiocset_t ڈیٹا = {param, جھوٹ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

منطقی param_مستعمل(param_t param)
{
	paramiocused_t ڈیٹا = {param, جھوٹ};
	boardctl(PARAMIOCUSED, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

خالی param_سیٹ_مستعمل(param_t param)
{
	paramiocsetused_t ڈیٹا = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
}

صحیح param_سیٹ_default_قدر(param_t param, ثابت خالی *val)
{
	paramiocsetdefault_t ڈیٹا = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

صحیح param_reset(param_t param)
{
	paramiocreset_t ڈیٹا = {param, سچ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

صحیح param_reset_no_notification(param_t param)
{
	paramiocreset_t ڈیٹا = {param, جھوٹ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

خالی
param_reset_all()
{
	paramiocresetgroup_t ڈیٹا = {PARAM_RESET_ALL, خالی_اشاریہ, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
}

خالی
param_reset_excludes(ثابت حرف *excludes[], صحیح عدد_excludes)
{
	paramiocresetgroup_t ڈیٹا = {PARAM_RESET_EXCLUDES, excludes, عدد_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
}

خالی
param_reset_specific(ثابت حرف *resets[], صحیح عدد_resets)
{
	paramiocresetgroup_t ڈیٹا = {PARAM_RESET_SPECIFIC, resets, عدد_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
}

صحیح param_save_default(منطقی blocking)
{
	paramiocsavedefault_t ڈیٹا = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

صحیح
param_بھرو_default()
{
	paramiocloaddefault_t ڈیٹا = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

صحیح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	paramiocexport_t ڈیٹا = {filename, PX4_ERROR};

	اگر (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t ڈیٹا = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<غیرعلامتی طویل>(&ڈیٹا));
	واپس ڈیٹا.ret;
}
