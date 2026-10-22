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
#ವ್ಯಾಖ್ಯಾನಿಸು PARAM_IMPLEMENTATION
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "parameters_ioctl.h"
#ಅನ್ತರ್ಭಾವ <parameters/px4_parameters.hpp>
#ಅನ್ತರ್ಭಾವ <sys/boardctl.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>

#ಅನ್ತರ್ಭಾವ "parameters_common.cpp"

ಶೂನ್ಯ
param_notify_changes()
{
	boardctl(PARAMIOCNOTIFY, ಖಾಲಿ);
}

param_t param_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	paramiocfind_t ದತ್ತಾಂಶ = {name, ನಿಜ, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

param_t param_ಕಂಡುಹಿಡಿ_no_notification(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	paramiocfind_t ದತ್ತಾಂಶ = {name, ಸುಳ್ಳು, PARAM_INVALID};
	boardctl(PARAMIOCFIND, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಚಿಹ್ನೆರಹಿತ param_ಎಣಿಕೆ_ಬಳಸಿದ()
{
	paramioccountused_t ದತ್ತಾಂಶ = {0};
	boardctl(PARAMIOCCOUNTUSED, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

param_t param_for_ಬಳಸಿದ_ಸೂಚ್ಯಂಕ(ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ)
{
	paramiocforusedindex_t ದತ್ತಾಂಶ = {ಸೂಚ್ಯಂಕ, 0};
	boardctl(PARAMIOCFORUSEDINDEX, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಪೂರ್ಣಾಂಕ param_get_ಬಳಸಿದ_ಸೂಚ್ಯಂಕ(param_t param)
{
	paramiocgetusedindex_t ದತ್ತಾಂಶ = {param, 0};
	boardctl(PARAMIOCGETUSEDINDEX, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ತಾರ್ಕಿಕ
param_ಮೌಲ್ಯ_unsaved(param_t param)
{
	paramiocunsaved_t ದತ್ತಾಂಶ = {param, ಸುಳ್ಳು};
	boardctl(PARAMIOCUNSAVED, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಪೂರ್ಣಾಂಕ
param_get(param_t param, ಶೂನ್ಯ *val)
{
	paramiocget_t ದತ್ತಾಂಶ = {param, ಸುಳ್ಳು, val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಪೂರ್ಣಾಂಕ
param_get_default_ಮೌಲ್ಯ(param_t param, ಶೂನ್ಯ *default_val)
{
	paramiocget_t ದತ್ತಾಂಶ = {param, ನಿಜ, default_val, PX4_ERROR};
	boardctl(PARAMIOCGET, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಶೂನ್ಯ
param_control_autosave(ತಾರ್ಕಿಕ enable)
{
	paramiocautosave_t ದತ್ತಾಂಶ = {enable};
	boardctl(PARAMIOCAUTOSAVE, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
}

ಪೂರ್ಣಾಂಕ param_ಗಣ(param_t param, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val)
{
	paramiocset_t ದತ್ತಾಂಶ = {param, ನಿಜ, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಪೂರ್ಣಾಂಕ param_ಗಣ_no_notification(param_t param, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val)
{
	paramiocset_t ದತ್ತಾಂಶ = {param, ಸುಳ್ಳು, val, PX4_ERROR};
	boardctl(PARAMIOCSET, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ತಾರ್ಕಿಕ param_ಬಳಸಿದ(param_t param)
{
	paramiocused_t ದತ್ತಾಂಶ = {param, ಸುಳ್ಳು};
	boardctl(PARAMIOCUSED, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಶೂನ್ಯ param_ಗಣ_ಬಳಸಿದ(param_t param)
{
	paramiocsetused_t ದತ್ತಾಂಶ = {param};
	boardctl(PARAMIOCSETUSED, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
}

ಪೂರ್ಣಾಂಕ param_ಗಣ_default_ಮೌಲ್ಯ(param_t param, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val)
{
	paramiocsetdefault_t ದತ್ತಾಂಶ = {param, val, PX4_ERROR};
	boardctl(PARAMIOCSETDEFAULT, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಪೂರ್ಣಾಂಕ param_reset(param_t param)
{
	paramiocreset_t ದತ್ತಾಂಶ = {param, ನಿಜ, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಪೂರ್ಣಾಂಕ param_reset_no_notification(param_t param)
{
	paramiocreset_t ದತ್ತಾಂಶ = {param, ಸುಳ್ಳು, PX4_ERROR};
	boardctl(PARAMIOCRESET, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಶೂನ್ಯ
param_reset_all()
{
	paramiocresetgroup_t ದತ್ತಾಂಶ = {PARAM_RESET_ALL, ಶೂನ್ಯಸೂಚಕ, 0};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
}

ಶೂನ್ಯ
param_reset_excludes(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *excludes[], ಪೂರ್ಣಾಂಕ ಅಂಕೆ_excludes)
{
	paramiocresetgroup_t ದತ್ತಾಂಶ = {PARAM_RESET_EXCLUDES, excludes, ಅಂಕೆ_excludes};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
}

ಶೂನ್ಯ
param_reset_specific(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *resets[], ಪೂರ್ಣಾಂಕ ಅಂಕೆ_resets)
{
	paramiocresetgroup_t ದತ್ತಾಂಶ = {PARAM_RESET_SPECIFIC, resets, ಅಂಕೆ_resets};
	boardctl(PARAMIOCRESETGROUP, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
}

ಪೂರ್ಣಾಂಕ param_save_default(ತಾರ್ಕಿಕ blocking)
{
	paramiocsavedefault_t ದತ್ತಾಂಶ = {blocking, PX4_ERROR};
	boardctl(PARAMIOCSAVEDEFAULT, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಪೂರ್ಣಾಂಕ
param_ತುಂಬು_default()
{
	paramiocloaddefault_t ದತ್ತಾಂಶ = {PX4_ERROR};
	boardctl(PARAMIOCLOADDEFAULT, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

ಪೂರ್ಣಾಂಕ
param_export(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, param_filter_func filter)
{
	paramiocexport_t ದತ್ತಾಂಶ = {filename, PX4_ERROR};

	ಆದರೆ (filter) { PX4_ERR("ERROR: filter not supported in userside blob"); }

	boardctl(PARAMIOCEXPORT, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}

uint32_t param_hash_check()
{
	paramiochash_t ದತ್ತಾಂಶ = {0};
	boardctl(PARAMIOCHASH, reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ>(&ದತ್ತಾಂಶ));
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ret;
}
