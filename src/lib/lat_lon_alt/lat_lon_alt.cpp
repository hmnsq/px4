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

#ضمن "lat_lon_alt.hpp"

کارول ماتریکس::Vector3f;
کارول ماتریکس::Vector3d;
کارول ماتریکس::Vector2d;

LatLonAlt LatLonAlt::fromEcef(ثابت Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	ثابت مزدوج k1 = جذر(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	ثابت مزدوج k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	ثابت مزدوج beta = جذر(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	ثابت مزدوج E = (k1 - k2) / beta;
	ثابت مزدوج F = (k1 + k2) / beta;

	ثابت مزدوج P = 4.0 / 3.0 * (E * F + 1);
	ثابت مزدوج Q = 2 * (E * E - F * F);
	ثابت مزدوج D = P * P * P + Q * Q;
	ثابت مزدوج V = cbrt(جذر(D) - Q) - cbrt(جذر(D) + Q);
	ثابت مزدوج G = 0.5 * (جذر(E * E + V) + E);
	ثابت مزدوج T = جذر(G * G + (F - V * G) / (2 * G - E)) - G;

	ثابت مزدوج lon = atan2(p_ecef(1), p_ecef(0));
	ثابت مزدوج lat = ماتریکس::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * جذر(1.0 - Wgs84::eccentricity2)));
	ثابت مزدوج alt = (beta - Wgs84::equatorial_radius * T) * کوساین(lat) +
			   (p_ecef(2) - ماتریکس::sign(p_ecef(2)) * Wgs84::equatorial_radius * جذر(1.0 - Wgs84::eccentricity2)) * ساین(lat);

	LatLonAlt lla;
	lla.سېټ_Lat_Lon_Rad(lat, lon);
	lla.سېټ_Altitude(static_cast<عائم>(alt));
	أرجع lla;
}

Vector3d LatLonAlt::منزل_Ecef() ثابت
{
	ثابت مزدوج cos_lat = کوساین(_latitude_rad);
	ثابت مزدوج sin_lat = ساین(_latitude_rad);
	ثابت مزدوج cos_lon = کوساین(_longitude_rad);
	ثابت مزدوج sin_lon = ساین(_longitude_rad);

	ثابت مزدوج r_e = Wgs84::equatorial_radius / جذر(1.0 - std::توان(Wgs84::eccentricity * sin_lat, 2.0));
	ثابت مزدوج r_ټول = r_e + static_cast<مزدوج>(_altitude);

	أرجع Vector3d(r_ټول * cos_lat * cos_lon,
			r_ټول * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<مزدوج>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::compute_Angular_شرح_Nav_چوکاټ(ثابت Vector3f &v_ned) ثابت
{
	مزدوج r_n;
	مزدوج r_e;
	compute_Radii_د_Curvature(_latitude_rad, r_n, r_e);
	أرجع Vector3f(
		       v_ned(1) / (static_cast<عائم>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<عائم>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<عائم>(r_e) + _altitude));
}

Vector2d LatLonAlt::delta_Lat_Lon_منزل_Delta_XY(ثابت مزدوج latitude, ثابت عائم altitude)
{
	مزدوج r_n;
	مزدوج r_e;
	compute_Radii_د_Curvature(latitude, r_n, r_e);
	ثابت مزدوج dn_dlat = r_n + static_cast<مزدوج>(altitude);
	ثابت مزدوج de_dlon = (r_e + static_cast<مزدوج>(altitude)) * کوساین(latitude);

	أرجع Vector2d(dn_dlat, de_dlon);
}

فراغ LatLonAlt::compute_Radii_د_Curvature(ثابت مزدوج latitude, مزدوج &meridian_radius_د_curvature,
					مزدوج &transverse_radius_د_curvature)
{
	ثابت مزدوج tmp = 1.0 - توان(Wgs84::eccentricity * ساین(latitude), 2);
	ثابت مزدوج sqrt_tmp = std::جذر(tmp);
	meridian_radius_د_curvature = Wgs84::meridian_radius_د_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_د_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

ماتریکس::Dcmf LatLonAlt::compute_Rot_Ecef_منزل_Ned() ثابت
{
	ثابت مزدوج cos_lat = کوساین(_latitude_rad);
	ثابت مزدوج sin_lat = ساین(_latitude_rad);
	ثابت مزدوج cos_lon = کوساین(_longitude_rad);
	ثابت مزدوج sin_lon = ساین(_longitude_rad);

	ثابت عائم val[] = {(عائم)(-sin_lat * cos_lon), (عائم)(-sin_lat * sin_lon), (عائم)cos_lat,
			     (عائم) - sin_lon, (عائم)cos_lon, 0.f,
			     (عائم)(-cos_lat * cos_lon), (عائم)(-cos_lat * sin_lon), (عائم) - sin_lat
			    };

	أرجع ماتریکس::Dcmf(val);
}

LatLonAlt LatLonAlt::چلوونکی+(ثابت ماتریکس::Vector3f &delta_pos) ثابت
{
	ثابت ماتریکس::Vector2d d_lat_lon_منزل_d_xy = delta_Lat_Lon_منزل_Delta_XY(latitude_rad(), altitude());
	ثابت مزدوج latitude_rad = ماتریکس::wrap_pi(_latitude_rad + static_cast<مزدوج>(delta_pos(0)) / d_lat_lon_منزل_d_xy(0));
	ثابت مزدوج longitude_rad = ماتریکس::wrap_pi(_longitude_rad + static_cast<مزدوج>(delta_pos(1)) / d_lat_lon_منزل_d_xy(1));
	ثابت عائم altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.سېټ_Lat_Lon_Rad(latitude_rad, longitude_rad);
	lla_new.سېټ_Altitude(altitude);
	أرجع lla_new;
}

فراغ LatLonAlt::چلوونکی+=(ثابت ماتریکس::Vector3f &delta_pos)
{
	ماتریکس::Vector2d d_lat_lon_منزل_d_xy = delta_Lat_Lon_منزل_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = ماتریکس::wrap_pi(_latitude_rad + static_cast<مزدوج>(delta_pos(0)) / d_lat_lon_منزل_d_xy(0));
	_longitude_rad = ماتریکس::wrap_pi(_longitude_rad + static_cast<مزدوج>(delta_pos(1)) / d_lat_lon_منزل_d_xy(1));
	_altitude -= delta_pos(2);
}

فراغ LatLonAlt::چلوونکی+=(ثابت ماتریکس::Vector2f &delta_pos)
{
	ماتریکس::Vector2d d_lat_lon_منزل_d_xy = delta_Lat_Lon_منزل_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = ماتریکس::wrap_pi(_latitude_rad + static_cast<مزدوج>(delta_pos(0)) / d_lat_lon_منزل_d_xy(0));
	_longitude_rad = ماتریکس::wrap_pi(_longitude_rad + static_cast<مزدوج>(delta_pos(1)) / d_lat_lon_منزل_d_xy(1));
}

ماتریکس::Vector3f LatLonAlt::چلوونکی-(ثابت LatLonAlt &lla) ثابت
{
	ثابت مزدوج delta_lat = ماتریکس::wrap_pi(_latitude_rad - lla.latitude_rad());
	ثابت مزدوج delta_lon = ماتریکس::wrap_pi(_longitude_rad - lla.longitude_rad());
	ثابت عائم delta_alt = _altitude - lla.altitude();

	ثابت ماتریکس::Vector2d d_lat_lon_منزل_d_xy = delta_Lat_Lon_منزل_Delta_XY(_latitude_rad, _altitude);
	أرجع ماتریکس::Vector3f(static_cast<عائم>(delta_lat * d_lat_lon_منزل_d_xy(0)),
				static_cast<عائم>(delta_lon * d_lat_lon_منزل_d_xy(1)),
				-delta_alt);
}
