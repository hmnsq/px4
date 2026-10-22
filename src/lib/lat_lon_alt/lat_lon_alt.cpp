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

#अन्तर्भाव "lat_lon_alt.hpp"

उपयुज्य matrix::Vector3f;
उपयुज्य matrix::Vector3d;
उपयुज्य matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(ध्रुव Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	ध्रुव द्विगुण k1 = वर्गमूलम्(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	ध्रुव द्विगुण k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	ध्रुव द्विगुण beta = वर्गमूलम्(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	ध्रुव द्विगुण E = (k1 - k2) / beta;
	ध्रुव द्विगुण F = (k1 + k2) / beta;

	ध्रुव द्विगुण P = 4.0 / 3.0 * (E * F + 1);
	ध्रुव द्विगुण Q = 2 * (E * E - F * F);
	ध्रुव द्विगुण D = P * P * P + Q * Q;
	ध्रुव द्विगुण V = cbrt(वर्गमूलम्(D) - Q) - cbrt(वर्गमूलम्(D) + Q);
	ध्रुव द्विगुण G = 0.5 * (वर्गमूलम्(E * E + V) + E);
	ध्रुव द्विगुण T = वर्गमूलम्(G * G + (F - V * G) / (2 * G - E)) - G;

	ध्रुव द्विगुण lon = atan2(p_ecef(1), p_ecef(0));
	ध्रुव द्विगुण lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * वर्गमूलम्(1.0 - Wgs84::eccentricity2)));
	ध्रुव द्विगुण alt = (beta - Wgs84::equatorial_radius * T) * कोटिज्या(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * वर्गमूलम्(1.0 - Wgs84::eccentricity2)) * ज्या(lat);

	LatLonAlt lla;
	lla.समुच्चयः_Lat_Lon_Rad(lat, lon);
	lla.समुच्चयः_Altitude(static_cast<भिन्न>(alt));
	प्रत्यागम lla;
}

Vector3d LatLonAlt::गन्तव्यम्_Ecef() ध्रुव
{
	ध्रुव द्विगुण cos_lat = कोटिज्या(_latitude_rad);
	ध्रुव द्विगुण sin_lat = ज्या(_latitude_rad);
	ध्रुव द्विगुण cos_lon = कोटिज्या(_longitude_rad);
	ध्रुव द्विगुण sin_lon = ज्या(_longitude_rad);

	ध्रुव द्विगुण r_e = Wgs84::equatorial_radius / वर्गमूलम्(1.0 - std::घातः(Wgs84::eccentricity * sin_lat, 2.0));
	ध्रुव द्विगुण r_योग = r_e + static_cast<द्विगुण>(_altitude);

	प्रत्यागम Vector3d(r_योग * cos_lat * cos_lon,
			r_योग * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<द्विगुण>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::compute_Angular_दरः_Nav_Frame(ध्रुव Vector3f &v_ned) ध्रुव
{
	द्विगुण r_n;
	द्विगुण r_e;
	compute_Radii_सम्बन्धि_Curvature(_latitude_rad, r_n, r_e);
	प्रत्यागम Vector3f(
		       v_ned(1) / (static_cast<भिन्न>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<भिन्न>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<भिन्न>(r_e) + _altitude));
}

Vector2d LatLonAlt::delta_Lat_Lon_गन्तव्यम्_Delta_XY(ध्रुव द्विगुण latitude, ध्रुव भिन्न altitude)
{
	द्विगुण r_n;
	द्विगुण r_e;
	compute_Radii_सम्बन्धि_Curvature(latitude, r_n, r_e);
	ध्रुव द्विगुण dn_dlat = r_n + static_cast<द्विगुण>(altitude);
	ध्रुव द्विगुण de_dlon = (r_e + static_cast<द्विगुण>(altitude)) * कोटिज्या(latitude);

	प्रत्यागम Vector2d(dn_dlat, de_dlon);
}

शून्य LatLonAlt::compute_Radii_सम्बन्धि_Curvature(ध्रुव द्विगुण latitude, द्विगुण &meridian_radius_सम्बन्धि_curvature,
					द्विगुण &transverse_radius_सम्बन्धि_curvature)
{
	ध्रुव द्विगुण tmp = 1.0 - घातः(Wgs84::eccentricity * ज्या(latitude), 2);
	ध्रुव द्विगुण sqrt_tmp = std::वर्गमूलम्(tmp);
	meridian_radius_सम्बन्धि_curvature = Wgs84::meridian_radius_सम्बन्धि_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_सम्बन्धि_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::compute_Rot_Ecef_गन्तव्यम्_Ned() ध्रुव
{
	ध्रुव द्विगुण cos_lat = कोटिज्या(_latitude_rad);
	ध्रुव द्विगुण sin_lat = ज्या(_latitude_rad);
	ध्रुव द्विगुण cos_lon = कोटिज्या(_longitude_rad);
	ध्रुव द्विगुण sin_lon = ज्या(_longitude_rad);

	ध्रुव भिन्न val[] = {(भिन्न)(-sin_lat * cos_lon), (भिन्न)(-sin_lat * sin_lon), (भिन्न)cos_lat,
			     (भिन्न) - sin_lon, (भिन्न)cos_lon, 0.f,
			     (भिन्न)(-cos_lat * cos_lon), (भिन्न)(-cos_lat * sin_lon), (भिन्न) - sin_lat
			    };

	प्रत्यागम matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::संक्रियकः+(ध्रुव matrix::Vector3f &delta_pos) ध्रुव
{
	ध्रुव matrix::Vector2d d_lat_lon_गन्तव्यम्_d_xy = delta_Lat_Lon_गन्तव्यम्_Delta_XY(latitude_rad(), altitude());
	ध्रुव द्विगुण latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<द्विगुण>(delta_pos(0)) / d_lat_lon_गन्तव्यम्_d_xy(0));
	ध्रुव द्विगुण longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<द्विगुण>(delta_pos(1)) / d_lat_lon_गन्तव्यम्_d_xy(1));
	ध्रुव भिन्न altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.समुच्चयः_Lat_Lon_Rad(latitude_rad, longitude_rad);
	lla_new.समुच्चयः_Altitude(altitude);
	प्रत्यागम lla_new;
}

शून्य LatLonAlt::संक्रियकः+=(ध्रुव matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_गन्तव्यम्_d_xy = delta_Lat_Lon_गन्तव्यम्_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<द्विगुण>(delta_pos(0)) / d_lat_lon_गन्तव्यम्_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<द्विगुण>(delta_pos(1)) / d_lat_lon_गन्तव्यम्_d_xy(1));
	_altitude -= delta_pos(2);
}

शून्य LatLonAlt::संक्रियकः+=(ध्रुव matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_गन्तव्यम्_d_xy = delta_Lat_Lon_गन्तव्यम्_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<द्विगुण>(delta_pos(0)) / d_lat_lon_गन्तव्यम्_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<द्विगुण>(delta_pos(1)) / d_lat_lon_गन्तव्यम्_d_xy(1));
}

matrix::Vector3f LatLonAlt::संक्रियकः-(ध्रुव LatLonAlt &lla) ध्रुव
{
	ध्रुव द्विगुण delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	ध्रुव द्विगुण delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	ध्रुव भिन्न delta_alt = _altitude - lla.altitude();

	ध्रुव matrix::Vector2d d_lat_lon_गन्तव्यम्_d_xy = delta_Lat_Lon_गन्तव्यम्_Delta_XY(_latitude_rad, _altitude);
	प्रत्यागम matrix::Vector3f(static_cast<भिन्न>(delta_lat * d_lat_lon_गन्तव्यम्_d_xy(0)),
				static_cast<भिन्न>(delta_lon * d_lat_lon_गन्तव्यम्_d_xy(1)),
				-delta_alt);
}
