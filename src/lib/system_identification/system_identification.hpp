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

#ଅନ୍ତର୍ଭାଵ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>
#ଅନ୍ତର୍ଭାଵ <mathlib/mathlib.h>
#ଅନ୍ତର୍ଭାଵ <mathlib/math/filter/LowPassFilter2p.hpp>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>

#ଅନ୍ତର୍ଭାଵ "arx_rls.hpp"

ଶ୍ରେଣୀ SystemIdentification final
{
ସର୍ବସାଧାରଣ:
	SystemIdentification() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	~SystemIdentification() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଶୂନ୍ୟ reset(ଅଚଳ matrix::Vector<ଦଶମିକ, 5> &id_state_init = {});
	ଶୂନ୍ୟ update(ଦଶମିକ u, ଦଶମିକ y); // update filters and model
	ଶୂନ୍ୟ update(); // update model only (to be called after updateFilters)
	ଶୂନ୍ୟ updateFilters(ଦଶମିକ u, ଦଶମିକ y);
	ତାର୍କିକ areFiltersInitialized() ଅଚଳ { ଫେରନ୍ତୁ _are_filters_initialized; }
	ଶୂନ୍ୟ updateFitness();
	ଅଚଳ matrix::Vector<ଦଶମିକ, 5> &getCoefficients() ଅଚଳ { ଫେରନ୍ତୁ _rls.getCoefficients(); }
	ଅଚଳ matrix::Vector<ଦଶମିକ, 5> getVariances() ଅଚଳ { ଫେରନ୍ତୁ _rls.getVariances(); }
	ଅଚଳ matrix::Vector<ଦଶମିକ, 5> &getDiffEstimate() ଅଚଳ { ଫେରନ୍ତୁ _rls.getDiffEstimate(); }
	ଦଶମିକ getFitness() ଅଚଳ { ଫେରନ୍ତୁ _fitness_lpf.getState(); }
	ଦଶମିକ getInnovation() ଅଚଳ { ଫେରନ୍ତୁ _rls.getInnovation(); }

	ଶୂନ୍ୟ setLpfCutoffFrequency(ଦଶମିକ sample_freq, ଦଶମିକ cutoff)
	{
		_u_lpf.set_cutoff_frequency(sample_freq, cutoff);
		_y_lpf.set_cutoff_frequency(sample_freq, cutoff);
	}
	ଶୂନ୍ୟ setHpfCutoffFrequency(ଦଶମିକ sample_freq, ଦଶମିକ cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	ଶୂନ୍ୟ setForgettingFactor(ଦଶମିକ time_constant, ଦଶମିକ dt) { _rls.setForgettingFactor(time_constant, dt); }
	ଶୂନ୍ୟ setFitnessLpfTimeConstant(ଦଶମିକ time_constant, ଦଶମିକ dt)
	{
		_fitness_lpf.setParameters(dt, time_constant);
		_dt = dt;
	}

	ଦଶମିକ get_Filtered_ନିବେଶ_Data() ଅଚଳ { ଫେରନ୍ତୁ _u_hpf; }
	ଦଶମିକ getFilteredOutputData() ଅଚଳ { ଫେରନ୍ତୁ _y_hpf; }

ବ୍ୟକ୍ତିଗତ:
	ArxRls<2, 2, 1> _rls;
	math::ତଳ_Pass_Filter2p<ଦଶମିକ> _u_lpf{400.f, 30.f};
	math::ତଳ_Pass_Filter2p<ଦଶମିକ> _y_lpf{400.f, 30.f};

	//TODO: replace by HighPassFilter class
	ଦଶମିକ _alpha_hpf{0.f};
	ଦଶମିକ _u_hpf{0.f};
	ଦଶମିକ _y_hpf{0.f};

	ଦଶମିକ _u_prev{0.f};
	ଦଶମିକ _y_prev{0.f};

	ତାର୍କିକ _are_filters_initialized{ମିଥ୍ୟା};

	AlphaFilter<ଦଶମିକ> _fitness_lpf;
	ଦଶମିକ _dt{0.1f};
};
