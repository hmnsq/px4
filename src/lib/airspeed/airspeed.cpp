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

#ଅନ୍ତର୍ଭାଵ "airspeed.h"

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>
#ଅନ୍ତର୍ଭାଵ <lib/atmosphere/atmosphere.h>

using atmosphere::getDensityFromPressureAndTemp;
using atmosphere::kAirDensitySeaLevelStandardAtmos;

ଦଶମିକ calc_IAS_corrected(ଗଣନା AIRSPEED_COMPENSATION_MODEL pmodel, ଗଣନା AIRSPEED_SENSOR_MODEL smodel,
			 ଦଶମିକ tube_len, ଦଶମିକ tube_dia_mm, ଦଶମିକ differential_pressure, ଦଶମିକ pressure_ambient, ଦଶମିକ temperature_celsius)
{
	ଯଦି (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	// air density in kg/m3
	ଅଚଳ ଦଶମିକ rho_air = getDensityFromPressureAndTemp(pressure_ambient, temperature_celsius);

	ଅଚଳ ଦଶମିକ dp = fabsf(differential_pressure);
	ଦଶମିକ dp_tot = dp;

	ଦଶମିକ dv = 0.0f;

	ବାଛ (smodel) {

	ସ୍ଥିତି AIRSPEED_SENSOR_MODEL_MEMBRANE: {
			// do nothing
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି AIRSPEED_SENSOR_MODEL_SDP3X: {
			// assumes a metal pitot tube with round tip as here: https://drotek.com/shop/2986-large_default/sdp3x-airspeed-sensor-kit-sdp31.jpg
			// and tubing as provided by px4/drotek (1.5 mm diameter)
			// The tube_len represents the length of the tubes connecting the pitot to the sensor.
			ବାଛ (pmodel) {
			ସ୍ଥିତି AIRSPEED_COMPENSATION_MODEL_PITOT:
			ସ୍ଥିତି AIRSPEED_COMPENSATION_MODEL_NO_PITOT: {
					ଅଚଳ ଦଶମିକ dp_corr = dp * 96600.0f / pressure_ambient;
					// flow through sensor
					ଦଶମିକ flow_SDP33 = (300.805f - 300.878f / (0.00344205f * powf(dp_corr, 0.68698f) + 1.0f)) * 1.29f / rho_air;

					// for too small readings the compensation might result in a negative flow which causes numerical issues
					ଯଦି (flow_SDP33 < 0.0f) {
						flow_SDP33 = 0.0f;
					}

					ଦଶମିକ dp_pitot = 0.0f;

					ବାଛ (pmodel) {
					ସ୍ଥିତି AIRSPEED_COMPENSATION_MODEL_PITOT:
						dp_pitot = (0.0032f * flow_SDP33 * flow_SDP33 + 0.0123f * flow_SDP33 + 1.0f) * 1.29f / rho_air;
						ଭାଙ୍ଗ;

					ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
						// do nothing
						ଭାଙ୍ଗ;
					}

					// pressure drop through tube
					ଅଚଳ ଦଶମିକ dp_tube = (flow_SDP33 * 0.674f) / 450.0f * tube_len * rho_air / 1.29f;

					// speed at pitot-tube tip due to flow through sensor
					dv = 0.125f * flow_SDP33;

					// sum of all pressure drops
					dp_tot = dp_corr + dp_tube + dp_pitot;
				}
				ଭାଙ୍ଗ;

			ସ୍ଥିତି AIRSPEED_COMPENSATION_TUBE_PRESSURE_LOSS: {
					// Pressure loss compensation as defined in https://goo.gl/UHV1Vv.
					// tube_dia_mm: Diameter in mm of the pitot and tubes, must have the same diameter.
					// tube_len: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.

					// check if the tube diameter and dp is nonzero to avoid division by 0
					ଯଦି ((tube_dia_mm > 0.0f) && (dp > 0.0f)) {
						ଅଚଳ ଦଶମିକ d_tubePow4 = powf(tube_dia_mm * 1e-3f, 4);
						ଅଚଳ ଦଶମିକ denominator = M_PI_F * d_tubePow4 * rho_air * dp;

						// avoid division by 0
						ଦଶମିକ eps = 0.0f;

						ଯଦି (fabsf(denominator) > 1e-32f) {
							ଅଚଳ ଦଶମିକ viscosity = (18.205f + 0.0484f * (temperature_celsius - 20.0f)) * 1e-6f;

							// 4.79 * 1e-7 -> mass flow through sensor
							// 59.5 -> dp sensor constant where linear and quadratic contribution to dp vs flow is equal
							eps = -64.0f * tube_len * viscosity * 4.79f * 1e-7f * (sqrtf(1.0f + 8.0f * dp / 59.3319f) - 1.0f) / denominator;
						}

						// range check on eps
						ଯଦି (fabsf(eps) >= 1.0f) {
							eps = 0.0f;
						}

						// pressure correction
						dp_tot = dp / (1.0f + eps);
					}
				}
				ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
					// do nothing
				}
				ଭାଙ୍ଗ;
			}

		}
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
			// do nothing
		}
		ଭାଙ୍ଗ;
	}

	// computed airspeed without correction for inflow-speed at tip of pitot-tube
	ଅଚଳ ଦଶମିକ airspeed_uncorrected = sqrtf(2.0f * dp_tot / kAirDensitySeaLevelStandardAtmos);

	// corrected airspeed
	ଅଚଳ ଦଶମିକ airspeed_corrected = airspeed_uncorrected + dv;

	// return result with correct sign
	ଫେରନ୍ତୁ (differential_pressure > 0.0f) ? airspeed_corrected : -airspeed_corrected;
}

ଦଶମିକ calc_IAS(ଦଶମିକ differential_pressure)
{
	ଯଦି (differential_pressure > 0.0f) {
		ଫେରନ୍ତୁ sqrtf((2.0f * differential_pressure) / kAirDensitySeaLevelStandardAtmos);

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ -sqrtf((2.0f * fabsf(differential_pressure)) / kAirDensitySeaLevelStandardAtmos);
	}

}

ଦଶମିକ calc_TAS_from_CAS(ଦଶମିକ speed_calibrated, ଦଶମିକ pressure_ambient, ଦଶମିକ temperature_celsius)
{
	ଯଦି (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	ଫେରନ୍ତୁ speed_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / getDensityFromPressureAndTemp(pressure_ambient,
					temperature_celsius));
}

ଦଶମିକ calc_CAS_from_IAS(ଦଶମିକ speed_indicated, ଦଶମିକ scale)
{
	ଫେରନ୍ତୁ speed_indicated * scale;
}

ଦଶମିକ calc_TAS(ଦଶମିକ total_pressure, ଦଶମିକ static_pressure, ଦଶମିକ temperature_celsius)
{
	ଦଶମିକ density = getDensityFromPressureAndTemp(static_pressure, temperature_celsius);

	ଯଦି (density < 0.0001f || !PX4_ISFINITE(density)) {
		density = kAirDensitySeaLevelStandardAtmos;
	}

	ଦଶମିକ pressure_difference = total_pressure - static_pressure;

	ଯଦି (pressure_difference > 0) {
		ଫେରନ୍ତୁ sqrtf((2.0f * (pressure_difference)) / density);

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ -sqrtf((2.0f * fabsf(pressure_difference)) / density);
	}
}

ଦଶମିକ calc_calibrated_from_true_airspeed(ଦଶମିକ speed_true, ଦଶମିକ air_density)
{
	ଫେରନ୍ତୁ speed_true * sqrtf(air_density / kAirDensitySeaLevelStandardAtmos);
}

ଦଶମିକ calc_true_from_calibrated_airspeed(ଦଶମିକ speed_calibrated, ଦଶମିକ air_density)
{
	ଫେରନ୍ତୁ speed_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
}
