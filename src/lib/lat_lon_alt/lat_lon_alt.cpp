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

#અન્તર્ભાવ "lat_lon_alt.hpp"

using matrix::Vector3f;
using matrix::Vector3d;
using matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(અચલ Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	અચલ દ્વિગુણ k1 = sqrt(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	અચલ દ્વિગુણ k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	અચલ દ્વિગુણ beta = sqrt(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	અચલ દ્વિગુણ E = (k1 - k2) / beta;
	અચલ દ્વિગુણ F = (k1 + k2) / beta;

	અચલ દ્વિગુણ P = 4.0 / 3.0 * (E * F + 1);
	અચલ દ્વિગુણ Q = 2 * (E * E - F * F);
	અચલ દ્વિગુણ D = P * P * P + Q * Q;
	અચલ દ્વિગુણ V = cbrt(sqrt(D) - Q) - cbrt(sqrt(D) + Q);
	અચલ દ્વિગુણ G = 0.5 * (sqrt(E * E + V) + E);
	અચલ દ્વિગુણ T = sqrt(G * G + (F - V * G) / (2 * G - E)) - G;

	અચલ દ્વિગુણ lon = atan2(p_ecef(1), p_ecef(0));
	અચલ દ્વિગુણ lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * sqrt(1.0 - Wgs84::eccentricity2)));
	અચલ દ્વિગુણ alt = (beta - Wgs84::equatorial_radius * T) * cos(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * sqrt(1.0 - Wgs84::eccentricity2)) * sin(lat);

	LatLonAlt lla;
	lla.setLatLonRad(lat, lon);
	lla.setAltitude(static_cast<દશાંશ>(alt));
	પાછા lla;
}

Vector3d LatLonAlt::toEcef() અચલ
{
	અચલ દ્વિગુણ cos_lat = cos(_latitude_rad);
	અચલ દ્વિગુણ sin_lat = sin(_latitude_rad);
	અચલ દ્વિગુણ cos_lon = cos(_longitude_rad);
	અચલ દ્વિગુણ sin_lon = sin(_longitude_rad);

	અચલ દ્વિગુણ r_e = Wgs84::equatorial_radius / sqrt(1.0 - std::pow(Wgs84::eccentricity * sin_lat, 2.0));
	અચલ દ્વિગુણ r_total = r_e + static_cast<દ્વિગુણ>(_altitude);

	પાછા Vector3d(r_total * cos_lat * cos_lon,
			r_total * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<દ્વિગુણ>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::computeAngularRateNavFrame(અચલ Vector3f &v_ned) અચલ
{
	દ્વિગુણ r_n;
	દ્વિગુણ r_e;
	computeRadiiOfCurvature(_latitude_rad, r_n, r_e);
	પાછા Vector3f(
		       v_ned(1) / (static_cast<દશાંશ>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<દશાંશ>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<દશાંશ>(r_e) + _altitude));
}

Vector2d LatLonAlt::deltaLatLonToDeltaXY(અચલ દ્વિગુણ latitude, અચલ દશાંશ altitude)
{
	દ્વિગુણ r_n;
	દ્વિગુણ r_e;
	computeRadiiOfCurvature(latitude, r_n, r_e);
	અચલ દ્વિગુણ dn_dlat = r_n + static_cast<દ્વિગુણ>(altitude);
	અચલ દ્વિગુણ de_dlon = (r_e + static_cast<દ્વિગુણ>(altitude)) * cos(latitude);

	પાછા Vector2d(dn_dlat, de_dlon);
}

શૂન્ય LatLonAlt::computeRadiiOfCurvature(અચલ દ્વિગુણ latitude, દ્વિગુણ &meridian_radius_of_curvature,
					દ્વિગુણ &transverse_radius_of_curvature)
{
	અચલ દ્વિગુણ tmp = 1.0 - pow(Wgs84::eccentricity * sin(latitude), 2);
	અચલ દ્વિગુણ sqrt_tmp = std::sqrt(tmp);
	meridian_radius_of_curvature = Wgs84::meridian_radius_of_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_of_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::computeRotEcefToNed() અચલ
{
	અચલ દ્વિગુણ cos_lat = cos(_latitude_rad);
	અચલ દ્વિગુણ sin_lat = sin(_latitude_rad);
	અચલ દ્વિગુણ cos_lon = cos(_longitude_rad);
	અચલ દ્વિગુણ sin_lon = sin(_longitude_rad);

	અચલ દશાંશ val[] = {(દશાંશ)(-sin_lat * cos_lon), (દશાંશ)(-sin_lat * sin_lon), (દશાંશ)cos_lat,
			     (દશાંશ) - sin_lon, (દશાંશ)cos_lon, 0.f,
			     (દશાંશ)(-cos_lat * cos_lon), (દશાંશ)(-cos_lat * sin_lon), (દશાંશ) - sin_lat
			    };

	પાછા matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::operator+(અચલ matrix::Vector3f &delta_pos) અચલ
{
	અચલ matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(latitude_rad(), altitude());
	અચલ દ્વિગુણ latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<દ્વિગુણ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	અચલ દ્વિગુણ longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<દ્વિગુણ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	અચલ દશાંશ altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.setLatLonRad(latitude_rad, longitude_rad);
	lla_new.setAltitude(altitude);
	પાછા lla_new;
}

શૂન્ય LatLonAlt::operator+=(અચલ matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<દ્વિગુણ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<દ્વિગુણ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	_altitude -= delta_pos(2);
}

શૂન્ય LatLonAlt::operator+=(અચલ matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<દ્વિગુણ>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<દ્વિગુણ>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
}

matrix::Vector3f LatLonAlt::operator-(અચલ LatLonAlt &lla) અચલ
{
	અચલ દ્વિગુણ delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	અચલ દ્વિગુણ delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	અચલ દશાંશ delta_alt = _altitude - lla.altitude();

	અચલ matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	પાછા matrix::Vector3f(static_cast<દશાંશ>(delta_lat * d_lat_lon_to_d_xy(0)),
				static_cast<દશાંશ>(delta_lon * d_lat_lon_to_d_xy(1)),
				-delta_alt);
}
