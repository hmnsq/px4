/****************************************************************************
 *
 *   Copyright (c) 2024 PX4 Development Team. All rights reserved.
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
 * @file signal_generator.hpp
 */

#pragma once

നാമാകാശ signal_generator
{

വരിയിൽ ദശാംശം getLinearSineSweep(ദശാംശം f_start, ദശാംശം f_end, ദശാംശം duration, ദശാംശം t)
{
	എങ്കിൽ (t > duration) {
		തിരികെ 0.f;
	}

	സ്ഥിരം ദശാംശം w_start = f_start * M_TWOPI_F;
	സ്ഥിരം ദശാംശം w_end = f_end * M_TWOPI_F;

	തിരികെ sinf(w_start * t + 0.5f * (w_end - w_start) * t * t / duration);
}

വരിയിൽ ദശാംശം getLogSineSweep(ദശാംശം f_start, ദശാംശം f_end, ദശാംശം duration, ദശാംശം t)
{
	എങ്കിൽ (t > duration) {
		തിരികെ 0.f;
	}

	സ്ഥിരം ദശാംശം f_ratio = f_end / fmaxf(f_start, 0.1f);

	തിരികെ sinf(M_TWOPI_F * f_start * duration * (powf(f_ratio, t / duration) - 1.f) / logf(f_ratio));
}

} /* namespace signal_generator */
