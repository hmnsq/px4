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

#لارښوونه once

#تعریف_کړه PARAM_IMPLEMENTATION
#ضمن "param.h"
#ضمن <px4_platform/board_ctrl.h>
#ضمن <px4_platform_common/defines.h>

#تعریف_کړه _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#تعریف_کړه PARAMIOCNOTIFY	_PARAMIOC(1)

#تعریف_کړه PARAMIOCFIND	_PARAMIOC(2)
تعريفنوع بنية paramiocfind {
	ثابت حرف *name;
	ثابت bool notification;
	param_t ret;
} paramiocfind_t;


#تعریف_کړه PARAMIOCCOUNTUSED	_PARAMIOC(3)
تعريفنوع بنية paramioccountused {
	بینښه ret;
} paramioccountused_t;

#تعریف_کړه PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
تعريفنوع بنية paramiocforusedindex {
	ثابت بینښه شاخص;
	param_t ret;
} paramiocforusedindex_t;

#تعریف_کړه PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
تعريفنوع بنية paramiocgetusedindex {
	ثابت param_t param;
	بینښه ret;
} paramiocgetusedindex_t;

#تعریف_کړه PARAMIOCUNSAVED	_PARAMIOC(6)
تعريفنوع بنية paramiocunsaved {
	ثابت param_t param;
	bool ret;
} paramiocunsaved_t;

#تعریف_کړه PARAMIOCGET	_PARAMIOC(7)
تعريفنوع بنية paramiocget {
	ثابت param_t param;
	ثابت bool deflt;
	فراغ *ثابت val;
	صحيح ret;
} paramiocget_t;

#تعریف_کړه PARAMIOCAUTOSAVE	_PARAMIOC(8)
تعريفنوع بنية paramiocautosave {
	ثابت bool enable;
} paramiocautosave_t;

#تعریف_کړه PARAMIOCSET	_PARAMIOC(9)
تعريفنوع بنية paramiocset {
	ثابت param_t param;
	ثابت bool notification;
	ثابت فراغ *val;
	صحيح ret;
} paramiocset_t;

#تعریف_کړه PARAMIOCUSED	_PARAMIOC(10)
تعريفنوع بنية paramiocused {
	ثابت param_t param;
	bool ret;
} paramiocused_t;

#تعریف_کړه PARAMIOCSETUSED	_PARAMIOC(11)
تعريفنوع بنية paramiocsetused {
	ثابت param_t param;
} paramiocsetused_t;

#تعریف_کړه PARAMIOCSETDEFAULT	_PARAMIOC(12)
تعريفنوع بنية paramiocsetdefault {
	ثابت param_t param;
	ثابت فراغ *val;
	صحيح ret;
} paramiocsetdefault_t;

#تعریف_کړه PARAMIOCRESET	_PARAMIOC(13)
تعريفنوع بنية paramiocreset {
	ثابت param_t param;
	ثابت bool notification;
	صحيح ret;
} paramiocreset_t;

#تعریف_کړه PARAMIOCRESETGROUP	_PARAMIOC(14)
تعريفنوع تعداد {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

تعريفنوع بنية paramiocresetgroup {
	param_reset_type_t type;
	ثابت حرف **ډله;
	ثابت صحيح عدد_دننه_ډله;
} paramiocresetgroup_t;

#تعریف_کړه PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
تعريفنوع بنية paramiocsavedefault {
	bool blocking;
	صحيح ret;
} paramiocsavedefault_t;

#تعریف_کړه PARAMIOCLOADDEFAULT	_PARAMIOC(16)
تعريفنوع بنية paramiocloaddefault {
	صحيح ret;
} paramiocloaddefault_t;


#تعریف_کړه PARAMIOCEXPORT	_PARAMIOC(17)
تعريفنوع بنية paramiocexport {
	ثابت حرف *filename;
	صحيح ret;
} paramiocexport_t;

#تعریف_کړه PARAMIOCHASH	_PARAMIOC(18)
تعريفنوع بنية paramiochash {
	uint32_t ret;
} paramiochash_t;

صحيح param_ioctl(بینښه صحيح cmd, بینښه اوږد arg);
