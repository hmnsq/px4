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

#define PARAM_IMPLEMENTATION

#شامل <errno.h>

#شامل "param.h"
#شامل "parameters_ioctl.h"
#شامل <px4_platform_common/defines.h>

صحیح	param_ioctl(unsigned صحیح cmd, unsigned long arg)
{
	صحیح ret = OK;

	انتخاب (cmd) {
	حالت PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		بشکن;

	حالت PARAMIOCFIND: {
			paramiocfind_t *data = (paramiocfind_t *)arg;

			اگر (data->notification) {
				data->ret = param_find(data->name);

			} وگرنه {
				data->ret = param_find_no_notification(data->name);
			}
		}
		بشکن;

	حالت PARAMIOCCOUNTUSED: {
			paramioccountused_t *data = (paramioccountused_t *)arg;
			data->ret = param_شمار_used();
		}
		بشکن;

	حالت PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *data = (paramiocforusedindex_t *)arg;
			data->ret =  param_for_used_index(data->index);
		}
		بشکن;

	حالت PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *data = (paramiocgetusedindex_t *)arg;
			data->ret = param_get_used_index(data->param);
		}
		بشکن;

	حالت PARAMIOCUNSAVED: {
			paramiocunsaved_t *data = (paramiocunsaved_t *)arg;
			data->ret = param_مقدار_unsaved(data->param);
		}
		بشکن;

	حالت PARAMIOCGET: {
			paramiocget_t *data = (paramiocget_t *)arg;

			اگر (data->deflt) {
				data->ret = param_get_default_مقدار(data->param, data->val);

			} وگرنه {
				data->ret = param_get(data->param, data->val);
			}
		}
		بشکن;

	حالت PARAMIOCAUTOSAVE: {
			paramiocautosave_t *data = (paramiocautosave_t *)arg;
			param_control_autosave(data->enable);
		}
		بشکن;

	حالت PARAMIOCSET: {
			paramiocset_t *data = (paramiocset_t *)arg;

			اگر (data->notification) {
				data->ret = param_set(data->param, data->val);

			} وگرنه {
				data->ret = param_set_no_notification(data->param, data->val);
			}
		}
		بشکن;

	حالت PARAMIOCUSED: {
			paramiocused_t *data = (paramiocused_t *)arg;
			data->ret = param_used(data->param);
		}
		بشکن;

	حالت PARAMIOCSETUSED: {
			paramiocsetused_t *data = (paramiocsetused_t *)arg;
			param_set_used(data->param);
		}
		بشکن;

	حالت PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *data = (paramiocsetdefault_t *)arg;
			data->ret = param_set_default_مقدار(data->param, data->val);
		}
		بشکن;

	حالت PARAMIOCRESET: {
			paramiocreset_t *data = (paramiocreset_t *)arg;

			اگر (data->notification) {
				data->ret = param_reset(data->param);

			} وگرنه {
				data->ret = param_reset_no_notification(data->param);
			}
		}
		بشکن;

	حالت PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *data = (paramiocresetgroup_t *)arg;

			اگر (data->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(data->group, data->رقم_in_group);

			} وگرنه اگر (data->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(data->group, data->رقم_in_group);

			} وگرنه {
				param_reset_all();
			}
		}
		بشکن;

	حالت PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *data = (paramiocsavedefault_t *)arg;
			data->ret = param_save_default(data->blocking);
		}
		بشکن;

	حالت PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *data = (paramiocloaddefault_t *)arg;
			data->ret = param_load_default();
		}
		بشکن;

	حالت PARAMIOCEXPORT: {
			paramiocexport_t *data = (paramiocexport_t *)arg;
			data->ret = param_export(data->filename, nullptr);
		}
		بشکن;

	حالت PARAMIOCHASH: {
			paramiochash_t *data = (paramiochash_t *)arg;
			data->ret = param_hash_check();
		}
		بشکن;

	پیشفرض:
		ret = -ENOTTY;
		بشکن;
	}

	بازگشت ret;
}
