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

#ਹਦਾਇਤ once

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAM_IMPLEMENTATION
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform/board_ctrl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCNOTIFY	_PARAMIOC(1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCFIND	_PARAMIOC(2)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocfind {
	ਅਚਲ ਅੱਖਰ *name;
	ਅਚਲ bool notification;
	param_t ret;
} paramiocfind_t;


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCCOUNTUSED	_PARAMIOC(3)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramioccountused {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ret;
} paramioccountused_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocforusedindex {
	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ;
	param_t ret;
} paramiocforusedindex_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocgetusedindex {
	ਅਚਲ param_t param;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ret;
} paramiocgetusedindex_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCUNSAVED	_PARAMIOC(6)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocunsaved {
	ਅਚਲ param_t param;
	bool ret;
} paramiocunsaved_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCGET	_PARAMIOC(7)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocget {
	ਅਚਲ param_t param;
	ਅਚਲ bool deflt;
	ਖਾਲੀ *ਅਚਲ val;
	ਪੂਰਨ ret;
} paramiocget_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCAUTOSAVE	_PARAMIOC(8)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocautosave {
	ਅਚਲ bool enable;
} paramiocautosave_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCSET	_PARAMIOC(9)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocset {
	ਅਚਲ param_t param;
	ਅਚਲ bool notification;
	ਅਚਲ ਖਾਲੀ *val;
	ਪੂਰਨ ret;
} paramiocset_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCUSED	_PARAMIOC(10)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocused {
	ਅਚਲ param_t param;
	bool ret;
} paramiocused_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCSETUSED	_PARAMIOC(11)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocsetused {
	ਅਚਲ param_t param;
} paramiocsetused_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCSETDEFAULT	_PARAMIOC(12)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocsetdefault {
	ਅਚਲ param_t param;
	ਅਚਲ ਖਾਲੀ *val;
	ਪੂਰਨ ret;
} paramiocsetdefault_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCRESET	_PARAMIOC(13)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocreset {
	ਅਚਲ param_t param;
	ਅਚਲ bool notification;
	ਪੂਰਨ ret;
} paramiocreset_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCRESETGROUP	_PARAMIOC(14)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocresetgroup {
	param_reset_type_t type;
	ਅਚਲ ਅੱਖਰ **ਸਮੂਹ;
	ਅਚਲ ਪੂਰਨ ਅੰਕ_ਵਿੱਚ_ਸਮੂਹ;
} paramiocresetgroup_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocsavedefault {
	bool blocking;
	ਪੂਰਨ ret;
} paramiocsavedefault_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCLOADDEFAULT	_PARAMIOC(16)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocloaddefault {
	ਪੂਰਨ ret;
} paramiocloaddefault_t;


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCEXPORT	_PARAMIOC(17)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiocexport {
	ਅਚਲ ਅੱਖਰ *filename;
	ਪੂਰਨ ret;
} paramiocexport_t;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAMIOCHASH	_PARAMIOC(18)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ paramiochash {
	uint32_t ret;
} paramiochash_t;

ਪੂਰਨ param_ioctl(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cmd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg);
