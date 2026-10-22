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
#నిర్వచించు PARAM_IMPLEMENTATION
#అన్తర్భావ "param.h"
#అన్తర్భావ "parameters_ioctl.h"
#అన్తర్భావ <parameters/px4_parameters.hpp>
#అన్తర్భావ <sys/boardctl.h>
#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/log.h>

#అన్తర్భావ "parameters_common.cpp"

శూన్య
param_notify_changes()
{
	boardctl(PARAMIOCNOTIFY, ఖాళీ);
}

param_t param_కనుగొను(స్థిరరాశి అక్షరం *name)
{
	paramiocfind_t దత్తాంశం = {name, నిజం, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

param_t param_కనుగొను_no_notification(స్థిరరాశి అక్షరం *name)
{
	paramiocfind_t దత్తాంశం = {name, అబద్ధం, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

గుర్తులేని param_లెక్క_వాడిన()
{
	paramioccountused_t దత్తాంశం = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

param_t param_for_వాడిన_సూచీ(గుర్తులేని సూచీ)
{
	paramiocforusedindex_t దత్తాంశం = {సూచీ, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

పూర్ణాంకం param_get_వాడిన_సూచీ(param_t param)
{
	paramiocgetusedindex_t దత్తాంశం = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

తార్కిక
param_విలువ_unsaved(param_t param)
{
	paramiocunsaved_t దత్తాంశం = {param, అబద్ధం};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

పూర్ణాంకం
param_get(param_t param, శూన్య *val)
{
	paramiocget_t దత్తాంశం = {param, అబద్ధం, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

పూర్ణాంకం
param_get_default_విలువ(param_t param, శూన్య *default_val)
{
	paramiocget_t దత్తాంశం = {param, నిజం, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

శూన్య
param_control_autosave(తార్కిక enable)
{
	paramiocautosave_t దత్తాంశం = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
}

పూర్ణాంకం param_సమితి(param_t param, స్థిరరాశి శూన్య *val)
{
	paramiocset_t దత్తాంశం = {param, నిజం, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

పూర్ణాంకం param_సమితి_no_notification(param_t param, స్థిరరాశి శూన్య *val)
{
	paramiocset_t దత్తాంశం = {param, అబద్ధం, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

తార్కిక param_వాడిన(param_t param)
{
	paramiocused_t దత్తాంశం = {param, అబద్ధం};
	boardctl(PARAMIOCUSED, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

శూన్య param_సమితి_వాడిన(param_t param)
{
	paramiocsetused_t దత్తాంశం = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
}

పూర్ణాంకం param_సమితి_default_విలువ(param_t param, స్థిరరాశి శూన్య *val)
{
	paramiocsetdefault_t దత్తాంశం = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

పూర్ణాంకం param_reset(param_t param)
{
	paramiocreset_t దత్తాంశం = {param, నిజం, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

పూర్ణాంకం param_reset_no_notification(param_t param)
{
	paramiocreset_t దత్తాంశం = {param, అబద్ధం, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

శూన్య
param_reset_all()
{
	paramiocresetgroup_t దత్తాంశం = {PARAM_RESET_ALL, శూన్యసూచిక, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
}

శూన్య
param_reset_excludes(స్థిరరాశి అక్షరం *excludes[], పూర్ణాంకం అంకె_excludes)
{
	paramiocresetgroup_t దత్తాంశం = {PARAM_RESET_EXCLUDES, excludes, అంకె_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
}

శూన్య
param_reset_specific(స్థిరరాశి అక్షరం *resets[], పూర్ణాంకం అంకె_resets)
{
	paramiocresetgroup_t దత్తాంశం = {PARAM_RESET_SPECIFIC, resets, అంకె_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
}

పూర్ణాంకం param_save_default(తార్కిక blocking)
{
	paramiocsavedefault_t దత్తాంశం = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

పూర్ణాంకం
param_ఎక్కించు_default()
{
	paramiocloaddefault_t దత్తాంశం = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

పూర్ణాంకం
param_export(స్థిరరాశి అక్షరం *filename, param_filter_func filter)
{
	paramiocexport_t దత్తాంశం = {filename, PX4_ERROR};

	ఒకవేళ (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t దత్తాంశం = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<గుర్తులేని దీర్ఘ>(&దత్తాంశం));
	తిరిగి దత్తాంశం.ret;
}
