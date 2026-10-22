/****************************************************************************
 *
 *   Copyright (C) 2023 PX4 Development Team. All rights reserved.
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
 * @file atmosphere.cpp
 *
 */

#شامل <geo/geo.h>
#شامل "atmosphere.h"
نامخلا atmosphere
{


اعشاری get_Density_From_Pressure_And_عارضی(ثابت اعشاری pressure_pa, ثابت اعشاری temperature_celsius)
{
	واپس (pressure_pa / (kAirGasConstant * (temperature_celsius - kAbsoluteNullCelsius)));
}
اعشاری getPressureFromAltitude(ثابت اعشاری altitude_m)
{

	واپس kPressRefSeaLevelPa * powf((altitude_m * k_عارضی_Gradient + k_عارضی_Ref_Kelvin) / k_عارضی_Ref_Kelvin,
					  -CONSTANTS_ONE_G / (k_عارضی_Gradient * kAirGasConstant));
}
اعشاری getAltitudeFromPressure(اعشاری pressure_pa, اعشاری pressure_sealevel_pa)
{
	// calculate altitude using the hypsometric equation

	ثابت اعشاری pressure_تناسب = pressure_pa / pressure_sealevel_pa;

	/*
	 * Solve:
	 *
	 *     /        -(aR / g)     \
	 *    | (p / p1)          . T1 | - T1
	 *     \                      /
	 * h = -------------------------------  + h1
	 *                   a
	 */
	واپس (((powf(pressure_تناسب, (-(k_عارضی_Gradient * kAirGasConstant) / CONSTANTS_ONE_G))) * k_عارضی_Ref_Kelvin) -
		k_عارضی_Ref_Kelvin) / k_عارضی_Gradient;

}
اعشاری getStandardTemperatureAtAltitude(اعشاری altitude_m)
{
	واپس 15.0f + k_عارضی_Gradient * altitude_m;
}
}
