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

#ਅਨ੍ਤਰ੍ਭਾਵ "airspeed.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/atmosphere/atmosphere.h>

ਵਰਤੋਂ atmosphere::get_Density_From_Pressure_And_ਆਰਜ਼ੀ;
ਵਰਤੋਂ atmosphere::kAirDensitySeaLevelStandardAtmos;

ਦਸ਼ਮਲਵ calc_IAS_corrected(ਗਿਣਤੀ AIRSPEED_COMPENSATION_MODEL pmodel, ਗਿਣਤੀ AIRSPEED_SENSOR_MODEL smodel,
			 ਦਸ਼ਮਲਵ tube_len, ਦਸ਼ਮਲਵ tube_dia_mm, ਦਸ਼ਮਲਵ differential_pressure, ਦਸ਼ਮਲਵ pressure_ambient, ਦਸ਼ਮਲਵ temperature_celsius)
{
	ਜੇ (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	// air density in kg/m3
	ਅਚਲ ਦਸ਼ਮਲਵ rho_air = get_Density_From_Pressure_And_ਆਰਜ਼ੀ(pressure_ambient, temperature_celsius);

	ਅਚਲ ਦਸ਼ਮਲਵ dp = fabsf(differential_pressure);
	ਦਸ਼ਮਲਵ dp_tot = dp;

	ਦਸ਼ਮਲਵ dv = 0.0f;

	ਚੋਣ (smodel) {

	ਹਾਲਤ AIRSPEED_SENSOR_MODEL_MEMBRANE: {
			// do nothing
		}
		ਰੋਕੋ;

	ਹਾਲਤ AIRSPEED_SENSOR_MODEL_SDP3X: {
			// assumes a metal pitot tube with round tip as here: https://drotek.com/shop/2986-large_default/sdp3x-airspeed-sensor-kit-sdp31.jpg
			// and tubing as provided by px4/drotek (1.5 mm diameter)
			// The tube_len represents the length of the tubes connecting the pitot to the sensor.
			ਚੋਣ (pmodel) {
			ਹਾਲਤ AIRSPEED_COMPENSATION_MODEL_PITOT:
			ਹਾਲਤ AIRSPEED_COMPENSATION_MODEL_NO_PITOT: {
					ਅਚਲ ਦਸ਼ਮਲਵ dp_corr = dp * 96600.0f / pressure_ambient;
					// flow through sensor
					ਦਸ਼ਮਲਵ flow_SDP33 = (300.805f - 300.878f / (0.00344205f * powf(dp_corr, 0.68698f) + 1.0f)) * 1.29f / rho_air;

					// for too small readings the compensation might result in a negative flow which causes numerical issues
					ਜੇ (flow_SDP33 < 0.0f) {
						flow_SDP33 = 0.0f;
					}

					ਦਸ਼ਮਲਵ dp_pitot = 0.0f;

					ਚੋਣ (pmodel) {
					ਹਾਲਤ AIRSPEED_COMPENSATION_MODEL_PITOT:
						dp_pitot = (0.0032f * flow_SDP33 * flow_SDP33 + 0.0123f * flow_SDP33 + 1.0f) * 1.29f / rho_air;
						ਰੋਕੋ;

					ਮੂਲ:
						// do nothing
						ਰੋਕੋ;
					}

					// pressure drop through tube
					ਅਚਲ ਦਸ਼ਮਲਵ dp_tube = (flow_SDP33 * 0.674f) / 450.0f * tube_len * rho_air / 1.29f;

					// speed at pitot-tube tip due to flow through sensor
					dv = 0.125f * flow_SDP33;

					// sum of all pressure drops
					dp_tot = dp_corr + dp_tube + dp_pitot;
				}
				ਰੋਕੋ;

			ਹਾਲਤ AIRSPEED_COMPENSATION_TUBE_PRESSURE_LOSS: {
					// Pressure loss compensation as defined in https://goo.gl/UHV1Vv.
					// tube_dia_mm: Diameter in mm of the pitot and tubes, must have the same diameter.
					// tube_len: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.

					// check if the tube diameter and dp is nonzero to avoid division by 0
					ਜੇ ((tube_dia_mm > 0.0f) && (dp > 0.0f)) {
						ਅਚਲ ਦਸ਼ਮਲਵ d_tubePow4 = powf(tube_dia_mm * 1e-3f, 4);
						ਅਚਲ ਦਸ਼ਮਲਵ denominator = M_PI_F * d_tubePow4 * rho_air * dp;

						// avoid division by 0
						ਦਸ਼ਮਲਵ eps = 0.0f;

						ਜੇ (fabsf(denominator) > 1e-32f) {
							ਅਚਲ ਦਸ਼ਮਲਵ viscosity = (18.205f + 0.0484f * (temperature_celsius - 20.0f)) * 1e-6f;

							// 4.79 * 1e-7 -> mass flow through sensor
							// 59.5 -> dp sensor constant where linear and quadratic contribution to dp vs flow is equal
							eps = -64.0f * tube_len * viscosity * 4.79f * 1e-7f * (sqrtf(1.0f + 8.0f * dp / 59.3319f) - 1.0f) / denominator;
						}

						// range check on eps
						ਜੇ (fabsf(eps) >= 1.0f) {
							eps = 0.0f;
						}

						// pressure correction
						dp_tot = dp / (1.0f + eps);
					}
				}
				ਰੋਕੋ;

			ਮੂਲ: {
					// do nothing
				}
				ਰੋਕੋ;
			}

		}
		ਰੋਕੋ;

	ਮੂਲ: {
			// do nothing
		}
		ਰੋਕੋ;
	}

	// computed airspeed without correction for inflow-speed at tip of pitot-tube
	ਅਚਲ ਦਸ਼ਮਲਵ airspeed_uncorrected = sqrtf(2.0f * dp_tot / kAirDensitySeaLevelStandardAtmos);

	// corrected airspeed
	ਅਚਲ ਦਸ਼ਮਲਵ airspeed_corrected = airspeed_uncorrected + dv;

	// return result with correct sign
	ਵਾਪਸ (differential_pressure > 0.0f) ? airspeed_corrected : -airspeed_corrected;
}

ਦਸ਼ਮਲਵ calc_IAS(ਦਸ਼ਮਲਵ differential_pressure)
{
	ਜੇ (differential_pressure > 0.0f) {
		ਵਾਪਸ sqrtf((2.0f * differential_pressure) / kAirDensitySeaLevelStandardAtmos);

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -sqrtf((2.0f * fabsf(differential_pressure)) / kAirDensitySeaLevelStandardAtmos);
	}

}

ਦਸ਼ਮਲਵ calc_TAS_from_CAS(ਦਸ਼ਮਲਵ ਰਫਤਾਰ_calibrated, ਦਸ਼ਮਲਵ pressure_ambient, ਦਸ਼ਮਲਵ temperature_celsius)
{
	ਜੇ (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	ਵਾਪਸ ਰਫਤਾਰ_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / get_Density_From_Pressure_And_ਆਰਜ਼ੀ(pressure_ambient,
					temperature_celsius));
}

ਦਸ਼ਮਲਵ calc_CAS_from_IAS(ਦਸ਼ਮਲਵ ਰਫਤਾਰ_indicated, ਦਸ਼ਮਲਵ scale)
{
	ਵਾਪਸ ਰਫਤਾਰ_indicated * scale;
}

ਦਸ਼ਮਲਵ calc_TAS(ਦਸ਼ਮਲਵ ਕੁੱਲ_pressure, ਦਸ਼ਮਲਵ static_pressure, ਦਸ਼ਮਲਵ temperature_celsius)
{
	ਦਸ਼ਮਲਵ density = get_Density_From_Pressure_And_ਆਰਜ਼ੀ(static_pressure, temperature_celsius);

	ਜੇ (density < 0.0001f || !PX4_ISFINITE(density)) {
		density = kAirDensitySeaLevelStandardAtmos;
	}

	ਦਸ਼ਮਲਵ pressure_difference = ਕੁੱਲ_pressure - static_pressure;

	ਜੇ (pressure_difference > 0) {
		ਵਾਪਸ sqrtf((2.0f * (pressure_difference)) / density);

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -sqrtf((2.0f * fabsf(pressure_difference)) / density);
	}
}

ਦਸ਼ਮਲਵ calc_calibrated_from_true_airspeed(ਦਸ਼ਮਲਵ ਰਫਤਾਰ_true, ਦਸ਼ਮਲਵ air_density)
{
	ਵਾਪਸ ਰਫਤਾਰ_true * sqrtf(air_density / kAirDensitySeaLevelStandardAtmos);
}

ਦਸ਼ਮਲਵ calc_true_from_calibrated_airspeed(ਦਸ਼ਮਲਵ ਰਫਤਾਰ_calibrated, ਦਸ਼ਮਲਵ air_density)
{
	ਵਾਪਸ ਰਫਤਾਰ_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
}
