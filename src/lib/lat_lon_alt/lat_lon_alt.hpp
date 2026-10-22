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

#ہدایت once

#شامل "mathlib/math/Limits.hpp"
#شامل <matrix/math.hpp>

جماعت LatLonAlt
{
عوامی:
	LatLonAlt() = طےشدہ;
	LatLonAlt(ثابت LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(ثابت دوہرا latitude_deg, ثابت دوہرا longitude_deg, ثابت اعشاری altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	ساکن LatLonAlt fromEcef(ثابت میٹرکس::Vector3d &p_ecef);
	میٹرکس::Vector3d مقصد_Ecef() ثابت;

	خالی سیٹ_Zero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	دوہرا latitude_deg() ثابت { واپس math::degrees(latitude_rad()); }
	دوہرا longitude_deg() ثابت { واپس math::degrees(longitude_rad()); }

	ثابت دوہرا &latitude_rad() ثابت { واپس _latitude_rad; }
	ثابت دوہرا &longitude_rad() ثابت { واپس _longitude_rad; }
	اعشاری altitude() ثابت { واپس _altitude; }

	خالی سیٹ_Latitude_Deg(ثابت دوہرا &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	خالی سیٹ_Longitude_Deg(ثابت دوہرا &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	خالی سیٹ_Altitude(ثابت اعشاری altitude) { _altitude = altitude; }

	خالی سیٹ_Lat_Lon(ثابت LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	خالی سیٹ_Lat_Lon_Deg(ثابت دوہرا latitude, ثابت دوہرا longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	خالی سیٹ_Lat_Lon_Rad(ثابت دوہرا latitude, ثابت دوہرا longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	خالی print() ثابت { لکھو("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (دوہرا)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt عامل+(ثابت میٹرکس::Vector3f &delta_pos) ثابت;
	خالی عامل+=(ثابت میٹرکس::Vector3f &delta_pos);
	خالی عامل+=(ثابت میٹرکس::Vector2f &delta_pos);
	میٹرکس::Vector3f عامل-(ثابت LatLonAlt &lla) ثابت;

	خالی عامل=(ثابت LatLonAlt &lla)
	{
		// Protect against self-assignment
		اگر (یہ == &lla) {
			واپس;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	میٹرکس::Vector3f compute_Angular_شرح_Nav_چوکھٹا(ثابت میٹرکس::Vector3f &v_ned) ثابت;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	میٹرکس::Dcmf compute_Rot_Ecef_مقصد_Ned() ثابت;

	ساخت Wgs84 {
		ساکن constexpr دوہرا equatorial_radius = 6378137.0;
		ساکن constexpr دوہرا eccentricity = 0.0818191908425;
		ساکن constexpr دوہرا eccentricity2 = eccentricity * eccentricity;
		ساکن constexpr دوہرا meridian_radius_کا_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		ساکن constexpr دوہرا gravity_equator = 9.7803253359;
		ساکن constexpr دوہرا somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		ساکن اعشاری gravity(ثابت دوہرا latitude_rad)
		{
			ثابت دوہرا sin_lat = جیب(latitude_rad);
			ثابت دوہرا sin_lat2 = sin_lat * sin_lat;
			ثابت دوہرا g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::جذر(1.0 - eccentricity2 * sin_lat2);
			واپس static_cast<اعشاری>(g);
		}
	};

نجی:
	// Convert between curvilinear and cartesian errors
	ساکن میٹرکس::Vector2d delta_Lat_Lon_مقصد_Delta_XY(ثابت دوہرا latitude, ثابت اعشاری altitude);

	ساکن خالی compute_Radii_کا_Curvature(ثابت دوہرا latitude, دوہرا &meridian_radius_کا_curvature,
					    دوہرا &transverse_radius_کا_curvature);

	دوہرا _latitude_rad{0.0};
	دوہرا _longitude_rad{0.0};
	اعشاری _altitude{0.0};
};
