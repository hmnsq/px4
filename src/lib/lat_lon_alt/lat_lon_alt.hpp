/****************************************************************************
 *
 *   Copyright (c) 2024 PX4 Development Team. All rights reserved.
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

#pragma once

#અન્તર્ભાવ "mathlib/math/Limits.hpp"
#અન્તર્ભાવ <matrix/math.hpp>

વર્ગ LatLonAlt
{
જાહેર:
	LatLonAlt() = મૂળભૂત;
	LatLonAlt(અચલ LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(અચલ દ્વિગુણ latitude_deg, અચલ દ્વિગુણ longitude_deg, અચલ દશાંશ altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	સ્થિર LatLonAlt fromEcef(અચલ matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() અચલ;

	શૂન્ય setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	દ્વિગુણ latitude_deg() અચલ { પાછા math::degrees(latitude_rad()); }
	દ્વિગુણ longitude_deg() અચલ { પાછા math::degrees(longitude_rad()); }

	અચલ દ્વિગુણ &latitude_rad() અચલ { પાછા _latitude_rad; }
	અચલ દ્વિગુણ &longitude_rad() અચલ { પાછા _longitude_rad; }
	દશાંશ altitude() અચલ { પાછા _altitude; }

	શૂન્ય setLatitudeDeg(અચલ દ્વિગુણ &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	શૂન્ય setLongitudeDeg(અચલ દ્વિગુણ &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	શૂન્ય setAltitude(અચલ દશાંશ altitude) { _altitude = altitude; }

	શૂન્ય setLatLon(અચલ LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	શૂન્ય setLatLonDeg(અચલ દ્વિગુણ latitude, અચલ દ્વિગુણ longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	શૂન્ય setLatLonRad(અચલ દ્વિગુણ latitude, અચલ દ્વિગુણ longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	શૂન્ય print() અચલ { printf("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (દ્વિગુણ)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt operator+(અચલ matrix::Vector3f &delta_pos) અચલ;
	શૂન્ય operator+=(અચલ matrix::Vector3f &delta_pos);
	શૂન્ય operator+=(અચલ matrix::Vector2f &delta_pos);
	matrix::Vector3f operator-(અચલ LatLonAlt &lla) અચલ;

	શૂન્ય operator=(અચલ LatLonAlt &lla)
	{
		// Protect against self-assignment
		જો (આ == &lla) {
			પાછા;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	matrix::Vector3f computeAngularRateNavFrame(અચલ matrix::Vector3f &v_ned) અચલ;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	matrix::Dcmf computeRotEcefToNed() અચલ;

	સંરચના Wgs84 {
		સ્થિર constexpr દ્વિગુણ equatorial_radius = 6378137.0;
		સ્થિર constexpr દ્વિગુણ eccentricity = 0.0818191908425;
		સ્થિર constexpr દ્વિગુણ eccentricity2 = eccentricity * eccentricity;
		સ્થિર constexpr દ્વિગુણ meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		સ્થિર constexpr દ્વિગુણ gravity_equator = 9.7803253359;
		સ્થિર constexpr દ્વિગુણ somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		સ્થિર દશાંશ gravity(અચલ દ્વિગુણ latitude_rad)
		{
			અચલ દ્વિગુણ sin_lat = sin(latitude_rad);
			અચલ દ્વિગુણ sin_lat2 = sin_lat * sin_lat;
			અચલ દ્વિગુણ g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::sqrt(1.0 - eccentricity2 * sin_lat2);
			પાછા static_cast<દશાંશ>(g);
		}
	};

ખાનગી:
	// Convert between curvilinear and cartesian errors
	સ્થિર matrix::Vector2d deltaLatLonToDeltaXY(અચલ દ્વિગુણ latitude, અચલ દશાંશ altitude);

	સ્થિર શૂન્ય computeRadiiOfCurvature(અચલ દ્વિગુણ latitude, દ્વિગુણ &meridian_radius_of_curvature,
					    દ્વિગુણ &transverse_radius_of_curvature);

	દ્વિગુણ _latitude_rad{0.0};
	દ્વિગુણ _longitude_rad{0.0};
	દશાંશ _altitude{0.0};
};
