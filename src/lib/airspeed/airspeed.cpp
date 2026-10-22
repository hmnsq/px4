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

#অন্তর্ভাৱ "airspeed.h"

#অন্তর্ভাৱ <px4_platform_common/defines.h>
#অন্তর্ভাৱ <lib/atmosphere/atmosphere.h>

using atmosphere::getDensityFromPressureAndTemp;
using atmosphere::kAirDensitySeaLevelStandardAtmos;

ভিন্ন calc_IAS_corrected(গণনা AIRSPEED_COMPENSATION_MODEL pmodel, গণনা AIRSPEED_SENSOR_MODEL smodel,
			 ভিন্ন tube_len, ভিন্ন tube_dia_mm, ভিন্ন differential_pressure, ভিন্ন pressure_ambient, ভিন্ন temperature_celsius)
{
	যদি (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	// air density in kg/m3
	ধ্রুৱ ভিন্ন rho_air = getDensityFromPressureAndTemp(pressure_ambient, temperature_celsius);

	ধ্রুৱ ভিন্ন dp = fabsf(differential_pressure);
	ভিন্ন dp_tot = dp;

	ভিন্ন dv = 0.0f;

	ৱিকল্প (smodel) {

	পক্ষ AIRSPEED_SENSOR_MODEL_MEMBRANE: {
			// do nothing
		}
		ভঙ্গ;

	পক্ষ AIRSPEED_SENSOR_MODEL_SDP3X: {
			// assumes a metal pitot tube with round tip as here: https://drotek.com/shop/2986-large_default/sdp3x-airspeed-sensor-kit-sdp31.jpg
			// and tubing as provided by px4/drotek (1.5 mm diameter)
			// The tube_len represents the length of the tubes connecting the pitot to the sensor.
			ৱিকল্প (pmodel) {
			পক্ষ AIRSPEED_COMPENSATION_MODEL_PITOT:
			পক্ষ AIRSPEED_COMPENSATION_MODEL_NO_PITOT: {
					ধ্রুৱ ভিন্ন dp_corr = dp * 96600.0f / pressure_ambient;
					// flow through sensor
					ভিন্ন flow_SDP33 = (300.805f - 300.878f / (0.00344205f * powf(dp_corr, 0.68698f) + 1.0f)) * 1.29f / rho_air;

					// for too small readings the compensation might result in a negative flow which causes numerical issues
					যদি (flow_SDP33 < 0.0f) {
						flow_SDP33 = 0.0f;
					}

					ভিন্ন dp_pitot = 0.0f;

					ৱিকল্প (pmodel) {
					পক্ষ AIRSPEED_COMPENSATION_MODEL_PITOT:
						dp_pitot = (0.0032f * flow_SDP33 * flow_SDP33 + 0.0123f * flow_SDP33 + 1.0f) * 1.29f / rho_air;
						ভঙ্গ;

					স্ৱভাৱ:
						// do nothing
						ভঙ্গ;
					}

					// pressure drop through tube
					ধ্রুৱ ভিন্ন dp_tube = (flow_SDP33 * 0.674f) / 450.0f * tube_len * rho_air / 1.29f;

					// speed at pitot-tube tip due to flow through sensor
					dv = 0.125f * flow_SDP33;

					// sum of all pressure drops
					dp_tot = dp_corr + dp_tube + dp_pitot;
				}
				ভঙ্গ;

			পক্ষ AIRSPEED_COMPENSATION_TUBE_PRESSURE_LOSS: {
					// Pressure loss compensation as defined in https://goo.gl/UHV1Vv.
					// tube_dia_mm: Diameter in mm of the pitot and tubes, must have the same diameter.
					// tube_len: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.

					// check if the tube diameter and dp is nonzero to avoid division by 0
					যদি ((tube_dia_mm > 0.0f) && (dp > 0.0f)) {
						ধ্রুৱ ভিন্ন d_tubePow4 = powf(tube_dia_mm * 1e-3f, 4);
						ধ্রুৱ ভিন্ন denominator = M_PI_F * d_tubePow4 * rho_air * dp;

						// avoid division by 0
						ভিন্ন eps = 0.0f;

						যদি (fabsf(denominator) > 1e-32f) {
							ধ্রুৱ ভিন্ন viscosity = (18.205f + 0.0484f * (temperature_celsius - 20.0f)) * 1e-6f;

							// 4.79 * 1e-7 -> mass flow through sensor
							// 59.5 -> dp sensor constant where linear and quadratic contribution to dp vs flow is equal
							eps = -64.0f * tube_len * viscosity * 4.79f * 1e-7f * (sqrtf(1.0f + 8.0f * dp / 59.3319f) - 1.0f) / denominator;
						}

						// range check on eps
						যদি (fabsf(eps) >= 1.0f) {
							eps = 0.0f;
						}

						// pressure correction
						dp_tot = dp / (1.0f + eps);
					}
				}
				ভঙ্গ;

			স্ৱভাৱ: {
					// do nothing
				}
				ভঙ্গ;
			}

		}
		ভঙ্গ;

	স্ৱভাৱ: {
			// do nothing
		}
		ভঙ্গ;
	}

	// computed airspeed without correction for inflow-speed at tip of pitot-tube
	ধ্রুৱ ভিন্ন airspeed_uncorrected = sqrtf(2.0f * dp_tot / kAirDensitySeaLevelStandardAtmos);

	// corrected airspeed
	ধ্রুৱ ভিন্ন airspeed_corrected = airspeed_uncorrected + dv;

	// return result with correct sign
	ঘূৰাই (differential_pressure > 0.0f) ? airspeed_corrected : -airspeed_corrected;
}

ভিন্ন calc_IAS(ভিন্ন differential_pressure)
{
	যদি (differential_pressure > 0.0f) {
		ঘূৰাই sqrtf((2.0f * differential_pressure) / kAirDensitySeaLevelStandardAtmos);

	} অন্যথা {
		ঘূৰাই -sqrtf((2.0f * fabsf(differential_pressure)) / kAirDensitySeaLevelStandardAtmos);
	}

}

ভিন্ন calc_TAS_from_CAS(ভিন্ন speed_calibrated, ভিন্ন pressure_ambient, ভিন্ন temperature_celsius)
{
	যদি (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	ঘূৰাই speed_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / getDensityFromPressureAndTemp(pressure_ambient,
					temperature_celsius));
}

ভিন্ন calc_CAS_from_IAS(ভিন্ন speed_indicated, ভিন্ন scale)
{
	ঘূৰাই speed_indicated * scale;
}

ভিন্ন calc_TAS(ভিন্ন total_pressure, ভিন্ন static_pressure, ভিন্ন temperature_celsius)
{
	ভিন্ন density = getDensityFromPressureAndTemp(static_pressure, temperature_celsius);

	যদি (density < 0.0001f || !PX4_ISFINITE(density)) {
		density = kAirDensitySeaLevelStandardAtmos;
	}

	ভিন্ন pressure_difference = total_pressure - static_pressure;

	যদি (pressure_difference > 0) {
		ঘূৰাই sqrtf((2.0f * (pressure_difference)) / density);

	} অন্যথা {
		ঘূৰাই -sqrtf((2.0f * fabsf(pressure_difference)) / density);
	}
}

ভিন্ন calc_calibrated_from_true_airspeed(ভিন্ন speed_true, ভিন্ন air_density)
{
	ঘূৰাই speed_true * sqrtf(air_density / kAirDensitySeaLevelStandardAtmos);
}

ভিন্ন calc_true_from_calibrated_airspeed(ভিন্ন speed_calibrated, ভিন্ন air_density)
{
	ঘূৰাই speed_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
}
