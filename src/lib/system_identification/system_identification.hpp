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
	SystemIdentification() = पूर्वनिर्धारित;
	~SystemIdentification() = पूर्वनिर्धारित;

	शून्य reset(स्थिरांक matrix::Vector<दशांश, 5> &id_state_init = {});
	शून्य update(दशांश u, दशांश y); // update filters and model
	शून्य update(); // update model only (to be called after updateFilters)
	शून्य updateFilters(दशांश u, दशांश y);
	तार्किक areFiltersInitialized() स्थिरांक { परत _are_filters_initialized; }
	शून्य updateFitness();
	स्थिरांक matrix::Vector<दशांश, 5> &getCoefficients() स्थिरांक { परत _rls.getCoefficients(); }
	स्थिरांक matrix::Vector<दशांश, 5> getVariances() स्थिरांक { परत _rls.getVariances(); }
	स्थिरांक matrix::Vector<दशांश, 5> &getDiffEstimate() स्थिरांक { परत _rls.getDiffEstimate(); }
	दशांश getFitness() स्थिरांक { परत _fitness_lpf.getState(); }
	दशांश getInnovation() स्थिरांक { परत _rls.getInnovation(); }

	शून्य setLpfCutoffFrequency(दशांश sample_freq, दशांश cutoff)
	{
		_u_lpf.set_cutoff_frequency(sample_freq, cutoff);
		_y_lpf.set_cutoff_frequency(sample_freq, cutoff);
	}
	शून्य setHpfCutoffFrequency(दशांश sample_freq, दशांश cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	शून्य setForgettingFactor(दशांश time_constant, दशांश dt) { _rls.setForgettingFactor(time_constant, dt); }
	शून्य setFitnessLpfTimeConstant(दशांश time_constant, दशांश dt)
	{
		_fitness_lpf.setParameters(dt, time_constant);
		_dt = dt;
	}

	दशांश get_Filtered_आदान_Data() स्थिरांक { परत _u_hpf; }
	दशांश getFilteredOutputData() स्थिरांक { परत _y_hpf; }

खाजगी:
	ArxRls<2, 2, 1> _rls;
	math::खालचा_Pass_Filter2p<दशांश> _u_lpf{400.f, 30.f};
	math::खालचा_Pass_Filter2p<दशांश> _y_lpf{400.f, 30.f};

	//TODO: replace by HighPassFilter class
	दशांश _alpha_hpf{0.f};
	दशांश _u_hpf{0.f};
	दशांश _y_hpf{0.f};

	दशांश _u_prev{0.f};
	दशांश _y_prev{0.f};

	तार्किक _are_filters_initialized{असत्य};

	AlphaFilter<दशांश> _fitness_lpf;
	दशांश _dt{0.1f};
};
