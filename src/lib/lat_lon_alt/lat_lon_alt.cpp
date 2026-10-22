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

#అన్తర్భావ "lat_lon_alt.hpp"

ఉపయోగించి మాతృక::Vector3f;
ఉపయోగించి మాతృక::Vector3d;
ఉపయోగించి మాతృక::Vector2d;

LatLonAlt LatLonAlt::fromEcef(స్థిరరాశి Vector3d &p_ecef)
{
	// Convert position using Borkowski closed-form exact solution
	// P. D. Groves, "Principles of GNSS, inertial, and multisensor integrated navigation systems, 2nd edition (appendix C)
	స్థిరరాశి ద్విగుణ k1 = వర్గమూలం(1 - Wgs84::eccentricity2) * fabs(p_ecef(2));
	స్థిరరాశి ద్విగుణ k2 = Wgs84::eccentricity2 * Wgs84::equatorial_radius;
	స్థిరరాశి ద్విగుణ beta = వర్గమూలం(p_ecef(0) * p_ecef(0) + p_ecef(1) * p_ecef(1));
	స్థిరరాశి ద్విగుణ E = (k1 - k2) / beta;
	స్థిరరాశి ద్విగుణ F = (k1 + k2) / beta;

	స్థిరరాశి ద్విగుణ P = 4.0 / 3.0 * (E * F + 1);
	స్థిరరాశి ద్విగుణ Q = 2 * (E * E - F * F);
	స్థిరరాశి ద్విగుణ D = P * P * P + Q * Q;
	స్థిరరాశి ద్విగుణ V = cbrt(వర్గమూలం(D) - Q) - cbrt(వర్గమూలం(D) + Q);
	స్థిరరాశి ద్విగుణ G = 0.5 * (వర్గమూలం(E * E + V) + E);
	స్థిరరాశి ద్విగుణ T = వర్గమూలం(G * G + (F - V * G) / (2 * G - E)) - G;

	స్థిరరాశి ద్విగుణ lon = atan2(p_ecef(1), p_ecef(0));
	స్థిరరాశి ద్విగుణ lat = మాతృక::sign(p_ecef(2)) * atan((1.0 - T * T) / (2.0 * T * వర్గమూలం(1.0 - Wgs84::eccentricity2)));
	స్థిరరాశి ద్విగుణ alt = (beta - Wgs84::equatorial_radius * T) * కోజ్యా(lat) +
			   (p_ecef(2) - మాతృక::sign(p_ecef(2)) * Wgs84::equatorial_radius * వర్గమూలం(1.0 - Wgs84::eccentricity2)) * జ్యా(lat);

	LatLonAlt lla;
	lla.సమితి_Lat_Lon_Rad(lat, lon);
	lla.సమితి_Altitude(static_cast<దశాంశం>(alt));
	తిరిగి lla;
}

Vector3d LatLonAlt::గమ్యం_Ecef() స్థిరరాశి
{
	స్థిరరాశి ద్విగుణ cos_lat = కోజ్యా(_latitude_rad);
	స్థిరరాశి ద్విగుణ sin_lat = జ్యా(_latitude_rad);
	స్థిరరాశి ద్విగుణ cos_lon = కోజ్యా(_longitude_rad);
	స్థిరరాశి ద్విగుణ sin_lon = జ్యా(_longitude_rad);

	స్థిరరాశి ద్విగుణ r_e = Wgs84::equatorial_radius / వర్గమూలం(1.0 - std::ఘాతం(Wgs84::eccentricity * sin_lat, 2.0));
	స్థిరరాశి ద్విగుణ r_మొత్తం = r_e + static_cast<ద్విగుణ>(_altitude);

	తిరిగి Vector3d(r_మొత్తం * cos_lat * cos_lon,
			r_మొత్తం * cos_lat * sin_lon,
			((1.0 - Wgs84::eccentricity2) * r_e + static_cast<ద్విగుణ>(_altitude)) * sin_lat);
}

Vector3f LatLonAlt::compute_Angular_రేటు_Nav_చట్రం(స్థిరరాశి Vector3f &v_ned) స్థిరరాశి
{
	ద్విగుణ r_n;
	ద్విగుణ r_e;
	compute_Radii_యొక్క_Curvature(_latitude_rad, r_n, r_e);
	తిరిగి Vector3f(
		       v_ned(1) / (static_cast<దశాంశం>(r_e) + _altitude),
		       -v_ned(0) / (static_cast<దశాంశం>(r_n) + _altitude),
		       -v_ned(1) * tanf(_latitude_rad) / (static_cast<దశాంశం>(r_e) + _altitude));
}

Vector2d LatLonAlt::delta_Lat_Lon_గమ్యం_Delta_XY(స్థిరరాశి ద్విగుణ latitude, స్థిరరాశి దశాంశం altitude)
{
	ద్విగుణ r_n;
	ద్విగుణ r_e;
	compute_Radii_యొక్క_Curvature(latitude, r_n, r_e);
	స్థిరరాశి ద్విగుణ dn_dlat = r_n + static_cast<ద్విగుణ>(altitude);
	స్థిరరాశి ద్విగుణ de_dlon = (r_e + static_cast<ద్విగుణ>(altitude)) * కోజ్యా(latitude);

	తిరిగి Vector2d(dn_dlat, de_dlon);
}

శూన్య LatLonAlt::compute_Radii_యొక్క_Curvature(స్థిరరాశి ద్విగుణ latitude, ద్విగుణ &meridian_radius_యొక్క_curvature,
					ద్విగుణ &transverse_radius_యొక్క_curvature)
{
	స్థిరరాశి ద్విగుణ tmp = 1.0 - ఘాతం(Wgs84::eccentricity * జ్యా(latitude), 2);
	స్థిరరాశి ద్విగుణ sqrt_tmp = std::వర్గమూలం(tmp);
	meridian_radius_యొక్క_curvature = Wgs84::meridian_radius_యొక్క_curvature_numerator / (tmp * sqrt_tmp);
	transverse_radius_యొక్క_curvature = Wgs84::equatorial_radius / sqrt_tmp;
}

మాతృక::Dcmf LatLonAlt::compute_Rot_Ecef_గమ్యం_Ned() స్థిరరాశి
{
	స్థిరరాశి ద్విగుణ cos_lat = కోజ్యా(_latitude_rad);
	స్థిరరాశి ద్విగుణ sin_lat = జ్యా(_latitude_rad);
	స్థిరరాశి ద్విగుణ cos_lon = కోజ్యా(_longitude_rad);
	స్థిరరాశి ద్విగుణ sin_lon = జ్యా(_longitude_rad);

	స్థిరరాశి దశాంశం val[] = {(దశాంశం)(-sin_lat * cos_lon), (దశాంశం)(-sin_lat * sin_lon), (దశాంశం)cos_lat,
			     (దశాంశం) - sin_lon, (దశాంశం)cos_lon, 0.f,
			     (దశాంశం)(-cos_lat * cos_lon), (దశాంశం)(-cos_lat * sin_lon), (దశాంశం) - sin_lat
			    };

	తిరిగి మాతృక::Dcmf(val);
}

LatLonAlt LatLonAlt::క్రియాకారకం+(స్థిరరాశి మాతృక::Vector3f &delta_pos) స్థిరరాశి
{
	స్థిరరాశి మాతృక::Vector2d d_lat_lon_గమ్యం_d_xy = delta_Lat_Lon_గమ్యం_Delta_XY(latitude_rad(), altitude());
	స్థిరరాశి ద్విగుణ latitude_rad = మాతృక::wrap_pi(_latitude_rad + static_cast<ద్విగుణ>(delta_pos(0)) / d_lat_lon_గమ్యం_d_xy(0));
	స్థిరరాశి ద్విగుణ longitude_rad = మాతృక::wrap_pi(_longitude_rad + static_cast<ద్విగుణ>(delta_pos(1)) / d_lat_lon_గమ్యం_d_xy(1));
	స్థిరరాశి దశాంశం altitude = _altitude - delta_pos(2);

	LatLonAlt lla_new;
	lla_new.సమితి_Lat_Lon_Rad(latitude_rad, longitude_rad);
	lla_new.సమితి_Altitude(altitude);
	తిరిగి lla_new;
}

శూన్య LatLonAlt::క్రియాకారకం+=(స్థిరరాశి మాతృక::Vector3f &delta_pos)
{
	మాతృక::Vector2d d_lat_lon_గమ్యం_d_xy = delta_Lat_Lon_గమ్యం_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = మాతృక::wrap_pi(_latitude_rad + static_cast<ద్విగుణ>(delta_pos(0)) / d_lat_lon_గమ్యం_d_xy(0));
	_longitude_rad = మాతృక::wrap_pi(_longitude_rad + static_cast<ద్విగుణ>(delta_pos(1)) / d_lat_lon_గమ్యం_d_xy(1));
	_altitude -= delta_pos(2);
}

శూన్య LatLonAlt::క్రియాకారకం+=(స్థిరరాశి మాతృక::Vector2f &delta_pos)
{
	మాతృక::Vector2d d_lat_lon_గమ్యం_d_xy = delta_Lat_Lon_గమ్యం_Delta_XY(_latitude_rad, _altitude);
	_latitude_rad = మాతృక::wrap_pi(_latitude_rad + static_cast<ద్విగుణ>(delta_pos(0)) / d_lat_lon_గమ్యం_d_xy(0));
	_longitude_rad = మాతృక::wrap_pi(_longitude_rad + static_cast<ద్విగుణ>(delta_pos(1)) / d_lat_lon_గమ్యం_d_xy(1));
}

మాతృక::Vector3f LatLonAlt::క్రియాకారకం-(స్థిరరాశి LatLonAlt &lla) స్థిరరాశి
{
	స్థిరరాశి ద్విగుణ delta_lat = మాతృక::wrap_pi(_latitude_rad - lla.latitude_rad());
	స్థిరరాశి ద్విగుణ delta_lon = మాతృక::wrap_pi(_longitude_rad - lla.longitude_rad());
	స్థిరరాశి దశాంశం delta_alt = _altitude - lla.altitude();

	స్థిరరాశి మాతృక::Vector2d d_lat_lon_గమ్యం_d_xy = delta_Lat_Lon_గమ్యం_Delta_XY(_latitude_rad, _altitude);
	తిరిగి మాతృక::Vector3f(static_cast<దశాంశం>(delta_lat * d_lat_lon_గమ్యం_d_xy(0)),
				static_cast<దశాంశం>(delta_lon * d_lat_lon_గమ్యం_d_xy(1)),
				-delta_alt);
}
