/****************************************************************************
 *
 *   Copyright (c) 2015 PX4 Development Team. All rights reserved.
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
 * @file flashparams.h
 *
 * Global flash based parameter store.
 *
 * This provides the mechanisms to interface to the PX4
 * parameter system but replace the IO with non file based flash
 * i/o routines. So that the code my be implemented on a SMALL memory
 * foot print device.
 *
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _SYSTEMLIB_FLASHPARAMS_FLASHPARAMS_H
#ವ್ಯಾಖ್ಯಾನಿಸು _SYSTEMLIB_FLASHPARAMS_FLASHPARAMS_H

#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ "../DynamicSparseLayer.h"

__BEGIN_DECLS

/*
 * When using the flash based parameter store we have to force
 * the param_values and 2 functions to be global
 */

__EXPORT ಬಾಹ್ಯ DynamicSparseLayer user_config;
__EXPORT ಪೂರ್ಣಾಂಕ param_ಗಣ_external(param_t param, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val, bool mark_saved, bool notify_changes);
__EXPORT ಶೂನ್ಯ param_get_external(param_t param, ಶೂನ್ಯ *val);

/* The interface hooks to the Flash based storage. The caller is responsible for locking */
__EXPORT ಪೂರ್ಣಾಂಕ flash_param_save(param_filter_func filter);
__EXPORT ಪೂರ್ಣಾಂಕ flash_param_import();

__END_DECLS

#ಆದರೆ_ಅಂತ್ಯ /* _SYSTEMLIB_FLASHPARAMS_FLASHPARAMS_H */
