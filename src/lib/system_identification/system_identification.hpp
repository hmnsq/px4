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

#अन्तर्भाव <lib/mathlib/math/filter/AlphaFilter.hpp>
#अन्तर्भाव <matrix/matrix/math.hpp>
#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <mathlib/math/filter/LowPassFilter2p.hpp>
#अन्तर्भाव <px4_platform_common/defines.h>

#अन्तर्भाव "arx_rls.hpp"

वर्ग SystemIdentification final
{
सार्वजनिक:
	SystemIdentification() = स्वभाव;
	~SystemIdentification() = स्वभाव;

	शून्य reset(ध्रुव matrix::Vector<भिन्न, 5> &id_state_init = {});
	शून्य update(भिन्न u, भिन्न y); // update filters and model
	शून्य update(); // update model only (to be called after updateFilters)
	शून्य updateFilters(भिन्न u, भिन्न y);
	तार्किक areFiltersInitialized() ध्रुव { प्रत्यागम _are_filters_initialized; }
	शून्य updateFitness();
	ध्रुव matrix::Vector<भिन्न, 5> &getCoefficients() ध्रुव { प्रत्यागम _rls.getCoefficients(); }
	ध्रुव matrix::Vector<भिन्न, 5> getVariances() ध्रुव { प्रत्यागम _rls.getVariances(); }
	ध्रुव matrix::Vector<भिन्न, 5> &getDiffEstimate() ध्रुव { प्रत्यागम _rls.getDiffEstimate(); }
	भिन्न getFitness() ध्रुव { प्रत्यागम _fitness_lpf.getState(); }
	भिन्न getInnovation() ध्रुव { प्रत्यागम _rls.getInnovation(); }

	शून्य setLpfCutoffFrequency(भिन्न sample_freq, भिन्न cutoff)
	{
		_u_lpf.set_cutoff_frequency(sample_freq, cutoff);
		_y_lpf.set_cutoff_frequency(sample_freq, cutoff);
	}
	शून्य setHpfCutoffFrequency(भिन्न sample_freq, भिन्न cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	शून्य setForgettingFactor(भिन्न time_constant, भिन्न dt) { _rls.setForgettingFactor(time_constant, dt); }
	शून्य setFitnessLpfTimeConstant(भिन्न time_constant, भिन्न dt)
	{
		_fitness_lpf.setParameters(dt, time_constant);
		_dt = dt;
	}

	भिन्न get_Filtered_निवेश_Data() ध्रुव { प्रत्यागम _u_hpf; }
	भिन्न getFilteredOutputData() ध्रुव { प्रत्यागम _y_hpf; }

गुप्त:
	ArxRls<2, 2, 1> _rls;
	math::निचला_Pass_Filter2p<भिन्न> _u_lpf{400.f, 30.f};
	math::निचला_Pass_Filter2p<भिन्न> _y_lpf{400.f, 30.f};

	//TODO: replace by HighPassFilter class
	भिन्न _alpha_hpf{0.f};
	भिन्न _u_hpf{0.f};
	भिन्न _y_hpf{0.f};

	भिन्न _u_prev{0.f};
	भिन्न _y_prev{0.f};

	तार्किक _are_filters_initialized{असत्य};

	AlphaFilter<भिन्न> _fitness_lpf;
	भिन्न _dt{0.1f};
};
