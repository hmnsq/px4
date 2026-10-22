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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _SYSTEMLIB_FLASHPARAMS_FLASHPARAMS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _SYSTEMLIB_FLASHPARAMS_FLASHPARAMS_H

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "../DynamicSparseLayer.h"

__BEGIN_DECLS

/*
 * When using the flash based parameter store we have to force
 * the param_values and 2 functions to be global
 */

__EXPORT ਬਾਹਰੀ DynamicSparseLayer user_config;
__EXPORT ਪੂਰਨ param_ਸੈੱਟ_external(param_t param, ਅਚਲ ਖਾਲੀ *val, bool mark_saved, bool notify_changes);
__EXPORT ਖਾਲੀ param_get_external(param_t param, ਖਾਲੀ *val);

/* The interface hooks to the Flash based storage. The caller is responsible for locking */
__EXPORT ਪੂਰਨ flash_param_save(param_filter_func filter);
__EXPORT ਪੂਰਨ flash_param_import();

__END_DECLS

#ਜੇ_ਅੰਤ /* _SYSTEMLIB_FLASHPARAMS_FLASHPARAMS_H */
