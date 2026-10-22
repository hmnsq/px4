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

#ಅನ್ತರ್ಭಾವ "lat_lon_alt.hpp"

using matrix::Vector3f;
using matrix::Vector3d;
using matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(ಧ್ರುವ Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	ಧ್ರುವ ದ್ವಿಗುಣ k1 = sqrt(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	ಧ್ರುವ ದ್ವಿಗುಣ k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	ಧ್ರುವ ದ್ವಿಗುಣ beta = sqrt(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	ಧ್ರುವ ದ್ವಿಗುಣ E = (k1 - k2) / beta;
	ಧ್ರುವ ದ್ವಿಗುಣ F = (k1 + k2) / beta;

	ಧ್ರುವ ದ್ವಿಗುಣ P = 4.0 / 3.0 * (E * F + 1);
	ಧ್ರುವ ದ್ವಿಗುಣ Q = 2 * (E * E - F * F);
	ಧ್ರುವ ದ್ವಿಗುಣ D = P * P * P + Q * Q;
	ಧ್ರುವ ದ್ವಿಗುಣ V = cbrt(sqrt(D) - Q) - cbrt(sqrt(D) + Q);
	ಧ್ರುವ ದ್ವಿಗುಣ G = 0.5 * (sqrt(E * E + V) + E);
	ಧ್ರುವ ದ್ವಿಗುಣ T = sqrt(G * G + (F - V * G) / (2 * G - E)) - G;

	ಧ್ರುವ ದ್ವಿಗುಣ lon = atan2(p_ecef(1), p_ecef(0));
	ಧ್ರುವ ದ್ವಿಗುಣ lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * sqrt(1.0 - Wgs84::eccentricity2)));
	ಧ್ರುವ ದ್ವಿಗುಣ alt = (beta - Wgs84::equatorial_radius * T) * cos(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * sqrt(1.0 - Wgs84::eccentricity2)) * sin(lat);

	LatLonAlt lla;
	lla.setLatLonRad(lat, lon);
	lla.setAltitude(static_cast<ಭಿನ್ನ>(alt));
	ಪ್ರತ್ಯಾಗಮ lla;
}

Vector3d LatLonAlt::toEcef() ಧ್ರುವ
{
	ಧ್ರುವ ದ್ವಿಗುಣ cos_lat = cos(_latitude_rad);
	ಧ್ರುವ ದ್ವಿಗುಣ sin_lat = sin(_latitude_rad);
	ಧ್ರುವ ದ್ವಿಗುಣ cos_lon = cos(_longitude_rad);
	ಧ್ರುವ ದ್ವಿಗುಣ sin_lon = sin(_longitude_rad);

	ಧ್ರುವ ದ್ವಿಗುಣ r_e = Wgs84::equatorial_radius / sqrt(1.0 - std::pow(Wgs84::eccentricity * sin_lat, 2.0));
	ಧ್ರುವ ದ್ವಿಗುಣ r_total = r_e + static_cast<ದ್ವಿಗುಣ>(_altitude);

	ಪ್ರತ್ಯಾಗಮ Vector3d(r_total * cos_lat * cos_lon,
			r_total * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<ದ್ವಿಗುಣ>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::computeAngularRateNavFrame(ಧ್ರುವ Vector3f &v_ned) ಧ್ರುವ
{
	ದ್ವಿಗುಣ r_n;
	ದ್ವಿಗುಣ r_e;
	computeRadiiOfCurvature(_latitude_rad, r_n, r_e);
	ಪ್ರತ್ಯಾಗಮ Vector3f(
		       v_ned(1) / (static_cast<ಭಿನ್ನ>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<ಭಿನ್ನ>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<ಭಿನ್ನ>(r_e) + _altitude));
}

Vector2d LatLonAlt::deltaLatLonToDeltaXY(ಧ್ರುವ ದ್ವಿಗುಣ latitude, ಧ್ರುವ ಭಿನ್ನ altitude)
{
	ದ್ವಿಗುಣ r_n;
	ದ್ವಿಗುಣ r_e;
	computeRadiiOfCurvature(latitude, r_n, r_e);
	ಧ್ರುವ ದ್ವಿಗುಣ dn_dlat = r_n + static_cast<ದ್ವಿಗುಣ>(altitude);
	ಧ್ರುವ ದ್ವಿಗುಣ de_dlon = (r_e + static_cast<ದ್ವಿಗುಣ>(altitude)) * cos(latitude);

	ಪ್ರತ್ಯಾಗಮ Vector2d(dn_dlat, de_dlon);
}

ಶೂನ್ಯ LatLonAlt::computeRadiiOfCurvature(ಧ್ರುವ ದ್ವಿಗುಣ latitude, ದ್ವಿಗುಣ &meridian_radius_of_curvature,
					ದ್ವಿಗುಣ &transverse_radius_of_curvature)
{
	ಧ್ರುವ ದ್ವಿಗುಣ tmp = 1.0 - pow(Wgs84::eccentricity * sin(latitude), 2);
	ಧ್ರುವ ದ್ವಿಗುಣ sqrt_tmp = std::sqrt(tmp);
	meridian_radius_of_curvature = Wgs84::meridian_radius_of_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_of_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::computeRotEcefToNed() ಧ್ರುವ
{
	ಧ್ರುವ ದ್ವಿಗುಣ cos_lat = cos(_latitude_rad);
	ಧ್ರುವ ದ್ವಿಗುಣ sin_lat = sin(_latitude_rad);
	ಧ್ರುವ ದ್ವಿಗುಣ cos_lon = cos(_longitude_rad);
	ಧ್ರುವ ದ್ವಿಗುಣ sin_lon = sin(_longitude_rad);

	ಧ್ರುವ ಭಿನ್ನ val[] = {(ಭಿನ್ನ)(-sin_lat * cos_lon), (ಭಿನ್ನ)(-sin_lat * sin_lon), (ಭಿನ್ನ)cos_lat,
			     (ಭಿನ್ನ) - sin_lon, (ಭಿನ್ನ)cos_lon, 0.f,
			     (ಭಿನ್ನ)(-cos_lat * cos_lon), (ಭಿನ್ನ)(-cos_lat * sin_lon), (ಭಿನ್ನ) - sin_lat
			    };

	ಪ್ರತ್ಯಾಗಮ matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::operator+(ಧ್ರುವ matrix::Vector3f &delta_pos) ಧ್ರುವ
{
	ಧ್ರುವ matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(latitude_rad(), altitude());
	ಧ್ರುವ ದ್ವಿಗುಣ latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ದ್ವಿಗುಣ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	ಧ್ರುವ ದ್ವಿಗುಣ longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ದ್ವಿಗುಣ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	ಧ್ರುವ ಭಿನ್ನ altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.setLatLonRad(latitude_rad, longitude_rad);
	lla_new.setAltitude(altitude);
	ಪ್ರತ್ಯಾಗಮ lla_new;
}

ಶೂನ್ಯ LatLonAlt::operator+=(ಧ್ರುವ matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ದ್ವಿಗುಣ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ದ್ವಿಗುಣ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	_altitude -= delta_pos(2);
}

ಶೂನ್ಯ LatLonAlt::operator+=(ಧ್ರುವ matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ದ್ವಿಗುಣ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ದ್ವಿಗುಣ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
}

matrix::Vector3f LatLonAlt::operator-(ಧ್ರುವ LatLonAlt &lla) ಧ್ರುವ
{
	ಧ್ರುವ ದ್ವಿಗುಣ delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	ಧ್ರುವ ದ್ವಿಗುಣ delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	ಧ್ರುವ ಭಿನ್ನ delta_alt = _altitude - lla.altitude();

	ಧ್ರುವ matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	ಪ್ರತ್ಯಾಗಮ matrix::Vector3f(static_cast<ಭಿನ್ನ>(delta_lat * d_lat_lon_to_d_xy(0)),
				static_cast<ಭಿನ್ನ>(delta_lon * d_lat_lon_to_d_xy(1)),
				-delta_alt);
}
