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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "mathlib/math/Limits.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>

ਵਰਗ LatLonAlt
{
ਜਨਤਕ:
	LatLonAlt() = ਮੂਲ;
	LatLonAlt(ਅਚਲ LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(ਅਚਲ ਦੁੱਗਣਾ latitude_deg, ਅਚਲ ਦੁੱਗਣਾ longitude_deg, ਅਚਲ ਦਸ਼ਮਲਵ altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	ਸਥਿਰ LatLonAlt fromEcef(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3d &p_ecef);
	ਮੈਟ੍ਰਿਕਸ::Vector3d ਮੰਜ਼ਿਲ_Ecef() ਅਚਲ;

	ਖਾਲੀ ਸੈੱਟ_Zero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	ਦੁੱਗਣਾ latitude_deg() ਅਚਲ { ਵਾਪਸ math::degrees(latitude_rad()); }
	ਦੁੱਗਣਾ longitude_deg() ਅਚਲ { ਵਾਪਸ math::degrees(longitude_rad()); }

	ਅਚਲ ਦੁੱਗਣਾ &latitude_rad() ਅਚਲ { ਵਾਪਸ _latitude_rad; }
	ਅਚਲ ਦੁੱਗਣਾ &longitude_rad() ਅਚਲ { ਵਾਪਸ _longitude_rad; }
	ਦਸ਼ਮਲਵ altitude() ਅਚਲ { ਵਾਪਸ _altitude; }

	ਖਾਲੀ ਸੈੱਟ_Latitude_Deg(ਅਚਲ ਦੁੱਗਣਾ &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	ਖਾਲੀ ਸੈੱਟ_Longitude_Deg(ਅਚਲ ਦੁੱਗਣਾ &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	ਖਾਲੀ ਸੈੱਟ_Altitude(ਅਚਲ ਦਸ਼ਮਲਵ altitude) { _altitude = altitude; }

	ਖਾਲੀ ਸੈੱਟ_Lat_Lon(ਅਚਲ LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	ਖਾਲੀ ਸੈੱਟ_Lat_Lon_Deg(ਅਚਲ ਦੁੱਗਣਾ latitude, ਅਚਲ ਦੁੱਗਣਾ longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	ਖਾਲੀ ਸੈੱਟ_Lat_Lon_Rad(ਅਚਲ ਦੁੱਗਣਾ latitude, ਅਚਲ ਦੁੱਗਣਾ longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	ਖਾਲੀ print() ਅਚਲ { ਲਿਖੋ("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (ਦੁੱਗਣਾ)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt ਸੰਚਾਲਕ+(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &delta_pos) ਅਚਲ;
	ਖਾਲੀ ਸੰਚਾਲਕ+=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &delta_pos);
	ਖਾਲੀ ਸੰਚਾਲਕ+=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &delta_pos);
	ਮੈਟ੍ਰਿਕਸ::Vector3f ਸੰਚਾਲਕ-(ਅਚਲ LatLonAlt &lla) ਅਚਲ;

	ਖਾਲੀ ਸੰਚਾਲਕ=(ਅਚਲ LatLonAlt &lla)
	{
		// Protect against self-assignment
		ਜੇ (ਇਹ == &lla) {
			ਵਾਪਸ;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	ਮੈਟ੍ਰਿਕਸ::Vector3f compute_Angular_ਦਰ_Nav_ਚੌਖਟਾ(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &v_ned) ਅਚਲ;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	ਮੈਟ੍ਰਿਕਸ::Dcmf compute_Rot_Ecef_ਮੰਜ਼ਿਲ_Ned() ਅਚਲ;

	ਬਣਤਰ Wgs84 {
		ਸਥਿਰ constexpr ਦੁੱਗਣਾ equatorial_radius = 6378137.0;
		ਸਥਿਰ constexpr ਦੁੱਗਣਾ eccentricity = 0.0818191908425;
		ਸਥਿਰ constexpr ਦੁੱਗਣਾ eccentricity2 = eccentricity * eccentricity;
		ਸਥਿਰ constexpr ਦੁੱਗਣਾ meridian_radius_ਦਾ_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		ਸਥਿਰ constexpr ਦੁੱਗਣਾ gravity_equator = 9.7803253359;
		ਸਥਿਰ constexpr ਦੁੱਗਣਾ somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		ਸਥਿਰ ਦਸ਼ਮਲਵ gravity(ਅਚਲ ਦੁੱਗਣਾ latitude_rad)
		{
			ਅਚਲ ਦੁੱਗਣਾ sin_lat = ਜੀਵਾ(latitude_rad);
			ਅਚਲ ਦੁੱਗਣਾ sin_lat2 = sin_lat * sin_lat;
			ਅਚਲ ਦੁੱਗਣਾ g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::ਵਰਗਮੂਲ(1.0 - eccentricity2 * sin_lat2);
			ਵਾਪਸ static_cast<ਦਸ਼ਮਲਵ>(g);
		}
	};

ਨਿਜੀ:
	// Convert between curvilinear and cartesian errors
	ਸਥਿਰ ਮੈਟ੍ਰਿਕਸ::Vector2d delta_Lat_Lon_ਮੰਜ਼ਿਲ_Delta_XY(ਅਚਲ ਦੁੱਗਣਾ latitude, ਅਚਲ ਦਸ਼ਮਲਵ altitude);

	ਸਥਿਰ ਖਾਲੀ compute_Radii_ਦਾ_Curvature(ਅਚਲ ਦੁੱਗਣਾ latitude, ਦੁੱਗਣਾ &meridian_radius_ਦਾ_curvature,
					    ਦੁੱਗਣਾ &transverse_radius_ਦਾ_curvature);

	ਦੁੱਗਣਾ _latitude_rad{0.0};
	ਦੁੱਗਣਾ _longitude_rad{0.0};
	ਦਸ਼ਮਲਵ _altitude{0.0};
};
