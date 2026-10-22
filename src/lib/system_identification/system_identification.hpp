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

#شامل <lib/mathlib/math/filter/AlphaFilter.hpp>
#شامل <matrix/matrix/math.hpp>
#شامل <mathlib/mathlib.h>
#شامل <mathlib/math/filter/LowPassFilter2p.hpp>
#شامل <px4_platform_common/defines.h>

#شامل "arx_rls.hpp"

رده SystemIdentification final
{
عمومی:
	SystemIdentification() = پیشفرض;
	~SystemIdentification() = پیشفرض;

	تهی reset(ثابت matrix::Vector<اعشاری, 5> &id_state_init = {});
	تهی update(اعشاری u, اعشاری y); // update filters and model
	تهی update(); // update model only (to be called after updateFilters)
	تهی updateFilters(اعشاری u, اعشاری y);
	منطقی areFiltersInitialized() ثابت { بازگشت _are_filters_initialized; }
	تهی updateFitness();
	ثابت matrix::Vector<اعشاری, 5> &getCoefficients() ثابت { بازگشت _rls.getCoefficients(); }
	ثابت matrix::Vector<اعشاری, 5> getVariances() ثابت { بازگشت _rls.getVariances(); }
	ثابت matrix::Vector<اعشاری, 5> &getDiffEstimate() ثابت { بازگشت _rls.getDiffEstimate(); }
	اعشاری getFitness() ثابت { بازگشت _fitness_lpf.getState(); }
	اعشاری getInnovation() ثابت { بازگشت _rls.getInnovation(); }

	تهی setLpfCutoffFrequency(اعشاری sample_freq, اعشاری cutoff)
	{
		_u_lpf.set_cutoff_frequency(sample_freq, cutoff);
		_y_lpf.set_cutoff_frequency(sample_freq, cutoff);
	}
	تهی setHpfCutoffFrequency(اعشاری sample_freq, اعشاری cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	تهی setForgettingFactor(اعشاری time_constant, اعشاری dt) { _rls.setForgettingFactor(time_constant, dt); }
	تهی setFitnessLpfTimeConstant(اعشاری time_constant, اعشاری dt)
	{
		_fitness_lpf.setParameters(dt, time_constant);
		_dt = dt;
	}

	اعشاری get_Filtered_درون_داد_Data() ثابت { بازگشت _u_hpf; }
	اعشاری getFilteredOutputData() ثابت { بازگشت _y_hpf; }

خصوصی:
	ArxRls<2, 2, 1> _rls;
	math::کم_Pass_Filter2p<اعشاری> _u_lpf{400.f, 30.f};
	math::کم_Pass_Filter2p<اعشاری> _y_lpf{400.f, 30.f};

	//TODO: replace by HighPassFilter class
	اعشاری _alpha_hpf{0.f};
	اعشاری _u_hpf{0.f};
	اعشاری _y_hpf{0.f};

	اعشاری _u_prev{0.f};
	اعشاری _y_prev{0.f};

	منطقی _are_filters_initialized{نادرست};

	AlphaFilter<اعشاری> _fitness_lpf;
	اعشاری _dt{0.1f};
};
