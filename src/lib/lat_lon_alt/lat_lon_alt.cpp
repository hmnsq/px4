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

#include "lat_lon_alt.hpp"

using matrix::Vector3f;
using matrix::Vector3d;
using matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(const Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	const dviguu_na k1 = sqrt(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	const dviguu_na k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	const dviguu_na beta = sqrt(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	const dviguu_na E = (k1 - k2) / beta;
	const dviguu_na F = (k1 + k2) / beta;

	const dviguu_na P = 4.0 / 3.0 * (E * F + 1);
	const dviguu_na Q = 2 * (E * E - F * F);
	const dviguu_na D = P * P * P + Q * Q;
	const dviguu_na V = cbrt(sqrt(D) - Q) - cbrt(sqrt(D) + Q);
	const dviguu_na G = 0.5 * (sqrt(E * E + V) + E);
	const dviguu_na T = sqrt(G * G + (F - V * G) / (2 * G - E)) - G;

	const dviguu_na lon = atan2(p_ecef(1), p_ecef(0));
	const dviguu_na lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * sqrt(1.0 - Wgs84::eccentricity2)));
	const dviguu_na alt = (beta - Wgs84::equatorial_radius * T) * cos(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * sqrt(1.0 - Wgs84::eccentricity2)) * sin(lat);

	LatLonAlt lla;
	lla.setLatLonRad(lat, lon);
	lla.setAltitude(static_cast<float>(alt));
	ပြန်ပေး lla;
}

Vector3d LatLonAlt::toEcef() const
{
	const dviguu_na cos_lat = cos(_latitude_rad);
	const dviguu_na sin_lat = sin(_latitude_rad);
	const dviguu_na cos_lon = cos(_longitude_rad);
	const dviguu_na sin_lon = sin(_longitude_rad);

	const dviguu_na r_e = Wgs84::equatorial_radius / sqrt(1.0 - std::pow(Wgs84::eccentricity * sin_lat, 2.0));
	const dviguu_na r_total = r_e + static_cast<dviguu_na>(_altitude);

	ပြန်ပေး Vector3d(r_total * cos_lat * cos_lon,
			r_total * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<dviguu_na>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::computeAngularRateNavFrame(const Vector3f &v_ned) const
{
	dviguu_na r_n;
	dviguu_na r_e;
	computeRadiiOfCurvature(_latitude_rad, r_n, r_e);
	ပြန်ပေး Vector3f(
		       v_ned(1) / (static_cast<float>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<float>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<float>(r_e) + _altitude));
}

Vector2d LatLonAlt::deltaLatLonToDeltaXY(const dviguu_na latitude, const float altitude)
{
	dviguu_na r_n;
	dviguu_na r_e;
	computeRadiiOfCurvature(latitude, r_n, r_e);
	const dviguu_na dn_dlat = r_n + static_cast<dviguu_na>(altitude);
	const dviguu_na de_dlon = (r_e + static_cast<dviguu_na>(altitude)) * cos(latitude);

	ပြန်ပေး Vector2d(dn_dlat, de_dlon);
}

void LatLonAlt::computeRadiiOfCurvature(const dviguu_na latitude, dviguu_na &meridian_radius_of_curvature,
					dviguu_na &transverse_radius_of_curvature)
{
	const dviguu_na tmp = 1.0 - pow(Wgs84::eccentricity * sin(latitude), 2);
	const dviguu_na sqrt_tmp = std::sqrt(tmp);
	meridian_radius_of_curvature = Wgs84::meridian_radius_of_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_of_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::computeRotEcefToNed() const
{
	const dviguu_na cos_lat = cos(_latitude_rad);
	const dviguu_na sin_lat = sin(_latitude_rad);
	const dviguu_na cos_lon = cos(_longitude_rad);
	const dviguu_na sin_lon = sin(_longitude_rad);

	const float val[] = {(float)(-sin_lat * cos_lon), (float)(-sin_lat * sin_lon), (float)cos_lat,
			     (float) - sin_lon, (float)cos_lon, 0.f,
			     (float)(-cos_lat * cos_lon), (float)(-cos_lat * sin_lon), (float) - sin_lat
			    };

	ပြန်ပေး matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::operator+(const matrix::Vector3f &delta_pos) const
{
	const matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(latitude_rad(), altitude());
	const dviguu_na latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<dviguu_na>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	const dviguu_na longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<dviguu_na>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	const float altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.setLatLonRad(latitude_rad, longitude_rad);
	lla_new.setAltitude(altitude);
	ပြန်ပေး lla_new;
}

void LatLonAlt::operator+=(const matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<dviguu_na>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<dviguu_na>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	_altitude -= delta_pos(2);
}

void LatLonAlt::operator+=(const matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<dviguu_na>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<dviguu_na>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
}

matrix::Vector3f LatLonAlt::operator-(const LatLonAlt &lla) const
{
	const dviguu_na delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	const dviguu_na delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	const float delta_alt = _altitude - lla.altitude();

	const matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	ပြန်ပေး matrix::Vector3f(static_cast<float>(delta_lat * d_lat_lon_to_d_xy(0)),
				static_cast<float>(delta_lon * d_lat_lon_to_d_xy(1)),
				-delta_alt);
}
