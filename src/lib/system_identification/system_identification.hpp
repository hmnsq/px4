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
 * @file system_identification.hpp
 *
 * @author Mathieu Bresciani <mathieu@auterion.com>
 */

#pragma once

#ضمن <lib/mathlib/math/filter/AlphaFilter.hpp>
#ضمن <matrix/matrix/math.hpp>
#ضمن <mathlib/mathlib.h>
#ضمن <mathlib/math/filter/LowPassFilter2p.hpp>
#ضمن <px4_platform_common/defines.h>

#ضمن "arx_rls.hpp"

صنف SystemIdentification final
{
عام:
	SystemIdentification() = افتراضي;
	~SystemIdentification() = افتراضي;

	فراغ reset(ثابت matrix::Vector<عائم, 5> &id_state_init = {});
	فراغ update(عائم u, عائم y); // update filters and model
	فراغ update(); // update model only (to be called after updateFilters)
	فراغ updateFilters(عائم u, عائم y);
	منطقي areFiltersInitialized() ثابت { ܦܢܐ _are_filters_initialized; }
	فراغ updateFitness();
	ثابت matrix::Vector<عائم, 5> &getCoefficients() ثابت { ܦܢܐ _rls.getCoefficients(); }
	ثابت matrix::Vector<عائم, 5> getVariances() ثابت { ܦܢܐ _rls.getVariances(); }
	ثابت matrix::Vector<عائم, 5> &getDiffEstimate() ثابت { ܦܢܐ _rls.getDiffEstimate(); }
	عائم getFitness() ثابت { ܦܢܐ _fitness_lpf.getState(); }
	عائم getInnovation() ثابت { ܦܢܐ _rls.getInnovation(); }

	فراغ setLpfCutoffFrequency(عائم sample_freq, عائم cutoff)
	{
		_u_lpf.set_cutoff_frequency(sample_freq, cutoff);
		_y_lpf.set_cutoff_frequency(sample_freq, cutoff);
	}
	فراغ setHpfCutoffFrequency(عائم sample_freq, عائم cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	فراغ setForgettingFactor(عائم time_constant, عائم dt) { _rls.setForgettingFactor(time_constant, dt); }
	فراغ setFitnessLpfTimeConstant(عائم time_constant, عائم dt)
	{
		_fitness_lpf.setParameters(dt, time_constant);
		_dt = dt;
	}

	عائم getFilteredInputData() ثابت { ܦܢܐ _u_hpf; }
	عائم getFilteredOutputData() ثابت { ܦܢܐ _y_hpf; }

خاص:
	ArxRls<2, 2, 1> _rls;
	math::LowPassFilter2p<عائم> _u_lpf{400.f, 30.f};
	math::LowPassFilter2p<عائم> _y_lpf{400.f, 30.f};

	//TODO: replace by HighPassFilter class
	عائم _alpha_hpf{0.f};
	عائم _u_hpf{0.f};
	عائم _y_hpf{0.f};

	عائم _u_prev{0.f};
	عائم _y_prev{0.f};

	منطقي _are_filters_initialized{ܕܓܠܐ};

	AlphaFilter<عائم> _fitness_lpf;
	عائم _dt{0.1f};
};
