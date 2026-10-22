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

#അന്തര്ഭാവ "lat_lon_alt.hpp"

using matrix::Vector3f;
using matrix::Vector3d;
using matrix::Vector2d;

LatLonAlt LatLonAlt::fromEcef(സ്ഥിരം Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	സ്ഥിരം ഇരട്ട k1 = sqrt(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	സ്ഥിരം ഇരട്ട k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	സ്ഥിരം ഇരട്ട beta = sqrt(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	സ്ഥിരം ഇരട്ട E = (k1 - k2) / beta;
	സ്ഥിരം ഇരട്ട F = (k1 + k2) / beta;

	സ്ഥിരം ഇരട്ട P = 4.0 / 3.0 * (E * F + 1);
	സ്ഥിരം ഇരട്ട Q = 2 * (E * E - F * F);
	സ്ഥിരം ഇരട്ട D = P * P * P + Q * Q;
	സ്ഥിരം ഇരട്ട V = cbrt(sqrt(D) - Q) - cbrt(sqrt(D) + Q);
	സ്ഥിരം ഇരട്ട G = 0.5 * (sqrt(E * E + V) + E);
	സ്ഥിരം ഇരട്ട T = sqrt(G * G + (F - V * G) / (2 * G - E)) - G;

	സ്ഥിരം ഇരട്ട lon = atan2(p_ecef(1), p_ecef(0));
	സ്ഥിരം ഇരട്ട lat = matrix::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * sqrt(1.0 - Wgs84::eccentricity2)));
	സ്ഥിരം ഇരട്ട alt = (beta - Wgs84::equatorial_radius * T) * cos(lat) +
			   (p_ecef(2) - matrix::sign(p_ecef(2)) * Wgs84::equatorial_radius * sqrt(1.0 - Wgs84::eccentricity2)) * sin(lat);

	LatLonAlt lla;
	lla.setLatLonRad(lat, lon);
	lla.setAltitude(static_cast<ദശാംശം>(alt));
	തിരികെ lla;
}

Vector3d LatLonAlt::toEcef() സ്ഥിരം
{
	സ്ഥിരം ഇരട്ട cos_lat = cos(_latitude_rad);
	സ്ഥിരം ഇരട്ട sin_lat = sin(_latitude_rad);
	സ്ഥിരം ഇരട്ട cos_lon = cos(_longitude_rad);
	സ്ഥിരം ഇരട്ട sin_lon = sin(_longitude_rad);

	സ്ഥിരം ഇരട്ട r_e = Wgs84::equatorial_radius / sqrt(1.0 - std::pow(Wgs84::eccentricity * sin_lat, 2.0));
	സ്ഥിരം ഇരട്ട r_total = r_e + static_cast<ഇരട്ട>(_altitude);

	തിരികെ Vector3d(r_total * cos_lat * cos_lon,
			r_total * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<ഇരട്ട>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::computeAngularRateNavFrame(സ്ഥിരം Vector3f &v_ned) സ്ഥിരം
{
	ഇരട്ട r_n;
	ഇരട്ട r_e;
	computeRadiiOfCurvature(_latitude_rad, r_n, r_e);
	തിരികെ Vector3f(
		       v_ned(1) / (static_cast<ദശാംശം>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<ദശാംശം>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<ദശാംശം>(r_e) + _altitude));
}

Vector2d LatLonAlt::deltaLatLonToDeltaXY(സ്ഥിരം ഇരട്ട latitude, സ്ഥിരം ദശാംശം altitude)
{
	ഇരട്ട r_n;
	ഇരട്ട r_e;
	computeRadiiOfCurvature(latitude, r_n, r_e);
	സ്ഥിരം ഇരട്ട dn_dlat = r_n + static_cast<ഇരട്ട>(altitude);
	സ്ഥിരം ഇരട്ട de_dlon = (r_e + static_cast<ഇരട്ട>(altitude)) * cos(latitude);

	തിരികെ Vector2d(dn_dlat, de_dlon);
}

ശൂന്യം LatLonAlt::computeRadiiOfCurvature(സ്ഥിരം ഇരട്ട latitude, ഇരട്ട &meridian_radius_of_curvature,
					ഇരട്ട &transverse_radius_of_curvature)
{
	സ്ഥിരം ഇരട്ട tmp = 1.0 - pow(Wgs84::eccentricity * sin(latitude), 2);
	സ്ഥിരം ഇരട്ട sqrt_tmp = std::sqrt(tmp);
	meridian_radius_of_curvature = Wgs84::meridian_radius_of_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_of_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

matrix::Dcmf LatLonAlt::computeRotEcefToNed() സ്ഥിരം
{
	സ്ഥിരം ഇരട്ട cos_lat = cos(_latitude_rad);
	സ്ഥിരം ഇരട്ട sin_lat = sin(_latitude_rad);
	സ്ഥിരം ഇരട്ട cos_lon = cos(_longitude_rad);
	സ്ഥിരം ഇരട്ട sin_lon = sin(_longitude_rad);

	സ്ഥിരം ദശാംശം val[] = {(ദശാംശം)(-sin_lat * cos_lon), (ദശാംശം)(-sin_lat * sin_lon), (ദശാംശം)cos_lat,
			     (ദശാംശം) - sin_lon, (ദശാംശം)cos_lon, 0.f,
			     (ദശാംശം)(-cos_lat * cos_lon), (ദശാംശം)(-cos_lat * sin_lon), (ദശാംശം) - sin_lat
			    };

	തിരികെ matrix::Dcmf(val);
}

LatLonAlt LatLonAlt::operator+(സ്ഥിരം matrix::Vector3f &delta_pos) സ്ഥിരം
{
	സ്ഥിരം matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(latitude_rad(), altitude());
	സ്ഥിരം ഇരട്ട latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ഇരട്ട>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	സ്ഥിരം ഇരട്ട longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ഇരട്ട>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	സ്ഥിരം ദശാംശം altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.setLatLonRad(latitude_rad, longitude_rad);
	lla_new.setAltitude(altitude);
	തിരികെ lla_new;
}

ശൂന്യം LatLonAlt::operator+=(സ്ഥിരം matrix::Vector3f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ഇരട്ട>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ഇരട്ട>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
	_altitude -= delta_pos(2);
}

ശൂന്യം LatLonAlt::operator+=(സ്ഥിരം matrix::Vector2f &delta_pos)
{
	matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	_latitude_rad = matrix::wrap_pi(_latitude_rad + static_cast<ഇരട്ട>(delta_pos(0)) / d_lat_lon_to_d_xy(0));
	_longitude_rad = matrix::wrap_pi(_longitude_rad + static_cast<ഇരട്ട>(delta_pos(1)) / d_lat_lon_to_d_xy(1));
}

matrix::Vector3f LatLonAlt::operator-(സ്ഥിരം LatLonAlt &lla) സ്ഥിരം
{
	സ്ഥിരം ഇരട്ട delta_lat = matrix::wrap_pi(_latitude_rad - lla.latitude_rad());
	സ്ഥിരം ഇരട്ട delta_lon = matrix::wrap_pi(_longitude_rad - lla.longitude_rad());
	സ്ഥിരം ദശാംശം delta_alt = _altitude - lla.altitude();

	സ്ഥിരം matrix::Vector2d d_lat_lon_to_d_xy = deltaLatLonToDeltaXY(_latitude_rad, _altitude);
	തിരികെ matrix::Vector3f(static_cast<ദശാംശം>(delta_lat * d_lat_lon_to_d_xy(0)),
				static_cast<ദശാംശം>(delta_lon * d_lat_lon_to_d_xy(1)),
				-delta_alt);
}
