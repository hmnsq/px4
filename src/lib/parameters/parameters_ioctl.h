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

#ہدایت once

#تعریف_کرو PARAM_IMPLEMENTATION
#شامل "param.h"
#شامل <px4_platform/board_ctrl.h>
#شامل <px4_platform_common/defines.h>

#تعریف_کرو _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#تعریف_کرو PARAMIOCNOTIFY	_PARAMIOC(1)

#تعریف_کرو PARAMIOCFIND	_PARAMIOC(2)
قسمتعریف ساخت paramiocfind {
	ثابت حرف *name;
	ثابت bool notification;
	param_t ret;
} paramiocfind_t;


#تعریف_کرو PARAMIOCCOUNTUSED	_PARAMIOC(3)
قسمتعریف ساخت paramioccountused {
	غیرعلامتی ret;
} paramioccountused_t;

#تعریف_کرو PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
قسمتعریف ساخت paramiocforusedindex {
	ثابت غیرعلامتی اشاریہ;
	param_t ret;
} paramiocforusedindex_t;

#تعریف_کرو PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
قسمتعریف ساخت paramiocgetusedindex {
	ثابت param_t param;
	غیرعلامتی ret;
} paramiocgetusedindex_t;

#تعریف_کرو PARAMIOCUNSAVED	_PARAMIOC(6)
قسمتعریف ساخت paramiocunsaved {
	ثابت param_t param;
	bool ret;
} paramiocunsaved_t;

#تعریف_کرو PARAMIOCGET	_PARAMIOC(7)
قسمتعریف ساخت paramiocget {
	ثابت param_t param;
	ثابت bool deflt;
	خالی *ثابت val;
	صحیح ret;
} paramiocget_t;

#تعریف_کرو PARAMIOCAUTOSAVE	_PARAMIOC(8)
قسمتعریف ساخت paramiocautosave {
	ثابت bool enable;
} paramiocautosave_t;

#تعریف_کرو PARAMIOCSET	_PARAMIOC(9)
قسمتعریف ساخت paramiocset {
	ثابت param_t param;
	ثابت bool notification;
	ثابت خالی *val;
	صحیح ret;
} paramiocset_t;

#تعریف_کرو PARAMIOCUSED	_PARAMIOC(10)
قسمتعریف ساخت paramiocused {
	ثابت param_t param;
	bool ret;
} paramiocused_t;

#تعریف_کرو PARAMIOCSETUSED	_PARAMIOC(11)
قسمتعریف ساخت paramiocsetused {
	ثابت param_t param;
} paramiocsetused_t;

#تعریف_کرو PARAMIOCSETDEFAULT	_PARAMIOC(12)
قسمتعریف ساخت paramiocsetdefault {
	ثابت param_t param;
	ثابت خالی *val;
	صحیح ret;
} paramiocsetdefault_t;

#تعریف_کرو PARAMIOCRESET	_PARAMIOC(13)
قسمتعریف ساخت paramiocreset {
	ثابت param_t param;
	ثابت bool notification;
	صحیح ret;
} paramiocreset_t;

#تعریف_کرو PARAMIOCRESETGROUP	_PARAMIOC(14)
قسمتعریف شمار {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

قسمتعریف ساخت paramiocresetgroup {
	param_reset_type_t type;
	ثابت حرف **گروہ;
	ثابت صحیح عدد_اندر_گروہ;
} paramiocresetgroup_t;

#تعریف_کرو PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
قسمتعریف ساخت paramiocsavedefault {
	bool blocking;
	صحیح ret;
} paramiocsavedefault_t;

#تعریف_کرو PARAMIOCLOADDEFAULT	_PARAMIOC(16)
قسمتعریف ساخت paramiocloaddefault {
	صحیح ret;
} paramiocloaddefault_t;


#تعریف_کرو PARAMIOCEXPORT	_PARAMIOC(17)
قسمتعریف ساخت paramiocexport {
	ثابت حرف *filename;
	صحیح ret;
} paramiocexport_t;

#تعریف_کرو PARAMIOCHASH	_PARAMIOC(18)
قسمتعریف ساخت paramiochash {
	uint32_t ret;
} paramiochash_t;

صحیح param_ioctl(غیرعلامتی صحیح cmd, غیرعلامتی طویل arg);
