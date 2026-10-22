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
#define PARAM_IMPLEMENTATION
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

param_t param_find(ثابت حرف *name)
{
	paramiocfind_t data = {name, true, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

param_t param_find_no_notification(ثابت حرف *name)
{
	paramiocfind_t data = {name, خطأ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

unsigned param_كدي_used()
{
	paramioccountused_t data = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

param_t param_for_used_index(unsigned index)
{
	paramiocforusedindex_t data = {index, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

صحيح param_get_used_index(param_t param)
{
	paramiocgetusedindex_t data = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

منطقي
param_درج_unsaved(param_t param)
{
	paramiocunsaved_t data = {param, خطأ};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

صحيح
param_get(param_t param, فراغ *val)
{
	paramiocget_t data = {param, خطأ, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

صحيح
param_get_default_درج(param_t param, فراغ *default_val)
{
	paramiocget_t data = {param, true, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

فراغ
param_control_autosave(منطقي enable)
{
	paramiocautosave_t data = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<unsigned long>(&data));
}

صحيح param_set(param_t param, ثابت فراغ *val)
{
	paramiocset_t data = {param, true, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

صحيح param_set_no_notification(param_t param, ثابت فراغ *val)
{
	paramiocset_t data = {param, خطأ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

منطقي param_used(param_t param)
{
	paramiocused_t data = {param, خطأ};
	boardctl(PARAMIOCUSED, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

فراغ param_set_used(param_t param)
{
	paramiocsetused_t data = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<unsigned long>(&data));
}

صحيح param_set_default_درج(param_t param, ثابت فراغ *val)
{
	paramiocsetdefault_t data = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

صحيح param_reset(param_t param)
{
	paramiocreset_t data = {param, true, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

صحيح param_reset_no_notification(param_t param)
{
	paramiocreset_t data = {param, خطأ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

فراغ
param_reset_all()
{
	paramiocresetgroup_t data = {PARAM_RESET_ALL, nullptr, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح لمب_excludes)
{
	paramiocresetgroup_t data = {PARAM_RESET_EXCLUDES, excludes, لمب_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

فراغ
param_reset_specific(ثابت حرف *resets[], صحيح لمب_resets)
{
	paramiocresetgroup_t data = {PARAM_RESET_SPECIFIC, resets, لمب_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

صحيح param_save_default(منطقي blocking)
{
	paramiocsavedefault_t data = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

صحيح
param_load_default()
{
	paramiocloaddefault_t data = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	paramiocexport_t data = {filename, PX4_ERROR};

	إذا (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t data = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<unsigned long>(&data));
	أرجع data.ret;
}
