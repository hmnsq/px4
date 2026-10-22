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
 * @file parameters_ioctl.h
 *
 * User space - kernel space interface to global parameter store.
 */

#ಸೂಚನೆ once

#ವ್ಯಾಖ್ಯಾನಿಸು PARAM_IMPLEMENTATION
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ <px4_platform/board_ctrl.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

#ವ್ಯಾಖ್ಯಾನಿಸು _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCNOTIFY	_PARAMIOC(1)

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCFIND	_PARAMIOC(2)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocfind {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
	ಸ್ಥಿರಾಂಕ bool notification;
	param_t ret;
} paramiocfind_t;


#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCCOUNTUSED	_PARAMIOC(3)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramioccountused {
	ಚಿಹ್ನೆರಹಿತ ret;
} paramioccountused_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocforusedindex {
	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ;
	param_t ret;
} paramiocforusedindex_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocgetusedindex {
	ಸ್ಥಿರಾಂಕ param_t param;
	ಚಿಹ್ನೆರಹಿತ ret;
} paramiocgetusedindex_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCUNSAVED	_PARAMIOC(6)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocunsaved {
	ಸ್ಥಿರಾಂಕ param_t param;
	bool ret;
} paramiocunsaved_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCGET	_PARAMIOC(7)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocget {
	ಸ್ಥಿರಾಂಕ param_t param;
	ಸ್ಥಿರಾಂಕ bool deflt;
	ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ val;
	ಪೂರ್ಣಾಂಕ ret;
} paramiocget_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCAUTOSAVE	_PARAMIOC(8)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocautosave {
	ಸ್ಥಿರಾಂಕ bool enable;
} paramiocautosave_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCSET	_PARAMIOC(9)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocset {
	ಸ್ಥಿರಾಂಕ param_t param;
	ಸ್ಥಿರಾಂಕ bool notification;
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val;
	ಪೂರ್ಣಾಂಕ ret;
} paramiocset_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCUSED	_PARAMIOC(10)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocused {
	ಸ್ಥಿರಾಂಕ param_t param;
	bool ret;
} paramiocused_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCSETUSED	_PARAMIOC(11)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocsetused {
	ಸ್ಥಿರಾಂಕ param_t param;
} paramiocsetused_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCSETDEFAULT	_PARAMIOC(12)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocsetdefault {
	ಸ್ಥಿರಾಂಕ param_t param;
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val;
	ಪೂರ್ಣಾಂಕ ret;
} paramiocsetdefault_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCRESET	_PARAMIOC(13)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocreset {
	ಸ್ಥಿರಾಂಕ param_t param;
	ಸ್ಥಿರಾಂಕ bool notification;
	ಪೂರ್ಣಾಂಕ ret;
} paramiocreset_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCRESETGROUP	_PARAMIOC(14)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocresetgroup {
	param_reset_type_t type;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಗುಂಪು;
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಒಳ_ಗುಂಪು;
} paramiocresetgroup_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocsavedefault {
	bool blocking;
	ಪೂರ್ಣಾಂಕ ret;
} paramiocsavedefault_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCLOADDEFAULT	_PARAMIOC(16)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocloaddefault {
	ಪೂರ್ಣಾಂಕ ret;
} paramiocloaddefault_t;


#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCEXPORT	_PARAMIOC(17)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiocexport {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename;
	ಪೂರ್ಣಾಂಕ ret;
} paramiocexport_t;

#ವ್ಯಾಖ್ಯಾನಿಸು PARAMIOCHASH	_PARAMIOC(18)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ paramiochash {
	uint32_t ret;
} paramiochash_t;

ಪೂರ್ಣಾಂಕ param_ioctl(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cmd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg);
