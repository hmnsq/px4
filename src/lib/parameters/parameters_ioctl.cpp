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

#تعریف_کړه PARAM_IMPLEMENTATION

#ضمن <errno.h>

#ضمن "param.h"
#ضمن "parameters_ioctl.h"
#ضمن <px4_platform_common/defines.h>

صحيح	param_ioctl(بینښه صحيح cmd, بینښه اوږد arg)
{
	صحيح ret = OK;

	بدل (cmd) {
	حالة PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		اكسر;

	حالة PARAMIOCFIND: {
			paramiocfind_t *معلومات = (paramiocfind_t *)arg;

			إذا (معلومات->notification) {
				معلومات->ret = param_پیدا_کړه(معلومات->name);

			} وإلا {
				معلومات->ret = param_پیدا_کړه_no_notification(معلومات->name);
			}
		}
		اكسر;

	حالة PARAMIOCCOUNTUSED: {
			paramioccountused_t *معلومات = (paramioccountused_t *)arg;
			معلومات->ret = param_شمېر_کارول_شوی();
		}
		اكسر;

	حالة PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *معلومات = (paramiocforusedindex_t *)arg;
			معلومات->ret =  param_for_کارول_شوی_شاخص(معلومات->شاخص);
		}
		اكسر;

	حالة PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *معلومات = (paramiocgetusedindex_t *)arg;
			معلومات->ret = param_get_کارول_شوی_شاخص(معلومات->param);
		}
		اكسر;

	حالة PARAMIOCUNSAVED: {
			paramiocunsaved_t *معلومات = (paramiocunsaved_t *)arg;
			معلومات->ret = param_ارزښت_unsaved(معلومات->param);
		}
		اكسر;

	حالة PARAMIOCGET: {
			paramiocget_t *معلومات = (paramiocget_t *)arg;

			إذا (معلومات->deflt) {
				معلومات->ret = param_get_default_ارزښت(معلومات->param, معلومات->val);

			} وإلا {
				معلومات->ret = param_get(معلومات->param, معلومات->val);
			}
		}
		اكسر;

	حالة PARAMIOCAUTOSAVE: {
			paramiocautosave_t *معلومات = (paramiocautosave_t *)arg;
			param_control_autosave(معلومات->enable);
		}
		اكسر;

	حالة PARAMIOCSET: {
			paramiocset_t *معلومات = (paramiocset_t *)arg;

			إذا (معلومات->notification) {
				معلومات->ret = param_سېټ(معلومات->param, معلومات->val);

			} وإلا {
				معلومات->ret = param_سېټ_no_notification(معلومات->param, معلومات->val);
			}
		}
		اكسر;

	حالة PARAMIOCUSED: {
			paramiocused_t *معلومات = (paramiocused_t *)arg;
			معلومات->ret = param_کارول_شوی(معلومات->param);
		}
		اكسر;

	حالة PARAMIOCSETUSED: {
			paramiocsetused_t *معلومات = (paramiocsetused_t *)arg;
			param_سېټ_کارول_شوی(معلومات->param);
		}
		اكسر;

	حالة PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *معلومات = (paramiocsetdefault_t *)arg;
			معلومات->ret = param_سېټ_default_ارزښت(معلومات->param, معلومات->val);
		}
		اكسر;

	حالة PARAMIOCRESET: {
			paramiocreset_t *معلومات = (paramiocreset_t *)arg;

			إذا (معلومات->notification) {
				معلومات->ret = param_reset(معلومات->param);

			} وإلا {
				معلومات->ret = param_reset_no_notification(معلومات->param);
			}
		}
		اكسر;

	حالة PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *معلومات = (paramiocresetgroup_t *)arg;

			إذا (معلومات->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(معلومات->ډله, معلومات->عدد_دننه_ډله);

			} وإلا إذا (معلومات->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(معلومات->ډله, معلومات->عدد_دننه_ډله);

			} وإلا {
				param_reset_all();
			}
		}
		اكسر;

	حالة PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *معلومات = (paramiocsavedefault_t *)arg;
			معلومات->ret = param_save_default(معلومات->blocking);
		}
		اكسر;

	حالة PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *معلومات = (paramiocloaddefault_t *)arg;
			معلومات->ret = param_بارول_default();
		}
		اكسر;

	حالة PARAMIOCEXPORT: {
			paramiocexport_t *معلومات = (paramiocexport_t *)arg;
			معلومات->ret = param_export(معلومات->filename, تش_اشاره);
		}
		اكسر;

	حالة PARAMIOCHASH: {
			paramiochash_t *معلومات = (paramiochash_t *)arg;
			معلومات->ret = param_hash_check();
		}
		اكسر;

	افتراضي:
		ret = -ENOTTY;
		اكسر;
	}

	أرجع ret;
}
