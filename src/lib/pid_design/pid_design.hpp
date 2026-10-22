/****************************************************************************
 *
 *   Copyright (c) 2020-2021 PX4 Development Team. All rights reserved.
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
 * @file pid_design.hpp
 * @brief Helper functions to design PID controllers using identified models
 *
 * @author Mathieu Bresciani <mathieu@auterion.com>
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>
#ଅନ୍ତର୍ଭାଵ <mathlib/mathlib.h>

ନାମାକାଶ pid_design
{

/*
* Compute a set of PID gains using General Minimum Variance Control law design
*
* @param num: the coefficients of the numerator of the discrete-time plant transfer function
* @param den: the coefficients of the denominator of the discrete-time plant transfer function
* @param dt: the sampling time in seconds
* @param sigma: the desired closed-loop rise time in seconds
* @param delta: the damping index (between 0 and 2). 0 is critical damping, 1 is Butterworth
* @param lbda: the "detuning" coefficients. This affects the gain of the controller kc only (increase to detune the controller)
*
* @return The PID gains in standard form: u = kc*[1 + ki*dt + kd/dt]*e
*         kc: the controller gain
*         ki: the integral gain (= 1/Ti)
*         kd: the derivative gain (= Td)
*
* Reference:
* T.Yamatoto, K.Fujii and M.Kaneda, Design and implementation of a self-tuning pid controller, 1998
*/

ଧାଡ଼ିଭିତରେ matrix::Vector3f computePidGmvc(ଅଚଳ matrix::Vector3f &ଅଙ୍କ, ଅଚଳ matrix::Vector3f &den, ଦଶମିକ dt,
				       ଦଶମିକ sigma = 0.1f, ଦଶମିକ delta = 1.f, ଦଶମିକ lbda = 0.5f)
{
	sigma = math::constrain(sigma, 0.01f, 1.f);
	delta = math::constrain(delta, 0.f, 1.f);
	lbda = math::constrain(lbda, 0.f, 10.f);

	ଅଚଳ ଦଶମିକ a1 = den(1);
	ଅଚଳ ଦଶମିକ a2 = den(2);
	ଅଚଳ ଦଶମିକ b0 = ଅଙ୍କ(0);
	ଅଚଳ ଦଶମିକ b1 = ଅଙ୍କ(1);
	ଅଚଳ ଦଶମିକ b2 = ଅଙ୍କ(2);

	// Solve GMVC law (see derivation in pid_synthesis_symbolic.py)
	ଅଚଳ ଦଶମିକ rho = dt / sigma;
	ଅଚଳ ଦଶମିକ mu = 0.25f * (1.f - delta) + 0.51f * delta; // mu is in the interval [0.25 0.51]
	ଅଚଳ ଦଶମିକ p1 = -2.f * expf(-rho / (2.f * mu)) * cosf(sqrtf(4.f * mu - 1.f) * rho / (2.f * mu));
	ଅଚଳ ଦଶମିକ p2 = expf(-rho / mu);
	ଅଚଳ ଦଶମିକ e1 = -a1 + p1 + 1.f;
	ଅଚଳ ଦଶମିକ f0 = -a1 * e1 + a1 - a2 + e1 + p2;
	ଅଚଳ ଦଶମିକ f1 = a1 * e1 - a2 * e1 + a2;
	ଅଚଳ ଦଶମିକ f2 = a2 * e1;

	// Translate to PID gains
	ଅଚଳ ଦଶମିକ nu = lbda + (e1 + 1.f) * (b0 + b1 + b2);

	ଯଦି (fabsf(nu) < FLT_EPSILON) {
		ଫେରନ୍ତୁ matrix::Vector3f();
	}

	ଅଚଳ ଦଶମିକ kc = -(f1 + 2.f * f2) / nu;
	ଦଶମିକ ki = -(f0 + f1 + f2) / (dt * (f1 + 2.f * f2));
	ki /= 5.f; // This is not part of the original implementation but is required to produce reasonable gains
	ଅଚଳ ଦଶମିକ kd = -dt * f2 / (f1 + 2.f * f2);

	ଫେରନ୍ତୁ matrix::Vector3f(kc, ki, kd);
}
} // namespace pid_design
