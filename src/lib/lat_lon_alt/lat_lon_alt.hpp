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

#ಅನ್ತರ್ಭಾವ "mathlib/math/Limits.hpp"
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

ವರ್ಗ LatLonAlt
{
ಸಾರ್ವಜನಿಕ:
	LatLonAlt() = ಸ್ವಭಾವ;
	LatLonAlt(ಧ್ರುವ LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(ಧ್ರುವ ದ್ವಿಗುಣ latitude_deg, ಧ್ರುವ ದ್ವಿಗುಣ longitude_deg, ಧ್ರುವ ಭಿನ್ನ altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	ಸ್ಥಿರ LatLonAlt fromEcef(ಧ್ರುವ matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() ಧ್ರುವ;

	ಶೂನ್ಯ setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	ದ್ವಿಗುಣ latitude_deg() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ math::degrees(latitude_rad()); }
	ದ್ವಿಗುಣ longitude_deg() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ math::degrees(longitude_rad()); }

	ಧ್ರುವ ದ್ವಿಗುಣ &latitude_rad() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _latitude_rad; }
	ಧ್ರುವ ದ್ವಿಗುಣ &longitude_rad() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _longitude_rad; }
	ಭಿನ್ನ altitude() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _altitude; }

	ಶೂನ್ಯ setLatitudeDeg(ಧ್ರುವ ದ್ವಿಗುಣ &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	ಶೂನ್ಯ setLongitudeDeg(ಧ್ರುವ ದ್ವಿಗುಣ &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	ಶೂನ್ಯ setAltitude(ಧ್ರುವ ಭಿನ್ನ altitude) { _altitude = altitude; }

	ಶೂನ್ಯ setLatLon(ಧ್ರುವ LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	ಶೂನ್ಯ setLatLonDeg(ಧ್ರುವ ದ್ವಿಗುಣ latitude, ಧ್ರುವ ದ್ವಿಗುಣ longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	ಶೂನ್ಯ setLatLonRad(ಧ್ರುವ ದ್ವಿಗುಣ latitude, ಧ್ರುವ ದ್ವಿಗುಣ longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	ಶೂನ್ಯ print() ಧ್ರುವ { printf("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (ದ್ವಿಗುಣ)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt operator+(ಧ್ರುವ matrix::Vector3f &delta_pos) ಧ್ರುವ;
	ಶೂನ್ಯ operator+=(ಧ್ರುವ matrix::Vector3f &delta_pos);
	ಶೂನ್ಯ operator+=(ಧ್ರುವ matrix::Vector2f &delta_pos);
	matrix::Vector3f operator-(ಧ್ರುವ LatLonAlt &lla) ಧ್ರುವ;

	ಶೂನ್ಯ operator=(ಧ್ರುವ LatLonAlt &lla)
	{
		// Protect against self-assignment
		ಯದಿ (ಏತತ್ == &lla) {
			ಪ್ರತ್ಯಾಗಮ;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	matrix::Vector3f computeAngularRateNavFrame(ಧ್ರುವ matrix::Vector3f &v_ned) ಧ್ರುವ;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	matrix::Dcmf computeRotEcefToNed() ಧ್ರುವ;

	ಸಂರಚನಾ Wgs84 {
		ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ equatorial_radius = 6378137.0;
		ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ eccentricity = 0.0818191908425;
		ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ eccentricity2 = eccentricity * eccentricity;
		ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ gravity_equator = 9.7803253359;
		ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		ಸ್ಥಿರ ಭಿನ್ನ gravity(ಧ್ರುವ ದ್ವಿಗುಣ latitude_rad)
		{
			ಧ್ರುವ ದ್ವಿಗುಣ sin_lat = sin(latitude_rad);
			ಧ್ರುವ ದ್ವಿಗುಣ sin_lat2 = sin_lat * sin_lat;
			ಧ್ರುವ ದ್ವಿಗುಣ g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::sqrt(1.0 - eccentricity2 * sin_lat2);
			ಪ್ರತ್ಯಾಗಮ static_cast<ಭಿನ್ನ>(g);
		}
	};

ಗುಪ್ತ:
	// Convert between curvilinear and cartesian errors
	ಸ್ಥಿರ matrix::Vector2d deltaLatLonToDeltaXY(ಧ್ರುವ ದ್ವಿಗುಣ latitude, ಧ್ರುವ ಭಿನ್ನ altitude);

	ಸ್ಥಿರ ಶೂನ್ಯ computeRadiiOfCurvature(ಧ್ರುವ ದ್ವಿಗುಣ latitude, ದ್ವಿಗುಣ &meridian_radius_of_curvature,
					    ದ್ವಿಗುಣ &transverse_radius_of_curvature);

	ದ್ವಿಗುಣ _latitude_rad{0.0};
	ದ್ವಿಗುಣ _longitude_rad{0.0};
	ಭಿನ್ನ _altitude{0.0};
};
