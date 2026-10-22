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

#شامل "airspeed.h"

#شامل <px4_platform_common/defines.h>
#شامل <lib/atmosphere/atmosphere.h>

استعمال atmosphere::get_Density_From_Pressure_And_عارضی;
استعمال atmosphere::kAirDensitySeaLevelStandardAtmos;

اعشاری calc_IAS_corrected(شمار AIRSPEED_COMPENSATION_MODEL pmodel, شمار AIRSPEED_SENSOR_MODEL smodel,
			 اعشاری tube_len, اعشاری tube_dia_mm, اعشاری differential_pressure, اعشاری pressure_ambient, اعشاری temperature_celsius)
{
	اگر (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	// air density in kg/m3
	ثابت اعشاری rho_air = get_Density_From_Pressure_And_عارضی(pressure_ambient, temperature_celsius);

	ثابت اعشاری dp = fabsf(differential_pressure);
	اعشاری dp_tot = dp;

	اعشاری dv = 0.0f;

	انتخاب (smodel) {

	حالت AIRSPEED_SENSOR_MODEL_MEMBRANE: {
			// do nothing
		}
		توڑو;

	حالت AIRSPEED_SENSOR_MODEL_SDP3X: {
			// assumes a metal pitot tube with round tip as here: https://drotek.com/shop/2986-large_default/sdp3x-airspeed-sensor-kit-sdp31.jpg
			// and tubing as provided by px4/drotek (1.5 mm diameter)
			// The tube_len represents the length of the tubes connecting the pitot to the sensor.
			انتخاب (pmodel) {
			حالت AIRSPEED_COMPENSATION_MODEL_PITOT:
			حالت AIRSPEED_COMPENSATION_MODEL_NO_PITOT: {
					ثابت اعشاری dp_corr = dp * 96600.0f / pressure_ambient;
					// flow through sensor
					اعشاری flow_SDP33 = (300.805f - 300.878f / (0.00344205f * powf(dp_corr, 0.68698f) + 1.0f)) * 1.29f / rho_air;

					// for too small readings the compensation might result in a negative flow which causes numerical issues
					اگر (flow_SDP33 < 0.0f) {
						flow_SDP33 = 0.0f;
					}

					اعشاری dp_pitot = 0.0f;

					انتخاب (pmodel) {
					حالت AIRSPEED_COMPENSATION_MODEL_PITOT:
						dp_pitot = (0.0032f * flow_SDP33 * flow_SDP33 + 0.0123f * flow_SDP33 + 1.0f) * 1.29f / rho_air;
						توڑو;

					طےشدہ:
						// do nothing
						توڑو;
					}

					// pressure drop through tube
					ثابت اعشاری dp_tube = (flow_SDP33 * 0.674f) / 450.0f * tube_len * rho_air / 1.29f;

					// speed at pitot-tube tip due to flow through sensor
					dv = 0.125f * flow_SDP33;

					// sum of all pressure drops
					dp_tot = dp_corr + dp_tube + dp_pitot;
				}
				توڑو;

			حالت AIRSPEED_COMPENSATION_TUBE_PRESSURE_LOSS: {
					// Pressure loss compensation as defined in https://goo.gl/UHV1Vv.
					// tube_dia_mm: Diameter in mm of the pitot and tubes, must have the same diameter.
					// tube_len: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.

					// check if the tube diameter and dp is nonzero to avoid division by 0
					اگر ((tube_dia_mm > 0.0f) && (dp > 0.0f)) {
						ثابت اعشاری d_tubePow4 = powf(tube_dia_mm * 1e-3f, 4);
						ثابت اعشاری denominator = M_PI_F * d_tubePow4 * rho_air * dp;

						// avoid division by 0
						اعشاری eps = 0.0f;

						اگر (fabsf(denominator) > 1e-32f) {
							ثابت اعشاری viscosity = (18.205f + 0.0484f * (temperature_celsius - 20.0f)) * 1e-6f;

							// 4.79 * 1e-7 -> mass flow through sensor
							// 59.5 -> dp sensor constant where linear and quadratic contribution to dp vs flow is equal
							eps = -64.0f * tube_len * viscosity * 4.79f * 1e-7f * (sqrtf(1.0f + 8.0f * dp / 59.3319f) - 1.0f) / denominator;
						}

						// range check on eps
						اگر (fabsf(eps) >= 1.0f) {
							eps = 0.0f;
						}

						// pressure correction
						dp_tot = dp / (1.0f + eps);
					}
				}
				توڑو;

			طےشدہ: {
					// do nothing
				}
				توڑو;
			}

		}
		توڑو;

	طےشدہ: {
			// do nothing
		}
		توڑو;
	}

	// computed airspeed without correction for inflow-speed at tip of pitot-tube
	ثابت اعشاری airspeed_uncorrected = sqrtf(2.0f * dp_tot / kAirDensitySeaLevelStandardAtmos);

	// corrected airspeed
	ثابت اعشاری airspeed_corrected = airspeed_uncorrected + dv;

	// return result with correct sign
	واپس (differential_pressure > 0.0f) ? airspeed_corrected : -airspeed_corrected;
}

اعشاری calc_IAS(اعشاری differential_pressure)
{
	اگر (differential_pressure > 0.0f) {
		واپس sqrtf((2.0f * differential_pressure) / kAirDensitySeaLevelStandardAtmos);

	} ورنہ {
		واپس -sqrtf((2.0f * fabsf(differential_pressure)) / kAirDensitySeaLevelStandardAtmos);
	}

}

اعشاری calc_TAS_from_CAS(اعشاری رفتار_calibrated, اعشاری pressure_ambient, اعشاری temperature_celsius)
{
	اگر (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	واپس رفتار_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / get_Density_From_Pressure_And_عارضی(pressure_ambient,
					temperature_celsius));
}

اعشاری calc_CAS_from_IAS(اعشاری رفتار_indicated, اعشاری scale)
{
	واپس رفتار_indicated * scale;
}

اعشاری calc_TAS(اعشاری کل_pressure, اعشاری static_pressure, اعشاری temperature_celsius)
{
	اعشاری density = get_Density_From_Pressure_And_عارضی(static_pressure, temperature_celsius);

	اگر (density < 0.0001f || !PX4_ISFINITE(density)) {
		density = kAirDensitySeaLevelStandardAtmos;
	}

	اعشاری pressure_difference = کل_pressure - static_pressure;

	اگر (pressure_difference > 0) {
		واپس sqrtf((2.0f * (pressure_difference)) / density);

	} ورنہ {
		واپس -sqrtf((2.0f * fabsf(pressure_difference)) / density);
	}
}

اعشاری calc_calibrated_from_true_airspeed(اعشاری رفتار_true, اعشاری air_density)
{
	واپس رفتار_true * sqrtf(air_density / kAirDensitySeaLevelStandardAtmos);
}

اعشاری calc_true_from_calibrated_airspeed(اعشاری رفتار_calibrated, اعشاری air_density)
{
	واپس رفتار_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
}
