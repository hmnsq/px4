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

#సూచన once

#నిర్వచించు PARAM_IMPLEMENTATION
#అన్తర్భావ "param.h"
#అన్తర్భావ <px4_platform/board_ctrl.h>
#అన్తర్భావ <px4_platform_common/defines.h>

#నిర్వచించు _PARAMIOC(_n) (_PX4_IOC(_PARAMIOCBASE, _n))

#నిర్వచించు PARAMIOCNOTIFY	_PARAMIOC(1)

#నిర్వచించు PARAMIOCFIND	_PARAMIOC(2)
రకనిర్వచనం నిర్మాణం paramiocfind {
	స్థిరరాశి అక్షరం *name;
	స్థిరరాశి bool notification;
	param_t ret;
} paramiocfind_t;


#నిర్వచించు PARAMIOCCOUNTUSED	_PARAMIOC(3)
రకనిర్వచనం నిర్మాణం paramioccountused {
	గుర్తులేని ret;
} paramioccountused_t;

#నిర్వచించు PARAMIOCFORUSEDINDEX	_PARAMIOC(4)
రకనిర్వచనం నిర్మాణం paramiocforusedindex {
	స్థిరరాశి గుర్తులేని సూచీ;
	param_t ret;
} paramiocforusedindex_t;

#నిర్వచించు PARAMIOCGETUSEDINDEX	_PARAMIOC(5)
రకనిర్వచనం నిర్మాణం paramiocgetusedindex {
	స్థిరరాశి param_t param;
	గుర్తులేని ret;
} paramiocgetusedindex_t;

#నిర్వచించు PARAMIOCUNSAVED	_PARAMIOC(6)
రకనిర్వచనం నిర్మాణం paramiocunsaved {
	స్థిరరాశి param_t param;
	bool ret;
} paramiocunsaved_t;

#నిర్వచించు PARAMIOCGET	_PARAMIOC(7)
రకనిర్వచనం నిర్మాణం paramiocget {
	స్థిరరాశి param_t param;
	స్థిరరాశి bool deflt;
	శూన్య *స్థిరరాశి val;
	పూర్ణాంకం ret;
} paramiocget_t;

#నిర్వచించు PARAMIOCAUTOSAVE	_PARAMIOC(8)
రకనిర్వచనం నిర్మాణం paramiocautosave {
	స్థిరరాశి bool enable;
} paramiocautosave_t;

#నిర్వచించు PARAMIOCSET	_PARAMIOC(9)
రకనిర్వచనం నిర్మాణం paramiocset {
	స్థిరరాశి param_t param;
	స్థిరరాశి bool notification;
	స్థిరరాశి శూన్య *val;
	పూర్ణాంకం ret;
} paramiocset_t;

#నిర్వచించు PARAMIOCUSED	_PARAMIOC(10)
రకనిర్వచనం నిర్మాణం paramiocused {
	స్థిరరాశి param_t param;
	bool ret;
} paramiocused_t;

#నిర్వచించు PARAMIOCSETUSED	_PARAMIOC(11)
రకనిర్వచనం నిర్మాణం paramiocsetused {
	స్థిరరాశి param_t param;
} paramiocsetused_t;

#నిర్వచించు PARAMIOCSETDEFAULT	_PARAMIOC(12)
రకనిర్వచనం నిర్మాణం paramiocsetdefault {
	స్థిరరాశి param_t param;
	స్థిరరాశి శూన్య *val;
	పూర్ణాంకం ret;
} paramiocsetdefault_t;

#నిర్వచించు PARAMIOCRESET	_PARAMIOC(13)
రకనిర్వచనం నిర్మాణం paramiocreset {
	స్థిరరాశి param_t param;
	స్థిరరాశి bool notification;
	పూర్ణాంకం ret;
} paramiocreset_t;

#నిర్వచించు PARAMIOCRESETGROUP	_PARAMIOC(14)
రకనిర్వచనం గణన {
	PARAM_RESET_ALL,
	PARAM_RESET_EXCLUDES,
	PARAM_RESET_SPECIFIC
} param_reset_type_t;

రకనిర్వచనం నిర్మాణం paramiocresetgroup {
	param_reset_type_t type;
	స్థిరరాశి అక్షరం **సమూహం;
	స్థిరరాశి పూర్ణాంకం అంకె_లోపల_సమూహం;
} paramiocresetgroup_t;

#నిర్వచించు PARAMIOCSAVEDEFAULT	_PARAMIOC(15)
రకనిర్వచనం నిర్మాణం paramiocsavedefault {
	bool blocking;
	పూర్ణాంకం ret;
} paramiocsavedefault_t;

#నిర్వచించు PARAMIOCLOADDEFAULT	_PARAMIOC(16)
రకనిర్వచనం నిర్మాణం paramiocloaddefault {
	పూర్ణాంకం ret;
} paramiocloaddefault_t;


#నిర్వచించు PARAMIOCEXPORT	_PARAMIOC(17)
రకనిర్వచనం నిర్మాణం paramiocexport {
	స్థిరరాశి అక్షరం *filename;
	పూర్ణాంకం ret;
} paramiocexport_t;

#నిర్వచించు PARAMIOCHASH	_PARAMIOC(18)
రకనిర్వచనం నిర్మాణం paramiochash {
	uint32_t ret;
} paramiochash_t;

పూర్ణాంకం param_ioctl(గుర్తులేని పూర్ణాంకం cmd, గుర్తులేని దీర్ఘ arg);
