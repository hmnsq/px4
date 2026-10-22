/****************************************************************************
 *
 * Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

/*
 * @file AirspeedDirectionController.hpp
 *
 * Original Author:  Thomas Stastny <tstastny@ethz.ch>
 * Refactored to better suite new control API: Roman Bapst <roman@auterion.com>
 *
 * * Notes:
 * - The wind estimate should be dynamic enough to capture ~1-2 second length gusts,
 *   Otherwise the performance will suffer.
 *
 * Acknowledgements and References:
 *
 * The logic is mostly based on [1] and Paper III of [2].
 * TODO: Concise, up to date documentation and stability analysis for the following
 *       implementation.
 *
 * [1] T. Stastny and R. Siegwart. "On Flying Backwards: Preventing Run-away of
 *     Small, Low-speed, Fixed-wing UAVs in Strong Winds". IEEE International Conference
 *     on Intelligent Robots and Systems (IROS). 2019.
 *     https://arxiv.org/pdf/1908.01381.pdf
 * [2] T. Stastny. "Low-Altitude Control and Local Re-Planning Strategies for Small
 *     Fixed-Wing UAVs". Doctoral Thesis, ETH Zürich. 2020.
 *     https://tstastny.github.io/pdf/tstastny_phd_thesis_wcover.pdf
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PX4_AIRSPEEDDIRECTIONONTROLLER_HPP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PX4_AIRSPEEDDIRECTIONONTROLLER_HPP

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>

ਵਰਗ AirspeedDirectionController
{
ਜਨਤਕ:
	AirspeedDirectionController();

	ਖਾਲੀ ਸੈੱਟ_PGain_From_Period_And_Damping(ਦਸ਼ਮਲਵ damping, ਦਸ਼ਮਲਵ period) {p_gain_ = 4.f * M_PI_F * damping / math::max(period, FLT_EPSILON);}
	ਦਸ਼ਮਲਵ controlHeading(ਅਚਲ ਦਸ਼ਮਲਵ heading_sp, ਅਚਲ ਦਸ਼ਮਲਵ heading, ਅਚਲ ਦਸ਼ਮਲਵ airspeed) ਅਚਲ;

ਨਿਜੀ:
	ਦਸ਼ਮਲਵ p_gain_{0.8885f}; // proportional gain (computed from period_ and damping_) [rad/s]
};

#ਜੇ_ਅੰਤ //PX4_AIRSPEEDDIRECTIONONTROLLER_HPP
