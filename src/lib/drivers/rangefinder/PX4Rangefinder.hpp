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

#ضمن <drivers/drv_hrt.h>
#ضمن <lib/conversion/rotation.h>
#ضمن <uORB/PublicationMulti.hpp>
#ضمن <uORB/topics/distance_sensor.h>

#ضمن <lib/failure_injection/FailureInjection.hpp>

صنف PX4Rangefinder
{
عام:
	PX4Rangefinder(ثابت uint32_t device_id,
		       ثابت uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);
	~PX4Rangefinder();

	// Set the MAV_DISTANCE_SENSOR type (LASER, ULTRASOUND, INFRARED, RADAR)
	فراغ set_rangefinder_type(uint8_t rangefinder_type) { _distance_sensor_pub.get().type = rangefinder_type; };

	فراغ set_device_id(ثابت uint32_t device_id) { _distance_sensor_pub.get().device_id = device_id; };
	فراغ set_device_type(ثابت uint8_t device_type);

	فراغ set_fov(ثابت عائم fov) { set_hfov(fov); set_vfov(fov); }
	فراغ set_hfov(ثابت عائم fov) { _distance_sensor_pub.get().h_fov = fov; }
	فراغ set_vfov(ثابت عائم fov) { _distance_sensor_pub.get().v_fov = fov; }

	فراغ set_max_distance(ثابت عائم distance) { _distance_sensor_pub.get().max_distance = distance; }
	فراغ set_min_distance(ثابت عائم distance) { _distance_sensor_pub.get().min_distance = distance; }

	فراغ set_variance(ثابت عائم variance) { _distance_sensor_pub.get().variance = variance; }

	فراغ set_orientation(ثابت uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);

	فراغ set_mode(ثابت uint8_t mode) { _distance_sensor_pub.get().mode = mode; }

	// update with quaterion sensor orientation with respect to the vehicle body frame
	فراغ update(ثابت hrt_abstime &timestamp_sample, ثابت عائم distance, ثابت int8_t quality = -1, ثابت عائم *q = nullptr,
		    uint8_t q_len = 4);

	صحيح get_instance() { أرجع _distance_sensor_pub.get_instance(); };
	uint32_t get_device_id() { أرجع _distance_sensor_pub.get().device_id; };

خاص:
	uORB::PublicationMultiData<distance_sensor_s> _distance_sensor_pub{ORB_ID(distance_sensor)};

	failure_injection::Config _failure_config;
	failure_injection::Stuck<distance_sensor_s> _stuck;
};
