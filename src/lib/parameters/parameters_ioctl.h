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
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ <px4_platform/board_ctrl.h>
#അന്തര്ഭാവ <px4_platform_common/defines.h>

#define _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#define PARAMIOCNOTIFY	_PARAMIOC(1)

#define PARAMIOCFIND	_PARAMIOC(2)
തരനിർവചനം ഘടന paramiocfind {
	സ്ഥിരം അക്ഷരം *name;
	സ്ഥിരം bool notification;
	param_t ret;
} paramiocfind_t;


#define PARAMIOCCOUNTUSED	_PARAMIOC(3)
തരനിർവചനം ഘടന paramioccountused {
	unsigned ret;
} paramioccountused_t;

#define PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
തരനിർവചനം ഘടന paramiocforusedindex {
	സ്ഥിരം unsigned index;
	param_t ret;
} paramiocforusedindex_t;

#define PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
തരനിർവചനം ഘടന paramiocgetusedindex {
	സ്ഥിരം param_t param;
	unsigned ret;
} paramiocgetusedindex_t;

#define PARAMIOCUNSAVED	_PARAMIOC(6)
തരനിർവചനം ഘടന paramiocunsaved {
	സ്ഥിരം param_t param;
	bool ret;
} paramiocunsaved_t;

#define PARAMIOCGET	_PARAMIOC(7)
തരനിർവചനം ഘടന paramiocget {
	സ്ഥിരം param_t param;
	സ്ഥിരം bool deflt;
	ശൂന്യം *സ്ഥിരം val;
	പൂർണ്ണസംഖ്യ ret;
} paramiocget_t;

#define PARAMIOCAUTOSAVE	_PARAMIOC(8)
തരനിർവചനം ഘടന paramiocautosave {
	സ്ഥിരം bool enable;
} paramiocautosave_t;

#define PARAMIOCSET	_PARAMIOC(9)
തരനിർവചനം ഘടന paramiocset {
	സ്ഥിരം param_t param;
	സ്ഥിരം bool notification;
	സ്ഥിരം ശൂന്യം *val;
	പൂർണ്ണസംഖ്യ ret;
} paramiocset_t;

#define PARAMIOCUSED	_PARAMIOC(10)
തരനിർവചനം ഘടന paramiocused {
	സ്ഥിരം param_t param;
	bool ret;
} paramiocused_t;

#define PARAMIOCSETUSED	_PARAMIOC(11)
തരനിർവചനം ഘടന paramiocsetused {
	സ്ഥിരം param_t param;
} paramiocsetused_t;

#define PARAMIOCSETDEFAULT	_PARAMIOC(12)
തരനിർവചനം ഘടന paramiocsetdefault {
	സ്ഥിരം param_t param;
	സ്ഥിരം ശൂന്യം *val;
	പൂർണ്ണസംഖ്യ ret;
} paramiocsetdefault_t;

#define PARAMIOCRESET	_PARAMIOC(13)
തരനിർവചനം ഘടന paramiocreset {
	സ്ഥിരം param_t param;
	സ്ഥിരം bool notification;
	പൂർണ്ണസംഖ്യ ret;
} paramiocreset_t;

#define PARAMIOCRESETGROUP	_PARAMIOC(14)
തരനിർവചനം എണ്ണൽ {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

തരനിർവചനം ഘടന paramiocresetgroup {
	param_reset_type_t type;
	സ്ഥിരം അക്ഷരം **group;
	സ്ഥിരം പൂർണ്ണസംഖ്യ അക്കം_in_group;
} paramiocresetgroup_t;

#define PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
തരനിർവചനം ഘടന paramiocsavedefault {
	bool blocking;
	പൂർണ്ണസംഖ്യ ret;
} paramiocsavedefault_t;

#define PARAMIOCLOADDEFAULT	_PARAMIOC(16)
തരനിർവചനം ഘടന paramiocloaddefault {
	പൂർണ്ണസംഖ്യ ret;
} paramiocloaddefault_t;


#define PARAMIOCEXPORT	_PARAMIOC(17)
തരനിർവചനം ഘടന paramiocexport {
	സ്ഥിരം അക്ഷരം *filename;
	പൂർണ്ണസംഖ്യ ret;
} paramiocexport_t;

#define PARAMIOCHASH	_PARAMIOC(18)
തരനിർവചനം ഘടന paramiochash {
	uint32_t ret;
} paramiochash_t;

പൂർണ്ണസംഖ്യ param_ioctl(unsigned പൂർണ്ണസംഖ്യ cmd, unsigned long arg);
