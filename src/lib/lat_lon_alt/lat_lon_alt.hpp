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

#അന്തര്ഭാവ "mathlib/math/Limits.hpp"
#അന്തര്ഭാവ <matrix/math.hpp>

വർഗ്ഗം LatLonAlt
{
പൊതു:
	LatLonAlt() = സ്ഥിരസ്ഥിതി;
	LatLonAlt(സ്ഥിരം LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(സ്ഥിരം ഇരട്ട latitude_deg, സ്ഥിരം ഇരട്ട longitude_deg, സ്ഥിരം ദശാംശം altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	സ്ഥിതം LatLonAlt fromEcef(സ്ഥിരം matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() സ്ഥിരം;

	ശൂന്യം setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	ഇരട്ട latitude_deg() സ്ഥിരം { തിരികെ math::degrees(latitude_rad()); }
	ഇരട്ട longitude_deg() സ്ഥിരം { തിരികെ math::degrees(longitude_rad()); }

	സ്ഥിരം ഇരട്ട &latitude_rad() സ്ഥിരം { തിരികെ _latitude_rad; }
	സ്ഥിരം ഇരട്ട &longitude_rad() സ്ഥിരം { തിരികെ _longitude_rad; }
	ദശാംശം altitude() സ്ഥിരം { തിരികെ _altitude; }

	ശൂന്യം setLatitudeDeg(സ്ഥിരം ഇരട്ട &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	ശൂന്യം setLongitudeDeg(സ്ഥിരം ഇരട്ട &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	ശൂന്യം setAltitude(സ്ഥിരം ദശാംശം altitude) { _altitude = altitude; }

	ശൂന്യം setLatLon(സ്ഥിരം LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	ശൂന്യം setLatLonDeg(സ്ഥിരം ഇരട്ട latitude, സ്ഥിരം ഇരട്ട longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	ശൂന്യം setLatLonRad(സ്ഥിരം ഇരട്ട latitude, സ്ഥിരം ഇരട്ട longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	ശൂന്യം print() സ്ഥിരം { printf("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (ഇരട്ട)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt operator+(സ്ഥിരം matrix::Vector3f &delta_pos) സ്ഥിരം;
	ശൂന്യം operator+=(സ്ഥിരം matrix::Vector3f &delta_pos);
	ശൂന്യം operator+=(സ്ഥിരം matrix::Vector2f &delta_pos);
	matrix::Vector3f operator-(സ്ഥിരം LatLonAlt &lla) സ്ഥിരം;

	ശൂന്യം operator=(സ്ഥിരം LatLonAlt &lla)
	{
		// Protect against self-assignment
		എങ്കിൽ (ഇത് == &lla) {
			തിരികെ;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	matrix::Vector3f computeAngularRateNavFrame(സ്ഥിരം matrix::Vector3f &v_ned) സ്ഥിരം;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	matrix::Dcmf computeRotEcefToNed() സ്ഥിരം;

	ഘടന Wgs84 {
		സ്ഥിതം constexpr ഇരട്ട equatorial_radius = 6378137.0;
		സ്ഥിതം constexpr ഇരട്ട eccentricity = 0.0818191908425;
		സ്ഥിതം constexpr ഇരട്ട eccentricity2 = eccentricity * eccentricity;
		സ്ഥിതം constexpr ഇരട്ട meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		സ്ഥിതം constexpr ഇരട്ട gravity_equator = 9.7803253359;
		സ്ഥിതം constexpr ഇരട്ട somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		സ്ഥിതം ദശാംശം gravity(സ്ഥിരം ഇരട്ട latitude_rad)
		{
			സ്ഥിരം ഇരട്ട sin_lat = sin(latitude_rad);
			സ്ഥിരം ഇരട്ട sin_lat2 = sin_lat * sin_lat;
			സ്ഥിരം ഇരട്ട g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::sqrt(1.0 - eccentricity2 * sin_lat2);
			തിരികെ static_cast<ദശാംശം>(g);
		}
	};

സ്വകാര്യ:
	// Convert between curvilinear and cartesian errors
	സ്ഥിതം matrix::Vector2d deltaLatLonToDeltaXY(സ്ഥിരം ഇരട്ട latitude, സ്ഥിരം ദശാംശം altitude);

	സ്ഥിതം ശൂന്യം computeRadiiOfCurvature(സ്ഥിരം ഇരട്ട latitude, ഇരട്ട &meridian_radius_of_curvature,
					    ഇരട്ട &transverse_radius_of_curvature);

	ഇരട്ട _latitude_rad{0.0};
	ഇരട്ട _longitude_rad{0.0};
	ദശാംശം _altitude{0.0};
};
