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

#निर्देशः once

#परिभाषय PARAM_IMPLEMENTATION
#अन्तर्भाव "param.h"
#अन्तर्भाव <px4_platform/board_ctrl.h>
#अन्तर्भाव <px4_platform_common/defines.h>

#परिभाषय _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#परिभाषय PARAMIOCNOTIFY	_PARAMIOC(1)

#परिभाषय PARAMIOCFIND	_PARAMIOC(2)
प्रकारनिर्देश संरचना paramiocfind {
	ध्रुव वर्ण *name;
	ध्रुव bool notification;
	param_t ret;
} paramiocfind_t;


#परिभाषय PARAMIOCCOUNTUSED	_PARAMIOC(3)
प्रकारनिर्देश संरचना paramioccountused {
	अचिह्नित ret;
} paramioccountused_t;

#परिभाषय PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
प्रकारनिर्देश संरचना paramiocforusedindex {
	ध्रुव अचिह्नित सूचकांक;
	param_t ret;
} paramiocforusedindex_t;

#परिभाषय PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
प्रकारनिर्देश संरचना paramiocgetusedindex {
	ध्रुव param_t param;
	अचिह्नित ret;
} paramiocgetusedindex_t;

#परिभाषय PARAMIOCUNSAVED	_PARAMIOC(6)
प्रकारनिर्देश संरचना paramiocunsaved {
	ध्रुव param_t param;
	bool ret;
} paramiocunsaved_t;

#परिभाषय PARAMIOCGET	_PARAMIOC(7)
प्रकारनिर्देश संरचना paramiocget {
	ध्रुव param_t param;
	ध्रुव bool deflt;
	शून्य *ध्रुव val;
	पूर्णाङ्क ret;
} paramiocget_t;

#परिभाषय PARAMIOCAUTOSAVE	_PARAMIOC(8)
प्रकारनिर्देश संरचना paramiocautosave {
	ध्रुव bool enable;
} paramiocautosave_t;

#परिभाषय PARAMIOCSET	_PARAMIOC(9)
प्रकारनिर्देश संरचना paramiocset {
	ध्रुव param_t param;
	ध्रुव bool notification;
	ध्रुव शून्य *val;
	पूर्णाङ्क ret;
} paramiocset_t;

#परिभाषय PARAMIOCUSED	_PARAMIOC(10)
प्रकारनिर्देश संरचना paramiocused {
	ध्रुव param_t param;
	bool ret;
} paramiocused_t;

#परिभाषय PARAMIOCSETUSED	_PARAMIOC(11)
प्रकारनिर्देश संरचना paramiocsetused {
	ध्रुव param_t param;
} paramiocsetused_t;

#परिभाषय PARAMIOCSETDEFAULT	_PARAMIOC(12)
प्रकारनिर्देश संरचना paramiocsetdefault {
	ध्रुव param_t param;
	ध्रुव शून्य *val;
	पूर्णाङ्क ret;
} paramiocsetdefault_t;

#परिभाषय PARAMIOCRESET	_PARAMIOC(13)
प्रकारनिर्देश संरचना paramiocreset {
	ध्रुव param_t param;
	ध्रुव bool notification;
	पूर्णाङ्क ret;
} paramiocreset_t;

#परिभाषय PARAMIOCRESETGROUP	_PARAMIOC(14)
प्रकारनिर्देश गणना {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

प्रकारनिर्देश संरचना paramiocresetgroup {
	param_reset_type_t type;
	ध्रुव वर्ण **गणः;
	ध्रुव पूर्णाङ्क अङ्कः_अन्तर्_गणः;
} paramiocresetgroup_t;

#परिभाषय PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
प्रकारनिर्देश संरचना paramiocsavedefault {
	bool blocking;
	पूर्णाङ्क ret;
} paramiocsavedefault_t;

#परिभाषय PARAMIOCLOADDEFAULT	_PARAMIOC(16)
प्रकारनिर्देश संरचना paramiocloaddefault {
	पूर्णाङ्क ret;
} paramiocloaddefault_t;


#परिभाषय PARAMIOCEXPORT	_PARAMIOC(17)
प्रकारनिर्देश संरचना paramiocexport {
	ध्रुव वर्ण *filename;
	पूर्णाङ्क ret;
} paramiocexport_t;

#परिभाषय PARAMIOCHASH	_PARAMIOC(18)
प्रकारनिर्देश संरचना paramiochash {
	uint32_t ret;
} paramiochash_t;

पूर्णाङ्क param_ioctl(अचिह्नित पूर्णाङ्क cmd, अचिह्नित दीर्घ arg);
