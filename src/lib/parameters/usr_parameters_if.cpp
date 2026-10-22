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

param_t param_لبھو(ثابت حرف *name)
{
	paramiocfind_t انکڑے = {name, سچ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

param_t param_لبھو_no_notification(ثابت حرف *name)
{
	paramiocfind_t انکڑے = {name, خطأ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

بنانشانی param_گنتی_ورتیا()
{
	paramioccountused_t انکڑے = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

param_t param_for_ورتیا_اشاریہ(بنانشانی اشاریہ)
{
	paramiocforusedindex_t انکڑے = {اشاریہ, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

صحيح param_get_ورتیا_اشاریہ(param_t param)
{
	paramiocgetusedindex_t انکڑے = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

منطقي
param_قدر_unsaved(param_t param)
{
	paramiocunsaved_t انکڑے = {param, خطأ};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

صحيح
param_get(param_t param, فراغ *val)
{
	paramiocget_t انکڑے = {param, خطأ, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

صحيح
param_get_default_قدر(param_t param, فراغ *default_val)
{
	paramiocget_t انکڑے = {param, سچ, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

فراغ
param_control_autosave(منطقي enable)
{
	paramiocautosave_t انکڑے = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<بنانشانی لما>(&انکڑے));
}

صحيح param_سیٹ(param_t param, ثابت فراغ *val)
{
	paramiocset_t انکڑے = {param, سچ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

صحيح param_سیٹ_no_notification(param_t param, ثابت فراغ *val)
{
	paramiocset_t انکڑے = {param, خطأ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

منطقي param_ورتیا(param_t param)
{
	paramiocused_t انکڑے = {param, خطأ};
	boardctl(PARAMIOCUSED, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

فراغ param_سیٹ_ورتیا(param_t param)
{
	paramiocsetused_t انکڑے = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<بنانشانی لما>(&انکڑے));
}

صحيح param_سیٹ_default_قدر(param_t param, ثابت فراغ *val)
{
	paramiocsetdefault_t انکڑے = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

صحيح param_reset(param_t param)
{
	paramiocreset_t انکڑے = {param, سچ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

صحيح param_reset_no_notification(param_t param)
{
	paramiocreset_t انکڑے = {param, خطأ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

فراغ
param_reset_all()
{
	paramiocresetgroup_t انکڑے = {PARAM_RESET_ALL, خالی_سوچک, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بنانشانی لما>(&انکڑے));
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح عدد_excludes)
{
	paramiocresetgroup_t انکڑے = {PARAM_RESET_EXCLUDES, excludes, عدد_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بنانشانی لما>(&انکڑے));
}

فراغ
param_reset_specific(ثابت حرف *resets[], صحيح عدد_resets)
{
	paramiocresetgroup_t انکڑے = {PARAM_RESET_SPECIFIC, resets, عدد_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<بنانشانی لما>(&انکڑے));
}

صحيح param_save_default(منطقي blocking)
{
	paramiocsavedefault_t انکڑے = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

صحيح
param_لدو_default()
{
	paramiocloaddefault_t انکڑے = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	paramiocexport_t انکڑے = {filename, PX4_ERROR};

	إذا (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t انکڑے = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<بنانشانی لما>(&انکڑے));
	أرجع انکڑے.ret;
}
