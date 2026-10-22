/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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

#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <lib/conversion/rotation.h>
#अन्तर्भाव <uORB/PublicationMulti.hpp>
#अन्तर्भाव <uORB/topics/distance_sensor.h>

#अन्तर्भाव <lib/failure_injection/FailureInjection.hpp>

वर्ग PX4Rangefinder
{
सार्वजनिक:
	PX4Rangefinder(स्थिरांक uint32_t device_id,
		       स्थिरांक uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);
	~PX4Rangefinder();

	// Set the MAV_DISTANCE_SENSOR type (LASER, ULTRASOUND, INFRARED, RADAR)
	शून्य set_rangefinder_type(uint8_t rangefinder_type) { _distance_sensor_pub.get().type = rangefinder_type; };

	शून्य set_device_id(स्थिरांक uint32_t device_id) { _distance_sensor_pub.get().device_id = device_id; };
	शून्य set_device_type(स्थिरांक uint8_t device_type);

	शून्य set_fov(स्थिरांक दशांश fov) { set_hfov(fov); set_vfov(fov); }
	शून्य set_hfov(स्थिरांक दशांश fov) { _distance_sensor_pub.get().h_fov = fov; }
	शून्य set_vfov(स्थिरांक दशांश fov) { _distance_sensor_pub.get().v_fov = fov; }

	शून्य set_max_distance(स्थिरांक दशांश distance) { _distance_sensor_pub.get().max_distance = distance; }
	शून्य set_min_distance(स्थिरांक दशांश distance) { _distance_sensor_pub.get().min_distance = distance; }

	शून्य set_variance(स्थिरांक दशांश variance) { _distance_sensor_pub.get().variance = variance; }

	शून्य set_orientation(स्थिरांक uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);

	शून्य set_mode(स्थिरांक uint8_t mode) { _distance_sensor_pub.get().mode = mode; }

	// update with quaterion sensor orientation with respect to the vehicle body frame
	शून्य update(स्थिरांक hrt_abstime &timestamp_sample, स्थिरांक दशांश distance, स्थिरांक int8_t quality = -1, स्थिरांक दशांश *q = nullptr,
		    uint8_t q_len = 4);

	पूर्णांक get_instance() { परत _distance_sensor_pub.get_instance(); };
	uint32_t get_device_id() { परत _distance_sensor_pub.get().device_id; };

खाजगी:
	uORB::PublicationMultiData<distance_sensor_s> _distance_sensor_pub{ORB_ID(distance_sensor)};

	failure_injection::Config _failure_config;
	failure_injection::Stuck<distance_sensor_s> _stuck;
};
