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

#ಅನ್ತರ್ಭಾವ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <mathlib/math/filter/LowPassFilter2p.hpp>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

#ಅನ್ತರ್ಭಾವ "arx_rls.hpp"

ವರ್ಗ SystemIdentification final
{
ಸಾರ್ವಜನಿಕ:
	SystemIdentification() = ಸ್ವಭಾವ;
	~SystemIdentification() = ಸ್ವಭಾವ;

	ಶೂನ್ಯ reset(ಧ್ರುವ matrix::Vector<ಭಿನ್ನ, 5> &id_state_init = {});
	ಶೂನ್ಯ update(ಭಿನ್ನ u, ಭಿನ್ನ y); // update filters and model
	ಶೂನ್ಯ update(); // update model only (to be called after updateFilters)
	ಶೂನ್ಯ updateFilters(ಭಿನ್ನ u, ಭಿನ್ನ y);
	ತಾರ್ಕಿಕ areFiltersInitialized() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _are_filters_initialized; }
	ಶೂನ್ಯ updateFitness();
	ಧ್ರುವ matrix::Vector<ಭಿನ್ನ, 5> &getCoefficients() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _rls.getCoefficients(); }
	ಧ್ರುವ matrix::Vector<ಭಿನ್ನ, 5> getVariances() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _rls.getVariances(); }
	ಧ್ರುವ matrix::Vector<ಭಿನ್ನ, 5> &getDiffEstimate() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _rls.getDiffEstimate(); }
	ಭಿನ್ನ getFitness() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _fitness_lpf.getState(); }
	ಭಿನ್ನ getInnovation() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _rls.getInnovation(); }

	ಶೂನ್ಯ setLpfCutoffFrequency(ಭಿನ್ನ sample_freq, ಭಿನ್ನ cutoff)
	{
		_u_lpf.set_cutoff_frequency(sample_freq, cutoff);
		_y_lpf.set_cutoff_frequency(sample_freq, cutoff);
	}
	ಶೂನ್ಯ setHpfCutoffFrequency(ಭಿನ್ನ sample_freq, ಭಿನ್ನ cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	ಶೂನ್ಯ setForgettingFactor(ಭಿನ್ನ time_constant, ಭಿನ್ನ dt) { _rls.setForgettingFactor(time_constant, dt); }
	ಶೂನ್ಯ setFitnessLpfTimeConstant(ಭಿನ್ನ time_constant, ಭಿನ್ನ dt)
	{
		_fitness_lpf.setParameters(dt, time_constant);
		_dt = dt;
	}

	ಭಿನ್ನ get_Filtered_ಒಳಹರಿವು_Data() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _u_hpf; }
	ಭಿನ್ನ getFilteredOutputData() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _y_hpf; }

ಗುಪ್ತ:
	ArxRls<2, 2, 1> _rls;
	math::ಕೆಳಗಿನ_Pass_Filter2p<ಭಿನ್ನ> _u_lpf{400.f, 30.f};
	math::ಕೆಳಗಿನ_Pass_Filter2p<ಭಿನ್ನ> _y_lpf{400.f, 30.f};

	//TODO: replace by HighPassFilter class
	ಭಿನ್ನ _alpha_hpf{0.f};
	ಭಿನ್ನ _u_hpf{0.f};
	ಭಿನ್ನ _y_hpf{0.f};

	ಭಿನ್ನ _u_prev{0.f};
	ಭಿನ್ನ _y_prev{0.f};

	ತಾರ್ಕಿಕ _are_filters_initialized{ಅಸತ್ಯ};

	AlphaFilter<ಭಿನ್ನ> _fitness_lpf;
	ಭಿನ್ನ _dt{0.1f};
};
