/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
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
 * @file ppm_decode.h
 *
 * PPM input decoder.
 */

#هدايت once

#ضمن <drivers/drv_hrt.h>

/**
 * Maximum number of channels that we will decode.
 */
#وصف_ڪريو PPM_MAX_CHANNELS	12

/* PPM input nominal min/max values */
#وصف_ڪريو PPM_MIN 1000
#وصف_ڪريو PPM_MAX 2000
#وصف_ڪريو PPM_MID ((PPM_MIN + PPM_MAX) / 2)

__BEGIN_DECLS

/*
 * PPM decoder state
 */
__EXPORT خارجي uint16_t	ppm_بفر[PPM_MAX_CHANNELS];	/**< decoded PPM channel values */
__EXPORT خارجي uint16_t	ppm_چوکٺو_ڊيگهائي;				/**< length of the decoded PPM frame (includes gap) */
__EXPORT خارجي بنانشان	ppm_decoded_channels;	/**< count of decoded channels */
__EXPORT خارجي hrt_abstime	ppm_آخري_valid_decode;	/**< timestamp of the last valid decode */

__END_DECLS
