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

#شامل <errno.h>

#شامل "param.h"
#شامل "parameters_ioctl.h"
#شامل <px4_platform_common/defines.h>

صحیح	param_ioctl(غیرعلامتی صحیح cmd, غیرعلامتی طویل arg)
{
	صحیح ret = OK;

	انتخاب (cmd) {
	حالت PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		توڑو;

	حالت PARAMIOCFIND: {
			paramiocfind_t *ڈیٹا = (paramiocfind_t *)arg;

			اگر (ڈیٹا->notification) {
				ڈیٹا->ret = param_ڈھونڈو(ڈیٹا->name);

			} ورنہ {
				ڈیٹا->ret = param_ڈھونڈو_no_notification(ڈیٹا->name);
			}
		}
		توڑو;

	حالت PARAMIOCCOUNTUSED: {
			paramioccountused_t *ڈیٹا = (paramioccountused_t *)arg;
			ڈیٹا->ret = param_گنتی_مستعمل();
		}
		توڑو;

	حالت PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *ڈیٹا = (paramiocforusedindex_t *)arg;
			ڈیٹا->ret =  param_for_مستعمل_اشاریہ(ڈیٹا->اشاریہ);
		}
		توڑو;

	حالت PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *ڈیٹا = (paramiocgetusedindex_t *)arg;
			ڈیٹا->ret = param_get_مستعمل_اشاریہ(ڈیٹا->param);
		}
		توڑو;

	حالت PARAMIOCUNSAVED: {
			paramiocunsaved_t *ڈیٹا = (paramiocunsaved_t *)arg;
			ڈیٹا->ret = param_قدر_unsaved(ڈیٹا->param);
		}
		توڑو;

	حالت PARAMIOCGET: {
			paramiocget_t *ڈیٹا = (paramiocget_t *)arg;

			اگر (ڈیٹا->deflt) {
				ڈیٹا->ret = param_get_default_قدر(ڈیٹا->param, ڈیٹا->val);

			} ورنہ {
				ڈیٹا->ret = param_get(ڈیٹا->param, ڈیٹا->val);
			}
		}
		توڑو;

	حالت PARAMIOCAUTOSAVE: {
			paramiocautosave_t *ڈیٹا = (paramiocautosave_t *)arg;
			param_control_autosave(ڈیٹا->enable);
		}
		توڑو;

	حالت PARAMIOCSET: {
			paramiocset_t *ڈیٹا = (paramiocset_t *)arg;

			اگر (ڈیٹا->notification) {
				ڈیٹا->ret = param_سیٹ(ڈیٹا->param, ڈیٹا->val);

			} ورنہ {
				ڈیٹا->ret = param_سیٹ_no_notification(ڈیٹا->param, ڈیٹا->val);
			}
		}
		توڑو;

	حالت PARAMIOCUSED: {
			paramiocused_t *ڈیٹا = (paramiocused_t *)arg;
			ڈیٹا->ret = param_مستعمل(ڈیٹا->param);
		}
		توڑو;

	حالت PARAMIOCSETUSED: {
			paramiocsetused_t *ڈیٹا = (paramiocsetused_t *)arg;
			param_سیٹ_مستعمل(ڈیٹا->param);
		}
		توڑو;

	حالت PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *ڈیٹا = (paramiocsetdefault_t *)arg;
			ڈیٹا->ret = param_سیٹ_default_قدر(ڈیٹا->param, ڈیٹا->val);
		}
		توڑو;

	حالت PARAMIOCRESET: {
			paramiocreset_t *ڈیٹا = (paramiocreset_t *)arg;

			اگر (ڈیٹا->notification) {
				ڈیٹا->ret = param_reset(ڈیٹا->param);

			} ورنہ {
				ڈیٹا->ret = param_reset_no_notification(ڈیٹا->param);
			}
		}
		توڑو;

	حالت PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *ڈیٹا = (paramiocresetgroup_t *)arg;

			اگر (ڈیٹا->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(ڈیٹا->گروہ, ڈیٹا->عدد_اندر_گروہ);

			} ورنہ اگر (ڈیٹا->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(ڈیٹا->گروہ, ڈیٹا->عدد_اندر_گروہ);

			} ورنہ {
				param_reset_all();
			}
		}
		توڑو;

	حالت PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *ڈیٹا = (paramiocsavedefault_t *)arg;
			ڈیٹا->ret = param_save_default(ڈیٹا->blocking);
		}
		توڑو;

	حالت PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *ڈیٹا = (paramiocloaddefault_t *)arg;
			ڈیٹا->ret = param_بھرو_default();
		}
		توڑو;

	حالت PARAMIOCEXPORT: {
			paramiocexport_t *ڈیٹا = (paramiocexport_t *)arg;
			ڈیٹا->ret = param_export(ڈیٹا->filename, خالی_اشاریہ);
		}
		توڑو;

	حالت PARAMIOCHASH: {
			paramiochash_t *ڈیٹا = (paramiochash_t *)arg;
			ڈیٹا->ret = param_hash_check();
		}
		توڑو;

	طےشدہ:
		ret = -ENOTTY;
		توڑو;
	}

	واپس ret;
}
