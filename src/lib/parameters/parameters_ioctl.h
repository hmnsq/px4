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
#অন্তর্ভাব "param.h"
#অন্তর্ভাব <px4_platform/board_ctrl.h>
#অন্তর্ভাব <px4_platform_common/defines.h>

#define _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#define PARAMIOCNOTIFY	_PARAMIOC(1)

#define PARAMIOCFIND	_PARAMIOC(2)
প্রকারনির্দেশ সংরচনা paramiocfind {
	ধ্রুব বর্ণ *name;
	ধ্রুব bool notification;
	param_t ret;
} paramiocfind_t;


#define PARAMIOCCOUNTUSED	_PARAMIOC(3)
প্রকারনির্দেশ সংরচনা paramioccountused {
	unsigned ret;
} paramioccountused_t;

#define PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
প্রকারনির্দেশ সংরচনা paramiocforusedindex {
	ধ্রুব unsigned index;
	param_t ret;
} paramiocforusedindex_t;

#define PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
প্রকারনির্দেশ সংরচনা paramiocgetusedindex {
	ধ্রুব param_t param;
	unsigned ret;
} paramiocgetusedindex_t;

#define PARAMIOCUNSAVED	_PARAMIOC(6)
প্রকারনির্দেশ সংরচনা paramiocunsaved {
	ধ্রুব param_t param;
	bool ret;
} paramiocunsaved_t;

#define PARAMIOCGET	_PARAMIOC(7)
প্রকারনির্দেশ সংরচনা paramiocget {
	ধ্রুব param_t param;
	ধ্রুব bool deflt;
	শূন্য *ধ্রুব val;
	পূর্ণাঙ্ক ret;
} paramiocget_t;

#define PARAMIOCAUTOSAVE	_PARAMIOC(8)
প্রকারনির্দেশ সংরচনা paramiocautosave {
	ধ্রুব bool enable;
} paramiocautosave_t;

#define PARAMIOCSET	_PARAMIOC(9)
প্রকারনির্দেশ সংরচনা paramiocset {
	ধ্রুব param_t param;
	ধ্রুব bool notification;
	ধ্রুব শূন্য *val;
	পূর্ণাঙ্ক ret;
} paramiocset_t;

#define PARAMIOCUSED	_PARAMIOC(10)
প্রকারনির্দেশ সংরচনা paramiocused {
	ধ্রুব param_t param;
	bool ret;
} paramiocused_t;

#define PARAMIOCSETUSED	_PARAMIOC(11)
প্রকারনির্দেশ সংরচনা paramiocsetused {
	ধ্রুব param_t param;
} paramiocsetused_t;

#define PARAMIOCSETDEFAULT	_PARAMIOC(12)
প্রকারনির্দেশ সংরচনা paramiocsetdefault {
	ধ্রুব param_t param;
	ধ্রুব শূন্য *val;
	পূর্ণাঙ্ক ret;
} paramiocsetdefault_t;

#define PARAMIOCRESET	_PARAMIOC(13)
প্রকারনির্দেশ সংরচনা paramiocreset {
	ধ্রুব param_t param;
	ধ্রুব bool notification;
	পূর্ণাঙ্ক ret;
} paramiocreset_t;

#define PARAMIOCRESETGROUP	_PARAMIOC(14)
প্রকারনির্দেশ গণনা {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

প্রকারনির্দেশ সংরচনা paramiocresetgroup {
	param_reset_type_t type;
	ধ্রুব বর্ণ **group;
	ধ্রুব পূর্ণাঙ্ক অঙ্ক_in_group;
} paramiocresetgroup_t;

#define PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
প্রকারনির্দেশ সংরচনা paramiocsavedefault {
	bool blocking;
	পূর্ণাঙ্ক ret;
} paramiocsavedefault_t;

#define PARAMIOCLOADDEFAULT	_PARAMIOC(16)
প্রকারনির্দেশ সংরচনা paramiocloaddefault {
	পূর্ণাঙ্ক ret;
} paramiocloaddefault_t;


#define PARAMIOCEXPORT	_PARAMIOC(17)
প্রকারনির্দেশ সংরচনা paramiocexport {
	ধ্রুব বর্ণ *filename;
	পূর্ণাঙ্ক ret;
} paramiocexport_t;

#define PARAMIOCHASH	_PARAMIOC(18)
প্রকারনির্দেশ সংরচনা paramiochash {
	uint32_t ret;
} paramiochash_t;

পূর্ণাঙ্ক param_ioctl(unsigned পূর্ণাঙ্ক cmd, unsigned long arg);
