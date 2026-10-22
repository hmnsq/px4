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

استعمال میٹرکس::Vector3f;
استعمال میٹرکس::Vector3d;
استعمال میٹرکس::Vector2d;

LatLonAlt LatLonAlt::fromEcef(ثابت Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	ثابت دوہرا k1 = جذر(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	ثابت دوہرا k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	ثابت دوہرا beta = جذر(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	ثابت دوہرا E = (k1 - k2) / beta;
	ثابت دوہرا F = (k1 + k2) / beta;

	ثابت دوہرا P = 4.0 / 3.0 * (E * F + 1);
	ثابت دوہرا Q = 2 * (E * E - F * F);
	ثابت دوہرا D = P * P * P + Q * Q;
	ثابت دوہرا V = cbrt(جذر(D) - Q) - cbrt(جذر(D) + Q);
	ثابت دوہرا G = 0.5 * (جذر(E * E + V) + E);
	ثابت دوہرا T = جذر(G * G + (F - V * G) / (2 * G - E)) - G;

	ثابت دوہرا lon = atan2(p_ecef(1), p_ecef(0));
	ثابت دوہرا lat = میٹرکس::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * جذر(1.0 - Wgs84::eccentricity2)));
	ثابت دوہرا alt = (beta - Wgs84::equatorial_radius * T) * جیب_تمام(lat) +
			   (p_ecef(2) - میٹرکس::sign(p_ecef(2)) * Wgs84::equatorial_radius * جذر(1.0 - Wgs84::eccentricity2)) * جیب(lat);

	LatLonAlt lla;
	lla.سیٹ_Lat_Lon_Rad(lat, lon);
	lla.سیٹ_Altitude(static_cast<اعشاری>(alt));
	واپس lla;
}

Vector3d LatLonAlt::مقصد_Ecef() ثابت
{
	ثابت دوہرا cos_lat = جیب_تمام(_latitude_rad);
	ثابت دوہرا sin_lat = جیب(_latitude_rad);
	ثابت دوہرا cos_lon = جیب_تمام(_longitude_rad);
	ثابت دوہرا sin_lon = جیب(_longitude_rad);

	ثابت دوہرا r_e = Wgs84::equatorial_radius / جذر(1.0 - std::قوت(Wgs84::eccentricity * sin_lat, 2.0));
	ثابت دوہرا r_کل = r_e + static_cast<دوہرا>(_altitude);

	واپس Vector3d(r_کل * cos_lat * cos_lon,
			r_کل * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<دوہرا>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::compute_Angular_شرح_Nav_چوکھٹا(ثابت Vector3f &v_ned) ثابت
{
	دوہرا r_n;
	دوہرا r_e;
	compute_Radii_کا_Curvature(_latitude_rad, r_n, r_e);
	واپس Vector3f(
		       v_ned(1) / (static_cast<اعشاری>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<اعشاری>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<اعشاری>(r_e) + _altitude));
}

Vector2d LatLonAlt::delta_Lat_Lon_مقصد_Delta_XY(ثابت دوہرا latitude, ثابت اعشاری altitude)
{
	دوہرا r_n;
	دوہرا r_e;
	compute_Radii_کا_Curvature(latitude, r_n, r_e);
	ثابت دوہرا dn_dlat = r_n + static_cast<دوہرا>(altitude);
	ثابت دوہرا de_dlon = (r_e + static_cast<دوہرا>(altitude)) * جیب_تمام(latitude);

	واپس Vector2d(dn_dlat, de_dlon);
}

خالی LatLonAlt::compute_Radii_کا_Curvature(ثابت دوہرا latitude, دوہرا &meridian_radius_کا_curvature,
					دوہرا &transverse_radius_کا_curvature)
{
	ثابت دوہرا tmp = 1.0 - قوت(Wgs84::eccentricity * جیب(latitude), 2);
	ثابت دوہرا sqrt_tmp = std::جذر(tmp);
	meridian_radius_کا_curvature = Wgs84::meridian_radius_کا_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_کا_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

میٹرکس::Dcmf LatLonAlt::compute_Rot_Ecef_مقصد_Ned() ثابت
{
	ثابت دوہرا cos_lat = جیب_تمام(_latitude_rad);
	ثابت دوہرا sin_lat = جیب(_latitude_rad);
	ثابت دوہرا cos_lon = جیب_تمام(_longitude_rad);
	ثابت دوہرا sin_lon = جیب(_longitude_rad);

	ثابت اعشاری val[] = {(اعشاری)(-sin_lat * cos_lon), (اعشاری)(-sin_lat * sin_lon), (اعشاری)cos_lat,
			     (اعشاری) - sin_lon, (اعشاری)cos_lon, 0.f,
			     (اعشاری)(-cos_lat * cos_lon), (اعشاری)(-cos_lat * sin_lon), (اعشاری) - sin_lat
			    };

	واپس میٹرکس::Dcmf(val);
}

LatLonAlt LatLonAlt::عامل+(ثابت میٹرکس::Vector3f &delta_pos) ثابت
{
	ثابت میٹرکس::Vector2d d_lat_lon_مقصد_d_xy = delta_Lat_Lon_مقصد_Delta_XY(latitude_rad(), altitude());
	ثابت دوہرا latitude_rad = میٹرکس::wrap_pi(_latitude_rad + static_cast<دوہرا>(delta_pos(0)) / d_lat_lon_مقصد_d_xy(0));
	ثابت دوہرا longitude_rad = میٹرکس::wrap_pi(_longitude_rad + static_cast<دوہرا>(delta_pos(1)) / d_lat_lon_مقصد_d_xy(1));
	ثابت اعشاری altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.سیٹ_Lat_Lon_Rad(latitude_rad, longitude_rad);
	lla_new.سیٹ_Altitude(altitude);
	واپس lla_new;
}

خالی LatLonAlt::عامل+=(ثابت میٹرکس::Vector3f &delta_pos)
{
	میٹرکس::Vector2d d_lat_lon_مقصد_d_xy = delta_Lat_Lon_مقصد_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = میٹرکس::wrap_pi(_latitude_rad + static_cast<دوہرا>(delta_pos(0)) / d_lat_lon_مقصد_d_xy(0));
	_longitude_rad = میٹرکس::wrap_pi(_longitude_rad + static_cast<دوہرا>(delta_pos(1)) / d_lat_lon_مقصد_d_xy(1));
	_altitude -= delta_pos(2);
}

خالی LatLonAlt::عامل+=(ثابت میٹرکس::Vector2f &delta_pos)
{
	میٹرکس::Vector2d d_lat_lon_مقصد_d_xy = delta_Lat_Lon_مقصد_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = میٹرکس::wrap_pi(_latitude_rad + static_cast<دوہرا>(delta_pos(0)) / d_lat_lon_مقصد_d_xy(0));
	_longitude_rad = میٹرکس::wrap_pi(_longitude_rad + static_cast<دوہرا>(delta_pos(1)) / d_lat_lon_مقصد_d_xy(1));
}

میٹرکس::Vector3f LatLonAlt::عامل-(ثابت LatLonAlt &lla) ثابت
{
	ثابت دوہرا delta_lat = میٹرکس::wrap_pi(_latitude_rad - lla.latitude_rad());
	ثابت دوہرا delta_lon = میٹرکس::wrap_pi(_longitude_rad - lla.longitude_rad());
	ثابت اعشاری delta_alt = _altitude - lla.altitude();

	ثابت میٹرکس::Vector2d d_lat_lon_مقصد_d_xy = delta_Lat_Lon_مقصد_Delta_XY(_latitude_rad, _altitude);
	واپس میٹرکس::Vector3f(static_cast<اعشاری>(delta_lat * d_lat_lon_مقصد_d_xy(0)),
				static_cast<اعشاری>(delta_lon * d_lat_lon_مقصد_d_xy(1)),
				-delta_alt);
}
