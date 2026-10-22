/****************************************************************************
 *
 *   Copyright (C) 2012-2013 PX4 Development Team. All rights reserved.
 *   Author: Lorenz Meier <lm@inf.ethz.ch>
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
 * @file airspeed.cpp
 * Airspeed estimation
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 *
 */

#അന്തര്ഭാവ "airspeed.h"

#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <lib/atmosphere/atmosphere.h>

using atmosphere::getDensityFromPressureAndTemp;
using atmosphere::kAirDensitySeaLevelStandardAtmos;

ദശാംശം calc_IAS_corrected(എണ്ണൽ AIRSPEED_COMPENSATION_MODEL pmodel, എണ്ണൽ AIRSPEED_SENSOR_MODEL smodel,
			 ദശാംശം tube_len, ദശാംശം tube_dia_mm, ദശാംശം differential_pressure, ദശാംശം pressure_ambient, ദശാംശം temperature_celsius)
{
	എങ്കിൽ (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	// air density in kg/m3
	സ്ഥിരം ദശാംശം rho_air = getDensityFromPressureAndTemp(pressure_ambient, temperature_celsius);

	സ്ഥിരം ദശാംശം dp = fabsf(differential_pressure);
	ദശാംശം dp_tot = dp;

	ദശാംശം dv = 0.0f;

	തിരഞ്ഞെടുപ്പ് (smodel) {

	അവസ്ഥ AIRSPEED_SENSOR_MODEL_MEMBRANE: {
			// do nothing
		}
		നിർത്തുക;

	അവസ്ഥ AIRSPEED_SENSOR_MODEL_SDP3X: {
			// assumes a metal pitot tube with round tip as here: https://drotek.com/shop/2986-large_default/sdp3x-airspeed-sensor-kit-sdp31.jpg
			// and tubing as provided by px4/drotek (1.5 mm diameter)
			// The tube_len represents the length of the tubes connecting the pitot to the sensor.
			തിരഞ്ഞെടുപ്പ് (pmodel) {
			അവസ്ഥ AIRSPEED_COMPENSATION_MODEL_PITOT:
			അവസ്ഥ AIRSPEED_COMPENSATION_MODEL_NO_PITOT: {
					സ്ഥിരം ദശാംശം dp_corr = dp * 96600.0f / pressure_ambient;
					// flow through sensor
					ദശാംശം flow_SDP33 = (300.805f - 300.878f / (0.00344205f * powf(dp_corr, 0.68698f) + 1.0f)) * 1.29f / rho_air;

					// for too small readings the compensation might result in a negative flow which causes numerical issues
					എങ്കിൽ (flow_SDP33 < 0.0f) {
						flow_SDP33 = 0.0f;
					}

					ദശാംശം dp_pitot = 0.0f;

					തിരഞ്ഞെടുപ്പ് (pmodel) {
					അവസ്ഥ AIRSPEED_COMPENSATION_MODEL_PITOT:
						dp_pitot = (0.0032f * flow_SDP33 * flow_SDP33 + 0.0123f * flow_SDP33 + 1.0f) * 1.29f / rho_air;
						നിർത്തുക;

					സ്ഥിരസ്ഥിതി:
						// do nothing
						നിർത്തുക;
					}

					// pressure drop through tube
					സ്ഥിരം ദശാംശം dp_tube = (flow_SDP33 * 0.674f) / 450.0f * tube_len * rho_air / 1.29f;

					// speed at pitot-tube tip due to flow through sensor
					dv = 0.125f * flow_SDP33;

					// sum of all pressure drops
					dp_tot = dp_corr + dp_tube + dp_pitot;
				}
				നിർത്തുക;

			അവസ്ഥ AIRSPEED_COMPENSATION_TUBE_PRESSURE_LOSS: {
					// Pressure loss compensation as defined in https://goo.gl/UHV1Vv.
					// tube_dia_mm: Diameter in mm of the pitot and tubes, must have the same diameter.
					// tube_len: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.

					// check if the tube diameter and dp is nonzero to avoid division by 0
					എങ്കിൽ ((tube_dia_mm > 0.0f) && (dp > 0.0f)) {
						സ്ഥിരം ദശാംശം d_tubePow4 = powf(tube_dia_mm * 1e-3f, 4);
						സ്ഥിരം ദശാംശം denominator = M_PI_F * d_tubePow4 * rho_air * dp;

						// avoid division by 0
						ദശാംശം eps = 0.0f;

						എങ്കിൽ (fabsf(denominator) > 1e-32f) {
							സ്ഥിരം ദശാംശം viscosity = (18.205f + 0.0484f * (temperature_celsius - 20.0f)) * 1e-6f;

							// 4.79 * 1e-7 -> mass flow through sensor
							// 59.5 -> dp sensor constant where linear and quadratic contribution to dp vs flow is equal
							eps = -64.0f * tube_len * viscosity * 4.79f * 1e-7f * (sqrtf(1.0f + 8.0f * dp / 59.3319f) - 1.0f) / denominator;
						}

						// range check on eps
						എങ്കിൽ (fabsf(eps) >= 1.0f) {
							eps = 0.0f;
						}

						// pressure correction
						dp_tot = dp / (1.0f + eps);
					}
				}
				നിർത്തുക;

			സ്ഥിരസ്ഥിതി: {
					// do nothing
				}
				നിർത്തുക;
			}

		}
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി: {
			// do nothing
		}
		നിർത്തുക;
	}

	// computed airspeed without correction for inflow-speed at tip of pitot-tube
	സ്ഥിരം ദശാംശം airspeed_uncorrected = sqrtf(2.0f * dp_tot / kAirDensitySeaLevelStandardAtmos);

	// corrected airspeed
	സ്ഥിരം ദശാംശം airspeed_corrected = airspeed_uncorrected + dv;

	// return result with correct sign
	തിരികെ (differential_pressure > 0.0f) ? airspeed_corrected : -airspeed_corrected;
}

ദശാംശം calc_IAS(ദശാംശം differential_pressure)
{
	എങ്കിൽ (differential_pressure > 0.0f) {
		തിരികെ sqrtf((2.0f * differential_pressure) / kAirDensitySeaLevelStandardAtmos);

	} അല്ലെങ്കിൽ {
		തിരികെ -sqrtf((2.0f * fabsf(differential_pressure)) / kAirDensitySeaLevelStandardAtmos);
	}

}

ദശാംശം calc_TAS_from_CAS(ദശാംശം speed_calibrated, ദശാംശം pressure_ambient, ദശാംശം temperature_celsius)
{
	എങ്കിൽ (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	തിരികെ speed_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / getDensityFromPressureAndTemp(pressure_ambient,
					temperature_celsius));
}

ദശാംശം calc_CAS_from_IAS(ദശാംശം speed_indicated, ദശാംശം scale)
{
	തിരികെ speed_indicated * scale;
}

ദശാംശം calc_TAS(ദശാംശം total_pressure, ദശാംശം static_pressure, ദശാംശം temperature_celsius)
{
	ദശാംശം density = getDensityFromPressureAndTemp(static_pressure, temperature_celsius);

	എങ്കിൽ (density < 0.0001f || !PX4_ISFINITE(density)) {
		density = kAirDensitySeaLevelStandardAtmos;
	}

	ദശാംശം pressure_difference = total_pressure - static_pressure;

	എങ്കിൽ (pressure_difference > 0) {
		തിരികെ sqrtf((2.0f * (pressure_difference)) / density);

	} അല്ലെങ്കിൽ {
		തിരികെ -sqrtf((2.0f * fabsf(pressure_difference)) / density);
	}
}

ദശാംശം calc_calibrated_from_true_airspeed(ദശാംശം speed_true, ദശാംശം air_density)
{
	തിരികെ speed_true * sqrtf(air_density / kAirDensitySeaLevelStandardAtmos);
}

ദശാംശം calc_true_from_calibrated_airspeed(ദശാംശം speed_calibrated, ദശാംശം air_density)
{
	തിരികെ speed_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
}
