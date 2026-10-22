/****************************************************************************
 *
 * Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

/*
 * @file CourseToAirspeedRefMapper.hpp
 *
 * Original Author:  Thomas Stastny <tstastny@ethz.ch>
 * Refactored to better suite new control API: Roman Bapst <roman@auterion.com>
 *
 * * Notes:
 * - The wind estimate should be dynamic enough to capture ~1-2 second length gusts,
 *   Otherwise the performance will suffer.
 *
 * Acknowledgements and References:
 *
 * The logic is mostly based on [1] and Paper III of [2].
 * TODO: Concise, up to date documentation and stability analysis for the following
 *       implementation.
 *
 * [1] T. Stastny and R. Siegwart. "On Flying Backwards: Preventing Run-away of
 *     Small, Low-speed, Fixed-wing UAVs in Strong Winds". IEEE International Conference
 *     on Intelligent Robots and Systems (IROS). 2019.
 *     https://arxiv.org/pdf/1908.01381.pdf
 * [2] T. Stastny. "Low-Altitude Control and Local Re-Planning Strategies for Small
 *     Fixed-Wing UAVs". Doctoral Thesis, ETH Zürich. 2020.
 *     https://tstastny.github.io/pdf/tstastny_phd_thesis_wcover.pdf
 */

#ifndef PX4_COURSETOAIRSPEEDREFMAPPER_HPP
#define PX4_COURSETOAIRSPEEDREFMAPPER_HPP


#അന്തര്ഭാവ <matrix/math.hpp>
#അന്തര്ഭാവ <lib/mathlib/mathlib.h>

വർഗ്ഗം CourseToAirspeedRefMapper
{
പൊതു:

	CourseToAirspeedRefMapper() {};

	~CourseToAirspeedRefMapper() = സ്ഥിരസ്ഥിതി;

	ദശാംശം mapCourseSetpointToHeadingSetpoint(സ്ഥിരം ദശാംശം bearing_setpoint,
			സ്ഥിരം matrix::Vector2f &wind_vel, ദശാംശം airspeed_sp) സ്ഥിരം;
	ദശാംശം getMinAirspeedForCurrentBearing(സ്ഥിരം ദശാംശം bearing_setpoint,
					      സ്ഥിരം matrix::Vector2f &wind_vel, ദശാംശം max_airspeed, ദശാംശം min_ground_speed) സ്ഥിരം;

സ്വകാര്യ:
	/*
	 * Projection of the air velocity vector onto the bearing line considering
	 * a connected wind triangle.
	 *
	 * @param[in] airspeed Vehicle true airspeed setpoint [m/s]
	 * @param[in] wind_cross_bearing 2D cross product of wind velocity and bearing vector [m/s]
	 * @return Projection of air velocity vector on bearing vector [m/s]
	 */
	ദശാംശം projectAirspOnBearing(സ്ഥിരം ദശാംശം airspeed, സ്ഥിരം ദശാംശം wind_cross_bearing) സ്ഥിരം;
	/*
	 * Check for binary bearing feasibility.
	 *
	 * @param[in] wind_cross_bearing 2D cross product of wind velocity and bearing vector [m/s]
	 * @param[in] wind_dot_bearing 2D dot product of wind velocity and bearing vector [m/s]
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @param[in] wind_speed Wind speed [m/s]
	 * @return Binary bearing feasibility: 1 if feasible, 0 if infeasible
	 */
	പൂർണ്ണസംഖ്യ bearingIsFeasible(സ്ഥിരം ദശാംശം wind_cross_bearing, സ്ഥിരം ദശാംശം wind_dot_bearing, സ്ഥിരം ദശാംശം airspeed,
			      സ്ഥിരം ദശാംശം wind_speed) സ്ഥിരം;
	/*
	 * Air velocity solution for a given wind velocity and bearing vector assuming
	 * a "high speed" (not backwards) solution in the excess wind case.
	 *
	 * @param[in] wind_cross_bearing 2D cross product of wind velocity and bearing vector [m/s]
	 * @param[in] airsp_dot_bearing 2D dot product of air velocity (solution) and bearing vector [m/s]
	 * @param[in] bearing_vec Bearing vector
	 * @return Air velocity reference vector [m/s]
	 */
	matrix::Vector2f solveWindTriangle(സ്ഥിരം ദശാംശം wind_cross_bearing, സ്ഥിരം ദശാംശം airsp_dot_bearing,
					   സ്ഥിരം matrix::Vector2f &bearing_vec) സ്ഥിരം;
	/*
	 * Air velocity solution for an infeasible bearing.
	 *
	 * @param[in] wind_vel Wind velocity vector [m/s]
	 * @param[in] bearing_vec Bearing vector
	 * @param[in] wind_speed Wind speed [m/s]
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @return Air velocity vector [m/s]
	 */
	matrix::Vector2f infeasibleAirVelRef(സ്ഥിരം matrix::Vector2f &wind_vel, സ്ഥിരം matrix::Vector2f &bearing_vec,
					     സ്ഥിരം ദശാംശം wind_speed, സ്ഥിരം ദശാംശം airspeed) സ്ഥിരം;
};

#endif //PX4_COURSETOAIRSPEEDREFMAPPER_HPP
