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
#अन्तर्भाव <px4_boardconfig.h>

#यदि defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
#अन्तर्भाव "f3442_standard/DaaF3442.h"
#अन्यथा
#अन्तर्भाव "crosstrack_standard/DaaCrosstrack.h"
#endif

#अन्तर्भाव <matrix/math.hpp>

#अन्तर्भाव <uORB/topics/detect_and_avoid.h>
#अन्तर्भाव <uORB/topics/transponder_report.h>

संरचना daa_आदान_s {
	matrix::Vector2d uav_lat_lon{};
	भिन्न uav_alt{0.f};
	matrix::Vector3f uav_vel_ned{};
	transponder_report_s transponder_report{};
};

class AdsbConflict
{
सार्वजनिक:
	AdsbConflict() = स्वभाव;
	~AdsbConflict() = स्वभाव;

	/**
	 * @brief Validate ownship + transponder inputs and compute the DAA output.
	 * Returns false on non-finite inputs or when the built standard needs a heading the report does not provide.
	*/
	bool calculate_daa_output(ध्रुव daa_आदान_s &daa_आदान, detect_and_avoid_s &daa_output);

	bool try_updating_params();

	स्थिर bool valid_wgs84_coordinates(ध्रुव द्विगुण latitude, ध्रुव द्विगुण longitude);

गुप्त:
#यदि defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	DaaF3442 _daa;
#अन्यथा
	DaaCrosstrack _daa;
#endif
};
