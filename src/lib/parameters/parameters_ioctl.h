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
#شامل "param.h"
#شامل <px4_platform/board_ctrl.h>
#شامل <px4_platform_common/defines.h>

#define _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#define PARAMIOCNOTIFY	_PARAMIOC(1)

#define PARAMIOCFIND	_PARAMIOC(2)
تعریفنوع ساختار paramiocfind {
	ثابت نویسه *name;
	ثابت bool notification;
	param_t ret;
} paramiocfind_t;


#define PARAMIOCCOUNTUSED	_PARAMIOC(3)
تعریفنوع ساختار paramioccountused {
	unsigned ret;
} paramioccountused_t;

#define PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
تعریفنوع ساختار paramiocforusedindex {
	ثابت unsigned index;
	param_t ret;
} paramiocforusedindex_t;

#define PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
تعریفنوع ساختار paramiocgetusedindex {
	ثابت param_t param;
	unsigned ret;
} paramiocgetusedindex_t;

#define PARAMIOCUNSAVED	_PARAMIOC(6)
تعریفنوع ساختار paramiocunsaved {
	ثابت param_t param;
	bool ret;
} paramiocunsaved_t;

#define PARAMIOCGET	_PARAMIOC(7)
تعریفنوع ساختار paramiocget {
	ثابت param_t param;
	ثابت bool deflt;
	تهی *ثابت val;
	صحیح ret;
} paramiocget_t;

#define PARAMIOCAUTOSAVE	_PARAMIOC(8)
تعریفنوع ساختار paramiocautosave {
	ثابت bool enable;
} paramiocautosave_t;

#define PARAMIOCSET	_PARAMIOC(9)
تعریفنوع ساختار paramiocset {
	ثابت param_t param;
	ثابت bool notification;
	ثابت تهی *val;
	صحیح ret;
} paramiocset_t;

#define PARAMIOCUSED	_PARAMIOC(10)
تعریفنوع ساختار paramiocused {
	ثابت param_t param;
	bool ret;
} paramiocused_t;

#define PARAMIOCSETUSED	_PARAMIOC(11)
تعریفنوع ساختار paramiocsetused {
	ثابت param_t param;
} paramiocsetused_t;

#define PARAMIOCSETDEFAULT	_PARAMIOC(12)
تعریفنوع ساختار paramiocsetdefault {
	ثابت param_t param;
	ثابت تهی *val;
	صحیح ret;
} paramiocsetdefault_t;

#define PARAMIOCRESET	_PARAMIOC(13)
تعریفنوع ساختار paramiocreset {
	ثابت param_t param;
	ثابت bool notification;
	صحیح ret;
} paramiocreset_t;

#define PARAMIOCRESETGROUP	_PARAMIOC(14)
تعریفنوع شمارش {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

تعریفنوع ساختار paramiocresetgroup {
	param_reset_type_t type;
	ثابت نویسه **group;
	ثابت صحیح رقم_in_group;
} paramiocresetgroup_t;

#define PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
تعریفنوع ساختار paramiocsavedefault {
	bool blocking;
	صحیح ret;
} paramiocsavedefault_t;

#define PARAMIOCLOADDEFAULT	_PARAMIOC(16)
تعریفنوع ساختار paramiocloaddefault {
	صحیح ret;
} paramiocloaddefault_t;


#define PARAMIOCEXPORT	_PARAMIOC(17)
تعریفنوع ساختار paramiocexport {
	ثابت نویسه *filename;
	صحیح ret;
} paramiocexport_t;

#define PARAMIOCHASH	_PARAMIOC(18)
تعریفنوع ساختار paramiochash {
	uint32_t ret;
} paramiochash_t;

صحیح param_ioctl(unsigned صحیح cmd, unsigned long arg);
