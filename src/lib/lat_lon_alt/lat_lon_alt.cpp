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

#ਅਨ੍ਤਰ੍ਭਾਵ "lat_lon_alt.hpp"

ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector3f;
ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector3d;
ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector2d;

LatLonAlt LatLonAlt::fromEcef(ਅਚਲ Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	ਅਚਲ ਦੁੱਗਣਾ k1 = ਵਰਗਮੂਲ(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	ਅਚਲ ਦੁੱਗਣਾ k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	ਅਚਲ ਦੁੱਗਣਾ beta = ਵਰਗਮੂਲ(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	ਅਚਲ ਦੁੱਗਣਾ E = (k1 - k2) / beta;
	ਅਚਲ ਦੁੱਗਣਾ F = (k1 + k2) / beta;

	ਅਚਲ ਦੁੱਗਣਾ P = 4.0 / 3.0 * (E * F + 1);
	ਅਚਲ ਦੁੱਗਣਾ Q = 2 * (E * E - F * F);
	ਅਚਲ ਦੁੱਗਣਾ D = P * P * P + Q * Q;
	ਅਚਲ ਦੁੱਗਣਾ V = cbrt(ਵਰਗਮੂਲ(D) - Q) - cbrt(ਵਰਗਮੂਲ(D) + Q);
	ਅਚਲ ਦੁੱਗਣਾ G = 0.5 * (ਵਰਗਮੂਲ(E * E + V) + E);
	ਅਚਲ ਦੁੱਗਣਾ T = ਵਰਗਮੂਲ(G * G + (F - V * G) / (2 * G - E)) - G;

	ਅਚਲ ਦੁੱਗਣਾ lon = atan2(p_ecef(1), p_ecef(0));
	ਅਚਲ ਦੁੱਗਣਾ lat = ਮੈਟ੍ਰਿਕਸ::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * ਵਰਗਮੂਲ(1.0 - Wgs84::eccentricity2)));
	ਅਚਲ ਦੁੱਗਣਾ alt = (beta - Wgs84::equatorial_radius * T) * ਕੋਜੀਵਾ(lat) +
			   (p_ecef(2) - ਮੈਟ੍ਰਿਕਸ::sign(p_ecef(2)) * Wgs84::equatorial_radius * ਵਰਗਮੂਲ(1.0 - Wgs84::eccentricity2)) * ਜੀਵਾ(lat);

	LatLonAlt lla;
	lla.ਸੈੱਟ_Lat_Lon_Rad(lat, lon);
	lla.ਸੈੱਟ_Altitude(static_cast<ਦਸ਼ਮਲਵ>(alt));
	ਵਾਪਸ lla;
}

Vector3d LatLonAlt::ਮੰਜ਼ਿਲ_Ecef() ਅਚਲ
{
	ਅਚਲ ਦੁੱਗਣਾ cos_lat = ਕੋਜੀਵਾ(_latitude_rad);
	ਅਚਲ ਦੁੱਗਣਾ sin_lat = ਜੀਵਾ(_latitude_rad);
	ਅਚਲ ਦੁੱਗਣਾ cos_lon = ਕੋਜੀਵਾ(_longitude_rad);
	ਅਚਲ ਦੁੱਗਣਾ sin_lon = ਜੀਵਾ(_longitude_rad);

	ਅਚਲ ਦੁੱਗਣਾ r_e = Wgs84::equatorial_radius / ਵਰਗਮੂਲ(1.0 - std::ਘਾਤ(Wgs84::eccentricity * sin_lat, 2.0));
	ਅਚਲ ਦੁੱਗਣਾ r_ਕੁੱਲ = r_e + static_cast<ਦੁੱਗਣਾ>(_altitude);

	ਵਾਪਸ Vector3d(r_ਕੁੱਲ * cos_lat * cos_lon,
			r_ਕੁੱਲ * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<ਦੁੱਗਣਾ>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::compute_Angular_ਦਰ_Nav_ਚੌਖਟਾ(ਅਚਲ Vector3f &v_ned) ਅਚਲ
{
	ਦੁੱਗਣਾ r_n;
	ਦੁੱਗਣਾ r_e;
	compute_Radii_ਦਾ_Curvature(_latitude_rad, r_n, r_e);
	ਵਾਪਸ Vector3f(
		       v_ned(1) / (static_cast<ਦਸ਼ਮਲਵ>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<ਦਸ਼ਮਲਵ>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<ਦਸ਼ਮਲਵ>(r_e) + _altitude));
}

Vector2d LatLonAlt::delta_Lat_Lon_ਮੰਜ਼ਿਲ_Delta_XY(ਅਚਲ ਦੁੱਗਣਾ latitude, ਅਚਲ ਦਸ਼ਮਲਵ altitude)
{
	ਦੁੱਗਣਾ r_n;
	ਦੁੱਗਣਾ r_e;
	compute_Radii_ਦਾ_Curvature(latitude, r_n, r_e);
	ਅਚਲ ਦੁੱਗਣਾ dn_dlat = r_n + static_cast<ਦੁੱਗਣਾ>(altitude);
	ਅਚਲ ਦੁੱਗਣਾ de_dlon = (r_e + static_cast<ਦੁੱਗਣਾ>(altitude)) * ਕੋਜੀਵਾ(latitude);

	ਵਾਪਸ Vector2d(dn_dlat, de_dlon);
}

ਖਾਲੀ LatLonAlt::compute_Radii_ਦਾ_Curvature(ਅਚਲ ਦੁੱਗਣਾ latitude, ਦੁੱਗਣਾ &meridian_radius_ਦਾ_curvature,
					ਦੁੱਗਣਾ &transverse_radius_ਦਾ_curvature)
{
	ਅਚਲ ਦੁੱਗਣਾ tmp = 1.0 - ਘਾਤ(Wgs84::eccentricity * ਜੀਵਾ(latitude), 2);
	ਅਚਲ ਦੁੱਗਣਾ sqrt_tmp = std::ਵਰਗਮੂਲ(tmp);
	meridian_radius_ਦਾ_curvature = Wgs84::meridian_radius_ਦਾ_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_ਦਾ_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

ਮੈਟ੍ਰਿਕਸ::Dcmf LatLonAlt::compute_Rot_Ecef_ਮੰਜ਼ਿਲ_Ned() ਅਚਲ
{
	ਅਚਲ ਦੁੱਗਣਾ cos_lat = ਕੋਜੀਵਾ(_latitude_rad);
	ਅਚਲ ਦੁੱਗਣਾ sin_lat = ਜੀਵਾ(_latitude_rad);
	ਅਚਲ ਦੁੱਗਣਾ cos_lon = ਕੋਜੀਵਾ(_longitude_rad);
	ਅਚਲ ਦੁੱਗਣਾ sin_lon = ਜੀਵਾ(_longitude_rad);

	ਅਚਲ ਦਸ਼ਮਲਵ val[] = {(ਦਸ਼ਮਲਵ)(-sin_lat * cos_lon), (ਦਸ਼ਮਲਵ)(-sin_lat * sin_lon), (ਦਸ਼ਮਲਵ)cos_lat,
			     (ਦਸ਼ਮਲਵ) - sin_lon, (ਦਸ਼ਮਲਵ)cos_lon, 0.f,
			     (ਦਸ਼ਮਲਵ)(-cos_lat * cos_lon), (ਦਸ਼ਮਲਵ)(-cos_lat * sin_lon), (ਦਸ਼ਮਲਵ) - sin_lat
			    };

	ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::Dcmf(val);
}

LatLonAlt LatLonAlt::ਸੰਚਾਲਕ+(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &delta_pos) ਅਚਲ
{
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d d_lat_lon_ਮੰਜ਼ਿਲ_d_xy = delta_Lat_Lon_ਮੰਜ਼ਿਲ_Delta_XY(latitude_rad(), altitude());
	ਅਚਲ ਦੁੱਗਣਾ latitude_rad = ਮੈਟ੍ਰਿਕਸ::wrap_pi(_latitude_rad + static_cast<ਦੁੱਗਣਾ>(delta_pos(0)) / d_lat_lon_ਮੰਜ਼ਿਲ_d_xy(0));
	ਅਚਲ ਦੁੱਗਣਾ longitude_rad = ਮੈਟ੍ਰਿਕਸ::wrap_pi(_longitude_rad + static_cast<ਦੁੱਗਣਾ>(delta_pos(1)) / d_lat_lon_ਮੰਜ਼ਿਲ_d_xy(1));
	ਅਚਲ ਦਸ਼ਮਲਵ altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.ਸੈੱਟ_Lat_Lon_Rad(latitude_rad, longitude_rad);
	lla_new.ਸੈੱਟ_Altitude(altitude);
	ਵਾਪਸ lla_new;
}

ਖਾਲੀ LatLonAlt::ਸੰਚਾਲਕ+=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &delta_pos)
{
	ਮੈਟ੍ਰਿਕਸ::Vector2d d_lat_lon_ਮੰਜ਼ਿਲ_d_xy = delta_Lat_Lon_ਮੰਜ਼ਿਲ_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = ਮੈਟ੍ਰਿਕਸ::wrap_pi(_latitude_rad + static_cast<ਦੁੱਗਣਾ>(delta_pos(0)) / d_lat_lon_ਮੰਜ਼ਿਲ_d_xy(0));
	_longitude_rad = ਮੈਟ੍ਰਿਕਸ::wrap_pi(_longitude_rad + static_cast<ਦੁੱਗਣਾ>(delta_pos(1)) / d_lat_lon_ਮੰਜ਼ਿਲ_d_xy(1));
	_altitude -= delta_pos(2);
}

ਖਾਲੀ LatLonAlt::ਸੰਚਾਲਕ+=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &delta_pos)
{
	ਮੈਟ੍ਰਿਕਸ::Vector2d d_lat_lon_ਮੰਜ਼ਿਲ_d_xy = delta_Lat_Lon_ਮੰਜ਼ਿਲ_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = ਮੈਟ੍ਰਿਕਸ::wrap_pi(_latitude_rad + static_cast<ਦੁੱਗਣਾ>(delta_pos(0)) / d_lat_lon_ਮੰਜ਼ਿਲ_d_xy(0));
	_longitude_rad = ਮੈਟ੍ਰਿਕਸ::wrap_pi(_longitude_rad + static_cast<ਦੁੱਗਣਾ>(delta_pos(1)) / d_lat_lon_ਮੰਜ਼ਿਲ_d_xy(1));
}

ਮੈਟ੍ਰਿਕਸ::Vector3f LatLonAlt::ਸੰਚਾਲਕ-(ਅਚਲ LatLonAlt &lla) ਅਚਲ
{
	ਅਚਲ ਦੁੱਗਣਾ delta_lat = ਮੈਟ੍ਰਿਕਸ::wrap_pi(_latitude_rad - lla.latitude_rad());
	ਅਚਲ ਦੁੱਗਣਾ delta_lon = ਮੈਟ੍ਰਿਕਸ::wrap_pi(_longitude_rad - lla.longitude_rad());
	ਅਚਲ ਦਸ਼ਮਲਵ delta_alt = _altitude - lla.altitude();

	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d d_lat_lon_ਮੰਜ਼ਿਲ_d_xy = delta_Lat_Lon_ਮੰਜ਼ਿਲ_Delta_XY(_latitude_rad, _altitude);
	ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::Vector3f(static_cast<ਦਸ਼ਮਲਵ>(delta_lat * d_lat_lon_ਮੰਜ਼ਿਲ_d_xy(0)),
				static_cast<ਦਸ਼ਮਲਵ>(delta_lon * d_lat_lon_ਮੰਜ਼ਿਲ_d_xy(1)),
				-delta_alt);
}
