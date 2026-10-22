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

#அந்தர்பாவ "mathlib/math/Limits.hpp"
#அந்தர்பாவ <matrix/math.hpp>

வகுப்பு LatLonAlt
{
பொது:
	LatLonAlt() = இயல்பு;
	LatLonAlt(மாறிலி LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(மாறிலி இரட்டை latitude_deg, மாறிலி இரட்டை longitude_deg, மாறிலி தசம altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	நிலையான LatLonAlt fromEcef(மாறிலி matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() மாறிலி;

	வெறுமை setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	இரட்டை latitude_deg() மாறிலி { திரும்பு math::degrees(latitude_rad()); }
	இரட்டை longitude_deg() மாறிலி { திரும்பு math::degrees(longitude_rad()); }

	மாறிலி இரட்டை &latitude_rad() மாறிலி { திரும்பு _latitude_rad; }
	மாறிலி இரட்டை &longitude_rad() மாறிலி { திரும்பு _longitude_rad; }
	தசம altitude() மாறிலி { திரும்பு _altitude; }

	வெறுமை setLatitudeDeg(மாறிலி இரட்டை &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	வெறுமை setLongitudeDeg(மாறிலி இரட்டை &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	வெறுமை setAltitude(மாறிலி தசம altitude) { _altitude = altitude; }

	வெறுமை setLatLon(மாறிலி LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	வெறுமை setLatLonDeg(மாறிலி இரட்டை latitude, மாறிலி இரட்டை longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	வெறுமை setLatLonRad(மாறிலி இரட்டை latitude, மாறிலி இரட்டை longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	வெறுமை print() மாறிலி { printf("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (இரட்டை)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt operator+(மாறிலி matrix::Vector3f &delta_pos) மாறிலி;
	வெறுமை operator+=(மாறிலி matrix::Vector3f &delta_pos);
	வெறுமை operator+=(மாறிலி matrix::Vector2f &delta_pos);
	matrix::Vector3f operator-(மாறிலி LatLonAlt &lla) மாறிலி;

	வெறுமை operator=(மாறிலி LatLonAlt &lla)
	{
		// Protect against self-assignment
		எனில் (இது == &lla) {
			திரும்பு;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	matrix::Vector3f computeAngularRateNavFrame(மாறிலி matrix::Vector3f &v_ned) மாறிலி;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	matrix::Dcmf computeRotEcefToNed() மாறிலி;

	கட்டமைப்பு Wgs84 {
		நிலையான constexpr இரட்டை equatorial_radius = 6378137.0;
		நிலையான constexpr இரட்டை eccentricity = 0.0818191908425;
		நிலையான constexpr இரட்டை eccentricity2 = eccentricity * eccentricity;
		நிலையான constexpr இரட்டை meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		நிலையான constexpr இரட்டை gravity_equator = 9.7803253359;
		நிலையான constexpr இரட்டை somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		நிலையான தசம gravity(மாறிலி இரட்டை latitude_rad)
		{
			மாறிலி இரட்டை sin_lat = sin(latitude_rad);
			மாறிலி இரட்டை sin_lat2 = sin_lat * sin_lat;
			மாறிலி இரட்டை g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::sqrt(1.0 - eccentricity2 * sin_lat2);
			திரும்பு static_cast<தசம>(g);
		}
	};

தனிப்பட்ட:
	// Convert between curvilinear and cartesian errors
	நிலையான matrix::Vector2d deltaLatLonToDeltaXY(மாறிலி இரட்டை latitude, மாறிலி தசம altitude);

	நிலையான வெறுமை computeRadiiOfCurvature(மாறிலி இரட்டை latitude, இரட்டை &meridian_radius_of_curvature,
					    இரட்டை &transverse_radius_of_curvature);

	இரட்டை _latitude_rad{0.0};
	இரட்டை _longitude_rad{0.0};
	தசம _altitude{0.0};
};
