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

#अन्तर्भाव "airspeed.h"

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <lib/atmosphere/atmosphere.h>

using atmosphere::getDensityFromPressureAndTemp;
using atmosphere::kAirDensitySeaLevelStandardAtmos;

भिन्न calc_IAS_corrected(गणना AIRSPEED_COMPENSATION_MODEL pmodel, गणना AIRSPEED_SENSOR_MODEL smodel,
			 भिन्न tube_len, भिन्न tube_dia_mm, भिन्न differential_pressure, भिन्न pressure_ambient, भिन्न temperature_celsius)
{
	यदि (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	// air density in kg/m3
	अचर भिन्न rho_air = getDensityFromPressureAndTemp(pressure_ambient, temperature_celsius);

	अचर भिन्न dp = fabsf(differential_pressure);
	भिन्न dp_tot = dp;

	भिन्न dv = 0.0f;

	विकल्प (smodel) {

	पक्ष AIRSPEED_SENSOR_MODEL_MEMBRANE: {
			// do nothing
		}
		रोक;

	पक्ष AIRSPEED_SENSOR_MODEL_SDP3X: {
			// assumes a metal pitot tube with round tip as here: https://drotek.com/shop/2986-large_default/sdp3x-airspeed-sensor-kit-sdp31.jpg
			// and tubing as provided by px4/drotek (1.5 mm diameter)
			// The tube_len represents the length of the tubes connecting the pitot to the sensor.
			विकल्प (pmodel) {
			पक्ष AIRSPEED_COMPENSATION_MODEL_PITOT:
			पक्ष AIRSPEED_COMPENSATION_MODEL_NO_PITOT: {
					अचर भिन्न dp_corr = dp * 96600.0f / pressure_ambient;
					// flow through sensor
					भिन्न flow_SDP33 = (300.805f - 300.878f / (0.00344205f * powf(dp_corr, 0.68698f) + 1.0f)) * 1.29f / rho_air;

					// for too small readings the compensation might result in a negative flow which causes numerical issues
					यदि (flow_SDP33 < 0.0f) {
						flow_SDP33 = 0.0f;
					}

					भिन्न dp_pitot = 0.0f;

					विकल्प (pmodel) {
					पक्ष AIRSPEED_COMPENSATION_MODEL_PITOT:
						dp_pitot = (0.0032f * flow_SDP33 * flow_SDP33 + 0.0123f * flow_SDP33 + 1.0f) * 1.29f / rho_air;
						रोक;

					स्वभाव:
						// do nothing
						रोक;
					}

					// pressure drop through tube
					अचर भिन्न dp_tube = (flow_SDP33 * 0.674f) / 450.0f * tube_len * rho_air / 1.29f;

					// speed at pitot-tube tip due to flow through sensor
					dv = 0.125f * flow_SDP33;

					// sum of all pressure drops
					dp_tot = dp_corr + dp_tube + dp_pitot;
				}
				रोक;

			पक्ष AIRSPEED_COMPENSATION_TUBE_PRESSURE_LOSS: {
					// Pressure loss compensation as defined in https://goo.gl/UHV1Vv.
					// tube_dia_mm: Diameter in mm of the pitot and tubes, must have the same diameter.
					// tube_len: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.

					// check if the tube diameter and dp is nonzero to avoid division by 0
					यदि ((tube_dia_mm > 0.0f) && (dp > 0.0f)) {
						अचर भिन्न d_tubePow4 = powf(tube_dia_mm * 1e-3f, 4);
						अचर भिन्न denominator = M_PI_F * d_tubePow4 * rho_air * dp;

						// avoid division by 0
						भिन्न eps = 0.0f;

						यदि (fabsf(denominator) > 1e-32f) {
							अचर भिन्न viscosity = (18.205f + 0.0484f * (temperature_celsius - 20.0f)) * 1e-6f;

							// 4.79 * 1e-7 -> mass flow through sensor
							// 59.5 -> dp sensor constant where linear and quadratic contribution to dp vs flow is equal
							eps = -64.0f * tube_len * viscosity * 4.79f * 1e-7f * (sqrtf(1.0f + 8.0f * dp / 59.3319f) - 1.0f) / denominator;
						}

						// range check on eps
						यदि (fabsf(eps) >= 1.0f) {
							eps = 0.0f;
						}

						// pressure correction
						dp_tot = dp / (1.0f + eps);
					}
				}
				रोक;

			स्वभाव: {
					// do nothing
				}
				रोक;
			}

		}
		रोक;

	स्वभाव: {
			// do nothing
		}
		रोक;
	}

	// computed airspeed without correction for inflow-speed at tip of pitot-tube
	अचर भिन्न airspeed_uncorrected = sqrtf(2.0f * dp_tot / kAirDensitySeaLevelStandardAtmos);

	// corrected airspeed
	अचर भिन्न airspeed_corrected = airspeed_uncorrected + dv;

	// return result with correct sign
	फर्काऊ (differential_pressure > 0.0f) ? airspeed_corrected : -airspeed_corrected;
}

भिन्न calc_IAS(भिन्न differential_pressure)
{
	यदि (differential_pressure > 0.0f) {
		फर्काऊ sqrtf((2.0f * differential_pressure) / kAirDensitySeaLevelStandardAtmos);

	} अन्यथा {
		फर्काऊ -sqrtf((2.0f * fabsf(differential_pressure)) / kAirDensitySeaLevelStandardAtmos);
	}

}

भिन्न calc_TAS_from_CAS(भिन्न speed_calibrated, भिन्न pressure_ambient, भिन्न temperature_celsius)
{
	यदि (!PX4_ISFINITE(temperature_celsius)) {
		temperature_celsius = 15.f; // ICAO Standard Atmosphere 15 degrees Celsius
	}

	फर्काऊ speed_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / getDensityFromPressureAndTemp(pressure_ambient,
					temperature_celsius));
}

भिन्न calc_CAS_from_IAS(भिन्न speed_indicated, भिन्न scale)
{
	फर्काऊ speed_indicated * scale;
}

भिन्न calc_TAS(भिन्न total_pressure, भिन्न static_pressure, भिन्न temperature_celsius)
{
	भिन्न density = getDensityFromPressureAndTemp(static_pressure, temperature_celsius);

	यदि (density < 0.0001f || !PX4_ISFINITE(density)) {
		density = kAirDensitySeaLevelStandardAtmos;
	}

	भिन्न pressure_difference = total_pressure - static_pressure;

	यदि (pressure_difference > 0) {
		फर्काऊ sqrtf((2.0f * (pressure_difference)) / density);

	} अन्यथा {
		फर्काऊ -sqrtf((2.0f * fabsf(pressure_difference)) / density);
	}
}

भिन्न calc_calibrated_from_true_airspeed(भिन्न speed_true, भिन्न air_density)
{
	फर्काऊ speed_true * sqrtf(air_density / kAirDensitySeaLevelStandardAtmos);
}

भिन्न calc_true_from_calibrated_airspeed(भिन्न speed_calibrated, भिन्न air_density)
{
	फर्काऊ speed_calibrated * sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
}
