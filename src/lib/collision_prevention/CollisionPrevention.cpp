/****************************************************************************
 *
 *   Copyright (c) 2018-2024 PX4 Development Team. All rights reserved.
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

/**
 * @file CollisionPrevention.cpp
 * CollisionPrevention controller.
 *
 */

#অন্তর্ভাব "CollisionPrevention.hpp"
#অন্তর্ভাব "ObstacleMath.hpp"
#অন্তর্ভাব <px4_platform_common/events.h>
#অন্তর্ভাব <math.h>


using নামাকাশ matrix;

CollisionPrevention::CollisionPrevention(ModuleParams *parent) :
	ModuleParams(parent)
{
	static_assert(BIN_আকার >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_আকার == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	_obstacle_map_body_frame.frame = obstacle_distance_s::MAV_FRAME_BODY_FRD;
	_obstacle_map_body_frame.increment = BIN_আকার;
	_obstacle_map_body_frame.min_distance = UINT16_MAX;

	জন্য (uint32_t i = 0 ; i < BIN_গুনতি; i++) {
		_obstacle_map_body_frame.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::getTime()
{
	ফেরত hrt_absolute_time();
}

hrt_abstime CollisionPrevention::getElapsedTime(ধ্রুবক hrt_abstime *ptr)
{
	ফেরত hrt_absolute_time() - *ptr;
}

যৌক্তিক CollisionPrevention::is_active()
{
	যৌক্তিক activated = _param_cp_dist.get() > 0;

	যদি (activated && !_was_active) {
		_time_activated = getTime();
	}

	_was_active = activated;
	ফেরত activated;
}

শূন্য CollisionPrevention::modifySetpoint(Vector2f &setpoint_accel, ধ্রুবক Vector2f &setpoint_vel)
{
	যদি (_vehicle_attitude_sub.updated()) {
		vehicle_attitude_s vehicle_attitude;

		যদি (_vehicle_attitude_sub.copy(&vehicle_attitude)) {
			_vehicle_attitude = Quatf(vehicle_attitude.q);
			_vehicle_yaw = Eulerf(_vehicle_attitude).psi();
		}
	}

	//calculate movement constraints based on range data
	ধ্রুবক Vector2f original_setpoint = setpoint_accel;
	_updateObstacleMap();
	update_Obstacle_তথ্য();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copyTo(constraints.original_setpoint);
	setpoint_accel.copyTo(constraints.adapted_setpoint);
	constraints.timestamp = getTime();
	_constraints_pub.publish(constraints);
}

শূন্য CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	জন্য (auto &dist_sens_sub : _distance_sensor_subs) {
		distance_sensor_s distance_sensor;

		যদি (dist_sens_sub.update(&distance_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			যদি ((getElapsedTime(&distance_sensor.timestamp) < RANGE_STREAM_TIMEOUT_US) &&
			    (distance_sensor.orientation != distance_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (distance_sensor.orientation != distance_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				_obstacle_map_body_frame.timestamp = math::max(_obstacle_map_body_frame.timestamp, distance_sensor.timestamp);
				_obstacle_map_body_frame.max_distance = math::max(_obstacle_map_body_frame.max_distance,
									(uint16_t)(distance_sensor.max_distance * 100.0f));
				_obstacle_map_body_frame.min_distance = math::min(_obstacle_map_body_frame.min_distance,
									(uint16_t)(distance_sensor.min_distance * 100.0f));

				add_Distance_Sensor_তথ্য(distance_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	যদি (_sub_obstacle_distance.update()) {
		ধ্রুবক obstacle_distance_s &obstacle_distance = _sub_obstacle_distance.get();

		// Update map with obstacle data if the data is not stale
		যদি (getElapsedTime(&obstacle_distance.timestamp) < RANGE_STREAM_TIMEOUT_US && obstacle_distance.increment > 0.f) {
			//update message description
			_obstacle_map_body_frame.timestamp = math::max(_obstacle_map_body_frame.timestamp, obstacle_distance.timestamp);
			_obstacle_map_body_frame.max_distance = math::max(_obstacle_map_body_frame.max_distance,
								obstacle_distance.max_distance);
			_obstacle_map_body_frame.min_distance = math::min(_obstacle_map_body_frame.min_distance,
								obstacle_distance.min_distance);
			add_Obstacle_Sensor_তথ্য(obstacle_distance, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	_obstacle_distance_fused_pub.publish(_obstacle_map_body_frame);
}

শূন্য CollisionPrevention::update_Obstacle_তথ্য()
{
	obstacle_তথ্য_present = মিথ্যা;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.setZero();

	জন্য (পূর্ণসংখ্যা i = 0; i < BIN_গুনতি; i++) {
		// if the data is stale, reset the bin
		যদি (getTime() - তথ্য_timestamps[i] > RANGE_STREAM_TIMEOUT_US) {
			_obstacle_map_body_frame.distances[i] = UINT16_MAX;
		}

		দশমিক angle = wrap_2pi(_vehicle_yaw + math::radians((দশমিক)i * BIN_আকার +
				       _obstacle_map_body_frame.angle_offset));
		ধ্রুবক Vector2f bin_direction = {cosf(angle), sinf(angle)};
		ধ্রুবক uint16_t bin_distance = _obstacle_map_body_frame.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		যদি (bin_distance < UINT16_MAX
		    && (getTime() - _obstacle_map_body_frame.timestamp) < RANGE_STREAM_TIMEOUT_US) {
			obstacle_তথ্য_present = সত্য;
		}

		যদি (bin_distance * 0.01f < _closest_dist) {
			_closest_dist = bin_distance * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

শূন্য CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, ধ্রুবক Vector2f &setpoint_vel)
{
	ধ্রুবক দশমিক setpoint_দৈর্ঘ্য = setpoint_accel.norm();
	_min_dist_to_keep = math::max(_obstacle_map_body_frame.min_distance / 100.0f, _param_cp_dist.get());

	ধ্রুবক hrt_abstime now = getTime();

	ধ্রুবক যৌক্তিক is_stick_deflected = setpoint_দৈর্ঘ্য > 0.001f;

	যদি (obstacle_তথ্য_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		দশমিক vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		যদি (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_দৈর্ঘ্য);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} অন্যথা যদি (!obstacle_তথ্য_present) {
		// allow no movement
		setpoint_accel.setZero();

		// if distance data is stale, switch to Loiter
		যদি (getElapsedTime(&_last_timeout_warning) > 1_s && getElapsedTime(&_time_activated) > 1_s) {
			যদি ((now - _obstacle_map_body_frame.timestamp) > TIMEOUT_HOLD_US &&
			    getElapsedTime(&_time_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			_last_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
শূন্য CollisionPrevention::add_Obstacle_Sensor_তথ্য(ধ্রুবক obstacle_distance_s &obstacle, ধ্রুবক দশমিক vehicle_yaw)
{

	দশমিক vehicle_orientation_deg = math::degrees(vehicle_yaw);


	যদি (obstacle.frame == obstacle.MAV_FRAME_GLOBAL || obstacle.frame == obstacle.MAV_FRAME_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		জন্য (পূর্ণসংখ্যা i = 0; i < BIN_গুনতি; i++) {
			জন্য (পূর্ণসংখ্যা j = 0; (j < 360 / obstacle.increment) && (j < BIN_গুনতি); j++) {
				দশমিক bin_lower_angle = ObstacleMath::get_lower_bound_angle(i, _obstacle_map_body_frame.increment,
							_obstacle_map_body_frame.angle_offset);
				দশমিক bin_upper_angle = ObstacleMath::get_lower_bound_angle(i + 1, _obstacle_map_body_frame.increment,
							_obstacle_map_body_frame.angle_offset);
				দশমিক বার্তা_lower_angle = ObstacleMath::get_lower_bound_angle(j, obstacle.increment,
							obstacle.angle_offset - vehicle_orientation_deg);
				দশমিক বার্তা_upper_angle = ObstacleMath::get_lower_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_offset - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				যদি (bin_lower_angle > bin_upper_angle) {
					bin_lower_angle -= 360;
				}

				যদি (বার্তা_lower_angle > বার্তা_upper_angle) {
					বার্তা_lower_angle -= 360;
				}

				// Check for overlaps.
				যদি ((বার্তা_lower_angle > bin_lower_angle && বার্তা_lower_angle < bin_upper_angle) ||
				    (বার্তা_upper_angle > bin_lower_angle && বার্তা_upper_angle < bin_upper_angle) ||
				    (বার্তা_lower_angle <= bin_lower_angle && বার্তা_upper_angle >= bin_upper_angle) ||
				    (বার্তা_lower_angle >= bin_lower_angle && বার্তা_upper_angle <= bin_upper_angle)) {
					যদি (obstacle.distances[j] != UINT16_MAX) {
						যদি (enter_তথ্য(i, obstacle.max_distance * 0.01f, obstacle.distances[j] * 0.01f)) {
							_obstacle_map_body_frame.distances[i] = obstacle.distances[j];
							তথ্য_timestamps[i] = _obstacle_map_body_frame.timestamp;
							তথ্য_maxranges[i] = obstacle.max_distance;
							তথ্য_fov[i] = 1;
						}
					}
				}

			}
		}

	} অন্যথা যদি (obstacle.frame == obstacle.MAV_FRAME_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		জন্য (পূর্ণসংখ্যা i = 0; i < BIN_গুনতি; i++) {
			জন্য (পূর্ণসংখ্যা j = 0; (j < 360 / obstacle.increment) && (j < BIN_গুনতি); j++) {
				দশমিক bin_lower_angle = ObstacleMath::get_lower_bound_angle(i, _obstacle_map_body_frame.increment,
							_obstacle_map_body_frame.angle_offset);
				দশমিক bin_upper_angle = ObstacleMath::get_lower_bound_angle(i + 1, _obstacle_map_body_frame.increment,
							_obstacle_map_body_frame.angle_offset);
				দশমিক বার্তা_lower_angle = ObstacleMath::get_lower_bound_angle(j, obstacle.increment, obstacle.angle_offset);
				দশমিক বার্তা_upper_angle = ObstacleMath::get_lower_bound_angle(j + 1, obstacle.increment, obstacle.angle_offset);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				যদি (bin_lower_angle > bin_upper_angle) {
					bin_lower_angle -= 360;
				}

				যদি (বার্তা_lower_angle > বার্তা_upper_angle) {
					বার্তা_lower_angle -= 360;
				}

				// Check for overlaps.
				যদি ((বার্তা_lower_angle > bin_lower_angle && বার্তা_lower_angle < bin_upper_angle) ||
				    (বার্তা_upper_angle > bin_lower_angle && বার্তা_upper_angle < bin_upper_angle) ||
				    (বার্তা_lower_angle <= bin_lower_angle && বার্তা_upper_angle >= bin_upper_angle) ||
				    (বার্তা_lower_angle >= bin_lower_angle && বার্তা_upper_angle <= bin_upper_angle)) {
					যদি (obstacle.distances[j] != UINT16_MAX) {

						যদি (enter_তথ্য(i, obstacle.max_distance * 0.01f, obstacle.distances[j] * 0.01f)) {
							_obstacle_map_body_frame.distances[i] = obstacle.distances[j];
							তথ্য_timestamps[i] = _obstacle_map_body_frame.timestamp;
							তথ্য_maxranges[i] = obstacle.max_distance;
							তথ্য_fov[i] = 1;
						}
					}
				}

			}
		}

	} অন্যথা {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.frame);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.frame);
	}
}

যৌক্তিক
CollisionPrevention::enter_তথ্য(পূর্ণসংখ্যা map_সূচক, দশমিক sensor_range, দশমিক sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	যদি (sensor_reading < sensor_range) {
		যদি ((_obstacle_map_body_frame.distances[map_সূচক] < তথ্য_maxranges[map_সূচক]
		     && sensor_range_cm <= তথ্য_maxranges[map_সূচক])
		    || _obstacle_map_body_frame.distances[map_সূচক] >= তথ্য_maxranges[map_সূচক]) {

			ফেরত সত্য;
		}

	} অন্যথা {
		যদি ((_obstacle_map_body_frame.distances[map_সূচক] >= তথ্য_maxranges[map_সূচক]
		     && sensor_range_cm >= তথ্য_maxranges[map_সূচক])
		    || (_obstacle_map_body_frame.distances[map_সূচক] < তথ্য_maxranges[map_সূচক]
			&& sensor_range_cm == তথ্য_maxranges[map_সূচক])) {

			ফেরত সত্য;
		}
	}

	ফেরত মিথ্যা;
}

যৌক্তিক
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	যদি (setpoint_সূচক < 0 || setpoint_সূচক >= BIN_গুনতি) {
		ফেরত মিথ্যা; // treat out-of-bounds as unsafe
	}

	ধ্রুবক যৌক্তিক no_তথ্য = (_obstacle_map_body_frame.distances[setpoint_সূচক] == UINT16_MAX);
	ধ্রুবক যৌক্তিক allow_movement_towards_no_তথ্য = param_cp_go_no_তথ্য.get();
	ধ্রুবক যৌক্তিক fov_at_setpoint = তথ্য_fov[setpoint_সূচক];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	ফেরত !no_তথ্য || (allow_movement_towards_no_তথ্য && !fov_at_setpoint);
}

শূন্য
CollisionPrevention::_transformSetpoint(ধ্রুবক Vector2f &setpoint)
{
	ধ্রুবক দশমিক sp_angle_body_frame = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	ধ্রুবক দশমিক sp_angle_with_offset_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_frame) -
					       _obstacle_map_body_frame.angle_offset);
	setpoint_সূচক = floor(sp_angle_with_offset_deg / BIN_আকার);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, setpoint_সূচক, _vehicle_yaw);
}

শূন্য
CollisionPrevention::add_Distance_Sensor_তথ্য(distance_sensor_s &distance_sensor, ধ্রুবক Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	দশমিক distance_reading = math::min(distance_sensor.current_distance, distance_sensor.max_distance);

	// negative values indicate out of range but valid measurements.
	যদি (fabsf(distance_sensor.current_distance - -1.f) < FLT_EPSILON && distance_sensor.signal_quality == 0) {
		distance_reading = distance_sensor.max_distance;
	}

	// discard values below min range
	যদি (distance_reading > distance_sensor.min_distance) {
		দশমিক sensor_yaw_body_rad = ObstacleMath::sensor_orientation_to_yaw_offset(static_cast<ObstacleMath::SensorOrientation>
					    (distance_sensor.orientation), distance_sensor.q);
		দশমিক sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		পূর্ণসংখ্যা lower_bound = (পূর্ণসংখ্যা)round((sensor_yaw_body_deg  - math::degrees(distance_sensor.h_fov / 2.0f)) / BIN_আকার);
		পূর্ণসংখ্যা upper_bound = (পূর্ণসংখ্যা)round((sensor_yaw_body_deg  + math::degrees(distance_sensor.h_fov / 2.0f)) / BIN_আকার);

		যদি (distance_reading < distance_sensor.max_distance) {
			ObstacleMath::project_distance_on_horizontal_plane(distance_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * distance_sensor.max_distance)); // convert to cm

		জন্য (পূর্ণসংখ্যা bin = lower_bound; bin <= upper_bound; ++bin) {
			পূর্ণসংখ্যা wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_গুনতি);

			যদি (enter_তথ্য(wrapped_bin, distance_sensor.max_distance, distance_reading)) {
				_obstacle_map_body_frame.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * distance_reading));
				তথ্য_timestamps[wrapped_bin] = _obstacle_map_body_frame.timestamp;
				তথ্য_maxranges[wrapped_bin] = sensor_range;
				তথ্য_fov[wrapped_bin] = 1;
			}
		}
	}
}

শূন্য
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, পূর্ণসংখ্যা &setpoint_সূচক_2, দশমিক vehicle_yaw_angle_rad)
{
	ধ্রুবক পূর্ণসংখ্যা guidance_bins = floor(_param_cp_guide_ang.get() / BIN_আকার);
	ধ্রুবক পূর্ণসংখ্যা sp_সূচক_original = setpoint_সূচক_2;
	দশমিক best_cost = 9999.f;
	পূর্ণসংখ্যা new_sp_সূচক = setpoint_সূচক_2;

	জন্য (পূর্ণসংখ্যা i = sp_সূচক_original - guidance_bins; i <= sp_সূচক_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		ধ্রুবক পূর্ণসংখ্যা filter_আকার = 1;
		দশমিক mean_dist = 0;

		জন্য (পূর্ণসংখ্যা j = i - filter_আকার; j <= i + filter_আকার; j++) {
			পূর্ণসংখ্যা bin = ObstacleMath::wrap_bin(j, BIN_গুনতি);

			যদি (_obstacle_map_body_frame.distances[bin] == UINT16_MAX) {
				mean_dist += _param_cp_dist.get() * 100.f;

			} অন্যথা {
				mean_dist += _obstacle_map_body_frame.distances[bin];
			}
		}

		ধ্রুবক পূর্ণসংখ্যা bin = ObstacleMath::wrap_bin(i, BIN_গুনতি);
		mean_dist = mean_dist / (2.f * filter_আকার + 1.f);
		ধ্রুবক দশমিক deviation_cost = _param_cp_dist.get() * 50.f * abs(i - sp_সূচক_original);
		ধ্রুবক দশমিক bin_cost = deviation_cost - mean_dist - _obstacle_map_body_frame.distances[bin];

		যদি (bin_cost < best_cost && _obstacle_map_body_frame.distances[bin] != UINT16_MAX) {
			best_cost = bin_cost;
			new_sp_সূচক = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	যদি (new_sp_সূচক != setpoint_সূচক_2) {
		দশমিক angle = math::radians((দশমিক)new_sp_সূচক * BIN_আকার + _obstacle_map_body_frame.angle_offset);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_সূচক_2 = new_sp_সূচক;
	}
}

দশমিক CollisionPrevention::_getObstacleDistance(ধ্রুবক Vector2f &direction)
{
	দশমিক obstacle_distance = 0.f;
	ধ্রুবক দশমিক direction_norm = direction.norm();

	যদি (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		ধ্রুবক দশমিক sp_angle_body_frame = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		ধ্রুবক দশমিক sp_angle_with_offset_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_frame) - _obstacle_map_body_frame.angle_offset);

		ধ্রুবক পূর্ণসংখ্যা dir_সূচক = ObstacleMath::get_bin_at_angle(BIN_আকার, sp_angle_with_offset_deg);
		obstacle_distance   = _obstacle_map_body_frame.distances[dir_সূচক] * 0.01f;
	}

	ফেরত obstacle_distance;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(ধ্রুবক দশমিক &setpoint_দৈর্ঘ্য)
{
	Vector2f new_setpoint{};
	ধ্রুবক Vector2f normal_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	ধ্রুবক Vector2f tangential_component = _setpoint_dir - normal_component;

	ধ্রুবক দশমিক normal_scale = _getScale(_closest_dist);


	ধ্রুবক দশমিক closest_dist_tangential = _getObstacleDistance(tangential_component);
	ধ্রুবক দশমিক tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	যদি (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + normal_component * normal_scale) * setpoint_দৈর্ঘ্য;

	} অন্যথা {
		new_setpoint = _setpoint_dir * setpoint_দৈর্ঘ্য;
	}

	ফেরত new_setpoint;
}

দশমিক
CollisionPrevention::_getScale(ধ্রুবক দশমিক &reference_distance)
{
	দশমিক scale = (reference_distance - _min_dist_to_keep);
	ধ্রুবক দশমিক scale_distance = math::max(_min_dist_to_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_distance, 2) : scale;
	scale = math::min(scale, 1.0f);
	ফেরত scale;
}

শূন্য CollisionPrevention::_getVelocityCompensationAcceleration(ধ্রুবক দশমিক vehicle_yaw_angle_rad,
		ধ্রুবক Vector2f &setpoint_vel,
		ধ্রুবক hrt_abstime now, দশমিক &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	জন্য (পূর্ণসংখ্যা i = 0; i < BIN_গুনতি; i++) {
		ধ্রুবক দশমিক max_range = তথ্য_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		দশমিক bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((দশমিক)i * BIN_আকার + _obstacle_map_body_frame.angle_offset));

		ধ্রুবক Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		দশমিক bin_distance = _obstacle_map_body_frame.distances[i];

		// only consider bins which are between min and max values
		যদি (bin_distance > _obstacle_map_body_frame.min_distance && bin_distance < UINT16_MAX) {
			ধ্রুবক দশমিক distance = bin_distance * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			ধ্রুবক দশমিক curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			দশমিক delay_distance = curr_vel_parallel * _param_cp_delay.get();

			ধ্রুবক hrt_abstime তথ্য_age = now - তথ্য_timestamps[i];

			যদি (distance < max_range) {
				delay_distance += curr_vel_parallel * (তথ্য_age * 1e-6f);
			}

			ধ্রুবক দশমিক stop_distance = distance - _min_dist_to_keep - delay_distance;

			দশমিক curr_acc_vel_constraint;

			যদি (stop_distance >= 0.f) {
				ধ্রুবক দশমিক max_vel = math::trajectory::computeMaxSpeedFromDistance(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_distance, 0.f);
				curr_acc_vel_constraint = _param_mpc_xy_vel_p_acc.get() * math::min(max_vel - curr_vel_parallel, 0.f);

			} অন্যথা {
				curr_acc_vel_constraint = -1.f * _param_mpc_xy_vel_p_acc.get() * curr_vel_parallel;
			}

			যদি (curr_acc_vel_constraint < vel_comp_accel) {
				vel_comp_accel = curr_acc_vel_constraint;
				vel_comp_accel_dir = bin_direction;
			}
		}
	}
}

শূন্য CollisionPrevention::_publishVehicleCmdDoLoiter()
{
	vehicle_command_s command{};
	command.command = vehicle_command_s::VEHICLE_CMD_DO_SET_MODE;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (দশমিক)PX4_CUSTOM_MAIN_MODE_AUTO;
	command.param3 = (দশমিক)PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
	command.target_system = 1;
	command.target_component = 1;
	command.source_system = 1;
	command.source_component = 1;
	command.confirmation = মিথ্যা;
	command.from_external = মিথ্যা;
	command.timestamp = getTime();
	_vehicle_command_pub.publish(command);
}
