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

#ଅନ୍ତର୍ଭାଵ "lat_lon_alt.hpp"

using matrix::Vector3f;
using matrix::Vector3d;
using matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(ଅଚଳ Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	ଅଚଳ ଦ୍ୱିଗୁଣ k1 = sqrt(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	ଅଚଳ ଦ୍ୱିଗୁଣ k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	ଅଚଳ ଦ୍ୱିଗୁଣ beta = sqrt(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	ଅଚଳ ଦ୍ୱିଗୁଣ E = (k1 - k2) / beta;
	ଅଚଳ ଦ୍ୱିଗୁଣ F = (k1 + k2) / beta;

	ଅଚଳ ଦ୍ୱିଗୁଣ P = 4.0 / 3.0 * (E * F + 1);
	ଅଚଳ ଦ୍ୱିଗୁଣ Q = 2 * (E * E - F * F);
	ଅଚଳ ଦ୍ୱିଗୁଣ D = P * P * P + Q * Q;
	ଅଚଳ ଦ୍ୱିଗୁଣ V = cbrt(sqrt(D) - Q) - cbrt(sqrt(D) + Q);
	ଅଚଳ ଦ୍ୱିଗୁଣ G = 0.5 * (sqrt(E * E + V) + E);
	ଅଚଳ ଦ୍ୱିଗୁଣ T = sqrt(G * G + (F - V * G) / (2 * G - E)) - G;

	ଅଚଳ ଦ୍ୱିଗୁଣ lon = atan2(p_ecef(1), p_ecef(0));
	ଅଚଳ ଦ୍ୱିଗୁଣ lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * sqrt(1.0 - Wgs84::eccentricity2)));
	ଅଚଳ ଦ୍ୱିଗୁଣ alt = (beta - Wgs84::equatorial_radius * T) * cos(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * sqrt(1.0 - Wgs84::eccentricity2)) * sin(lat);

	LatLonAlt lla;
	lla.setLatLonRad(lat, lon);
	lla.setAltitude(static_cast<ଦଶମିକ>(alt));
	ଫେରନ୍ତୁ lla;
}

Vector3d LatLonAlt::toEcef() ଅଚଳ
{
	ଅଚଳ ଦ୍ୱିଗୁଣ cos_lat = cos(_latitude_rad);
	ଅଚଳ ଦ୍ୱିଗୁଣ sin_lat = sin(_latitude_rad);
	ଅଚଳ ଦ୍ୱିଗୁଣ cos_lon = cos(_longitude_rad);
	ଅଚଳ ଦ୍ୱିଗୁଣ sin_lon = sin(_longitude_rad);

	ଅଚଳ ଦ୍ୱିଗୁଣ r_e = Wgs84::equatorial_radius / sqrt(1.0 - std::pow(Wgs84::eccentricity * sin_lat, 2.0));
	ଅଚଳ ଦ୍ୱିଗୁଣ r_total = r_e + static_cast<ଦ୍ୱିଗୁଣ>(_altitude);

	ଫେରନ୍ତୁ Vector3d(r_total * cos_lat * cos_lon,
			r_total * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<ଦ୍ୱିଗୁଣ>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::computeAngularRateNavFrame(ଅଚଳ Vector3f &v_ned) ଅଚଳ
{
	ଦ୍ୱିଗୁଣ r_n;
	ଦ୍ୱିଗୁଣ r_e;
	computeRadiiOfCurvature(_latitude_rad, r_n, r_e);
	ଫେରନ୍ତୁ Vector3f(
		       v_ned(1) / (static_cast<ଦଶମିକ>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<ଦଶମିକ>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<ଦଶମିକ>(r_e) + _altitude));
}

Vector2d LatLonAlt::deltaLatLonToDeltaXY(ଅଚଳ ଦ୍ୱିଗୁଣ latitude, ଅଚଳ ଦଶମିକ altitude)
{
	ଦ୍ୱିଗୁଣ r_n;
	ଦ୍ୱିଗୁଣ r_e;
	computeRadiiOfCurvature(latitude, r_n, r_e);
	ଅଚଳ ଦ୍ୱିଗୁଣ dn_dlat = r_n + static_cast<ଦ୍ୱିଗୁଣ>(altitude);
	ଅଚଳ ଦ୍ୱିଗୁଣ de_dlon = (r_e + static_cast<ଦ୍ୱିଗୁଣ>(altitude)) * cos(latitude);

	ଫେରନ୍ତୁ Vector2d(dn_dlat, de_dlon);
}

ଶୂନ୍ୟ LatLonAlt::computeRadiiOfCurvature(ଅଚଳ ଦ୍ୱିଗୁଣ latitude, ଦ୍ୱିଗୁଣ &meridian_radius_of_curvature,
					ଦ୍ୱିଗୁଣ &transverse_radius_of_curvature)
{
	ଅଚଳ ଦ୍ୱିଗୁଣ tmp = 1.0 - pow(Wgs84::eccentricity * sin(latitude), 2);
	ଅଚଳ ଦ୍ୱିଗୁଣ sqrt_tmp = std::sqrt(tmp);
	meridian_radius_of_curvature = Wgs84::meridian_radius_of_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_of_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::computeRotEcefToNed() ଅଚଳ
{
	ଅଚଳ ଦ୍ୱିଗୁଣ cos_lat = cos(_latitude_rad);
	ଅଚଳ ଦ୍ୱିଗୁଣ sin_lat = sin(_latitude_rad);
	ଅଚଳ ଦ୍ୱିଗୁଣ cos_lon = cos(_longitude_rad);
	ଅଚଳ ଦ୍ୱିଗୁଣ sin_lon = sin(_longitude_rad);

	ଅଚଳ ଦଶମିକ val[] = {(ଦଶମିକ)(-sin_lat * cos_lon), (ଦଶମିକ)(-sin_lat * sin_lon), (ଦଶମିକ)cos_lat,
			     (ଦଶମିକ) - sin_lon, (ଦଶମିକ)cos_lon, 0.f,
			     (ଦଶମିକ)(-cos_lat * cos_lon), (ଦଶମିକ)(-cos_lat * sin_lon), (ଦଶମିକ) - sin_lat
			    };

	ଫେରନ୍ତୁ matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::operator+(ଅଚଳ matrix::Vector3f &delta_pos) ଅଚଳ
{
	ଅଚଳ matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(latitude_rad(), altitude());
	ଅଚଳ ଦ୍ୱିଗୁଣ latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ଦ୍ୱିଗୁଣ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	ଅଚଳ ଦ୍ୱିଗୁଣ longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ଦ୍ୱିଗୁଣ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	ଅଚଳ ଦଶମିକ altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.setLatLonRad(latitude_rad, longitude_rad);
	lla_new.setAltitude(altitude);
	ଫେରନ୍ତୁ lla_new;
}

ଶୂନ୍ୟ LatLonAlt::operator+=(ଅଚଳ matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ଦ୍ୱିଗୁଣ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ଦ୍ୱିଗୁଣ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	_altitude -= delta_pos(2);
}

ଶୂନ୍ୟ LatLonAlt::operator+=(ଅଚଳ matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ଦ୍ୱିଗୁଣ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ଦ୍ୱିଗୁଣ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
}

matrix::Vector3f LatLonAlt::operator-(ଅଚଳ LatLonAlt &lla) ଅଚଳ
{
	ଅଚଳ ଦ୍ୱିଗୁଣ delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	ଅଚଳ ଦ୍ୱିଗୁଣ delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	ଅଚଳ ଦଶମିକ delta_alt = _altitude - lla.altitude();

	ଅଚଳ matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	ଫେରନ୍ତୁ matrix::Vector3f(static_cast<ଦଶମିକ>(delta_lat * d_lat_lon_to_d_xy(0)),
				static_cast<ଦଶମିକ>(delta_lon * d_lat_lon_to_d_xy(1)),
				-delta_alt);
}
