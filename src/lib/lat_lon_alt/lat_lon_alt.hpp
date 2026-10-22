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

#సూచన once

#అన్తర్భావ "mathlib/math/Limits.hpp"
#అన్తర్భావ <matrix/math.hpp>

తరగతి LatLonAlt
{
బహిరంగ:
	LatLonAlt() = అప్రమేయ;
	LatLonAlt(స్థిరరాశి LatLonAlt &lla)
	{
		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	LatLonAlt(స్థిరరాశి ద్విగుణ latitude_deg, స్థిరరాశి ద్విగుణ longitude_deg, స్థిరరాశి దశాంశం altitude_m)
	{
		_latitude_rad = math::radians(latitude_deg);
		_longitude_rad = math::radians(longitude_deg);
		_altitude = altitude_m;
	}


	స్థిర LatLonAlt fromEcef(స్థిరరాశి మాతృక::Vector3d &p_ecef);
	మాతృక::Vector3d గమ్యం_Ecef() స్థిరరాశి;

	శూన్య సమితి_Zero() { _latitude_rad = 0.0; _longitude_rad = 0.0; _altitude = 0.f; }

	ద్విగుణ latitude_deg() స్థిరరాశి { తిరిగి math::degrees(latitude_rad()); }
	ద్విగుణ longitude_deg() స్థిరరాశి { తిరిగి math::degrees(longitude_rad()); }

	స్థిరరాశి ద్విగుణ &latitude_rad() స్థిరరాశి { తిరిగి _latitude_rad; }
	స్థిరరాశి ద్విగుణ &longitude_rad() స్థిరరాశి { తిరిగి _longitude_rad; }
	దశాంశం altitude() స్థిరరాశి { తిరిగి _altitude; }

	శూన్య సమితి_Latitude_Deg(స్థిరరాశి ద్విగుణ &latitude_deg) { _latitude_rad = math::radians(latitude_deg); }
	శూన్య సమితి_Longitude_Deg(స్థిరరాశి ద్విగుణ &longitude_deg) { _longitude_rad = math::radians(longitude_deg); }
	శూన్య సమితి_Altitude(స్థిరరాశి దశాంశం altitude) { _altitude = altitude; }

	శూన్య సమితి_Lat_Lon(స్థిరరాశి LatLonAlt &lla) { _latitude_rad = lla.latitude_rad(); _longitude_rad = lla.longitude_rad(); }
	శూన్య సమితి_Lat_Lon_Deg(స్థిరరాశి ద్విగుణ latitude, స్థిరరాశి ద్విగుణ longitude) { _latitude_rad = math::radians(latitude); _longitude_rad = math::radians(longitude); }
	శూన్య సమితి_Lat_Lon_Rad(స్థిరరాశి ద్విగుణ latitude, స్థిరరాశి ద్విగుణ longitude) { _latitude_rad = latitude; _longitude_rad = longitude; }

	శూన్య print() స్థిరరాశి { రాయి("latitude = %f (deg), longitude = %f (deg), altitude = %f (m)\n", _latitude_rad, _longitude_rad, (ద్విగుణ)_altitude); }

	/*
	 * The plus and minus operators below use approximations and should only be used when the Cartesian component is small
	 */
	LatLonAlt క్రియాకారకం+(స్థిరరాశి మాతృక::Vector3f &delta_pos) స్థిరరాశి;
	శూన్య క్రియాకారకం+=(స్థిరరాశి మాతృక::Vector3f &delta_pos);
	శూన్య క్రియాకారకం+=(స్థిరరాశి మాతృక::Vector2f &delta_pos);
	మాతృక::Vector3f క్రియాకారకం-(స్థిరరాశి LatLonAlt &lla) స్థిరరాశి;

	శూన్య క్రియాకారకం=(స్థిరరాశి LatLonAlt &lla)
	{
		// Protect against self-assignment
		ఒకవేళ (ఇది == &lla) {
			తిరిగి;
		}

		_latitude_rad = lla.latitude_rad();
		_longitude_rad = lla.longitude_rad();
		_altitude = lla.altitude();
	}

	/*
	 * Compute the angular rate of the local navigation frame at the current latitude and height
	 * with respect to an inertial frame and resolved in the navigation frame
	 */
	మాతృక::Vector3f compute_Angular_రేటు_Nav_చట్రం(స్థిరరాశి మాతృక::Vector3f &v_ned) స్థిరరాశి;

	// Compute the ECEF to NED coordinate transformation matrix at the current position
	మాతృక::Dcmf compute_Rot_Ecef_గమ్యం_Ned() స్థిరరాశి;

	నిర్మాణం Wgs84 {
		స్థిర constexpr ద్విగుణ equatorial_radius = 6378137.0;
		స్థిర constexpr ద్విగుణ eccentricity = 0.0818191908425;
		స్థిర constexpr ద్విగుణ eccentricity2 = eccentricity * eccentricity;
		స్థిర constexpr ద్విగుణ meridian_radius_యొక్క_curvature_numerator = equatorial_radius * (1.0 - eccentricity2);
		స్థిర constexpr ద్విగుణ gravity_equator = 9.7803253359;
		స్థిర constexpr ద్విగుణ somigliana_coefficient = 0.001931851353;

		// Normal gravity at the ellipsoid surface as a function of latitude (Somigliana formula).
		// Captures the ~9.780 (equator) to ~9.832 m/s^2 (pole) variation. Input in radians.
		స్థిర దశాంశం gravity(స్థిరరాశి ద్విగుణ latitude_rad)
		{
			స్థిరరాశి ద్విగుణ sin_lat = జ్యా(latitude_rad);
			స్థిరరాశి ద్విగుణ sin_lat2 = sin_lat * sin_lat;
			స్థిరరాశి ద్విగుణ g = gravity_equator * (1.0 + somigliana_coefficient * sin_lat2)
					 / std::వర్గమూలం(1.0 - eccentricity2 * sin_lat2);
			తిరిగి static_cast<దశాంశం>(g);
		}
	};

అంతరంగ:
	// Convert between curvilinear and cartesian errors
	స్థిర మాతృక::Vector2d delta_Lat_Lon_గమ్యం_Delta_XY(స్థిరరాశి ద్విగుణ latitude, స్థిరరాశి దశాంశం altitude);

	స్థిర శూన్య compute_Radii_యొక్క_Curvature(స్థిరరాశి ద్విగుణ latitude, ద్విగుణ &meridian_radius_యొక్క_curvature,
					    ద్విగుణ &transverse_radius_యొక్క_curvature);

	ద్విగుణ _latitude_rad{0.0};
	ద్విగుణ _longitude_rad{0.0};
	దశాంశం _altitude{0.0};
};
