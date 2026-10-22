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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAM_IMPLEMENTATION
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parameters_ioctl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/px4_parameters.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/boardctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "parameters_common.cpp"

ਖਾਲੀ
param_notify_changes()
{
	boardctl(PARAMIOCNOTIFY, ਨਿਰਾ);
}

param_t param_ਲੱਭੋ(ਅਚਲ ਅੱਖਰ *name)
{
	paramiocfind_t ਅੰਕੜੇ = {name, ਸੱਚ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

param_t param_ਲੱਭੋ_no_notification(ਅਚਲ ਅੱਖਰ *name)
{
	paramiocfind_t ਅੰਕੜੇ = {name, ਝੂਠ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ param_ਗਿਣਤੀ_ਵਰਤਿਆ()
{
	paramioccountused_t ਅੰਕੜੇ = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

param_t param_for_ਵਰਤਿਆ_ਸੂਚਕਾਂਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ)
{
	paramiocforusedindex_t ਅੰਕੜੇ = {ਸੂਚਕਾਂਕ, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਪੂਰਨ param_get_ਵਰਤਿਆ_ਸੂਚਕਾਂਕ(param_t param)
{
	paramiocgetusedindex_t ਅੰਕੜੇ = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਤਾਰਕਿਕ
param_ਮੁੱਲ_unsaved(param_t param)
{
	paramiocunsaved_t ਅੰਕੜੇ = {param, ਝੂਠ};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਪੂਰਨ
param_get(param_t param, ਖਾਲੀ *val)
{
	paramiocget_t ਅੰਕੜੇ = {param, ਝੂਠ, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਪੂਰਨ
param_get_default_ਮੁੱਲ(param_t param, ਖਾਲੀ *default_val)
{
	paramiocget_t ਅੰਕੜੇ = {param, ਸੱਚ, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਖਾਲੀ
param_control_autosave(ਤਾਰਕਿਕ enable)
{
	paramiocautosave_t ਅੰਕੜੇ = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
}

ਪੂਰਨ param_ਸੈੱਟ(param_t param, ਅਚਲ ਖਾਲੀ *val)
{
	paramiocset_t ਅੰਕੜੇ = {param, ਸੱਚ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਪੂਰਨ param_ਸੈੱਟ_no_notification(param_t param, ਅਚਲ ਖਾਲੀ *val)
{
	paramiocset_t ਅੰਕੜੇ = {param, ਝੂਠ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਤਾਰਕਿਕ param_ਵਰਤਿਆ(param_t param)
{
	paramiocused_t ਅੰਕੜੇ = {param, ਝੂਠ};
	boardctl(PARAMIOCUSED, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਖਾਲੀ param_ਸੈੱਟ_ਵਰਤਿਆ(param_t param)
{
	paramiocsetused_t ਅੰਕੜੇ = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
}

ਪੂਰਨ param_ਸੈੱਟ_default_ਮੁੱਲ(param_t param, ਅਚਲ ਖਾਲੀ *val)
{
	paramiocsetdefault_t ਅੰਕੜੇ = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਪੂਰਨ param_reset(param_t param)
{
	paramiocreset_t ਅੰਕੜੇ = {param, ਸੱਚ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਪੂਰਨ param_reset_no_notification(param_t param)
{
	paramiocreset_t ਅੰਕੜੇ = {param, ਝੂਠ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਖਾਲੀ
param_reset_all()
{
	paramiocresetgroup_t ਅੰਕੜੇ = {PARAM_RESET_ALL, ਖਾਲੀ_ਸੂਚਕ, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
}

ਖਾਲੀ
param_reset_excludes(ਅਚਲ ਅੱਖਰ *excludes[], ਪੂਰਨ ਅੰਕ_excludes)
{
	paramiocresetgroup_t ਅੰਕੜੇ = {PARAM_RESET_EXCLUDES, excludes, ਅੰਕ_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
}

ਖਾਲੀ
param_reset_specific(ਅਚਲ ਅੱਖਰ *resets[], ਪੂਰਨ ਅੰਕ_resets)
{
	paramiocresetgroup_t ਅੰਕੜੇ = {PARAM_RESET_SPECIFIC, resets, ਅੰਕ_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
}

ਪੂਰਨ param_save_default(ਤਾਰਕਿਕ blocking)
{
	paramiocsavedefault_t ਅੰਕੜੇ = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਪੂਰਨ
param_ਲੱਦੋ_default()
{
	paramiocloaddefault_t ਅੰਕੜੇ = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

ਪੂਰਨ
param_export(ਅਚਲ ਅੱਖਰ *filename, param_filter_func filter)
{
	paramiocexport_t ਅੰਕੜੇ = {filename, PX4_ERROR};

	ਜੇ (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t ਅੰਕੜੇ = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ>(&ਅੰਕੜੇ));
	ਵਾਪਸ ਅੰਕੜੇ.ret;
}
