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

using matrix::Vector3f;
using matrix::Vector3d;
using matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(स्थिरांक Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	स्थिरांक द्विगुण k1 = वर्ग_मूल(1 - Wgs84::eccentricity2) * भ_असल(p_ecef(2));
	स्थिरांक द्विगुण k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	स्थिरांक द्विगुण beta = वर्ग_मूल(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	स्थिरांक द्विगुण E = (k1 - k2) / beta;
	स्थिरांक द्विगुण F = (k1 + k2) / beta;

	स्थिरांक द्विगुण P = 4.0 / 3.0 * (E * F + 1);
	स्थिरांक द्विगुण Q = 2 * (E * E - F * F);
	स्थिरांक द्विगुण D = P * P * P + Q * Q;
	स्थिरांक द्विगुण V = cbrt(वर्ग_मूल(D) - Q) - cbrt(वर्ग_मूल(D) + Q);
	स्थिरांक द्विगुण G = 0.5 * (वर्ग_मूल(E * E + V) + E);
	स्थिरांक द्विगुण T = वर्ग_मूल(G * G + (F - V * G) / (2 * G - E)) - G;

	स्थिरांक द्विगुण lon = atan2(p_ecef(1), p_ecef(0));
	स्थिरांक द्विगुण lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * वर्ग_मूल(1.0 - Wgs84::eccentricity2)));
	स्थिरांक द्विगुण alt = (beta - Wgs84::equatorial_radius * T) * cos(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * वर्ग_मूल(1.0 - Wgs84::eccentricity2)) * sin(lat);

	LatLonAlt lla;
	lla.setLatLonRad(lat, lon);
	lla.setAltitude(static_cast<दशांश>(alt));
	परत lla;
}

Vector3d LatLonAlt::toEcef() स्थिरांक
{
	स्थिरांक द्विगुण cos_lat = cos(_latitude_rad);
	स्थिरांक द्विगुण sin_lat = sin(_latitude_rad);
	स्थिरांक द्विगुण cos_lon = cos(_longitude_rad);
	स्थिरांक द्विगुण sin_lon = sin(_longitude_rad);

	स्थिरांक द्विगुण r_e = Wgs84::equatorial_radius / वर्ग_मूल(1.0 - std::घात(Wgs84::eccentricity * sin_lat, 2.0));
	स्थिरांक द्विगुण r_total = r_e + static_cast<द्विगुण>(_altitude);

	परत Vector3d(r_total * cos_lat * cos_lon,
			r_total * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<द्विगुण>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::computeAngularRateNavFrame(स्थिरांक Vector3f &v_ned) स्थिरांक
{
	द्विगुण r_n;
	द्विगुण r_e;
	computeRadiiOfCurvature(_latitude_rad, r_n, r_e);
	परत Vector3f(
		       v_ned(1) / (static_cast<दशांश>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<दशांश>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<दशांश>(r_e) + _altitude));
}

Vector2d LatLonAlt::deltaLatLonToDeltaXY(स्थिरांक द्विगुण latitude, स्थिरांक दशांश altitude)
{
	द्विगुण r_n;
	द्विगुण r_e;
	computeRadiiOfCurvature(latitude, r_n, r_e);
	स्थिरांक द्विगुण dn_dlat = r_n + static_cast<द्विगुण>(altitude);
	स्थिरांक द्विगुण de_dlon = (r_e + static_cast<द्विगुण>(altitude)) * cos(latitude);

	परत Vector2d(dn_dlat, de_dlon);
}

शून्य LatLonAlt::computeRadiiOfCurvature(स्थिरांक द्विगुण latitude, द्विगुण &meridian_radius_of_curvature,
					द्विगुण &transverse_radius_of_curvature)
{
	स्थिरांक द्विगुण tmp = 1.0 - घात(Wgs84::eccentricity * sin(latitude), 2);
	स्थिरांक द्विगुण sqrt_tmp = std::वर्ग_मूल(tmp);
	meridian_radius_of_curvature = Wgs84::meridian_radius_of_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_of_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::computeRotEcefToNed() स्थिरांक
{
	स्थिरांक द्विगुण cos_lat = cos(_latitude_rad);
	स्थिरांक द्विगुण sin_lat = sin(_latitude_rad);
	स्थिरांक द्विगुण cos_lon = cos(_longitude_rad);
	स्थिरांक द्विगुण sin_lon = sin(_longitude_rad);

	स्थिरांक दशांश val[] = {(दशांश)(-sin_lat * cos_lon), (दशांश)(-sin_lat * sin_lon), (दशांश)cos_lat,
			     (दशांश) - sin_lon, (दशांश)cos_lon, 0.f,
			     (दशांश)(-cos_lat * cos_lon), (दशांश)(-cos_lat * sin_lon), (दशांश) - sin_lat
			    };

	परत matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::चालक+(स्थिरांक matrix::Vector3f &delta_pos) स्थिरांक
{
	स्थिरांक matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(latitude_rad(), altitude());
	स्थिरांक द्विगुण latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<द्विगुण>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	स्थिरांक द्विगुण longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<द्विगुण>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	स्थिरांक दशांश altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.setLatLonRad(latitude_rad, longitude_rad);
	lla_new.setAltitude(altitude);
	परत lla_new;
}

शून्य LatLonAlt::चालक+=(स्थिरांक matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<द्विगुण>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<द्विगुण>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	_altitude -= delta_pos(2);
}

शून्य LatLonAlt::चालक+=(स्थिरांक matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<द्विगुण>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<द्विगुण>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
}

matrix::Vector3f LatLonAlt::चालक-(स्थिरांक LatLonAlt &lla) स्थिरांक
{
	स्थिरांक द्विगुण delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	स्थिरांक द्विगुण delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	स्थिरांक दशांश delta_alt = _altitude - lla.altitude();

	स्थिरांक matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	परत matrix::Vector3f(static_cast<दशांश>(delta_lat * d_lat_lon_to_d_xy(0)),
				static_cast<दशांश>(delta_lon * d_lat_lon_to_d_xy(1)),
				-delta_alt);
}
