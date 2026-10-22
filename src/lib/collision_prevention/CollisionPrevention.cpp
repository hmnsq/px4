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

#अन्तर्भाव "CollisionPrevention.hpp"
#अन्तर्भाव "ObstacleMath.hpp"
#अन्तर्भाव <px4_platform_common/events.h>
#अन्तर्भाव <math.h>


using नामाकाश matrix;

CollisionPrevention::CollisionPrevention(ModuleParams *parent) :
	ModuleParams(parent)
{
	static_assert(BIN_SIZE >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_SIZE == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	_obstacle_map_body_frame.frame = obstacle_distance_s::MAV_FRAME_BODY_FRD;
	_obstacle_map_body_frame.increment = BIN_SIZE;
	_obstacle_map_body_frame.min_distance = UINT16_MAX;

	प्रति (uint32_t i = 0 ; i < BIN_मोजणी; i++) {
		_obstacle_map_body_frame.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::getTime()
{
	प्रत्यागम hrt_absolute_time();
}

hrt_abstime CollisionPrevention::getElapsedTime(ध्रुव hrt_abstime *ptr)
{
	प्रत्यागम hrt_absolute_time() - *ptr;
}

तार्किक CollisionPrevention::is_active()
{
	तार्किक activated = _param_cp_dist.get() > 0;

	यदि (activated && !_was_active) {
		_time_activated = getTime();
	}

	_was_active = activated;
	प्रत्यागम activated;
}

शून्य CollisionPrevention::modifySetpoint(Vector2f &setpoint_accel, ध्रुव Vector2f &setpoint_vel)
{
	यदि (_vehicle_attitude_sub.updated()) {
		vehicle_attitude_s vehicle_attitude;

		यदि (_vehicle_attitude_sub.copy(&vehicle_attitude)) {
			_vehicle_attitude = Quatf(vehicle_attitude.q);
			_vehicle_yaw = Eulerf(_vehicle_attitude).psi();
		}
	}

	//calculate movement constraints based on range data
	ध्रुव Vector2f original_setpoint = setpoint_accel;
	_updateObstacleMap();
	_updateObstacleData();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copyTo(constraints.original_setpoint);
	setpoint_accel.copyTo(constraints.adapted_setpoint);
	constraints.timestamp = getTime();
	_constraints_pub.publish(constraints);
}

शून्य CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	प्रति (auto &dist_sens_sub : _distance_sensor_subs) {
		distance_sensor_s distance_sensor;

		यदि (dist_sens_sub.update(&distance_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			यदि ((getElapsedTime(&distance_sensor.timestamp) < RANGE_STREAM_TIMEOUT_US) &&
			    (distance_sensor.orientation != distance_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (distance_sensor.orientation != distance_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				_obstacle_map_body_frame.timestamp = math::max(_obstacle_map_body_frame.timestamp, distance_sensor.timestamp);
				_obstacle_map_body_frame.max_distance = math::max(_obstacle_map_body_frame.max_distance,
									(uint16_t)(distance_sensor.max_distance * 100.0f));
				_obstacle_map_body_frame.min_distance = math::min(_obstacle_map_body_frame.min_distance,
									(uint16_t)(distance_sensor.min_distance * 100.0f));

				_addDistanceSensorData(distance_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	यदि (_sub_obstacle_distance.update()) {
		ध्रुव obstacle_distance_s &obstacle_distance = _sub_obstacle_distance.get();

		// Update map with obstacle data if the data is not stale
		यदि (getElapsedTime(&obstacle_distance.timestamp) < RANGE_STREAM_TIMEOUT_US && obstacle_distance.increment > 0.f) {
			//update message description
			_obstacle_map_body_frame.timestamp = math::max(_obstacle_map_body_frame.timestamp, obstacle_distance.timestamp);
			_obstacle_map_body_frame.max_distance = math::max(_obstacle_map_body_frame.max_distance,
								obstacle_distance.max_distance);
			_obstacle_map_body_frame.min_distance = math::min(_obstacle_map_body_frame.min_distance,
								obstacle_distance.min_distance);
			_addObstacleSensorData(obstacle_distance, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	_obstacle_distance_fused_pub.publish(_obstacle_map_body_frame);
}

शून्य CollisionPrevention::_updateObstacleData()
{
	_obstacle_data_present = असत्य;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.setZero();

	प्रति (पूर्णाङ्क i = 0; i < BIN_मोजणी; i++) {
		// if the data is stale, reset the bin
		यदि (getTime() - _data_timestamps[i] > RANGE_STREAM_TIMEOUT_US) {
			_obstacle_map_body_frame.distances[i] = UINT16_MAX;
		}

		भिन्न angle = wrap_2pi(_vehicle_yaw + math::radians((भिन्न)i * BIN_SIZE +
				       _obstacle_map_body_frame.angle_offset));
		ध्रुव Vector2f bin_direction = {cosf(angle), sinf(angle)};
		ध्रुव uint16_t bin_distance = _obstacle_map_body_frame.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		यदि (bin_distance < UINT16_MAX
		    && (getTime() - _obstacle_map_body_frame.timestamp) < RANGE_STREAM_TIMEOUT_US) {
			_obstacle_data_present = सत्य;
		}

		यदि (bin_distance * 0.01f < _closest_dist) {
			_closest_dist = bin_distance * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

शून्य CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, ध्रुव Vector2f &setpoint_vel)
{
	ध्रुव भिन्न setpoint_length = setpoint_accel.norm();
	_min_dist_to_keep = math::max(_obstacle_map_body_frame.min_distance / 100.0f, _param_cp_dist.get());

	ध्रुव hrt_abstime now = getTime();

	ध्रुव तार्किक is_stick_deflected = setpoint_length > 0.001f;

	यदि (_obstacle_data_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		भिन्न vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		यदि (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_length);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} अन्यथा यदि (!_obstacle_data_present) {
		// allow no movement
		setpoint_accel.setZero();

		// if distance data is stale, switch to Loiter
		यदि (getElapsedTime(&_last_timeout_warning) > 1_s && getElapsedTime(&_time_activated) > 1_s) {
			यदि ((now - _obstacle_map_body_frame.timestamp) > TIMEOUT_HOLD_US &&
			    getElapsedTime(&_time_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			_last_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
शून्य CollisionPrevention::_addObstacleSensorData(ध्रुव obstacle_distance_s &obstacle, ध्रुव भिन्न vehicle_yaw)
{

	भिन्न vehicle_orientation_deg = math::degrees(vehicle_yaw);


	यदि (obstacle.frame == obstacle.MAV_FRAME_GLOBAL || obstacle.frame == obstacle.MAV_FRAME_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		प्रति (पूर्णाङ्क i = 0; i < BIN_मोजणी; i++) {
			प्रति (पूर्णाङ्क j = 0; (j < 360 / obstacle.increment) && (j < BIN_मोजणी); j++) {
				भिन्न bin_lower_angle = ObstacleMath::get_lower_bound_angle(i, _obstacle_map_body_frame.increment,
							_obstacle_map_body_frame.angle_offset);
				भिन्न bin_upper_angle = ObstacleMath::get_lower_bound_angle(i + 1, _obstacle_map_body_frame.increment,
							_obstacle_map_body_frame.angle_offset);
				भिन्न संदेश_lower_angle = ObstacleMath::get_lower_bound_angle(j, obstacle.increment,
							obstacle.angle_offset - vehicle_orientation_deg);
				भिन्न संदेश_upper_angle = ObstacleMath::get_lower_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_offset - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				यदि (bin_lower_angle > bin_upper_angle) {
					bin_lower_angle -= 360;
				}

				यदि (संदेश_lower_angle > संदेश_upper_angle) {
					संदेश_lower_angle -= 360;
				}

				// Check for overlaps.
				यदि ((संदेश_lower_angle > bin_lower_angle && संदेश_lower_angle < bin_upper_angle) ||
				    (संदेश_upper_angle > bin_lower_angle && संदेश_upper_angle < bin_upper_angle) ||
				    (संदेश_lower_angle <= bin_lower_angle && संदेश_upper_angle >= bin_upper_angle) ||
				    (संदेश_lower_angle >= bin_lower_angle && संदेश_upper_angle <= bin_upper_angle)) {
					यदि (obstacle.distances[j] != UINT16_MAX) {
						यदि (_enterData(i, obstacle.max_distance * 0.01f, obstacle.distances[j] * 0.01f)) {
							_obstacle_map_body_frame.distances[i] = obstacle.distances[j];
							_data_timestamps[i] = _obstacle_map_body_frame.timestamp;
							_data_maxranges[i] = obstacle.max_distance;
							_data_fov[i] = 1;
						}
					}
				}

			}
		}

	} अन्यथा यदि (obstacle.frame == obstacle.MAV_FRAME_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		प्रति (पूर्णाङ्क i = 0; i < BIN_मोजणी; i++) {
			प्रति (पूर्णाङ्क j = 0; (j < 360 / obstacle.increment) && (j < BIN_मोजणी); j++) {
				भिन्न bin_lower_angle = ObstacleMath::get_lower_bound_angle(i, _obstacle_map_body_frame.increment,
							_obstacle_map_body_frame.angle_offset);
				भिन्न bin_upper_angle = ObstacleMath::get_lower_bound_angle(i + 1, _obstacle_map_body_frame.increment,
							_obstacle_map_body_frame.angle_offset);
				भिन्न संदेश_lower_angle = ObstacleMath::get_lower_bound_angle(j, obstacle.increment, obstacle.angle_offset);
				भिन्न संदेश_upper_angle = ObstacleMath::get_lower_bound_angle(j + 1, obstacle.increment, obstacle.angle_offset);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				यदि (bin_lower_angle > bin_upper_angle) {
					bin_lower_angle -= 360;
				}

				यदि (संदेश_lower_angle > संदेश_upper_angle) {
					संदेश_lower_angle -= 360;
				}

				// Check for overlaps.
				यदि ((संदेश_lower_angle > bin_lower_angle && संदेश_lower_angle < bin_upper_angle) ||
				    (संदेश_upper_angle > bin_lower_angle && संदेश_upper_angle < bin_upper_angle) ||
				    (संदेश_lower_angle <= bin_lower_angle && संदेश_upper_angle >= bin_upper_angle) ||
				    (संदेश_lower_angle >= bin_lower_angle && संदेश_upper_angle <= bin_upper_angle)) {
					यदि (obstacle.distances[j] != UINT16_MAX) {

						यदि (_enterData(i, obstacle.max_distance * 0.01f, obstacle.distances[j] * 0.01f)) {
							_obstacle_map_body_frame.distances[i] = obstacle.distances[j];
							_data_timestamps[i] = _obstacle_map_body_frame.timestamp;
							_data_maxranges[i] = obstacle.max_distance;
							_data_fov[i] = 1;
						}
					}
				}

			}
		}

	} अन्यथा {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.frame);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.frame);
	}
}

तार्किक
CollisionPrevention::_enterData(पूर्णाङ्क map_index, भिन्न sensor_range, भिन्न sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	यदि (sensor_reading < sensor_range) {
		यदि ((_obstacle_map_body_frame.distances[map_index] < _data_maxranges[map_index]
		     && sensor_range_cm <= _data_maxranges[map_index])
		    || _obstacle_map_body_frame.distances[map_index] >= _data_maxranges[map_index]) {

			प्रत्यागम सत्य;
		}

	} अन्यथा {
		यदि ((_obstacle_map_body_frame.distances[map_index] >= _data_maxranges[map_index]
		     && sensor_range_cm >= _data_maxranges[map_index])
		    || (_obstacle_map_body_frame.distances[map_index] < _data_maxranges[map_index]
			&& sensor_range_cm == _data_maxranges[map_index])) {

			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

तार्किक
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	यदि (_setpoint_index < 0 || _setpoint_index >= BIN_मोजणी) {
		प्रत्यागम असत्य; // treat out-of-bounds as unsafe
	}

	ध्रुव तार्किक no_data = (_obstacle_map_body_frame.distances[_setpoint_index] == UINT16_MAX);
	ध्रुव तार्किक allow_movement_towards_no_data = _param_cp_go_no_data.get();
	ध्रुव तार्किक fov_at_setpoint = _data_fov[_setpoint_index];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	प्रत्यागम !no_data || (allow_movement_towards_no_data && !fov_at_setpoint);
}

शून्य
CollisionPrevention::_transformSetpoint(ध्रुव Vector2f &setpoint)
{
	ध्रुव भिन्न sp_angle_body_frame = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	ध्रुव भिन्न sp_angle_with_offset_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_frame) -
					       _obstacle_map_body_frame.angle_offset);
	_setpoint_index = floor(sp_angle_with_offset_deg / BIN_SIZE);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, _setpoint_index, _vehicle_yaw);
}

शून्य
CollisionPrevention::_addDistanceSensorData(distance_sensor_s &distance_sensor, ध्रुव Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	भिन्न distance_reading = math::min(distance_sensor.current_distance, distance_sensor.max_distance);

	// negative values indicate out of range but valid measurements.
	यदि (fabsf(distance_sensor.current_distance - -1.f) < FLT_EPSILON && distance_sensor.signal_quality == 0) {
		distance_reading = distance_sensor.max_distance;
	}

	// discard values below min range
	यदि (distance_reading > distance_sensor.min_distance) {
		भिन्न sensor_yaw_body_rad = ObstacleMath::sensor_orientation_to_yaw_offset(static_cast<ObstacleMath::SensorOrientation>
					    (distance_sensor.orientation), distance_sensor.q);
		भिन्न sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		पूर्णाङ्क lower_bound = (पूर्णाङ्क)round((sensor_yaw_body_deg  - math::degrees(distance_sensor.h_fov / 2.0f)) / BIN_SIZE);
		पूर्णाङ्क upper_bound = (पूर्णाङ्क)round((sensor_yaw_body_deg  + math::degrees(distance_sensor.h_fov / 2.0f)) / BIN_SIZE);

		यदि (distance_reading < distance_sensor.max_distance) {
			ObstacleMath::project_distance_on_horizontal_plane(distance_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * distance_sensor.max_distance)); // convert to cm

		प्रति (पूर्णाङ्क bin = lower_bound; bin <= upper_bound; ++bin) {
			पूर्णाङ्क wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_मोजणी);

			यदि (_enterData(wrapped_bin, distance_sensor.max_distance, distance_reading)) {
				_obstacle_map_body_frame.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * distance_reading));
				_data_timestamps[wrapped_bin] = _obstacle_map_body_frame.timestamp;
				_data_maxranges[wrapped_bin] = sensor_range;
				_data_fov[wrapped_bin] = 1;
			}
		}
	}
}

शून्य
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, पूर्णाङ्क &setpoint_index, भिन्न vehicle_yaw_angle_rad)
{
	ध्रुव पूर्णाङ्क guidance_bins = floor(_param_cp_guide_ang.get() / BIN_SIZE);
	ध्रुव पूर्णाङ्क sp_index_original = setpoint_index;
	भिन्न best_cost = 9999.f;
	पूर्णाङ्क new_sp_index = setpoint_index;

	प्रति (पूर्णाङ्क i = sp_index_original - guidance_bins; i <= sp_index_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		ध्रुव पूर्णाङ्क filter_size = 1;
		भिन्न mean_dist = 0;

		प्रति (पूर्णाङ्क j = i - filter_size; j <= i + filter_size; j++) {
			पूर्णाङ्क bin = ObstacleMath::wrap_bin(j, BIN_मोजणी);

			यदि (_obstacle_map_body_frame.distances[bin] == UINT16_MAX) {
				mean_dist += _param_cp_dist.get() * 100.f;

			} अन्यथा {
				mean_dist += _obstacle_map_body_frame.distances[bin];
			}
		}

		ध्रुव पूर्णाङ्क bin = ObstacleMath::wrap_bin(i, BIN_मोजणी);
		mean_dist = mean_dist / (2.f * filter_size + 1.f);
		ध्रुव भिन्न deviation_cost = _param_cp_dist.get() * 50.f * abs(i - sp_index_original);
		ध्रुव भिन्न bin_cost = deviation_cost - mean_dist - _obstacle_map_body_frame.distances[bin];

		यदि (bin_cost < best_cost && _obstacle_map_body_frame.distances[bin] != UINT16_MAX) {
			best_cost = bin_cost;
			new_sp_index = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	यदि (new_sp_index != setpoint_index) {
		भिन्न angle = math::radians((भिन्न)new_sp_index * BIN_SIZE + _obstacle_map_body_frame.angle_offset);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_index = new_sp_index;
	}
}

भिन्न CollisionPrevention::_getObstacleDistance(ध्रुव Vector2f &direction)
{
	भिन्न obstacle_distance = 0.f;
	ध्रुव भिन्न direction_norm = direction.norm();

	यदि (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		ध्रुव भिन्न sp_angle_body_frame = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		ध्रुव भिन्न sp_angle_with_offset_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_frame) - _obstacle_map_body_frame.angle_offset);

		ध्रुव पूर्णाङ्क dir_index = ObstacleMath::get_bin_at_angle(BIN_SIZE, sp_angle_with_offset_deg);
		obstacle_distance   = _obstacle_map_body_frame.distances[dir_index] * 0.01f;
	}

	प्रत्यागम obstacle_distance;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(ध्रुव भिन्न &setpoint_length)
{
	Vector2f new_setpoint{};
	ध्रुव Vector2f normal_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	ध्रुव Vector2f tangential_component = _setpoint_dir - normal_component;

	ध्रुव भिन्न normal_scale = _getScale(_closest_dist);


	ध्रुव भिन्न closest_dist_tangential = _getObstacleDistance(tangential_component);
	ध्रुव भिन्न tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	यदि (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + normal_component * normal_scale) * setpoint_length;

	} अन्यथा {
		new_setpoint = _setpoint_dir * setpoint_length;
	}

	प्रत्यागम new_setpoint;
}

भिन्न
CollisionPrevention::_getScale(ध्रुव भिन्न &reference_distance)
{
	भिन्न scale = (reference_distance - _min_dist_to_keep);
	ध्रुव भिन्न scale_distance = math::max(_min_dist_to_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_distance, 2) : scale;
	scale = math::min(scale, 1.0f);
	प्रत्यागम scale;
}

शून्य CollisionPrevention::_getVelocityCompensationAcceleration(ध्रुव भिन्न vehicle_yaw_angle_rad,
		ध्रुव Vector2f &setpoint_vel,
		ध्रुव hrt_abstime now, भिन्न &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	प्रति (पूर्णाङ्क i = 0; i < BIN_मोजणी; i++) {
		ध्रुव भिन्न max_range = _data_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		भिन्न bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((भिन्न)i * BIN_SIZE + _obstacle_map_body_frame.angle_offset));

		ध्रुव Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		भिन्न bin_distance = _obstacle_map_body_frame.distances[i];

		// only consider bins which are between min and max values
		यदि (bin_distance > _obstacle_map_body_frame.min_distance && bin_distance < UINT16_MAX) {
			ध्रुव भिन्न distance = bin_distance * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			ध्रुव भिन्न curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			भिन्न delay_distance = curr_vel_parallel * _param_cp_delay.get();

			ध्रुव hrt_abstime data_age = now - _data_timestamps[i];

			यदि (distance < max_range) {
				delay_distance += curr_vel_parallel * (data_age * 1e-6f);
			}

			ध्रुव भिन्न stop_distance = distance - _min_dist_to_keep - delay_distance;

			भिन्न curr_acc_vel_constraint;

			यदि (stop_distance >= 0.f) {
				ध्रुव भिन्न max_vel = math::trajectory::computeMaxSpeedFromDistance(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_distance, 0.f);
				curr_acc_vel_constraint = _param_mpc_xy_vel_p_acc.get() * math::min(max_vel - curr_vel_parallel, 0.f);

			} अन्यथा {
				curr_acc_vel_constraint = -1.f * _param_mpc_xy_vel_p_acc.get() * curr_vel_parallel;
			}

			यदि (curr_acc_vel_constraint < vel_comp_accel) {
				vel_comp_accel = curr_acc_vel_constraint;
				vel_comp_accel_dir = bin_direction;
			}
		}
	}
}

शून्य CollisionPrevention::_publishVehicleCmdDoLoiter()
{
	vehicle_command_s command{};
	command.command = vehicle_command_s::VEHICLE_CMD_DO_SET_MODE;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (भिन्न)PX4_CUSTOM_MAIN_MODE_AUTO;
	command.param3 = (भिन्न)PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
	command.target_system = 1;
	command.target_component = 1;
	command.source_system = 1;
	command.source_component = 1;
	command.confirmation = असत्य;
	command.from_external = असत्य;
	command.timestamp = getTime();
	_vehicle_command_pub.publish(command);
}
