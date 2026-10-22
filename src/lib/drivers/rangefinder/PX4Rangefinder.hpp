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

#رهنمود once

#ضمن <drivers/drv_hrt.h>
#ضمن <lib/conversion/rotation.h>
#ضمن <uORB/PublicationMulti.hpp>
#ضمن <uORB/topics/distance_sensor.h>

#ضمن <lib/failure_injection/FailureInjection.hpp>

صنف PX4Rangefinder
{
عام:
	PX4Rangefinder(ثابت uint32_t device_id,
		       ثابت uint8_t device_orientation = فاصله_sensor_s::ROTATION_DOWNWARD_FACING);
	~PX4Rangefinder();

	// Set the MAV_DISTANCE_SENSOR type (LASER, ULTRASOUND, INFRARED, RADAR)
	فراغ مجموعه_rangefinder_type(uint8_t rangefinder_type) { فاصله_sensor_pub_2.get().type = rangefinder_type; };

	فراغ مجموعه_device_id(ثابت uint32_t device_id) { فاصله_sensor_pub_2.get().device_id = device_id; };
	فراغ مجموعه_device_type(ثابت uint8_t device_type);

	فراغ مجموعه_fov(ثابت عائم fov) { مجموعه_hfov(fov); مجموعه_vfov(fov); }
	فراغ مجموعه_hfov(ثابت عائم fov) { فاصله_sensor_pub_2.get().h_fov = fov; }
	فراغ مجموعه_vfov(ثابت عائم fov) { فاصله_sensor_pub_2.get().v_fov = fov; }

	فراغ مجموعه_max_فاصله(ثابت عائم فاصله) { فاصله_sensor_pub_2.get().max_فاصله = فاصله; }
	فراغ مجموعه_min_فاصله(ثابت عائم فاصله) { فاصله_sensor_pub_2.get().min_فاصله = فاصله; }

	فراغ مجموعه_variance(ثابت عائم variance) { فاصله_sensor_pub_2.get().variance = variance; }

	فراغ مجموعه_orientation(ثابت uint8_t device_orientation = فاصله_sensor_s::ROTATION_DOWNWARD_FACING);

	فراغ مجموعه_نما(ثابت uint8_t نما) { فاصله_sensor_pub_2.get().نما = نما; }

	// update with quaterion sensor orientation with respect to the vehicle body frame
	فراغ update(ثابت hrt_abstime &timestamp_sample, ثابت عائم فاصله, ثابت int8_t quality = -1, ثابت عائم *q = اشاره_تهی,
		    uint8_t q_len = 4);

	صحيح get_instance() { أرجع فاصله_sensor_pub_2.get_instance(); };
	uint32_t get_device_id() { أرجع فاصله_sensor_pub_2.get().device_id; };

خاص:
	uORB::Publication_Multi_داده<فاصله_sensor_s> فاصله_sensor_pub_2{ORB_ID(فاصله_sensor)};

	failure_injection::Config _failure_config;
	failure_injection::Stuck<فاصله_sensor_s> _stuck;
};
