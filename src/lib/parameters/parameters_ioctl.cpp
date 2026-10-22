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

#تعریف_کن PARAM_IMPLEMENTATION

#ضمن <errno.h>

#ضمن "param.h"
#ضمن "parameters_ioctl.h"
#ضمن <px4_platform_common/defines.h>

صحيح	param_ioctl(بیعلامت صحيح cmd, بیعلامت دراز arg)
{
	صحيح ret = OK;

	بدل (cmd) {
	حالة PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		اكسر;

	حالة PARAMIOCFIND: {
			paramiocfind_t *داده = (paramiocfind_t *)arg;

			إذا (داده->notification) {
				داده->ret = param_پیدا_کن(داده->name);

			} وإلا {
				داده->ret = param_پیدا_کن_no_notification(داده->name);
			}
		}
		اكسر;

	حالة PARAMIOCCOUNTUSED: {
			paramioccountused_t *داده = (paramioccountused_t *)arg;
			داده->ret = param_شمار_استفاده_شده();
		}
		اكسر;

	حالة PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *داده = (paramiocforusedindex_t *)arg;
			داده->ret =  param_for_استفاده_شده_شاخص(داده->شاخص);
		}
		اكسر;

	حالة PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *داده = (paramiocgetusedindex_t *)arg;
			داده->ret = param_get_استفاده_شده_شاخص(داده->param);
		}
		اكسر;

	حالة PARAMIOCUNSAVED: {
			paramiocunsaved_t *داده = (paramiocunsaved_t *)arg;
			داده->ret = param_مقدار_unsaved(داده->param);
		}
		اكسر;

	حالة PARAMIOCGET: {
			paramiocget_t *داده = (paramiocget_t *)arg;

			إذا (داده->deflt) {
				داده->ret = param_get_default_مقدار(داده->param, داده->val);

			} وإلا {
				داده->ret = param_get(داده->param, داده->val);
			}
		}
		اكسر;

	حالة PARAMIOCAUTOSAVE: {
			paramiocautosave_t *داده = (paramiocautosave_t *)arg;
			param_control_autosave(داده->enable);
		}
		اكسر;

	حالة PARAMIOCSET: {
			paramiocset_t *داده = (paramiocset_t *)arg;

			إذا (داده->notification) {
				داده->ret = param_مجموعه(داده->param, داده->val);

			} وإلا {
				داده->ret = param_مجموعه_no_notification(داده->param, داده->val);
			}
		}
		اكسر;

	حالة PARAMIOCUSED: {
			paramiocused_t *داده = (paramiocused_t *)arg;
			داده->ret = param_استفاده_شده(داده->param);
		}
		اكسر;

	حالة PARAMIOCSETUSED: {
			paramiocsetused_t *داده = (paramiocsetused_t *)arg;
			param_مجموعه_استفاده_شده(داده->param);
		}
		اكسر;

	حالة PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *داده = (paramiocsetdefault_t *)arg;
			داده->ret = param_مجموعه_default_مقدار(داده->param, داده->val);
		}
		اكسر;

	حالة PARAMIOCRESET: {
			paramiocreset_t *داده = (paramiocreset_t *)arg;

			إذا (داده->notification) {
				داده->ret = param_reset(داده->param);

			} وإلا {
				داده->ret = param_reset_no_notification(داده->param);
			}
		}
		اكسر;

	حالة PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *داده = (paramiocresetgroup_t *)arg;

			إذا (داده->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(داده->گروه, داده->رقم_درون_گروه);

			} وإلا إذا (داده->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(داده->گروه, داده->رقم_درون_گروه);

			} وإلا {
				param_reset_all();
			}
		}
		اكسر;

	حالة PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *داده = (paramiocsavedefault_t *)arg;
			داده->ret = param_save_default(داده->blocking);
		}
		اكسر;

	حالة PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *داده = (paramiocloaddefault_t *)arg;
			داده->ret = param_بارگیری_default();
		}
		اكسر;

	حالة PARAMIOCEXPORT: {
			paramiocexport_t *داده = (paramiocexport_t *)arg;
			داده->ret = param_export(داده->filename, اشاره_تهی);
		}
		اكسر;

	حالة PARAMIOCHASH: {
			paramiochash_t *داده = (paramiochash_t *)arg;
			داده->ret = param_hash_check();
		}
		اكسر;

	افتراضي:
		ret = -ENOTTY;
		اكسر;
	}

	أرجع ret;
}
