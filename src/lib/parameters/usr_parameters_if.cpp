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
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "parameters_ioctl.h"
#અન્તર્ભાવ <parameters/px4_parameters.hpp>
#અન્તર્ભાવ <sys/boardctl.h>
#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <px4_platform_common/log.h>

#અન્તર્ભાવ "parameters_common.cpp"

શૂન્ય
param_notify_changes()
{
	boardctl(PARAMIOCNOTIFY, ખાલી);
}

param_t param_find(અચલ અક્ષર *name)
{
	paramiocfind_t data = {name, સાચું, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

param_t param_find_no_notification(અચલ અક્ષર *name)
{
	paramiocfind_t data = {name, ખોટું, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

unsigned param_ગણતરી_used()
{
	paramioccountused_t data = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

param_t param_for_used_index(unsigned index)
{
	paramiocforusedindex_t data = {index, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

પૂર્ણાંક param_get_used_index(param_t param)
{
	paramiocgetusedindex_t data = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

તાર્કિક
param_કિંમત_unsaved(param_t param)
{
	paramiocunsaved_t data = {param, ખોટું};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

પૂર્ણાંક
param_get(param_t param, શૂન્ય *val)
{
	paramiocget_t data = {param, ખોટું, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

પૂર્ણાંક
param_get_default_કિંમત(param_t param, શૂન્ય *default_val)
{
	paramiocget_t data = {param, સાચું, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

શૂન્ય
param_control_autosave(તાર્કિક enable)
{
	paramiocautosave_t data = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<unsigned long>(&data));
}

પૂર્ણાંક param_set(param_t param, અચલ શૂન્ય *val)
{
	paramiocset_t data = {param, સાચું, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

પૂર્ણાંક param_set_no_notification(param_t param, અચલ શૂન્ય *val)
{
	paramiocset_t data = {param, ખોટું, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

તાર્કિક param_used(param_t param)
{
	paramiocused_t data = {param, ખોટું};
	boardctl(PARAMIOCUSED, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

શૂન્ય param_set_used(param_t param)
{
	paramiocsetused_t data = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<unsigned long>(&data));
}

પૂર્ણાંક param_set_default_કિંમત(param_t param, અચલ શૂન્ય *val)
{
	paramiocsetdefault_t data = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

પૂર્ણાંક param_reset(param_t param)
{
	paramiocreset_t data = {param, સાચું, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

પૂર્ણાંક param_reset_no_notification(param_t param)
{
	paramiocreset_t data = {param, ખોટું, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

શૂન્ય
param_reset_all()
{
	paramiocresetgroup_t data = {PARAM_RESET_ALL, nullptr, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

શૂન્ય
param_reset_excludes(અચલ અક્ષર *excludes[], પૂર્ણાંક અંક_excludes)
{
	paramiocresetgroup_t data = {PARAM_RESET_EXCLUDES, excludes, અંક_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

શૂન્ય
param_reset_specific(અચલ અક્ષર *resets[], પૂર્ણાંક અંક_resets)
{
	paramiocresetgroup_t data = {PARAM_RESET_SPECIFIC, resets, અંક_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<unsigned long>(&data));
}

પૂર્ણાંક param_save_default(તાર્કિક blocking)
{
	paramiocsavedefault_t data = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

પૂર્ણાંક
param_load_default()
{
	paramiocloaddefault_t data = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

પૂર્ણાંક
param_export(અચલ અક્ષર *filename, param_filter_func filter)
{
	paramiocexport_t data = {filename, PX4_ERROR};

	જો (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t data = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<unsigned long>(&data));
	પાછા data.ret;
}
