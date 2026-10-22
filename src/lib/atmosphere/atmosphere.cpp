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

#అన్తర్భావ <geo/geo.h>
#అన్తర్భావ "atmosphere.h"
నామాకాశ atmosphere
{


దశాంశం get_Density_From_Pressure_And_తాత్కాలిక(స్థిరరాశి దశాంశం pressure_pa, స్థిరరాశి దశాంశం temperature_celsius)
{
	తిరిగి (pressure_pa / (kAirGasConstant * (temperature_celsius - kAbsoluteNullCelsius)));
}
దశాంశం getPressureFromAltitude(స్థిరరాశి దశాంశం altitude_m)
{

	తిరిగి kPressRefSeaLevelPa * powf((altitude_m * k_తాత్కాలిక_Gradient + k_తాత్కాలిక_Ref_Kelvin) / k_తాత్కాలిక_Ref_Kelvin,
					  -CONSTANTS_ONE_G / (k_తాత్కాలిక_Gradient * kAirGasConstant));
}
దశాంశం getAltitudeFromPressure(దశాంశం pressure_pa, దశాంశం pressure_sealevel_pa)
{
	// calculate altitude using the hypsometric equation

	స్థిరరాశి దశాంశం pressure_నిష్పత్తి = pressure_pa / pressure_sealevel_pa;

	/*
	 * Solve:
	 *
	 *     /        -(aR / g)     \
	 *    | (p / p1)          . T1 | - T1
	 *     \                      /
	 * h = -------------------------------  + h1
	 *                   a
	 */
	తిరిగి (((powf(pressure_నిష్పత్తి, (-(k_తాత్కాలిక_Gradient * kAirGasConstant) / CONSTANTS_ONE_G))) * k_తాత్కాలిక_Ref_Kelvin) -
		k_తాత్కాలిక_Ref_Kelvin) / k_తాత్కాలిక_Gradient;

}
దశాంశం getStandardTemperatureAtAltitude(దశాంశం altitude_m)
{
	తిరిగి 15.0f + k_తాత్కాలిక_Gradient * altitude_m;
}
}
