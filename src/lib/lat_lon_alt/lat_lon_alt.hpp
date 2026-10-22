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

#شامل "mathlib/math/Limits.hpp"
#شامل <matrix/math.hpp>

رده LatLonAlt
{
عمومی:
	LatLonAlt() = پیشفرض;
	LatLonAlt(ثابت LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(ثابت دوگانه latitude_deg, ثابت دوگانه longitude_deg, ثابت اعشاری altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	ایستا LatLonAlt fromEcef(ثابت matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() ثابت;

	تهی setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	دوگانه latitude_deg() ثابت { بازگشت math::degrees(latitude_rad()); }
	دوگانه longitude_deg() ثابت { بازگشت math::degrees(longitude_rad()); }

	ثابت دوگانه &latitude_rad() ثابت { بازگشت _latitude_rad; }
	ثابت دوگانه &longitude_rad() ثابت { بازگشت _longitude_rad; }
	اعشاری altitude() ثابت { بازگشت _altitude; }

	تهی setLatitudeDeg(ثابت دوگانه &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	تهی setLongitudeDeg(ثابت دوگانه &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	تهی setAltitude(ثابت اعشاری altitude) { _altitude = altitude; }

	تهی setLatLon(ثابت LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	تهی setLatLonDeg(ثابت دوگانه latitude, ثابت دوگانه longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	تهی setLatLonRad(ثابت دوگانه latitude, ثابت دوگانه longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	تهی print() ثابت { printf("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (دوگانه)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt operator+(ثابت matrix::Vector3f &delta_pos) ثابت;
	تهی operator+=(ثابت matrix::Vector3f &delta_pos);
	تهی operator+=(ثابت matrix::Vector2f &delta_pos);
	matrix::Vector3f operator-(ثابت LatLonAlt &lla) ثابت;

	تهی operator=(ثابت LatLonAlt &lla)
	{
		// Protect against self-assignment
		اگر (این == &lla) {
			بازگشت;
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

	ساختار Wgs84 {
		ایستا constexpr دوگانه equatorial_radius = 6378137.0;
		ایستا constexpr دوگانه eccentricity = 0.0818191908425;
		ایستا constexpr دوگانه eccentricity2 = eccentricity * eccentricity;
		ایستا constexpr دوگانه meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		ایستا constexpr دوگانه gravity_equator = 9.7803253359;
		ایستا constexpr دوگانه somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		ایستا اعشاری gravity(ثابت دوگانه latitude_rad)
		{
			ثابت دوگانه sin_lat = sin(latitude_rad);
			ثابت دوگانه sin_lat2 = sin_lat * sin_lat;
			ثابت دوگانه g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::sqrt(1.0 - eccentricity2 * sin_lat2);
			بازگشت static_cast<اعشاری>(g);
		}
	};

خصوصی:
	// Convert between curvilinear and cartesian errors
	ایستا matrix::Vector2d deltaLatLonToDeltaXY(ثابت دوگانه latitude, ثابت اعشاری altitude);

	ایستا تهی computeRadiiOfCurvature(ثابت دوگانه latitude, دوگانه &meridian_radius_of_curvature,
					    دوگانه &transverse_radius_of_curvature);

	دوگانه _latitude_rad{0.0};
	دوگانه _longitude_rad{0.0};
	اعشاری _altitude{0.0};
};
