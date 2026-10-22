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

#pragma once

#define PARAM_IMPLEMENTATION
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ <px4_platform/board_ctrl.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

#define _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#define PARAMIOCNOTIFY	_PARAMIOC(1)

#define PARAMIOCFIND	_PARAMIOC(2)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocfind {
	ಧ್ರುವ ವರ್ಣ *name;
	ಧ್ರುವ bool notification;
	param_t ret;
} paramiocfind_t;


#define PARAMIOCCOUNTUSED	_PARAMIOC(3)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramioccountused {
	unsigned ret;
} paramioccountused_t;

#define PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocforusedindex {
	ಧ್ರುವ unsigned index;
	param_t ret;
} paramiocforusedindex_t;

#define PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocgetusedindex {
	ಧ್ರುವ param_t param;
	unsigned ret;
} paramiocgetusedindex_t;

#define PARAMIOCUNSAVED	_PARAMIOC(6)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocunsaved {
	ಧ್ರುವ param_t param;
	bool ret;
} paramiocunsaved_t;

#define PARAMIOCGET	_PARAMIOC(7)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocget {
	ಧ್ರುವ param_t param;
	ಧ್ರುವ bool deflt;
	ಶೂನ್ಯ *ಧ್ರುವ val;
	ಪೂರ್ಣಾಙ್ಕ ret;
} paramiocget_t;

#define PARAMIOCAUTOSAVE	_PARAMIOC(8)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocautosave {
	ಧ್ರುವ bool enable;
} paramiocautosave_t;

#define PARAMIOCSET	_PARAMIOC(9)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocset {
	ಧ್ರುವ param_t param;
	ಧ್ರುವ bool notification;
	ಧ್ರುವ ಶೂನ್ಯ *val;
	ಪೂರ್ಣಾಙ್ಕ ret;
} paramiocset_t;

#define PARAMIOCUSED	_PARAMIOC(10)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocused {
	ಧ್ರುವ param_t param;
	bool ret;
} paramiocused_t;

#define PARAMIOCSETUSED	_PARAMIOC(11)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocsetused {
	ಧ್ರುವ param_t param;
} paramiocsetused_t;

#define PARAMIOCSETDEFAULT	_PARAMIOC(12)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocsetdefault {
	ಧ್ರುವ param_t param;
	ಧ್ರುವ ಶೂನ್ಯ *val;
	ಪೂರ್ಣಾಙ್ಕ ret;
} paramiocsetdefault_t;

#define PARAMIOCRESET	_PARAMIOC(13)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocreset {
	ಧ್ರುವ param_t param;
	ಧ್ರುವ bool notification;
	ಪೂರ್ಣಾಙ್ಕ ret;
} paramiocreset_t;

#define PARAMIOCRESETGROUP	_PARAMIOC(14)
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocresetgroup {
	param_reset_type_t type;
	ಧ್ರುವ ವರ್ಣ **group;
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_in_group;
} paramiocresetgroup_t;

#define PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocsavedefault {
	bool blocking;
	ಪೂರ್ಣಾಙ್ಕ ret;
} paramiocsavedefault_t;

#define PARAMIOCLOADDEFAULT	_PARAMIOC(16)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocloaddefault {
	ಪೂರ್ಣಾಙ್ಕ ret;
} paramiocloaddefault_t;


#define PARAMIOCEXPORT	_PARAMIOC(17)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiocexport {
	ಧ್ರುವ ವರ್ಣ *filename;
	ಪೂರ್ಣಾಙ್ಕ ret;
} paramiocexport_t;

#define PARAMIOCHASH	_PARAMIOC(18)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ paramiochash {
	uint32_t ret;
} paramiochash_t;

ಪೂರ್ಣಾಙ್ಕ param_ioctl(unsigned ಪೂರ್ಣಾಙ್ಕ cmd, unsigned long arg);
