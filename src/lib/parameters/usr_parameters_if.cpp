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
#अन्तर्भाव "param.h"
#अन्तर्भाव "parameters_ioctl.h"
#अन्तर्भाव <parameters/px4_parameters.hpp>
#अन्तर्भाव <sys/boardctl.h>
#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <px4_platform_common/log.h>

#अन्तर्भाव "parameters_common.cpp"

शून्य
param_notify_changes()
{
	boardctl(PARAMIOCNOTIFY, रिक्त);
}

param_t param_find(स्थिरांक अक्षर *name)
{
	paramiocfind_t data = {name, सत्य, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

param_t param_find_no_notification(स्थिरांक अक्षर *name)
{
	paramiocfind_t data = {name, असत्य, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

अचिन्हित param_मोजणी_used()
{
	paramioccountused_t data = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

param_t param_for_used_index(अचिन्हित index)
{
	paramiocforusedindex_t data = {index, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

पूर्णांक param_get_used_index(param_t param)
{
	paramiocgetusedindex_t data = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

तार्किक
param_मूल्य_unsaved(param_t param)
{
	paramiocunsaved_t data = {param, असत्य};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

पूर्णांक
param_get(param_t param, शून्य *val)
{
	paramiocget_t data = {param, असत्य, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

पूर्णांक
param_get_default_मूल्य(param_t param, शून्य *default_val)
{
	paramiocget_t data = {param, सत्य, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

शून्य
param_control_autosave(तार्किक enable)
{
	paramiocautosave_t data = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<अचिन्हित दीर्घ>(&data));
}

पूर्णांक param_set(param_t param, स्थिरांक शून्य *val)
{
	paramiocset_t data = {param, सत्य, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

पूर्णांक param_set_no_notification(param_t param, स्थिरांक शून्य *val)
{
	paramiocset_t data = {param, असत्य, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

तार्किक param_used(param_t param)
{
	paramiocused_t data = {param, असत्य};
	boardctl(PARAMIOCUSED, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

शून्य param_set_used(param_t param)
{
	paramiocsetused_t data = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<अचिन्हित दीर्घ>(&data));
}

पूर्णांक param_set_default_मूल्य(param_t param, स्थिरांक शून्य *val)
{
	paramiocsetdefault_t data = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

पूर्णांक param_reset(param_t param)
{
	paramiocreset_t data = {param, सत्य, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

पूर्णांक param_reset_no_notification(param_t param)
{
	paramiocreset_t data = {param, असत्य, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

शून्य
param_reset_all()
{
	paramiocresetgroup_t data = {PARAM_RESET_ALL, nullptr, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<अचिन्हित दीर्घ>(&data));
}

शून्य
param_reset_excludes(स्थिरांक अक्षर *excludes[], पूर्णांक अंक_excludes)
{
	paramiocresetgroup_t data = {PARAM_RESET_EXCLUDES, excludes, अंक_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<अचिन्हित दीर्घ>(&data));
}

शून्य
param_reset_specific(स्थिरांक अक्षर *resets[], पूर्णांक अंक_resets)
{
	paramiocresetgroup_t data = {PARAM_RESET_SPECIFIC, resets, अंक_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<अचिन्हित दीर्घ>(&data));
}

पूर्णांक param_save_default(तार्किक blocking)
{
	paramiocsavedefault_t data = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

पूर्णांक
param_load_default()
{
	paramiocloaddefault_t data = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

पूर्णांक
param_export(स्थिरांक अक्षर *filename, param_filter_func filter)
{
	paramiocexport_t data = {filename, PX4_ERROR};

	जर (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t data = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<अचिन्हित दीर्घ>(&data));
	परत data.ret;
}
