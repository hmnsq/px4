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

#ವ್ಯಾಖ್ಯಾನಿಸು PARAM_IMPLEMENTATION

#ಅನ್ತರ್ಭಾವ <errno.h>

#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "parameters_ioctl.h"
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

ಪೂರ್ಣಾಂಕ	param_ioctl(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cmd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg)
{
	ಪೂರ್ಣಾಂಕ ret = OK;

	ಆಯ್ಕೆ (cmd) {
	ಸ್ಥಿತಿ PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCFIND: {
			paramiocfind_t *ದತ್ತಾಂಶ = (paramiocfind_t *)arg;

			ಆದರೆ (ದತ್ತಾಂಶ->notification) {
				ದತ್ತಾಂಶ->ret = param_ಕಂಡುಹಿಡಿ(ದತ್ತಾಂಶ->name);

			} ಇಲ್ಲದಿದ್ದರೆ {
				ದತ್ತಾಂಶ->ret = param_ಕಂಡುಹಿಡಿ_no_notification(ದತ್ತಾಂಶ->name);
			}
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCCOUNTUSED: {
			paramioccountused_t *ದತ್ತಾಂಶ = (paramioccountused_t *)arg;
			ದತ್ತಾಂಶ->ret = param_ಎಣಿಕೆ_ಬಳಸಿದ();
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *ದತ್ತಾಂಶ = (paramiocforusedindex_t *)arg;
			ದತ್ತಾಂಶ->ret =  param_for_ಬಳಸಿದ_ಸೂಚ್ಯಂಕ(ದತ್ತಾಂಶ->ಸೂಚ್ಯಂಕ);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *ದತ್ತಾಂಶ = (paramiocgetusedindex_t *)arg;
			ದತ್ತಾಂಶ->ret = param_get_ಬಳಸಿದ_ಸೂಚ್ಯಂಕ(ದತ್ತಾಂಶ->param);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCUNSAVED: {
			paramiocunsaved_t *ದತ್ತಾಂಶ = (paramiocunsaved_t *)arg;
			ದತ್ತಾಂಶ->ret = param_ಮೌಲ್ಯ_unsaved(ದತ್ತಾಂಶ->param);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCGET: {
			paramiocget_t *ದತ್ತಾಂಶ = (paramiocget_t *)arg;

			ಆದರೆ (ದತ್ತಾಂಶ->deflt) {
				ದತ್ತಾಂಶ->ret = param_get_default_ಮೌಲ್ಯ(ದತ್ತಾಂಶ->param, ದತ್ತಾಂಶ->val);

			} ಇಲ್ಲದಿದ್ದರೆ {
				ದತ್ತಾಂಶ->ret = param_get(ದತ್ತಾಂಶ->param, ದತ್ತಾಂಶ->val);
			}
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCAUTOSAVE: {
			paramiocautosave_t *ದತ್ತಾಂಶ = (paramiocautosave_t *)arg;
			param_control_autosave(ದತ್ತಾಂಶ->enable);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCSET: {
			paramiocset_t *ದತ್ತಾಂಶ = (paramiocset_t *)arg;

			ಆದರೆ (ದತ್ತಾಂಶ->notification) {
				ದತ್ತಾಂಶ->ret = param_ಗಣ(ದತ್ತಾಂಶ->param, ದತ್ತಾಂಶ->val);

			} ಇಲ್ಲದಿದ್ದರೆ {
				ದತ್ತಾಂಶ->ret = param_ಗಣ_no_notification(ದತ್ತಾಂಶ->param, ದತ್ತಾಂಶ->val);
			}
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCUSED: {
			paramiocused_t *ದತ್ತಾಂಶ = (paramiocused_t *)arg;
			ದತ್ತಾಂಶ->ret = param_ಬಳಸಿದ(ದತ್ತಾಂಶ->param);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCSETUSED: {
			paramiocsetused_t *ದತ್ತಾಂಶ = (paramiocsetused_t *)arg;
			param_ಗಣ_ಬಳಸಿದ(ದತ್ತಾಂಶ->param);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *ದತ್ತಾಂಶ = (paramiocsetdefault_t *)arg;
			ದತ್ತಾಂಶ->ret = param_ಗಣ_default_ಮೌಲ್ಯ(ದತ್ತಾಂಶ->param, ದತ್ತಾಂಶ->val);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCRESET: {
			paramiocreset_t *ದತ್ತಾಂಶ = (paramiocreset_t *)arg;

			ಆದರೆ (ದತ್ತಾಂಶ->notification) {
				ದತ್ತಾಂಶ->ret = param_reset(ದತ್ತಾಂಶ->param);

			} ಇಲ್ಲದಿದ್ದರೆ {
				ದತ್ತಾಂಶ->ret = param_reset_no_notification(ದತ್ತಾಂಶ->param);
			}
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *ದತ್ತಾಂಶ = (paramiocresetgroup_t *)arg;

			ಆದರೆ (ದತ್ತಾಂಶ->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(ದತ್ತಾಂಶ->ಗುಂಪು, ದತ್ತಾಂಶ->ಅಂಕೆ_ಒಳ_ಗುಂಪು);

			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ದತ್ತಾಂಶ->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(ದತ್ತಾಂಶ->ಗುಂಪು, ದತ್ತಾಂಶ->ಅಂಕೆ_ಒಳ_ಗುಂಪು);

			} ಇಲ್ಲದಿದ್ದರೆ {
				param_reset_all();
			}
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *ದತ್ತಾಂಶ = (paramiocsavedefault_t *)arg;
			ದತ್ತಾಂಶ->ret = param_save_default(ದತ್ತಾಂಶ->blocking);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *ದತ್ತಾಂಶ = (paramiocloaddefault_t *)arg;
			ದತ್ತಾಂಶ->ret = param_ತುಂಬು_default();
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCEXPORT: {
			paramiocexport_t *ದತ್ತಾಂಶ = (paramiocexport_t *)arg;
			ದತ್ತಾಂಶ->ret = param_export(ದತ್ತಾಂಶ->filename, ಶೂನ್ಯಸೂಚಕ);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PARAMIOCHASH: {
			paramiochash_t *ದತ್ತಾಂಶ = (paramiochash_t *)arg;
			ದತ್ತಾಂಶ->ret = param_hash_check();
		}
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = -ENOTTY;
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ ret;
}
