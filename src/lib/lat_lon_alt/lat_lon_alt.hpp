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

#ଅନ୍ତର୍ଭାଵ "mathlib/math/Limits.hpp"
#ଅନ୍ତର୍ଭାଵ <matrix/math.hpp>

ଶ୍ରେଣୀ LatLonAlt
{
ସର୍ବସାଧାରଣ:
	LatLonAlt() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	LatLonAlt(ଅଚଳ LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(ଅଚଳ ଦ୍ୱିଗୁଣ latitude_deg, ଅଚଳ ଦ୍ୱିଗୁଣ longitude_deg, ଅଚଳ ଦଶମିକ altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	ସ୍ଥିର LatLonAlt fromEcef(ଅଚଳ matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() ଅଚଳ;

	ଶୂନ୍ୟ setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	ଦ୍ୱିଗୁଣ latitude_deg() ଅଚଳ { ଫେରନ୍ତୁ math::degrees(latitude_rad()); }
	ଦ୍ୱିଗୁଣ longitude_deg() ଅଚଳ { ଫେରନ୍ତୁ math::degrees(longitude_rad()); }

	ଅଚଳ ଦ୍ୱିଗୁଣ &latitude_rad() ଅଚଳ { ଫେରନ୍ତୁ _latitude_rad; }
	ଅଚଳ ଦ୍ୱିଗୁଣ &longitude_rad() ଅଚଳ { ଫେରନ୍ତୁ _longitude_rad; }
	ଦଶମିକ altitude() ଅଚଳ { ଫେରନ୍ତୁ _altitude; }

	ଶୂନ୍ୟ setLatitudeDeg(ଅଚଳ ଦ୍ୱିଗୁଣ &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	ଶୂନ୍ୟ setLongitudeDeg(ଅଚଳ ଦ୍ୱିଗୁଣ &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	ଶୂନ୍ୟ setAltitude(ଅଚଳ ଦଶମିକ altitude) { _altitude = altitude; }

	ଶୂନ୍ୟ setLatLon(ଅଚଳ LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	ଶୂନ୍ୟ setLatLonDeg(ଅଚଳ ଦ୍ୱିଗୁଣ latitude, ଅଚଳ ଦ୍ୱିଗୁଣ longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	ଶୂନ୍ୟ setLatLonRad(ଅଚଳ ଦ୍ୱିଗୁଣ latitude, ଅଚଳ ଦ୍ୱିଗୁଣ longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	ଶୂନ୍ୟ print() ଅଚଳ { printf("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (ଦ୍ୱିଗୁଣ)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt operator+(ଅଚଳ matrix::Vector3f &delta_pos) ଅଚଳ;
	ଶୂନ୍ୟ operator+=(ଅଚଳ matrix::Vector3f &delta_pos);
	ଶୂନ୍ୟ operator+=(ଅଚଳ matrix::Vector2f &delta_pos);
	matrix::Vector3f operator-(ଅଚଳ LatLonAlt &lla) ଅଚଳ;

	ଶୂନ୍ୟ operator=(ଅଚଳ LatLonAlt &lla)
	{
		// Protect against self-assignment
		ଯଦି (ଏହି == &lla) {
			ଫେରନ୍ତୁ;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	matrix::Vector3f computeAngularRateNavFrame(ଅଚଳ matrix::Vector3f &v_ned) ଅଚଳ;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	matrix::Dcmf computeRotEcefToNed() ଅଚଳ;

	ସଂରଚନା Wgs84 {
		ସ୍ଥିର constexpr ଦ୍ୱିଗୁଣ equatorial_radius = 6378137.0;
		ସ୍ଥିର constexpr ଦ୍ୱିଗୁଣ eccentricity = 0.0818191908425;
		ସ୍ଥିର constexpr ଦ୍ୱିଗୁଣ eccentricity2 = eccentricity * eccentricity;
		ସ୍ଥିର constexpr ଦ୍ୱିଗୁଣ meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		ସ୍ଥିର constexpr ଦ୍ୱିଗୁଣ gravity_equator = 9.7803253359;
		ସ୍ଥିର constexpr ଦ୍ୱିଗୁଣ somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		ସ୍ଥିର ଦଶମିକ gravity(ଅଚଳ ଦ୍ୱିଗୁଣ latitude_rad)
		{
			ଅଚଳ ଦ୍ୱିଗୁଣ sin_lat = sin(latitude_rad);
			ଅଚଳ ଦ୍ୱିଗୁଣ sin_lat2 = sin_lat * sin_lat;
			ଅଚଳ ଦ୍ୱିଗୁଣ g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::sqrt(1.0 - eccentricity2 * sin_lat2);
			ଫେରନ୍ତୁ static_cast<ଦଶମିକ>(g);
		}
	};

ବ୍ୟକ୍ତିଗତ:
	// Convert between curvilinear and cartesian errors
	ସ୍ଥିର matrix::Vector2d deltaLatLonToDeltaXY(ଅଚଳ ଦ୍ୱିଗୁଣ latitude, ଅଚଳ ଦଶମିକ altitude);

	ସ୍ଥିର ଶୂନ୍ୟ computeRadiiOfCurvature(ଅଚଳ ଦ୍ୱିଗୁଣ latitude, ଦ୍ୱିଗୁଣ &meridian_radius_of_curvature,
					    ଦ୍ୱିଗୁଣ &transverse_radius_of_curvature);

	ଦ୍ୱିଗୁଣ _latitude_rad{0.0};
	ଦ୍ୱିଗୁଣ _longitude_rad{0.0};
	ଦଶମିକ _altitude{0.0};
};
