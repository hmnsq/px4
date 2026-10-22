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
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "parameters_ioctl.h"
#അന്തര്ഭാവ <parameters/px4_parameters.hpp>
#അന്തര്ഭാവ <sys/boardctl.h>
#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <px4_platform_common/log.h>

#അന്തര്ഭാവ "parameters_common.cpp"

ശൂന്യം
param_notify_changes()
{
	boardctl(PARAMIOCNOTIFY, ശൂന്യ);
}

param_t param_find(സ്ഥിരം അക്ഷരം *name)
{
	paramiocfind_t data = {name, സത്യം, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

param_t param_find_no_notification(സ്ഥിരം അക്ഷരം *name)
{
	paramiocfind_t data = {name, അസത്യം, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

unsigned param_എണ്ണം_used()
{
	paramioccountused_t data = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

param_t param_for_used_index(unsigned index)
{
	paramiocforusedindex_t data = {index, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

പൂർണ്ണസംഖ്യ param_get_used_index(param_t param)
{
	paramiocgetusedindex_t data = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

തർക്കം
param_മൂല്യം_unsaved(param_t param)
{
	paramiocunsaved_t data = {param, അസത്യം};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

പൂർണ്ണസംഖ്യ
param_get(param_t param, ശൂന്യം *val)
{
	paramiocget_t data = {param, അസത്യം, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

പൂർണ്ണസംഖ്യ
param_get_default_മൂല്യം(param_t param, ശൂന്യം *default_val)
{
	paramiocget_t data = {param, സത്യം, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

ശൂന്യം
param_control_autosave(തർക്കം enable)
{
	paramiocautosave_t data = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<unsigned long>(&data));
}

പൂർണ്ണസംഖ്യ param_set(param_t param, സ്ഥിരം ശൂന്യം *val)
{
	paramiocset_t data = {param, സത്യം, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

പൂർണ്ണസംഖ്യ param_set_no_notification(param_t param, സ്ഥിരം ശൂന്യം *val)
{
	paramiocset_t data = {param, അസത്യം, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

തർക്കം param_used(param_t param)
{
	paramiocused_t data = {param, അസത്യം};
	boardctl(PARAMIOCUSED, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

ശൂന്യം param_set_used(param_t param)
{
	paramiocsetused_t data = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<unsigned long>(&data));
}

പൂർണ്ണസംഖ്യ param_set_default_മൂല്യം(param_t param, സ്ഥിരം ശൂന്യം *val)
{
	paramiocsetdefault_t data = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

പൂർണ്ണസംഖ്യ param_reset(param_t param)
{
	paramiocreset_t data = {param, സത്യം, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

പൂർണ്ണസംഖ്യ param_reset_no_notification(param_t param)
{
	paramiocreset_t data = {param, അസത്യം, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

ശൂന്യം
param_reset_all()
{
	paramiocresetgroup_t data = {PARAM_RESET_ALL, nullptr, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

ശൂന്യം
param_reset_excludes(സ്ഥിരം അക്ഷരം *excludes[], പൂർണ്ണസംഖ്യ അക്കം_excludes)
{
	paramiocresetgroup_t data = {PARAM_RESET_EXCLUDES, excludes, അക്കം_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

ശൂന്യം
param_reset_specific(സ്ഥിരം അക്ഷരം *resets[], പൂർണ്ണസംഖ്യ അക്കം_resets)
{
	paramiocresetgroup_t data = {PARAM_RESET_SPECIFIC, resets, അക്കം_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

പൂർണ്ണസംഖ്യ param_save_default(തർക്കം blocking)
{
	paramiocsavedefault_t data = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

പൂർണ്ണസംഖ്യ
param_load_default()
{
	paramiocloaddefault_t data = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

പൂർണ്ണസംഖ്യ
param_export(സ്ഥിരം അക്ഷരം *filename, param_filter_func filter)
{
	paramiocexport_t data = {filename, PX4_ERROR};

	എങ്കിൽ (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t data = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<unsigned long>(&data));
	തിരികെ data.ret;
}
