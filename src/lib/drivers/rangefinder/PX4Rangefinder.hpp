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

#निर्देशः once

#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <lib/conversion/rotation.h>
#अन्तर्भाव <uORB/PublicationMulti.hpp>
#अन्तर्भाव <uORB/topics/distance_sensor.h>

#अन्तर्भाव <lib/failure_injection/FailureInjection.hpp>

वर्ग PX4Rangefinder
{
सार्वजनिक:
	PX4Rangefinder(ध्रुव uint32_t device_id,
		       ध्रुव uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);
	~PX4Rangefinder();

	// Set the MAV_DISTANCE_SENSOR type (LASER, ULTRASOUND, INFRARED, RADAR)
	शून्य समुच्चयः_rangefinder_type(uint8_t rangefinder_type) { _distance_sensor_pub.get().type = rangefinder_type; };

	शून्य समुच्चयः_device_id(ध्रुव uint32_t device_id) { _distance_sensor_pub.get().device_id = device_id; };
	शून्य समुच्चयः_device_type(ध्रुव uint8_t device_type);

	शून्य समुच्चयः_fov(ध्रुव भिन्न fov) { समुच्चयः_hfov(fov); समुच्चयः_vfov(fov); }
	शून्य समुच्चयः_hfov(ध्रुव भिन्न fov) { _distance_sensor_pub.get().h_fov = fov; }
	शून्य समुच्चयः_vfov(ध्रुव भिन्न fov) { _distance_sensor_pub.get().v_fov = fov; }

	शून्य समुच्चयः_max_distance(ध्रुव भिन्न distance) { _distance_sensor_pub.get().max_distance = distance; }
	शून्य समुच्चयः_min_distance(ध्रुव भिन्न distance) { _distance_sensor_pub.get().min_distance = distance; }

	शून्य समुच्चयः_variance(ध्रुव भिन्न variance) { _distance_sensor_pub.get().variance = variance; }

	शून्य समुच्चयः_orientation(ध्रुव uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);

	शून्य समुच्चयः_बहुलकम्(ध्रुव uint8_t बहुलकम्) { _distance_sensor_pub.get().बहुलकम् = बहुलकम्; }

	// update with quaterion sensor orientation with respect to the vehicle body frame
	शून्य update(ध्रुव hrt_abstime &timestamp_sample, ध्रुव भिन्न distance, ध्रुव int8_t quality = -1, ध्रुव भिन्न *q = शून्यसूचकः,
		    uint8_t q_len = 4);

	पूर्णाङ्क get_instance() { प्रत्यागम _distance_sensor_pub.get_instance(); };
	uint32_t get_device_id() { प्रत्यागम _distance_sensor_pub.get().device_id; };

गुप्त:
	uORB::Publication_Multi_दत्त<distance_sensor_s> _distance_sensor_pub{ORB_ID(distance_sensor)};

	failure_injection::Config _failure_config;
	failure_injection::Stuck<distance_sensor_s> _stuck;
};
