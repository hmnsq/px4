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

#ಅನ್ತರ್ಭಾವ <geo/geo.h>
#ಅನ್ತರ್ಭಾವ "atmosphere.h"
ನಾಮಾಕಾಶ atmosphere
{


ದಶಮಾಂಶ get_Density_From_Pressure_And_ತಾತ್ಕಾಲಿಕ(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pressure_pa, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ temperature_celsius)
{
	ಹಿಂತಿರುಗಿ (pressure_pa / (kAirGasConstant * (temperature_celsius - kAbsoluteNullCelsius)));
}
ದಶಮಾಂಶ getPressureFromAltitude(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ altitude_m)
{

	ಹಿಂತಿರುಗಿ kPressRefSeaLevelPa * powf((altitude_m * k_ತಾತ್ಕಾಲಿಕ_Gradient + k_ತಾತ್ಕಾಲಿಕ_Ref_Kelvin) / k_ತಾತ್ಕಾಲಿಕ_Ref_Kelvin,
					  -CONSTANTS_ONE_G / (k_ತಾತ್ಕಾಲಿಕ_Gradient * kAirGasConstant));
}
ದಶಮಾಂಶ getAltitudeFromPressure(ದಶಮಾಂಶ pressure_pa, ದಶಮಾಂಶ pressure_sealevel_pa)
{
	// calculate altitude using the hypsometric equation

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pressure_ಅನುಪಾತ = pressure_pa / pressure_sealevel_pa;

	/*
	 * Solve:
	 *
	 *     /        -(aR / g)     \
	 *    | (p / p1)          . T1 | - T1
	 *     \                      /
	 * h = -------------------------------  + h1
	 *                   a
	 */
	ಹಿಂತಿರುಗಿ (((powf(pressure_ಅನುಪಾತ, (-(k_ತಾತ್ಕಾಲಿಕ_Gradient * kAirGasConstant) / CONSTANTS_ONE_G))) * k_ತಾತ್ಕಾಲಿಕ_Ref_Kelvin) -
		k_ತಾತ್ಕಾಲಿಕ_Ref_Kelvin) / k_ತಾತ್ಕಾಲಿಕ_Gradient;

}
ದಶಮಾಂಶ getStandardTemperatureAtAltitude(ದಶಮಾಂಶ altitude_m)
{
	ಹಿಂತಿರುಗಿ 15.0f + k_ತಾತ್ಕಾಲಿಕ_Gradient * altitude_m;
}
}
