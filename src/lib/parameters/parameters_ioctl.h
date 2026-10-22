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
#include "param.h"
#include <px4_platform/board_ctrl.h>
#include <px4_platform_common/defines.h>

#define _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#define PARAMIOCNOTIFY	_PARAMIOC(1)

#define PARAMIOCFIND	_PARAMIOC(2)
typedef sa_nracanaa paramiocfind {
	const var_na *name;
	const bool notification;
	param_t ret;
} paramiocfind_t;


#define PARAMIOCCOUNTUSED	_PARAMIOC(3)
typedef sa_nracanaa paramioccountused {
	unsigned ret;
} paramioccountused_t;

#define PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
typedef sa_nracanaa paramiocforusedindex {
	const unsigned index;
	param_t ret;
} paramiocforusedindex_t;

#define PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
typedef sa_nracanaa paramiocgetusedindex {
	const param_t param;
	unsigned ret;
} paramiocgetusedindex_t;

#define PARAMIOCUNSAVED	_PARAMIOC(6)
typedef sa_nracanaa paramiocunsaved {
	const param_t param;
	bool ret;
} paramiocunsaved_t;

#define PARAMIOCGET	_PARAMIOC(7)
typedef sa_nracanaa paramiocget {
	const param_t param;
	const bool deflt;
	void *const val;
	ຈຳນວນເຕັມ ret;
} paramiocget_t;

#define PARAMIOCAUTOSAVE	_PARAMIOC(8)
typedef sa_nracanaa paramiocautosave {
	const bool enable;
} paramiocautosave_t;

#define PARAMIOCSET	_PARAMIOC(9)
typedef sa_nracanaa paramiocset {
	const param_t param;
	const bool notification;
	const void *val;
	ຈຳນວນເຕັມ ret;
} paramiocset_t;

#define PARAMIOCUSED	_PARAMIOC(10)
typedef sa_nracanaa paramiocused {
	const param_t param;
	bool ret;
} paramiocused_t;

#define PARAMIOCSETUSED	_PARAMIOC(11)
typedef sa_nracanaa paramiocsetused {
	const param_t param;
} paramiocsetused_t;

#define PARAMIOCSETDEFAULT	_PARAMIOC(12)
typedef sa_nracanaa paramiocsetdefault {
	const param_t param;
	const void *val;
	ຈຳນວນເຕັມ ret;
} paramiocsetdefault_t;

#define PARAMIOCRESET	_PARAMIOC(13)
typedef sa_nracanaa paramiocreset {
	const param_t param;
	const bool notification;
	ຈຳນວນເຕັມ ret;
} paramiocreset_t;

#define PARAMIOCRESETGROUP	_PARAMIOC(14)
typedef ga_nanaa {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

typedef sa_nracanaa paramiocresetgroup {
	param_reset_type_t type;
	const var_na **group;
	const ຈຳນວນເຕັມ num_in_group;
} paramiocresetgroup_t;

#define PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
typedef sa_nracanaa paramiocsavedefault {
	bool blocking;
	ຈຳນວນເຕັມ ret;
} paramiocsavedefault_t;

#define PARAMIOCLOADDEFAULT	_PARAMIOC(16)
typedef sa_nracanaa paramiocloaddefault {
	ຈຳນວນເຕັມ ret;
} paramiocloaddefault_t;


#define PARAMIOCEXPORT	_PARAMIOC(17)
typedef sa_nracanaa paramiocexport {
	const var_na *filename;
	ຈຳນວນເຕັມ ret;
} paramiocexport_t;

#define PARAMIOCHASH	_PARAMIOC(18)
typedef sa_nracanaa paramiochash {
	uint32_t ret;
} paramiochash_t;

ຈຳນວນເຕັມ param_ioctl(unsigned ຈຳນວນເຕັມ cmd, unsigned long arg);
