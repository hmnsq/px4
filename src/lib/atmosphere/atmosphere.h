/****************************************************************************
 *
 *   Copyright (c) 2023 PX4 Development Team. All rights reserved.
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
 * @file atmosphere.h
 *
 */

#ifndef PX4_SRC_LIB_ATMOSPHERE_ATMOSPHERE_H_
#define PX4_SRC_LIB_ATMOSPHERE_ATMOSPHERE_H_

namespace atmosphere
{

// NOTE: We are currently only modelling the first layer of the US Standard Atmosphere 1976.
// This means that the functions are only valid up to an altitude of 11km.
நிலையான constexpr தசம kAirDensitySeaLevelStandardAtmos = 1.225f;				// kg/m^3

// [kg/m^3] air density of standard atmosphere at 11000m above mean sea level (this is the upper limit for the standard
// atmosphere model we are using, see atmosphere lib used)
நிலையான constexpr தசம kAirDensityStandardAtmos11000Amsl = 0.3639;

நிலையான constexpr தசம kAirGasConstant = 287.1f;					// J/(kg * K)
நிலையான constexpr தசம kAbsoluteNullCelsius = -273.15f;				// °C
நிலையான constexpr தசம kTempRefKelvin = 15.f - kAbsoluteNullCelsius; // temperature at base height in Kelvin
நிலையான constexpr தசம kTempGradient = -6.5f / 1000.f; // temperature gradient in degrees per meter
நிலையான constexpr தசம kPressRefSeaLevelPa = 101325.f; // pressure at sea level in Pa

/**
* Calculate air density given air pressure and temperature.
*
* @param pressure_pa ambient pressure in Pa
* @param temperature_celsius ambient temperature in degrees Celsius
*/
தசம getDensityFromPressureAndTemp(மாறிலி தசம pressure_pa, மாறிலி தசம temperature_celsius);

/**
* Calculate standard airpressure given altitude.
*
* @param altitude_m altitude above MSL in meters in the standard atmosphere
*/
தசம getPressureFromAltitude(மாறிலி தசம altitude_m);

/**
* Calculate altitude from air pressure and temperature.
*
* @param pressure_pa ambient pressure in Pa
* @param pressure_sealevel_pa sea level pressure in Pa
*/
தசம getAltitudeFromPressure(தசம pressure_pa, தசம pressure_sealevel_pa);

/**
* Get standard temperature at altitude.
*
* @param altitude_m Altitude msl in meters
* @return Standard temperature in degrees Celsius
*/
தசம getStandardTemperatureAtAltitude(தசம altitude_m);
}

#endif //PX4_SRC_LIB_ATMOSPHERE_ATMOSPHERE_H_
