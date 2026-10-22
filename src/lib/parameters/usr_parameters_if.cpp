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
#وصف_ڪريو PARAM_IMPLEMENTATION
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

param_t param_ڳوليو(ثابت حرف *name)
{
	paramiocfind_t معلومات = {name, سچ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

param_t param_ڳوليو_no_notification(ثابت حرف *name)
{
	paramiocfind_t معلومات = {name, خطأ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

بنانشان param_ڳڻپ_استعمال_ٿيل()
{
	paramioccountused_t معلومات = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

param_t param_for_استعمال_ٿيل_اشاريو(بنانشان اشاريو)
{
	paramiocforusedindex_t معلومات = {اشاريو, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

صحيح param_get_استعمال_ٿيل_اشاريو(param_t param)
{
	paramiocgetusedindex_t معلومات = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

منطقي
param_قدر_unsaved(param_t param)
{
	paramiocunsaved_t معلومات = {param, خطأ};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

صحيح
param_get(param_t param, فراغ *val)
{
	paramiocget_t معلومات = {param, خطأ, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

صحيح
param_get_default_قدر(param_t param, فراغ *default_val)
{
	paramiocget_t معلومات = {param, سچ, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

فراغ
param_control_autosave(منطقي enable)
{
	paramiocautosave_t معلومات = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
}

صحيح param_سيٽ(param_t param, ثابت فراغ *val)
{
	paramiocset_t معلومات = {param, سچ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

صحيح param_سيٽ_no_notification(param_t param, ثابت فراغ *val)
{
	paramiocset_t معلومات = {param, خطأ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

منطقي param_استعمال_ٿيل(param_t param)
{
	paramiocused_t معلومات = {param, خطأ};
	boardctl(PARAMIOCUSED, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

فراغ param_سيٽ_استعمال_ٿيل(param_t param)
{
	paramiocsetused_t معلومات = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
}

صحيح param_سيٽ_default_قدر(param_t param, ثابت فراغ *val)
{
	paramiocsetdefault_t معلومات = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

صحيح param_reset(param_t param)
{
	paramiocreset_t معلومات = {param, سچ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

صحيح param_reset_no_notification(param_t param)
{
	paramiocreset_t معلومات = {param, خطأ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

فراغ
param_reset_all()
{
	paramiocresetgroup_t معلومات = {PARAM_RESET_ALL, خالي_اشارو, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح انگ_excludes)
{
	paramiocresetgroup_t معلومات = {PARAM_RESET_EXCLUDES, excludes, انگ_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
}

فراغ
param_reset_specific(ثابت حرف *resets[], صحيح انگ_resets)
{
	paramiocresetgroup_t معلومات = {PARAM_RESET_SPECIFIC, resets, انگ_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
}

صحيح param_save_default(منطقي blocking)
{
	paramiocsavedefault_t معلومات = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

صحيح
param_ڀريو_default()
{
	paramiocloaddefault_t معلومات = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	paramiocexport_t معلومات = {filename, PX4_ERROR};

	إذا (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t معلومات = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<بنانشان ڊگهو>(&معلومات));
	أرجع معلومات.ret;
}
