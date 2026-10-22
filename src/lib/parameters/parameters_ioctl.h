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
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ <px4_platform/board_ctrl.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>

#define _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#define PARAMIOCNOTIFY	_PARAMIOC(1)

#define PARAMIOCFIND	_PARAMIOC(2)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocfind {
	ଅଚଳ ଅକ୍ଷର *name;
	ଅଚଳ bool notification;
	param_t ret;
} paramiocfind_t;


#define PARAMIOCCOUNTUSED	_PARAMIOC(3)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramioccountused {
	unsigned ret;
} paramioccountused_t;

#define PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocforusedindex {
	ଅଚଳ unsigned index;
	param_t ret;
} paramiocforusedindex_t;

#define PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocgetusedindex {
	ଅଚଳ param_t param;
	unsigned ret;
} paramiocgetusedindex_t;

#define PARAMIOCUNSAVED	_PARAMIOC(6)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocunsaved {
	ଅଚଳ param_t param;
	bool ret;
} paramiocunsaved_t;

#define PARAMIOCGET	_PARAMIOC(7)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocget {
	ଅଚଳ param_t param;
	ଅଚଳ bool deflt;
	ଶୂନ୍ୟ *ଅଚଳ val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
} paramiocget_t;

#define PARAMIOCAUTOSAVE	_PARAMIOC(8)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocautosave {
	ଅଚଳ bool enable;
} paramiocautosave_t;

#define PARAMIOCSET	_PARAMIOC(9)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocset {
	ଅଚଳ param_t param;
	ଅଚଳ bool notification;
	ଅଚଳ ଶୂନ୍ୟ *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
} paramiocset_t;

#define PARAMIOCUSED	_PARAMIOC(10)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocused {
	ଅଚଳ param_t param;
	bool ret;
} paramiocused_t;

#define PARAMIOCSETUSED	_PARAMIOC(11)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocsetused {
	ଅଚଳ param_t param;
} paramiocsetused_t;

#define PARAMIOCSETDEFAULT	_PARAMIOC(12)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocsetdefault {
	ଅଚଳ param_t param;
	ଅଚଳ ଶୂନ୍ୟ *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
} paramiocsetdefault_t;

#define PARAMIOCRESET	_PARAMIOC(13)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocreset {
	ଅଚଳ param_t param;
	ଅଚଳ bool notification;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
} paramiocreset_t;

#define PARAMIOCRESETGROUP	_PARAMIOC(14)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocresetgroup {
	param_reset_type_t type;
	ଅଚଳ ଅକ୍ଷର **group;
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_in_group;
} paramiocresetgroup_t;

#define PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocsavedefault {
	bool blocking;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
} paramiocsavedefault_t;

#define PARAMIOCLOADDEFAULT	_PARAMIOC(16)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocloaddefault {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
} paramiocloaddefault_t;


#define PARAMIOCEXPORT	_PARAMIOC(17)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiocexport {
	ଅଚଳ ଅକ୍ଷର *filename;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
} paramiocexport_t;

#define PARAMIOCHASH	_PARAMIOC(18)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା paramiochash {
	uint32_t ret;
} paramiochash_t;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_ioctl(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, unsigned long arg);
