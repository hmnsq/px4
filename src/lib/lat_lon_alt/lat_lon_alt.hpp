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

#अन्तर्भाव "mathlib/math/Limits.hpp"
#अन्तर्भाव <matrix/math.hpp>

वर्ग LatLonAlt
{
सार्वजनिक:
	LatLonAlt() = स्वभाव;
	LatLonAlt(ध्रुव LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(ध्रुव द्विगुण latitude_deg, ध्रुव द्विगुण longitude_deg, ध्रुव भिन्न altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	स्थिर LatLonAlt fromEcef(ध्रुव matrix::Vector3d &p_ecef);
	matrix::Vector3d toEcef() ध्रुव;

	शून्य setZero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	द्विगुण latitude_deg() ध्रुव { प्रत्यागम math::degrees(latitude_rad()); }
	द्विगुण longitude_deg() ध्रुव { प्रत्यागम math::degrees(longitude_rad()); }

	ध्रुव द्विगुण &latitude_rad() ध्रुव { प्रत्यागम _latitude_rad; }
	ध्रुव द्विगुण &longitude_rad() ध्रुव { प्रत्यागम _longitude_rad; }
	भिन्न altitude() ध्रुव { प्रत्यागम _altitude; }

	शून्य setLatitudeDeg(ध्रुव द्विगुण &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	शून्य setLongitudeDeg(ध्रुव द्विगुण &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	शून्य setAltitude(ध्रुव भिन्न altitude) { _altitude = altitude; }

	शून्य setLatLon(ध्रुव LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	शून्य setLatLonDeg(ध्रुव द्विगुण latitude, ध्रुव द्विगुण longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	शून्य setLatLonRad(ध्रुव द्विगुण latitude, ध्रुव द्विगुण longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	शून्य print() ध्रुव { म_लिखो("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (द्विगुण)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt चालक+(ध्रुव matrix::Vector3f &delta_pos) ध्रुव;
	शून्य चालक+=(ध्रुव matrix::Vector3f &delta_pos);
	शून्य चालक+=(ध्रुव matrix::Vector2f &delta_pos);
	matrix::Vector3f चालक-(ध्रुव LatLonAlt &lla) ध्रुव;

	शून्य चालक=(ध्रुव LatLonAlt &lla)
	{
		// Protect against self-assignment
		यदि (एतत् == &lla) {
			प्रत्यागम;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	matrix::Vector3f computeAngularRateNavFrame(ध्रुव matrix::Vector3f &v_ned) ध्रुव;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	matrix::Dcmf computeRotEcefToNed() ध्रुव;

	संरचना Wgs84 {
		स्थिर constexpr द्विगुण equatorial_radius = 6378137.0;
		स्थिर constexpr द्विगुण eccentricity = 0.0818191908425;
		स्थिर constexpr द्विगुण eccentricity2 = eccentricity * eccentricity;
		स्थिर constexpr द्विगुण meridian_radius_of_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		स्थिर constexpr द्विगुण gravity_equator = 9.7803253359;
		स्थिर constexpr द्विगुण somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		स्थिर भिन्न gravity(ध्रुव द्विगुण latitude_rad)
		{
			ध्रुव द्विगुण sin_lat = sin(latitude_rad);
			ध्रुव द्विगुण sin_lat2 = sin_lat * sin_lat;
			ध्रुव द्विगुण g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::वर्ग_मूल(1.0 - eccentricity2 * sin_lat2);
			प्रत्यागम static_cast<भिन्न>(g);
		}
	};

गुप्त:
	// Convert between curvilinear and cartesian errors
	स्थिर matrix::Vector2d deltaLatLonToDeltaXY(ध्रुव द्विगुण latitude, ध्रुव भिन्न altitude);

	स्थिर शून्य computeRadiiOfCurvature(ध्रुव द्विगुण latitude, द्विगुण &meridian_radius_of_curvature,
					    द्विगुण &transverse_radius_of_curvature);

	द्विगुण _latitude_rad{0.0};
	द्विगुण _longitude_rad{0.0};
	भिन्न _altitude{0.0};
};
