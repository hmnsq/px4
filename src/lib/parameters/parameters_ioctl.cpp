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

#تعریف_کرو PARAM_IMPLEMENTATION

#ضمن <errno.h>

#ضمن "param.h"
#ضمن "parameters_ioctl.h"
#ضمن <px4_platform_common/defines.h>

صحيح	param_ioctl(بنانشانی صحيح cmd, بنانشانی لما arg)
{
	صحيح ret = OK;

	بدل (cmd) {
	حالة PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		اكسر;

	حالة PARAMIOCFIND: {
			paramiocfind_t *انکڑے = (paramiocfind_t *)arg;

			إذا (انکڑے->notification) {
				انکڑے->ret = param_لبھو(انکڑے->name);

			} وإلا {
				انکڑے->ret = param_لبھو_no_notification(انکڑے->name);
			}
		}
		اكسر;

	حالة PARAMIOCCOUNTUSED: {
			paramioccountused_t *انکڑے = (paramioccountused_t *)arg;
			انکڑے->ret = param_گنتی_ورتیا();
		}
		اكسر;

	حالة PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *انکڑے = (paramiocforusedindex_t *)arg;
			انکڑے->ret =  param_for_ورتیا_اشاریہ(انکڑے->اشاریہ);
		}
		اكسر;

	حالة PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *انکڑے = (paramiocgetusedindex_t *)arg;
			انکڑے->ret = param_get_ورتیا_اشاریہ(انکڑے->param);
		}
		اكسر;

	حالة PARAMIOCUNSAVED: {
			paramiocunsaved_t *انکڑے = (paramiocunsaved_t *)arg;
			انکڑے->ret = param_قدر_unsaved(انکڑے->param);
		}
		اكسر;

	حالة PARAMIOCGET: {
			paramiocget_t *انکڑے = (paramiocget_t *)arg;

			إذا (انکڑے->deflt) {
				انکڑے->ret = param_get_default_قدر(انکڑے->param, انکڑے->val);

			} وإلا {
				انکڑے->ret = param_get(انکڑے->param, انکڑے->val);
			}
		}
		اكسر;

	حالة PARAMIOCAUTOSAVE: {
			paramiocautosave_t *انکڑے = (paramiocautosave_t *)arg;
			param_control_autosave(انکڑے->enable);
		}
		اكسر;

	حالة PARAMIOCSET: {
			paramiocset_t *انکڑے = (paramiocset_t *)arg;

			إذا (انکڑے->notification) {
				انکڑے->ret = param_سیٹ(انکڑے->param, انکڑے->val);

			} وإلا {
				انکڑے->ret = param_سیٹ_no_notification(انکڑے->param, انکڑے->val);
			}
		}
		اكسر;

	حالة PARAMIOCUSED: {
			paramiocused_t *انکڑے = (paramiocused_t *)arg;
			انکڑے->ret = param_ورتیا(انکڑے->param);
		}
		اكسر;

	حالة PARAMIOCSETUSED: {
			paramiocsetused_t *انکڑے = (paramiocsetused_t *)arg;
			param_سیٹ_ورتیا(انکڑے->param);
		}
		اكسر;

	حالة PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *انکڑے = (paramiocsetdefault_t *)arg;
			انکڑے->ret = param_سیٹ_default_قدر(انکڑے->param, انکڑے->val);
		}
		اكسر;

	حالة PARAMIOCRESET: {
			paramiocreset_t *انکڑے = (paramiocreset_t *)arg;

			إذا (انکڑے->notification) {
				انکڑے->ret = param_reset(انکڑے->param);

			} وإلا {
				انکڑے->ret = param_reset_no_notification(انکڑے->param);
			}
		}
		اكسر;

	حالة PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *انکڑے = (paramiocresetgroup_t *)arg;

			إذا (انکڑے->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(انکڑے->گروہ, انکڑے->عدد_وچ_گروہ);

			} وإلا إذا (انکڑے->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(انکڑے->گروہ, انکڑے->عدد_وچ_گروہ);

			} وإلا {
				param_reset_all();
			}
		}
		اكسر;

	حالة PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *انکڑے = (paramiocsavedefault_t *)arg;
			انکڑے->ret = param_save_default(انکڑے->blocking);
		}
		اكسر;

	حالة PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *انکڑے = (paramiocloaddefault_t *)arg;
			انکڑے->ret = param_لدو_default();
		}
		اكسر;

	حالة PARAMIOCEXPORT: {
			paramiocexport_t *انکڑے = (paramiocexport_t *)arg;
			انکڑے->ret = param_export(انکڑے->filename, خالی_سوچک);
		}
		اكسر;

	حالة PARAMIOCHASH: {
			paramiochash_t *انکڑے = (paramiochash_t *)arg;
			انکڑے->ret = param_hash_check();
		}
		اكسر;

	افتراضي:
		ret = -ENOTTY;
		اكسر;
	}

	أرجع ret;
}
