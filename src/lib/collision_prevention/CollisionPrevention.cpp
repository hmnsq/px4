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

#شامل "CollisionPrevention.hpp"
#شامل "ObstacleMath.hpp"
#شامل <px4_platform_common/events.h>
#شامل <math.h>


استعمال نامخلا میٹرکس;

CollisionPrevention::CollisionPrevention(ModuleParams *والد) :
	ModuleParams(والد)
{
	static_assert(BIN_سائز >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_سائز == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	obstacle_map_body_چوکھٹا.چوکھٹا = obstacle_فاصلہ_s::MAV_چوکھٹا_BODY_FRD;
	obstacle_map_body_چوکھٹا.increment = BIN_سائز;
	obstacle_map_body_چوکھٹا.min_فاصلہ = UINT16_MAX;

	برائے (uint32_t i = 0 ; i < BIN_گنتی; i++) {
		obstacle_map_body_چوکھٹا.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::get_وقت()
{
	واپس hrt_absolute_وقت();
}

hrt_abstime CollisionPrevention::get_Elapsed_وقت(ثابت hrt_abstime *ptr)
{
	واپس hrt_absolute_وقت() - *ptr;
}

منطقی CollisionPrevention::is_فعال()
{
	منطقی activated = _param_cp_dist.get() > 0;

	اگر (activated && !was_فعال) {
		وقت_activated = get_وقت();
	}

	was_فعال = activated;
	واپس activated;
}

خالی CollisionPrevention::modifySetpoint(Vector2f &setpoint_accel, ثابت Vector2f &setpoint_vel)
{
	اگر (_vehicle_attitude_sub.updated()) {
		vehicle_attitude_s vehicle_attitude;

		اگر (_vehicle_attitude_sub.copy(&vehicle_attitude)) {
			_vehicle_attitude = Quatf(vehicle_attitude.q);
			_vehicle_yaw = Eulerf(_vehicle_attitude).psi();
		}
	}

	//calculate movement constraints based on range data
	ثابت Vector2f original_setpoint = setpoint_accel;
	_updateObstacleMap();
	update_Obstacle_ڈیٹا();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copy_مقصد(constraints.original_setpoint);
	setpoint_accel.copy_مقصد(constraints.adapted_setpoint);
	constraints.timestamp = get_وقت();
	_constraints_pub.publish(constraints);
}

خالی CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	برائے (خودکار &dist_sens_sub : فاصلہ_sensor_subs) {
		فاصلہ_sensor_s فاصلہ_sensor;

		اگر (dist_sens_sub.update(&فاصلہ_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			اگر ((get_Elapsed_وقت(&فاصلہ_sensor.timestamp) < RANGE_دھارا_TIMEOUT_US) &&
			    (فاصلہ_sensor.orientation != فاصلہ_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (فاصلہ_sensor.orientation != فاصلہ_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				obstacle_map_body_چوکھٹا.timestamp = math::max(obstacle_map_body_چوکھٹا.timestamp, فاصلہ_sensor.timestamp);
				obstacle_map_body_چوکھٹا.max_فاصلہ = math::max(obstacle_map_body_چوکھٹا.max_فاصلہ,
									(uint16_t)(فاصلہ_sensor.max_فاصلہ * 100.0f));
				obstacle_map_body_چوکھٹا.min_فاصلہ = math::min(obstacle_map_body_چوکھٹا.min_فاصلہ,
									(uint16_t)(فاصلہ_sensor.min_فاصلہ * 100.0f));

				جوڑو_فاصلہ_Sensor_ڈیٹا(فاصلہ_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	اگر (sub_obstacle_فاصلہ.update()) {
		ثابت obstacle_فاصلہ_s &obstacle_فاصلہ = sub_obstacle_فاصلہ.get();

		// Update map with obstacle data if the data is not stale
		اگر (get_Elapsed_وقت(&obstacle_فاصلہ.timestamp) < RANGE_دھارا_TIMEOUT_US && obstacle_فاصلہ.increment > 0.f) {
			//update message description
			obstacle_map_body_چوکھٹا.timestamp = math::max(obstacle_map_body_چوکھٹا.timestamp, obstacle_فاصلہ.timestamp);
			obstacle_map_body_چوکھٹا.max_فاصلہ = math::max(obstacle_map_body_چوکھٹا.max_فاصلہ,
								obstacle_فاصلہ.max_فاصلہ);
			obstacle_map_body_چوکھٹا.min_فاصلہ = math::min(obstacle_map_body_چوکھٹا.min_فاصلہ,
								obstacle_فاصلہ.min_فاصلہ);
			جوڑو_Obstacle_Sensor_ڈیٹا(obstacle_فاصلہ, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	obstacle_فاصلہ_fused_pub.publish(obstacle_map_body_چوکھٹا);
}

خالی CollisionPrevention::update_Obstacle_ڈیٹا()
{
	obstacle_ڈیٹا_present = جھوٹ;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.سیٹ_Zero();

	برائے (صحیح i = 0; i < BIN_گنتی; i++) {
		// if the data is stale, reset the bin
		اگر (get_وقت() - ڈیٹا_timestamps[i] > RANGE_دھارا_TIMEOUT_US) {
			obstacle_map_body_چوکھٹا.distances[i] = UINT16_MAX;
		}

		اعشاری angle = wrap_2pi(_vehicle_yaw + math::radians((اعشاری)i * BIN_سائز +
				       obstacle_map_body_چوکھٹا.angle_انحراف));
		ثابت Vector2f bin_direction = {cosf(angle), sinf(angle)};
		ثابت uint16_t bin_فاصلہ = obstacle_map_body_چوکھٹا.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		اگر (bin_فاصلہ < UINT16_MAX
		    && (get_وقت() - obstacle_map_body_چوکھٹا.timestamp) < RANGE_دھارا_TIMEOUT_US) {
			obstacle_ڈیٹا_present = سچ;
		}

		اگر (bin_فاصلہ * 0.01f < _closest_dist) {
			_closest_dist = bin_فاصلہ * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

خالی CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, ثابت Vector2f &setpoint_vel)
{
	ثابت اعشاری setpoint_لمبائی = setpoint_accel.norm();
	min_dist_مقصد_keep = math::max(obstacle_map_body_چوکھٹا.min_فاصلہ / 100.0f, _param_cp_dist.get());

	ثابت hrt_abstime now = get_وقت();

	ثابت منطقی is_stick_deflected = setpoint_لمبائی > 0.001f;

	اگر (obstacle_ڈیٹا_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		اعشاری vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		اگر (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_لمبائی);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} ورنہ اگر (!obstacle_ڈیٹا_present) {
		// allow no movement
		setpoint_accel.سیٹ_Zero();

		// if distance data is stale, switch to Loiter
		اگر (get_Elapsed_وقت(&آخری_timeout_warning) > 1_s && get_Elapsed_وقت(&وقت_activated) > 1_s) {
			اگر ((now - obstacle_map_body_چوکھٹا.timestamp) > TIMEOUT_HOLD_US &&
			    get_Elapsed_وقت(&وقت_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			آخری_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
خالی CollisionPrevention::جوڑو_Obstacle_Sensor_ڈیٹا(ثابت obstacle_فاصلہ_s &obstacle, ثابت اعشاری vehicle_yaw)
{

	اعشاری vehicle_orientation_deg = math::degrees(vehicle_yaw);


	اگر (obstacle.چوکھٹا == obstacle.MAV_چوکھٹا_GLOBAL || obstacle.چوکھٹا == obstacle.MAV_چوکھٹا_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		برائے (صحیح i = 0; i < BIN_گنتی; i++) {
			برائے (صحیح j = 0; (j < 360 / obstacle.increment) && (j < BIN_گنتی); j++) {
				اعشاری bin_زیریں_angle = ObstacleMath::get_زیریں_bound_angle(i, obstacle_map_body_چوکھٹا.increment,
							obstacle_map_body_چوکھٹا.angle_انحراف);
				اعشاری bin_بالائی_angle = ObstacleMath::get_زیریں_bound_angle(i + 1, obstacle_map_body_چوکھٹا.increment,
							obstacle_map_body_چوکھٹا.angle_انحراف);
				اعشاری پیغام_زیریں_angle = ObstacleMath::get_زیریں_bound_angle(j, obstacle.increment,
							obstacle.angle_انحراف - vehicle_orientation_deg);
				اعشاری پیغام_بالائی_angle = ObstacleMath::get_زیریں_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_انحراف - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				اگر (bin_زیریں_angle > bin_بالائی_angle) {
					bin_زیریں_angle -= 360;
				}

				اگر (پیغام_زیریں_angle > پیغام_بالائی_angle) {
					پیغام_زیریں_angle -= 360;
				}

				// Check for overlaps.
				اگر ((پیغام_زیریں_angle > bin_زیریں_angle && پیغام_زیریں_angle < bin_بالائی_angle) ||
				    (پیغام_بالائی_angle > bin_زیریں_angle && پیغام_بالائی_angle < bin_بالائی_angle) ||
				    (پیغام_زیریں_angle <= bin_زیریں_angle && پیغام_بالائی_angle >= bin_بالائی_angle) ||
				    (پیغام_زیریں_angle >= bin_زیریں_angle && پیغام_بالائی_angle <= bin_بالائی_angle)) {
					اگر (obstacle.distances[j] != UINT16_MAX) {
						اگر (enter_ڈیٹا(i, obstacle.max_فاصلہ * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_چوکھٹا.distances[i] = obstacle.distances[j];
							ڈیٹا_timestamps[i] = obstacle_map_body_چوکھٹا.timestamp;
							ڈیٹا_maxranges[i] = obstacle.max_فاصلہ;
							ڈیٹا_fov[i] = 1;
						}
					}
				}

			}
		}

	} ورنہ اگر (obstacle.چوکھٹا == obstacle.MAV_چوکھٹا_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		برائے (صحیح i = 0; i < BIN_گنتی; i++) {
			برائے (صحیح j = 0; (j < 360 / obstacle.increment) && (j < BIN_گنتی); j++) {
				اعشاری bin_زیریں_angle = ObstacleMath::get_زیریں_bound_angle(i, obstacle_map_body_چوکھٹا.increment,
							obstacle_map_body_چوکھٹا.angle_انحراف);
				اعشاری bin_بالائی_angle = ObstacleMath::get_زیریں_bound_angle(i + 1, obstacle_map_body_چوکھٹا.increment,
							obstacle_map_body_چوکھٹا.angle_انحراف);
				اعشاری پیغام_زیریں_angle = ObstacleMath::get_زیریں_bound_angle(j, obstacle.increment, obstacle.angle_انحراف);
				اعشاری پیغام_بالائی_angle = ObstacleMath::get_زیریں_bound_angle(j + 1, obstacle.increment, obstacle.angle_انحراف);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				اگر (bin_زیریں_angle > bin_بالائی_angle) {
					bin_زیریں_angle -= 360;
				}

				اگر (پیغام_زیریں_angle > پیغام_بالائی_angle) {
					پیغام_زیریں_angle -= 360;
				}

				// Check for overlaps.
				اگر ((پیغام_زیریں_angle > bin_زیریں_angle && پیغام_زیریں_angle < bin_بالائی_angle) ||
				    (پیغام_بالائی_angle > bin_زیریں_angle && پیغام_بالائی_angle < bin_بالائی_angle) ||
				    (پیغام_زیریں_angle <= bin_زیریں_angle && پیغام_بالائی_angle >= bin_بالائی_angle) ||
				    (پیغام_زیریں_angle >= bin_زیریں_angle && پیغام_بالائی_angle <= bin_بالائی_angle)) {
					اگر (obstacle.distances[j] != UINT16_MAX) {

						اگر (enter_ڈیٹا(i, obstacle.max_فاصلہ * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_چوکھٹا.distances[i] = obstacle.distances[j];
							ڈیٹا_timestamps[i] = obstacle_map_body_چوکھٹا.timestamp;
							ڈیٹا_maxranges[i] = obstacle.max_فاصلہ;
							ڈیٹا_fov[i] = 1;
						}
					}
				}

			}
		}

	} ورنہ {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.چوکھٹا);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.چوکھٹا);
	}
}

منطقی
CollisionPrevention::enter_ڈیٹا(صحیح map_اشاریہ, اعشاری sensor_range, اعشاری sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	اگر (sensor_reading < sensor_range) {
		اگر ((obstacle_map_body_چوکھٹا.distances[map_اشاریہ] < ڈیٹا_maxranges[map_اشاریہ]
		     && sensor_range_cm <= ڈیٹا_maxranges[map_اشاریہ])
		    || obstacle_map_body_چوکھٹا.distances[map_اشاریہ] >= ڈیٹا_maxranges[map_اشاریہ]) {

			واپس سچ;
		}

	} ورنہ {
		اگر ((obstacle_map_body_چوکھٹا.distances[map_اشاریہ] >= ڈیٹا_maxranges[map_اشاریہ]
		     && sensor_range_cm >= ڈیٹا_maxranges[map_اشاریہ])
		    || (obstacle_map_body_چوکھٹا.distances[map_اشاریہ] < ڈیٹا_maxranges[map_اشاریہ]
			&& sensor_range_cm == ڈیٹا_maxranges[map_اشاریہ])) {

			واپس سچ;
		}
	}

	واپس جھوٹ;
}

منطقی
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	اگر (setpoint_اشاریہ < 0 || setpoint_اشاریہ >= BIN_گنتی) {
		واپس جھوٹ; // treat out-of-bounds as unsafe
	}

	ثابت منطقی no_ڈیٹا = (obstacle_map_body_چوکھٹا.distances[setpoint_اشاریہ] == UINT16_MAX);
	ثابت منطقی allow_movement_towards_no_ڈیٹا = param_cp_go_no_ڈیٹا.get();
	ثابت منطقی fov_at_setpoint = ڈیٹا_fov[setpoint_اشاریہ];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	واپس !no_ڈیٹا || (allow_movement_towards_no_ڈیٹا && !fov_at_setpoint);
}

خالی
CollisionPrevention::_transformSetpoint(ثابت Vector2f &setpoint)
{
	ثابت اعشاری sp_angle_body_چوکھٹا = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	ثابت اعشاری sp_angle_with_انحراف_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_چوکھٹا) -
					       obstacle_map_body_چوکھٹا.angle_انحراف);
	setpoint_اشاریہ = floor(sp_angle_with_انحراف_deg / BIN_سائز);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, setpoint_اشاریہ, _vehicle_yaw);
}

خالی
CollisionPrevention::جوڑو_فاصلہ_Sensor_ڈیٹا(فاصلہ_sensor_s &فاصلہ_sensor, ثابت Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	اعشاری فاصلہ_reading = math::min(فاصلہ_sensor.current_فاصلہ, فاصلہ_sensor.max_فاصلہ);

	// negative values indicate out of range but valid measurements.
	اگر (fabsf(فاصلہ_sensor.current_فاصلہ - -1.f) < FLT_EPSILON && فاصلہ_sensor.signal_quality == 0) {
		فاصلہ_reading = فاصلہ_sensor.max_فاصلہ;
	}

	// discard values below min range
	اگر (فاصلہ_reading > فاصلہ_sensor.min_فاصلہ) {
		اعشاری sensor_yaw_body_rad = ObstacleMath::sensor_orientation_مقصد_yaw_انحراف(static_cast<ObstacleMath::SensorOrientation>
					    (فاصلہ_sensor.orientation), فاصلہ_sensor.q);
		اعشاری sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		صحیح زیریں_bound = (صحیح)round((sensor_yaw_body_deg  - math::degrees(فاصلہ_sensor.h_fov / 2.0f)) / BIN_سائز);
		صحیح بالائی_bound = (صحیح)round((sensor_yaw_body_deg  + math::degrees(فاصلہ_sensor.h_fov / 2.0f)) / BIN_سائز);

		اگر (فاصلہ_reading < فاصلہ_sensor.max_فاصلہ) {
			ObstacleMath::project_فاصلہ_on_horizontal_plane(فاصلہ_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * فاصلہ_sensor.max_فاصلہ)); // convert to cm

		برائے (صحیح bin = زیریں_bound; bin <= بالائی_bound; ++bin) {
			صحیح wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_گنتی);

			اگر (enter_ڈیٹا(wrapped_bin, فاصلہ_sensor.max_فاصلہ, فاصلہ_reading)) {
				obstacle_map_body_چوکھٹا.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * فاصلہ_reading));
				ڈیٹا_timestamps[wrapped_bin] = obstacle_map_body_چوکھٹا.timestamp;
				ڈیٹا_maxranges[wrapped_bin] = sensor_range;
				ڈیٹا_fov[wrapped_bin] = 1;
			}
		}
	}
}

خالی
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, صحیح &setpoint_اشاریہ_2, اعشاری vehicle_yaw_angle_rad)
{
	ثابت صحیح guidance_bins = floor(_param_cp_guide_ang.get() / BIN_سائز);
	ثابت صحیح sp_اشاریہ_original = setpoint_اشاریہ_2;
	اعشاری بہترین_لاگت = 9999.f;
	صحیح new_sp_اشاریہ = setpoint_اشاریہ_2;

	برائے (صحیح i = sp_اشاریہ_original - guidance_bins; i <= sp_اشاریہ_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		ثابت صحیح filter_سائز = 1;
		اعشاری اوسط_dist = 0;

		برائے (صحیح j = i - filter_سائز; j <= i + filter_سائز; j++) {
			صحیح bin = ObstacleMath::wrap_bin(j, BIN_گنتی);

			اگر (obstacle_map_body_چوکھٹا.distances[bin] == UINT16_MAX) {
				اوسط_dist += _param_cp_dist.get() * 100.f;

			} ورنہ {
				اوسط_dist += obstacle_map_body_چوکھٹا.distances[bin];
			}
		}

		ثابت صحیح bin = ObstacleMath::wrap_bin(i, BIN_گنتی);
		اوسط_dist = اوسط_dist / (2.f * filter_سائز + 1.f);
		ثابت اعشاری deviation_لاگت = _param_cp_dist.get() * 50.f * مطلق_قدر(i - sp_اشاریہ_original);
		ثابت اعشاری bin_لاگت = deviation_لاگت - اوسط_dist - obstacle_map_body_چوکھٹا.distances[bin];

		اگر (bin_لاگت < بہترین_لاگت && obstacle_map_body_چوکھٹا.distances[bin] != UINT16_MAX) {
			بہترین_لاگت = bin_لاگت;
			new_sp_اشاریہ = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	اگر (new_sp_اشاریہ != setpoint_اشاریہ_2) {
		اعشاری angle = math::radians((اعشاری)new_sp_اشاریہ * BIN_سائز + obstacle_map_body_چوکھٹا.angle_انحراف);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_اشاریہ_2 = new_sp_اشاریہ;
	}
}

اعشاری CollisionPrevention::get_Obstacle_فاصلہ(ثابت Vector2f &direction)
{
	اعشاری obstacle_فاصلہ = 0.f;
	ثابت اعشاری direction_norm = direction.norm();

	اگر (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		ثابت اعشاری sp_angle_body_چوکھٹا = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		ثابت اعشاری sp_angle_with_انحراف_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_چوکھٹا) - obstacle_map_body_چوکھٹا.angle_انحراف);

		ثابت صحیح dir_اشاریہ = ObstacleMath::get_bin_at_angle(BIN_سائز, sp_angle_with_انحراف_deg);
		obstacle_فاصلہ   = obstacle_map_body_چوکھٹا.distances[dir_اشاریہ] * 0.01f;
	}

	واپس obstacle_فاصلہ;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(ثابت اعشاری &setpoint_لمبائی)
{
	Vector2f new_setpoint{};
	ثابت Vector2f معمول_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	ثابت Vector2f tangential_component = _setpoint_dir - معمول_component;

	ثابت اعشاری معمول_scale = _getScale(_closest_dist);


	ثابت اعشاری closest_dist_tangential = get_Obstacle_فاصلہ(tangential_component);
	ثابت اعشاری tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	اگر (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + معمول_component * معمول_scale) * setpoint_لمبائی;

	} ورنہ {
		new_setpoint = _setpoint_dir * setpoint_لمبائی;
	}

	واپس new_setpoint;
}

اعشاری
CollisionPrevention::_getScale(ثابت اعشاری &reference_فاصلہ)
{
	اعشاری scale = (reference_فاصلہ - min_dist_مقصد_keep);
	ثابت اعشاری scale_فاصلہ = math::max(min_dist_مقصد_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_فاصلہ, 2) : scale;
	scale = math::min(scale, 1.0f);
	واپس scale;
}

خالی CollisionPrevention::_getVelocityCompensationAcceleration(ثابت اعشاری vehicle_yaw_angle_rad,
		ثابت Vector2f &setpoint_vel,
		ثابت hrt_abstime now, اعشاری &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	برائے (صحیح i = 0; i < BIN_گنتی; i++) {
		ثابت اعشاری max_range = ڈیٹا_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		اعشاری bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((اعشاری)i * BIN_سائز + obstacle_map_body_چوکھٹا.angle_انحراف));

		ثابت Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		اعشاری bin_فاصلہ = obstacle_map_body_چوکھٹا.distances[i];

		// only consider bins which are between min and max values
		اگر (bin_فاصلہ > obstacle_map_body_چوکھٹا.min_فاصلہ && bin_فاصلہ < UINT16_MAX) {
			ثابت اعشاری فاصلہ = bin_فاصلہ * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			ثابت اعشاری curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			اعشاری delay_فاصلہ = curr_vel_parallel * _param_cp_delay.get();

			ثابت hrt_abstime ڈیٹا_age = now - ڈیٹا_timestamps[i];

			اگر (فاصلہ < max_range) {
				delay_فاصلہ += curr_vel_parallel * (ڈیٹا_age * 1e-6f);
			}

			ثابت اعشاری stop_فاصلہ = فاصلہ - min_dist_مقصد_keep - delay_فاصلہ;

			اعشاری curr_acc_vel_constraint;

			اگر (stop_فاصلہ >= 0.f) {
				ثابت اعشاری max_vel = math::trajectory::compute_Max_رفتار_From_فاصلہ(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_فاصلہ, 0.f);
				curr_acc_vel_constraint = _param_mpc_xy_vel_p_acc.get() * math::min(max_vel - curr_vel_parallel, 0.f);

			} ورنہ {
				curr_acc_vel_constraint = -1.f * _param_mpc_xy_vel_p_acc.get() * curr_vel_parallel;
			}

			اگر (curr_acc_vel_constraint < vel_comp_accel) {
				vel_comp_accel = curr_acc_vel_constraint;
				vel_comp_accel_dir = bin_direction;
			}
		}
	}
}

خالی CollisionPrevention::_publishVehicleCmdDoLoiter()
{
	vehicle_command_s command{};
	command.command = vehicle_command_s::VEHICLE_CMD_DO_سیٹ_منوال;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (اعشاری)PX4_CUSTOM_MAIN_منوال_AUTO;
	command.param3 = (اعشاری)PX4_CUSTOM_SUB_منوال_AUTO_LOITER;
	command.ہدف_system = 1;
	command.ہدف_component = 1;
	command.ماخذ_system = 1;
	command.ماخذ_component = 1;
	command.confirmation = جھوٹ;
	command.from_external = جھوٹ;
	command.timestamp = get_وقت();
	_vehicle_command_pub.publish(command);
}
