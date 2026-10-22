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

#অন্তর্ভাব "mathlib/math/Limits.hpp"
#অন্তর্ভাব <matrix/math.hpp>

শ্রেণী LatLonAlt
{
সর্বজনীন:
	LatLonAlt() = পূর্বনির্ধারিত;
	LatLonAlt(ধ্রুবক LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(ধ্রুবক দ্বিগুণ latitude_deg, ধ্রুবক দ্বিগুণ longitude_deg, ধ্রুবক দশমিক altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	স্থির LatLonAlt fromEcef(ধ্রুবক matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() ধ্রুবক;

	শূন্য setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	দ্বিগুণ latitude_deg() ধ্রুবক { ফেরত math::degrees(latitude_rad()); }
	দ্বিগুণ longitude_deg() ধ্রুবক { ফেরত math::degrees(longitude_rad()); }

	ধ্রুবক দ্বিগুণ &latitude_rad() ধ্রুবক { ফেরত _latitude_rad; }
	ধ্রুবক দ্বিগুণ &longitude_rad() ধ্রুবক { ফেরত _longitude_rad; }
	দশমিক altitude() ধ্রুবক { ফেরত _altitude; }

	শূন্য setLatitudeDeg(ধ্রুবক দ্বিগুণ &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	শূন্য setLongitudeDeg(ধ্রুবক দ্বিগুণ &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	শূন্য setAltitude(ধ্রুবক দশমিক altitude) { _altitude = altitude; }

	শূন্য setLatLon(ধ্রুবক LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	শূন্য setLatLonDeg(ধ্রুবক দ্বিগুণ latitude, ধ্রুবক দ্বিগুণ longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	শূন্য setLatLonRad(ধ্রুবক দ্বিগুণ latitude, ধ্রুবক দ্বিগুণ longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	শূন্য print() ধ্রুবক { printf("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (দ্বিগুণ)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt operator+(ধ্রুবক matrix::Vector3f &delta_pos) ধ্রুবক;
	শূন্য operator+=(ধ্রুবক matrix::Vector3f &delta_pos);
	শূন্য operator+=(ধ্রুবক matrix::Vector2f &delta_pos);
	matrix::Vector3f operator-(ধ্রুবক LatLonAlt &lla) ধ্রুবক;

	শূন্য operator=(ধ্রুবক LatLonAlt &lla)
	{
		// Protect against self-assignment
		যদি (এই == &lla) {
			ফেরত;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	matrix::Vector3f computeAngularRateNavFrame(ধ্রুবক matrix::Vector3f &v_ned) ধ্রুবক;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	matrix::Dcmf computeRotEcefToNed() ধ্রুবক;

	কাঠামো Wgs84 {
		স্থির constexpr দ্বিগুণ equatorial_radius = 6378137.0;
		স্থির constexpr দ্বিগুণ eccentricity = 0.0818191908425;
		স্থির constexpr দ্বিগুণ eccentricity2 = eccentricity * eccentricity;
		স্থির constexpr দ্বিগুণ meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		স্থির constexpr দ্বিগুণ gravity_equator = 9.7803253359;
		স্থির constexpr দ্বিগুণ somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		স্থির দশমিক gravity(ধ্রুবক দ্বিগুণ latitude_rad)
		{
			ধ্রুবক দ্বিগুণ sin_lat = sin(latitude_rad);
			ধ্রুবক দ্বিগুণ sin_lat2 = sin_lat * sin_lat;
			ধ্রুবক দ্বিগুণ g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::sqrt(1.0 - eccentricity2 * sin_lat2);
			ফেরত static_cast<দশমিক>(g);
		}
	};

ব্যক্তিগত:
	// Convert between curvilinear and cartesian errors
	স্থির matrix::Vector2d deltaLatLonToDeltaXY(ধ্রুবক দ্বিগুণ latitude, ধ্রুবক দশমিক altitude);

	স্থির শূন্য computeRadiiOfCurvature(ধ্রুবক দ্বিগুণ latitude, দ্বিগুণ &meridian_radius_of_curvature,
					    দ্বিগুণ &transverse_radius_of_curvature);

	দ্বিগুণ _latitude_rad{0.0};
	দ্বিগুণ _longitude_rad{0.0};
	দশমিক _altitude{0.0};
};
