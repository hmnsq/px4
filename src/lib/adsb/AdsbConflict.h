/****************************************************************************
 *
 *   Copyright (c) 2012-2026 PX4 Development Team. All rights reserved.
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

#pragma once

// Provides CONFIG_NAVIGATOR_ADSB_F3442 for the standard selection.
#அந்தர்பாவ <px4_boardconfig.h>

#எனில் defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
#அந்தர்பாவ "f3442_standard/DaaF3442.h"
#இல்லையெனில்
#அந்தர்பாவ "crosstrack_standard/DaaCrosstrack.h"
#endif

#அந்தர்பாவ <matrix/math.hpp>

#அந்தர்பாவ <uORB/topics/detect_and_avoid.h>
#அந்தர்பாவ <uORB/topics/transponder_report.h>

கட்டமைப்பு daa_உள்ளீடு_s {
	matrix::Vector2d uav_lat_lon{};
	தசம uav_alt{0.f};
	matrix::Vector3f uav_vel_ned{};
	transponder_report_s transponder_report{};
};

class AdsbConflict
{
பொது:
	AdsbConflict() = இயல்பு;
	~AdsbConflict() = இயல்பு;

	/**
	 * @brief Validate ownship + transponder inputs and compute the DAA output.
	 * Returns false on non-finite inputs or when the built standard needs a heading the report does not provide.
	*/
	bool calculate_daa_output(மாறிலி daa_உள்ளீடு_s &daa_உள்ளீடு, detect_and_avoid_s &daa_output);

	bool try_updating_params();

	நிலையான bool valid_wgs84_coordinates(மாறிலி இரட்டை latitude, மாறிலி இரட்டை longitude);

தனிப்பட்ட:
#எனில் defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	DaaF3442 _daa;
#இல்லையெனில்
	DaaCrosstrack _daa;
#endif
};
