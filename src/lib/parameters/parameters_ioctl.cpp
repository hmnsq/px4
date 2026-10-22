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
 * @file parameters_ioctl.cpp
 *
 * Protected build kernel space interface to global parameter store.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAM_IMPLEMENTATION

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parameters_ioctl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

ਪੂਰਨ	param_ioctl(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cmd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg)
{
	ਪੂਰਨ ret = OK;

	ਚੋਣ (cmd) {
	ਹਾਲਤ PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCFIND: {
			paramiocfind_t *ਅੰਕੜੇ = (paramiocfind_t *)arg;

			ਜੇ (ਅੰਕੜੇ->notification) {
				ਅੰਕੜੇ->ret = param_ਲੱਭੋ(ਅੰਕੜੇ->name);

			} ਨਹੀਂਤਾਂ {
				ਅੰਕੜੇ->ret = param_ਲੱਭੋ_no_notification(ਅੰਕੜੇ->name);
			}
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCCOUNTUSED: {
			paramioccountused_t *ਅੰਕੜੇ = (paramioccountused_t *)arg;
			ਅੰਕੜੇ->ret = param_ਗਿਣਤੀ_ਵਰਤਿਆ();
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *ਅੰਕੜੇ = (paramiocforusedindex_t *)arg;
			ਅੰਕੜੇ->ret =  param_for_ਵਰਤਿਆ_ਸੂਚਕਾਂਕ(ਅੰਕੜੇ->ਸੂਚਕਾਂਕ);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *ਅੰਕੜੇ = (paramiocgetusedindex_t *)arg;
			ਅੰਕੜੇ->ret = param_get_ਵਰਤਿਆ_ਸੂਚਕਾਂਕ(ਅੰਕੜੇ->param);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCUNSAVED: {
			paramiocunsaved_t *ਅੰਕੜੇ = (paramiocunsaved_t *)arg;
			ਅੰਕੜੇ->ret = param_ਮੁੱਲ_unsaved(ਅੰਕੜੇ->param);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCGET: {
			paramiocget_t *ਅੰਕੜੇ = (paramiocget_t *)arg;

			ਜੇ (ਅੰਕੜੇ->deflt) {
				ਅੰਕੜੇ->ret = param_get_default_ਮੁੱਲ(ਅੰਕੜੇ->param, ਅੰਕੜੇ->val);

			} ਨਹੀਂਤਾਂ {
				ਅੰਕੜੇ->ret = param_get(ਅੰਕੜੇ->param, ਅੰਕੜੇ->val);
			}
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCAUTOSAVE: {
			paramiocautosave_t *ਅੰਕੜੇ = (paramiocautosave_t *)arg;
			param_control_autosave(ਅੰਕੜੇ->enable);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCSET: {
			paramiocset_t *ਅੰਕੜੇ = (paramiocset_t *)arg;

			ਜੇ (ਅੰਕੜੇ->notification) {
				ਅੰਕੜੇ->ret = param_ਸੈੱਟ(ਅੰਕੜੇ->param, ਅੰਕੜੇ->val);

			} ਨਹੀਂਤਾਂ {
				ਅੰਕੜੇ->ret = param_ਸੈੱਟ_no_notification(ਅੰਕੜੇ->param, ਅੰਕੜੇ->val);
			}
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCUSED: {
			paramiocused_t *ਅੰਕੜੇ = (paramiocused_t *)arg;
			ਅੰਕੜੇ->ret = param_ਵਰਤਿਆ(ਅੰਕੜੇ->param);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCSETUSED: {
			paramiocsetused_t *ਅੰਕੜੇ = (paramiocsetused_t *)arg;
			param_ਸੈੱਟ_ਵਰਤਿਆ(ਅੰਕੜੇ->param);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *ਅੰਕੜੇ = (paramiocsetdefault_t *)arg;
			ਅੰਕੜੇ->ret = param_ਸੈੱਟ_default_ਮੁੱਲ(ਅੰਕੜੇ->param, ਅੰਕੜੇ->val);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCRESET: {
			paramiocreset_t *ਅੰਕੜੇ = (paramiocreset_t *)arg;

			ਜੇ (ਅੰਕੜੇ->notification) {
				ਅੰਕੜੇ->ret = param_reset(ਅੰਕੜੇ->param);

			} ਨਹੀਂਤਾਂ {
				ਅੰਕੜੇ->ret = param_reset_no_notification(ਅੰਕੜੇ->param);
			}
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *ਅੰਕੜੇ = (paramiocresetgroup_t *)arg;

			ਜੇ (ਅੰਕੜੇ->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(ਅੰਕੜੇ->ਸਮੂਹ, ਅੰਕੜੇ->ਅੰਕ_ਵਿੱਚ_ਸਮੂਹ);

			} ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕੜੇ->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(ਅੰਕੜੇ->ਸਮੂਹ, ਅੰਕੜੇ->ਅੰਕ_ਵਿੱਚ_ਸਮੂਹ);

			} ਨਹੀਂਤਾਂ {
				param_reset_all();
			}
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *ਅੰਕੜੇ = (paramiocsavedefault_t *)arg;
			ਅੰਕੜੇ->ret = param_save_default(ਅੰਕੜੇ->blocking);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *ਅੰਕੜੇ = (paramiocloaddefault_t *)arg;
			ਅੰਕੜੇ->ret = param_ਲੱਦੋ_default();
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCEXPORT: {
			paramiocexport_t *ਅੰਕੜੇ = (paramiocexport_t *)arg;
			ਅੰਕੜੇ->ret = param_export(ਅੰਕੜੇ->filename, ਖਾਲੀ_ਸੂਚਕ);
		}
		ਰੋਕੋ;

	ਹਾਲਤ PARAMIOCHASH: {
			paramiochash_t *ਅੰਕੜੇ = (paramiochash_t *)arg;
			ਅੰਕੜੇ->ret = param_hash_check();
		}
		ਰੋਕੋ;

	ਮੂਲ:
		ret = -ENOTTY;
		ਰੋਕੋ;
	}

	ਵਾਪਸ ret;
}
