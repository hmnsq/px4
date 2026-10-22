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

#شامل "lat_lon_alt.hpp"

using matrix::Vector3f;
using matrix::Vector3d;
using matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(ثابت Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	ثابت دوگانه k1 = sqrt(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	ثابت دوگانه k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	ثابت دوگانه beta = sqrt(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	ثابت دوگانه E = (k1 - k2) / beta;
	ثابت دوگانه F = (k1 + k2) / beta;

	ثابت دوگانه P = 4.0 / 3.0 * (E * F + 1);
	ثابت دوگانه Q = 2 * (E * E - F * F);
	ثابت دوگانه D = P * P * P + Q * Q;
	ثابت دوگانه V = cbrt(sqrt(D) - Q) - cbrt(sqrt(D) + Q);
	ثابت دوگانه G = 0.5 * (sqrt(E * E + V) + E);
	ثابت دوگانه T = sqrt(G * G + (F - V * G) / (2 * G - E)) - G;

	ثابت دوگانه lon = atan2(p_ecef(1), p_ecef(0));
	ثابت دوگانه lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * sqrt(1.0 - Wgs84::eccentricity2)));
	ثابت دوگانه alt = (beta - Wgs84::equatorial_radius * T) * cos(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * sqrt(1.0 - Wgs84::eccentricity2)) * sin(lat);

	LatLonAlt lla;
	lla.setLatLonRad(lat, lon);
	lla.setAltitude(static_cast<اعشاری>(alt));
	بازگشت lla;
}

Vector3d LatLonAlt::toEcef() ثابت
{
	ثابت دوگانه cos_lat = cos(_latitude_rad);
	ثابت دوگانه sin_lat = sin(_latitude_rad);
	ثابت دوگانه cos_lon = cos(_longitude_rad);
	ثابت دوگانه sin_lon = sin(_longitude_rad);

	ثابت دوگانه r_e = Wgs84::equatorial_radius / sqrt(1.0 - std::pow(Wgs84::eccentricity * sin_lat, 2.0));
	ثابت دوگانه r_total = r_e + static_cast<دوگانه>(_altitude);

	بازگشت Vector3d(r_total * cos_lat * cos_lon,
			r_total * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<دوگانه>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::computeAngularRateNavFrame(ثابت Vector3f &v_ned) ثابت
{
	دوگانه r_n;
	دوگانه r_e;
	computeRadiiOfCurvature(_latitude_rad, r_n, r_e);
	بازگشت Vector3f(
		       v_ned(1) / (static_cast<اعشاری>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<اعشاری>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<اعشاری>(r_e) + _altitude));
}

Vector2d LatLonAlt::deltaLatLonToDeltaXY(ثابت دوگانه latitude, ثابت اعشاری altitude)
{
	دوگانه r_n;
	دوگانه r_e;
	computeRadiiOfCurvature(latitude, r_n, r_e);
	ثابت دوگانه dn_dlat = r_n + static_cast<دوگانه>(altitude);
	ثابت دوگانه de_dlon = (r_e + static_cast<دوگانه>(altitude)) * cos(latitude);

	بازگشت Vector2d(dn_dlat, de_dlon);
}

تهی LatLonAlt::computeRadiiOfCurvature(ثابت دوگانه latitude, دوگانه &meridian_radius_of_curvature,
					دوگانه &transverse_radius_of_curvature)
{
	ثابت دوگانه tmp = 1.0 - pow(Wgs84::eccentricity * sin(latitude), 2);
	ثابت دوگانه sqrt_tmp = std::sqrt(tmp);
	meridian_radius_of_curvature = Wgs84::meridian_radius_of_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_of_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::computeRotEcefToNed() ثابت
{
	ثابت دوگانه cos_lat = cos(_latitude_rad);
	ثابت دوگانه sin_lat = sin(_latitude_rad);
	ثابت دوگانه cos_lon = cos(_longitude_rad);
	ثابت دوگانه sin_lon = sin(_longitude_rad);

	ثابت اعشاری val[] = {(اعشاری)(-sin_lat * cos_lon), (اعشاری)(-sin_lat * sin_lon), (اعشاری)cos_lat,
			     (اعشاری) - sin_lon, (اعشاری)cos_lon, 0.f,
			     (اعشاری)(-cos_lat * cos_lon), (اعشاری)(-cos_lat * sin_lon), (اعشاری) - sin_lat
			    };

	بازگشت matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::operator+(ثابت matrix::Vector3f &delta_pos) ثابت
{
	ثابت matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(latitude_rad(), altitude());
	ثابت دوگانه latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<دوگانه>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	ثابت دوگانه longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<دوگانه>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	ثابت اعشاری altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.setLatLonRad(latitude_rad, longitude_rad);
	lla_new.setAltitude(altitude);
	بازگشت lla_new;
}

تهی LatLonAlt::operator+=(ثابت matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<دوگانه>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<دوگانه>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	_altitude -= delta_pos(2);
}

تهی LatLonAlt::operator+=(ثابت matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<دوگانه>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<دوگانه>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
}

matrix::Vector3f LatLonAlt::operator-(ثابت LatLonAlt &lla) ثابت
{
	ثابت دوگانه delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	ثابت دوگانه delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	ثابت اعشاری delta_alt = _altitude - lla.altitude();

	ثابت matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	بازگشت matrix::Vector3f(static_cast<اعشاری>(delta_lat * d_lat_lon_to_d_xy(0)),
				static_cast<اعشاری>(delta_lon * d_lat_lon_to_d_xy(1)),
				-delta_alt);
}
