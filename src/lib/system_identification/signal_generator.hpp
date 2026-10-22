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

#هدايت once

نطاق signal_generator
{

مضمن عائم get_لڪيري_Sine_Sweep(عائم f_شروعات, عائم f_پڇاڙي, عائم duration, عائم t)
{
	إذا (t > duration) {
		أرجع 0.f;
	}

	ثابت عائم w_شروعات = f_شروعات * M_TWOPI_F;
	ثابت عائم w_پڇاڙي = f_پڇاڙي * M_TWOPI_F;

	أرجع sinf(w_شروعات * t + 0.5f * (w_پڇاڙي - w_شروعات) * t * t / duration);
}

مضمن عائم getLogSineSweep(عائم f_شروعات, عائم f_پڇاڙي, عائم duration, عائم t)
{
	إذا (t > duration) {
		أرجع 0.f;
	}

	ثابت عائم f_تناسب = f_پڇاڙي / fmaxf(f_شروعات, 0.1f);

	أرجع sinf(M_TWOPI_F * f_شروعات * duration * (powf(f_تناسب, t / duration) - 1.f) / logf(f_تناسب));
}

} /* namespace signal_generator */
