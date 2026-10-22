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

#ضمن "mathlib/math/Limits.hpp"
#ضمن <matrix/math.hpp>

صنف LatLonAlt
{
عام:
	LatLonAlt() = افتراضي;
	LatLonAlt(ثابت LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(ثابت مزدوج latitude_deg, ثابت مزدوج longitude_deg, ثابت عائم altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	ساكن LatLonAlt fromEcef(ثابت matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() ثابت;

	فراغ setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	مزدوج latitude_deg() ثابت { إرجاع math::degrees(latitude_rad()); }
	مزدوج longitude_deg() ثابت { إرجاع math::degrees(longitude_rad()); }

	ثابت مزدوج &latitude_rad() ثابت { إرجاع _latitude_rad; }
	ثابت مزدوج &longitude_rad() ثابت { إرجاع _longitude_rad; }
	عائم altitude() ثابت { إرجاع _altitude; }

	فراغ setLatitudeDeg(ثابت مزدوج &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	فراغ setLongitudeDeg(ثابت مزدوج &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	فراغ setAltitude(ثابت عائم altitude) { _altitude = altitude; }

	فراغ setLatLon(ثابت LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	فراغ setLatLonDeg(ثابت مزدوج latitude, ثابت مزدوج longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	فراغ setLatLonRad(ثابت مزدوج latitude, ثابت مزدوج longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	فراغ print() ثابت { printf("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (مزدوج)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt operator+(ثابت matrix::Vector3f &delta_pos) ثابت;
	فراغ operator+=(ثابت matrix::Vector3f &delta_pos);
	فراغ operator+=(ثابت matrix::Vector2f &delta_pos);
	matrix::Vector3f operator-(ثابت LatLonAlt &lla) ثابت;

	فراغ operator=(ثابت LatLonAlt &lla)
	{
		// Protect against self-assignment
		إذا (هذا == &lla) {
			إرجاع;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	matrix::Vector3f computeAngularRateNavFrame(ثابت matrix::Vector3f &v_ned) ثابت;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	matrix::Dcmf computeRotEcefToNed() ثابت;

	بنية Wgs84 {
		ساكن constexpr مزدوج equatorial_radius = 6378137.0;
		ساكن constexpr مزدوج eccentricity = 0.0818191908425;
		ساكن constexpr مزدوج eccentricity2 = eccentricity * eccentricity;
		ساكن constexpr مزدوج meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		ساكن constexpr مزدوج gravity_equator = 9.7803253359;
		ساكن constexpr مزدوج somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		ساكن عائم gravity(ثابت مزدوج latitude_rad)
		{
			ثابت مزدوج sin_lat = sin(latitude_rad);
			ثابت مزدوج sin_lat2 = sin_lat * sin_lat;
			ثابت مزدوج g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::sqrt(1.0 - eccentricity2 * sin_lat2);
			إرجاع static_cast<عائم>(g);
		}
	};

خاص:
	// Convert between curvilinear and cartesian errors
	ساكن matrix::Vector2d deltaLatLonToDeltaXY(ثابت مزدوج latitude, ثابت عائم altitude);

	ساكن فراغ computeRadiiOfCurvature(ثابت مزدوج latitude, مزدوج &meridian_radius_of_curvature,
					    مزدوج &transverse_radius_of_curvature);

	مزدوج _latitude_rad{0.0};
	مزدوج _longitude_rad{0.0};
	عائم _altitude{0.0};
};
