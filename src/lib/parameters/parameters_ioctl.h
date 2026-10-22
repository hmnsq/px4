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
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ <px4_platform/board_ctrl.h>
#અન્તર્ભાવ <px4_platform_common/defines.h>

#define _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#define PARAMIOCNOTIFY	_PARAMIOC(1)

#define PARAMIOCFIND	_PARAMIOC(2)
પ્રકારવ્યાખ્યા સંરચના paramiocfind {
	અચલ અક્ષર *name;
	અચલ bool notification;
	param_t ret;
} paramiocfind_t;


#define PARAMIOCCOUNTUSED	_PARAMIOC(3)
પ્રકારવ્યાખ્યા સંરચના paramioccountused {
	unsigned ret;
} paramioccountused_t;

#define PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
પ્રકારવ્યાખ્યા સંરચના paramiocforusedindex {
	અચલ unsigned index;
	param_t ret;
} paramiocforusedindex_t;

#define PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
પ્રકારવ્યાખ્યા સંરચના paramiocgetusedindex {
	અચલ param_t param;
	unsigned ret;
} paramiocgetusedindex_t;

#define PARAMIOCUNSAVED	_PARAMIOC(6)
પ્રકારવ્યાખ્યા સંરચના paramiocunsaved {
	અચલ param_t param;
	bool ret;
} paramiocunsaved_t;

#define PARAMIOCGET	_PARAMIOC(7)
પ્રકારવ્યાખ્યા સંરચના paramiocget {
	અચલ param_t param;
	અચલ bool deflt;
	શૂન્ય *અચલ val;
	પૂર્ણાંક ret;
} paramiocget_t;

#define PARAMIOCAUTOSAVE	_PARAMIOC(8)
પ્રકારવ્યાખ્યા સંરચના paramiocautosave {
	અચલ bool enable;
} paramiocautosave_t;

#define PARAMIOCSET	_PARAMIOC(9)
પ્રકારવ્યાખ્યા સંરચના paramiocset {
	અચલ param_t param;
	અચલ bool notification;
	અચલ શૂન્ય *val;
	પૂર્ણાંક ret;
} paramiocset_t;

#define PARAMIOCUSED	_PARAMIOC(10)
પ્રકારવ્યાખ્યા સંરચના paramiocused {
	અચલ param_t param;
	bool ret;
} paramiocused_t;

#define PARAMIOCSETUSED	_PARAMIOC(11)
પ્રકારવ્યાખ્યા સંરચના paramiocsetused {
	અચલ param_t param;
} paramiocsetused_t;

#define PARAMIOCSETDEFAULT	_PARAMIOC(12)
પ્રકારવ્યાખ્યા સંરચના paramiocsetdefault {
	અચલ param_t param;
	અચલ શૂન્ય *val;
	પૂર્ણાંક ret;
} paramiocsetdefault_t;

#define PARAMIOCRESET	_PARAMIOC(13)
પ્રકારવ્યાખ્યા સંરચના paramiocreset {
	અચલ param_t param;
	અચલ bool notification;
	પૂર્ણાંક ret;
} paramiocreset_t;

#define PARAMIOCRESETGROUP	_PARAMIOC(14)
પ્રકારવ્યાખ્યા ગણના {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

પ્રકારવ્યાખ્યા સંરચના paramiocresetgroup {
	param_reset_type_t type;
	અચલ અક્ષર **group;
	અચલ પૂર્ણાંક અંક_in_group;
} paramiocresetgroup_t;

#define PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
પ્રકારવ્યાખ્યા સંરચના paramiocsavedefault {
	bool blocking;
	પૂર્ણાંક ret;
} paramiocsavedefault_t;

#define PARAMIOCLOADDEFAULT	_PARAMIOC(16)
પ્રકારવ્યાખ્યા સંરચના paramiocloaddefault {
	પૂર્ણાંક ret;
} paramiocloaddefault_t;


#define PARAMIOCEXPORT	_PARAMIOC(17)
પ્રકારવ્યાખ્યા સંરચના paramiocexport {
	અચલ અક્ષર *filename;
	પૂર્ણાંક ret;
} paramiocexport_t;

#define PARAMIOCHASH	_PARAMIOC(18)
પ્રકારવ્યાખ્યા સંરચના paramiochash {
	uint32_t ret;
} paramiochash_t;

પૂર્ણાંક param_ioctl(unsigned પૂર્ણાંક cmd, unsigned long arg);
