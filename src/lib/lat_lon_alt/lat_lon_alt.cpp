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

#அந்தர்பாவ "lat_lon_alt.hpp"

using matrix::Vector3f;
using matrix::Vector3d;
using matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(மாறிலி Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	மாறிலி இரட்டை k1 = sqrt(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	மாறிலி இரட்டை k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	மாறிலி இரட்டை beta = sqrt(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	மாறிலி இரட்டை E = (k1 - k2) / beta;
	மாறிலி இரட்டை F = (k1 + k2) / beta;

	மாறிலி இரட்டை P = 4.0 / 3.0 * (E * F + 1);
	மாறிலி இரட்டை Q = 2 * (E * E - F * F);
	மாறிலி இரட்டை D = P * P * P + Q * Q;
	மாறிலி இரட்டை V = cbrt(sqrt(D) - Q) - cbrt(sqrt(D) + Q);
	மாறிலி இரட்டை G = 0.5 * (sqrt(E * E + V) + E);
	மாறிலி இரட்டை T = sqrt(G * G + (F - V * G) / (2 * G - E)) - G;

	மாறிலி இரட்டை lon = atan2(p_ecef(1), p_ecef(0));
	மாறிலி இரட்டை lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * sqrt(1.0 - Wgs84::eccentricity2)));
	மாறிலி இரட்டை alt = (beta - Wgs84::equatorial_radius * T) * cos(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * sqrt(1.0 - Wgs84::eccentricity2)) * sin(lat);

	LatLonAlt lla;
	lla.setLatLonRad(lat, lon);
	lla.setAltitude(static_cast<தசம>(alt));
	திரும்பு lla;
}

Vector3d LatLonAlt::toEcef() மாறிலி
{
	மாறிலி இரட்டை cos_lat = cos(_latitude_rad);
	மாறிலி இரட்டை sin_lat = sin(_latitude_rad);
	மாறிலி இரட்டை cos_lon = cos(_longitude_rad);
	மாறிலி இரட்டை sin_lon = sin(_longitude_rad);

	மாறிலி இரட்டை r_e = Wgs84::equatorial_radius / sqrt(1.0 - std::pow(Wgs84::eccentricity * sin_lat, 2.0));
	மாறிலி இரட்டை r_total = r_e + static_cast<இரட்டை>(_altitude);

	திரும்பு Vector3d(r_total * cos_lat * cos_lon,
			r_total * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<இரட்டை>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::computeAngularRateNavFrame(மாறிலி Vector3f &v_ned) மாறிலி
{
	இரட்டை r_n;
	இரட்டை r_e;
	computeRadiiOfCurvature(_latitude_rad, r_n, r_e);
	திரும்பு Vector3f(
		       v_ned(1) / (static_cast<தசம>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<தசம>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<தசம>(r_e) + _altitude));
}

Vector2d LatLonAlt::deltaLatLonToDeltaXY(மாறிலி இரட்டை latitude, மாறிலி தசம altitude)
{
	இரட்டை r_n;
	இரட்டை r_e;
	computeRadiiOfCurvature(latitude, r_n, r_e);
	மாறிலி இரட்டை dn_dlat = r_n + static_cast<இரட்டை>(altitude);
	மாறிலி இரட்டை de_dlon = (r_e + static_cast<இரட்டை>(altitude)) * cos(latitude);

	திரும்பு Vector2d(dn_dlat, de_dlon);
}

வெறுமை LatLonAlt::computeRadiiOfCurvature(மாறிலி இரட்டை latitude, இரட்டை &meridian_radius_of_curvature,
					இரட்டை &transverse_radius_of_curvature)
{
	மாறிலி இரட்டை tmp = 1.0 - pow(Wgs84::eccentricity * sin(latitude), 2);
	மாறிலி இரட்டை sqrt_tmp = std::sqrt(tmp);
	meridian_radius_of_curvature = Wgs84::meridian_radius_of_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_of_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::computeRotEcefToNed() மாறிலி
{
	மாறிலி இரட்டை cos_lat = cos(_latitude_rad);
	மாறிலி இரட்டை sin_lat = sin(_latitude_rad);
	மாறிலி இரட்டை cos_lon = cos(_longitude_rad);
	மாறிலி இரட்டை sin_lon = sin(_longitude_rad);

	மாறிலி தசம val[] = {(தசம)(-sin_lat * cos_lon), (தசம)(-sin_lat * sin_lon), (தசம)cos_lat,
			     (தசம) - sin_lon, (தசம)cos_lon, 0.f,
			     (தசம)(-cos_lat * cos_lon), (தசம)(-cos_lat * sin_lon), (தசம) - sin_lat
			    };

	திரும்பு matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::operator+(மாறிலி matrix::Vector3f &delta_pos) மாறிலி
{
	மாறிலி matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(latitude_rad(), altitude());
	மாறிலி இரட்டை latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<இரட்டை>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	மாறிலி இரட்டை longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<இரட்டை>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	மாறிலி தசம altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.setLatLonRad(latitude_rad, longitude_rad);
	lla_new.setAltitude(altitude);
	திரும்பு lla_new;
}

வெறுமை LatLonAlt::operator+=(மாறிலி matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<இரட்டை>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<இரட்டை>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	_altitude -= delta_pos(2);
}

வெறுமை LatLonAlt::operator+=(மாறிலி matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<இரட்டை>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<இரட்டை>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
}

matrix::Vector3f LatLonAlt::operator-(மாறிலி LatLonAlt &lla) மாறிலி
{
	மாறிலி இரட்டை delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	மாறிலி இரட்டை delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	மாறிலி தசம delta_alt = _altitude - lla.altitude();

	மாறிலி matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	திரும்பு matrix::Vector3f(static_cast<தசம>(delta_lat * d_lat_lon_to_d_xy(0)),
				static_cast<தசம>(delta_lon * d_lat_lon_to_d_xy(1)),
				-delta_alt);
}
