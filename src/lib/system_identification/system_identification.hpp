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

#અન્તર્ભાવ <lib/mathlib/math/filter/AlphaFilter.hpp>
#અન્તર્ભાવ <matrix/matrix/math.hpp>
#અન્તર્ભાવ <mathlib/mathlib.h>
#અન્તર્ભાવ <mathlib/math/filter/LowPassFilter2p.hpp>
#અન્તર્ભાવ <px4_platform_common/defines.h>

#અન્તર્ભાવ "arx_rls.hpp"

વર્ગ SystemIdentification final
{
જાહેર:
	SystemIdentification() = મૂળભૂત;
	~SystemIdentification() = મૂળભૂત;

	શૂન્ય reset(અચલ matrix::Vector<દશાંશ, 5> &id_state_init = {});
	શૂન્ય update(દશાંશ u, દશાંશ y); // update filters and model
	શૂન્ય update(); // update model only (to be called after updateFilters)
	શૂન્ય updateFilters(દશાંશ u, દશાંશ y);
	તાર્કિક areFiltersInitialized() અચલ { પાછા _are_filters_initialized; }
	શૂન્ય updateFitness();
	અચલ matrix::Vector<દશાંશ, 5> &getCoefficients() અચલ { પાછા _rls.getCoefficients(); }
	અચલ matrix::Vector<દશાંશ, 5> getVariances() અચલ { પાછા _rls.getVariances(); }
	અચલ matrix::Vector<દશાંશ, 5> &getDiffEstimate() અચલ { પાછા _rls.getDiffEstimate(); }
	દશાંશ getFitness() અચલ { પાછા _fitness_lpf.getState(); }
	દશાંશ getInnovation() અચલ { પાછા _rls.getInnovation(); }

	શૂન્ય setLpfCutoffFrequency(દશાંશ sample_freq, દશાંશ cutoff)
	{
		_u_lpf.set_cutoff_frequency(sample_freq, cutoff);
		_y_lpf.set_cutoff_frequency(sample_freq, cutoff);
	}
	શૂન્ય setHpfCutoffFrequency(દશાંશ sample_freq, દશાંશ cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	શૂન્ય setForgettingFactor(દશાંશ time_constant, દશાંશ dt) { _rls.setForgettingFactor(time_constant, dt); }
	શૂન્ય setFitnessLpfTimeConstant(દશાંશ time_constant, દશાંશ dt)
	{
		_fitness_lpf.setParameters(dt, time_constant);
		_dt = dt;
	}

	દશાંશ get_Filtered_આગત_Data() અચલ { પાછા _u_hpf; }
	દશાંશ getFilteredOutputData() અચલ { પાછા _y_hpf; }

ખાનગી:
	ArxRls<2, 2, 1> _rls;
	math::નીચલું_Pass_Filter2p<દશાંશ> _u_lpf{400.f, 30.f};
	math::નીચલું_Pass_Filter2p<દશાંશ> _y_lpf{400.f, 30.f};

	//TODO: replace by HighPassFilter class
	દશાંશ _alpha_hpf{0.f};
	દશાંશ _u_hpf{0.f};
	દશાંશ _y_hpf{0.f};

	દશાંશ _u_prev{0.f};
	દશાંશ _y_prev{0.f};

	તાર્કિક _are_filters_initialized{ખોટું};

	AlphaFilter<દશાંશ> _fitness_lpf;
	દશાંશ _dt{0.1f};
};
