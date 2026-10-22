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
#تعریف_کن PARAM_IMPLEMENTATION
#ضمن "param.h"
#ضمن "parameters_ioctl.h"
#ضمن <parameters/px4_parameters.hpp>
#ضمن <sys/boardctl.h>
#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/log.h>

#ضمن "parameters_common.cpp"

فراغ
param_notify_changes()
{
	boardctl(PARAMIOCNOTIFY, عدم);
}

param_t param_پیدا_کن(ثابت حرف *name)
{
	paramiocfind_t داده = {name, درست, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

param_t param_پیدا_کن_no_notification(ثابت حرف *name)
{
	paramiocfind_t داده = {name, خطأ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

بیعلامت param_شمار_استفاده_شده()
{
	paramioccountused_t داده = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

param_t param_for_استفاده_شده_شاخص(بیعلامت شاخص)
{
	paramiocforusedindex_t داده = {شاخص, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

صحيح param_get_استفاده_شده_شاخص(param_t param)
{
	paramiocgetusedindex_t داده = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

منطقي
param_مقدار_unsaved(param_t param)
{
	paramiocunsaved_t داده = {param, خطأ};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

صحيح
param_get(param_t param, فراغ *val)
{
	paramiocget_t داده = {param, خطأ, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

صحيح
param_get_default_مقدار(param_t param, فراغ *default_val)
{
	paramiocget_t داده = {param, درست, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

فراغ
param_control_autosave(منطقي enable)
{
	paramiocautosave_t داده = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<بیعلامت دراز>(&داده));
}

صحيح param_مجموعه(param_t param, ثابت فراغ *val)
{
	paramiocset_t داده = {param, درست, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

صحيح param_مجموعه_no_notification(param_t param, ثابت فراغ *val)
{
	paramiocset_t داده = {param, خطأ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

منطقي param_استفاده_شده(param_t param)
{
	paramiocused_t داده = {param, خطأ};
	boardctl(PARAMIOCUSED, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

فراغ param_مجموعه_استفاده_شده(param_t param)
{
	paramiocsetused_t داده = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<بیعلامت دراز>(&داده));
}

صحيح param_مجموعه_default_مقدار(param_t param, ثابت فراغ *val)
{
	paramiocsetdefault_t داده = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

صحيح param_reset(param_t param)
{
	paramiocreset_t داده = {param, درست, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

صحيح param_reset_no_notification(param_t param)
{
	paramiocreset_t داده = {param, خطأ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

فراغ
param_reset_all()
{
	paramiocresetgroup_t داده = {PARAM_RESET_ALL, اشاره_تهی, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بیعلامت دراز>(&داده));
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح رقم_excludes)
{
	paramiocresetgroup_t داده = {PARAM_RESET_EXCLUDES, excludes, رقم_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بیعلامت دراز>(&داده));
}

فراغ
param_reset_specific(ثابت حرف *resets[], صحيح رقم_resets)
{
	paramiocresetgroup_t داده = {PARAM_RESET_SPECIFIC, resets, رقم_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بیعلامت دراز>(&داده));
}

صحيح param_save_default(منطقي blocking)
{
	paramiocsavedefault_t داده = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

صحيح
param_بارگیری_default()
{
	paramiocloaddefault_t داده = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	paramiocexport_t داده = {filename, PX4_ERROR};

	إذا (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t داده = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<بیعلامت دراز>(&داده));
	أرجع داده.ret;
}
