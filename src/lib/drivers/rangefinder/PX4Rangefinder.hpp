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

#અન્તર્ભાવ <drivers/drv_hrt.h>
#અન્તર્ભાવ <lib/conversion/rotation.h>
#અન્તર્ભાવ <uORB/PublicationMulti.hpp>
#અન્તર્ભાવ <uORB/topics/distance_sensor.h>

#અન્તર્ભાવ <lib/failure_injection/FailureInjection.hpp>

વર્ગ PX4Rangefinder
{
જાહેર:
	PX4Rangefinder(અચલ uint32_t device_id,
		       અચલ uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);
	~PX4Rangefinder();

	// Set the MAV_DISTANCE_SENSOR type (LASER, ULTRASOUND, INFRARED, RADAR)
	શૂન્ય set_rangefinder_type(uint8_t rangefinder_type) { _distance_sensor_pub.get().type = rangefinder_type; };

	શૂન્ય set_device_id(અચલ uint32_t device_id) { _distance_sensor_pub.get().device_id = device_id; };
	શૂન્ય set_device_type(અચલ uint8_t device_type);

	શૂન્ય set_fov(અચલ દશાંશ fov) { set_hfov(fov); set_vfov(fov); }
	શૂન્ય set_hfov(અચલ દશાંશ fov) { _distance_sensor_pub.get().h_fov = fov; }
	શૂન્ય set_vfov(અચલ દશાંશ fov) { _distance_sensor_pub.get().v_fov = fov; }

	શૂન્ય set_max_distance(અચલ દશાંશ distance) { _distance_sensor_pub.get().max_distance = distance; }
	શૂન્ય set_min_distance(અચલ દશાંશ distance) { _distance_sensor_pub.get().min_distance = distance; }

	શૂન્ય set_variance(અચલ દશાંશ variance) { _distance_sensor_pub.get().variance = variance; }

	શૂન્ય set_orientation(અચલ uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);

	શૂન્ય set_mode(અચલ uint8_t mode) { _distance_sensor_pub.get().mode = mode; }

	// update with quaterion sensor orientation with respect to the vehicle body frame
	શૂન્ય update(અચલ hrt_abstime &timestamp_sample, અચલ દશાંશ distance, અચલ int8_t quality = -1, અચલ દશાંશ *q = nullptr,
		    uint8_t q_len = 4);

	પૂર્ણાંક get_instance() { પાછા _distance_sensor_pub.get_instance(); };
	uint32_t get_device_id() { પાછા _distance_sensor_pub.get().device_id; };

ખાનગી:
	uORB::PublicationMultiData<distance_sensor_s> _distance_sensor_pub{ORB_ID(distance_sensor)};

	failure_injection::Config _failure_config;
	failure_injection::Stuck<distance_sensor_s> _stuck;
};
