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

#অন্তর্ভাব <geo/geo.h>
#অন্তর্ভাব "atmosphere.h"
নামাকাশ atmosphere
{


দশমিক get_Density_From_Pressure_And_অস্থায়ী(ধ্রুবক দশমিক pressure_pa, ধ্রুবক দশমিক temperature_celsius)
{
	ফেরত (pressure_pa / (kAirGasConstant * (temperature_celsius - kAbsoluteNullCelsius)));
}
দশমিক getPressureFromAltitude(ধ্রুবক দশমিক altitude_m)
{

	ফেরত kPressRefSeaLevelPa * powf((altitude_m * k_অস্থায়ী_Gradient + k_অস্থায়ী_Ref_Kelvin) / k_অস্থায়ী_Ref_Kelvin,
					  -CONSTANTS_ONE_G / (k_অস্থায়ী_Gradient * kAirGasConstant));
}
দশমিক getAltitudeFromPressure(দশমিক pressure_pa, দশমিক pressure_sealevel_pa)
{
	// calculate altitude using the hypsometric equation

	ধ্রুবক দশমিক pressure_ratio = pressure_pa / pressure_sealevel_pa;

	/*
	 * Solve:
	 *
	 *     /        -(aR / g)     \
	 *    | (p / p1)          . T1 | - T1
	 *     \                      /
	 * h = -------------------------------  + h1
	 *                   a
	 */
	ফেরত (((powf(pressure_ratio, (-(k_অস্থায়ী_Gradient * kAirGasConstant) / CONSTANTS_ONE_G))) * k_অস্থায়ী_Ref_Kelvin) -
		k_অস্থায়ী_Ref_Kelvin) / k_অস্থায়ী_Gradient;

}
দশমিক getStandardTemperatureAtAltitude(দশমিক altitude_m)
{
	ফেরত 15.0f + k_অস্থায়ী_Gradient * altitude_m;
}
}
