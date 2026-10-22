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

#ضمن "CollisionPrevention.hpp"
#ضمن "ObstacleMath.hpp"
#ضمن <px4_platform_common/events.h>
#ضمن <math.h>


کارول نطاق ماتریکس;

CollisionPrevention::CollisionPrevention(ModuleParams *پلار) :
	ModuleParams(پلار)
{
	static_assert(BIN_اندازه >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_اندازه == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	obstacle_map_body_چوکاټ.چوکاټ = obstacle_واټن_s::MAV_چوکاټ_BODY_FRD;
	obstacle_map_body_چوکاټ.increment = BIN_اندازه;
	obstacle_map_body_چوکاټ.min_واټن = UINT16_MAX;

	لكل (uint32_t i = 0 ; i < BIN_شمېر; i++) {
		obstacle_map_body_چوکاټ.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::get_وخت()
{
	أرجع hrt_absolute_وخت();
}

hrt_abstime CollisionPrevention::get_Elapsed_وخت(ثابت hrt_abstime *ptr)
{
	أرجع hrt_absolute_وخت() - *ptr;
}

منطقي CollisionPrevention::is_فعال()
{
	منطقي activated = _param_cp_dist.get() > 0;

	إذا (activated && !was_فعال) {
		وخت_activated = get_وخت();
	}

	was_فعال = activated;
	أرجع activated;
}

فراغ CollisionPrevention::modifySetpoint(Vector2f &setpoint_accel, ثابت Vector2f &setpoint_vel)
{
	إذا (_vehicle_attitude_sub.updated()) {
		vehicle_attitude_s vehicle_attitude;

		إذا (_vehicle_attitude_sub.copy(&vehicle_attitude)) {
			_vehicle_attitude = Quatf(vehicle_attitude.q);
			_vehicle_yaw = Eulerf(_vehicle_attitude).psi();
		}
	}

	//calculate movement constraints based on range data
	ثابت Vector2f original_setpoint = setpoint_accel;
	_updateObstacleMap();
	update_Obstacle_معلومات();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copy_منزل(constraints.original_setpoint);
	setpoint_accel.copy_منزل(constraints.adapted_setpoint);
	constraints.timestamp = get_وخت();
	_constraints_pub.publish(constraints);
}

فراغ CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	لكل (خپلکاری &dist_sens_sub : واټن_sensor_subs) {
		واټن_sensor_s واټن_sensor;

		إذا (dist_sens_sub.update(&واټن_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			إذا ((get_Elapsed_وخت(&واټن_sensor.timestamp) < RANGE_بهیر_TIMEOUT_US) &&
			    (واټن_sensor.orientation != واټن_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (واټن_sensor.orientation != واټن_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				obstacle_map_body_چوکاټ.timestamp = math::max(obstacle_map_body_چوکاټ.timestamp, واټن_sensor.timestamp);
				obstacle_map_body_چوکاټ.max_واټن = math::max(obstacle_map_body_چوکاټ.max_واټن,
									(uint16_t)(واټن_sensor.max_واټن * 100.0f));
				obstacle_map_body_چوکاټ.min_واټن = math::min(obstacle_map_body_چوکاټ.min_واټن,
									(uint16_t)(واټن_sensor.min_واټن * 100.0f));

				جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	إذا (sub_obstacle_واټن.update()) {
		ثابت obstacle_واټن_s &obstacle_واټن = sub_obstacle_واټن.get();

		// Update map with obstacle data if the data is not stale
		إذا (get_Elapsed_وخت(&obstacle_واټن.timestamp) < RANGE_بهیر_TIMEOUT_US && obstacle_واټن.increment > 0.f) {
			//update message description
			obstacle_map_body_چوکاټ.timestamp = math::max(obstacle_map_body_چوکاټ.timestamp, obstacle_واټن.timestamp);
			obstacle_map_body_چوکاټ.max_واټن = math::max(obstacle_map_body_چوکاټ.max_واټن,
								obstacle_واټن.max_واټن);
			obstacle_map_body_چوکاټ.min_واټن = math::min(obstacle_map_body_چوکاټ.min_واټن,
								obstacle_واټن.min_واټن);
			جمع_کړه_Obstacle_Sensor_معلومات(obstacle_واټن, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	obstacle_واټن_fused_pub.publish(obstacle_map_body_چوکاټ);
}

فراغ CollisionPrevention::update_Obstacle_معلومات()
{
	obstacle_معلومات_present = خطأ;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.سېټ_Zero();

	لكل (صحيح i = 0; i < BIN_شمېر; i++) {
		// if the data is stale, reset the bin
		إذا (get_وخت() - معلومات_timestamps[i] > RANGE_بهیر_TIMEOUT_US) {
			obstacle_map_body_چوکاټ.distances[i] = UINT16_MAX;
		}

		عائم angle = wrap_2pi(_vehicle_yaw + math::radians((عائم)i * BIN_اندازه +
				       obstacle_map_body_چوکاټ.angle_دننه_واټن));
		ثابت Vector2f bin_direction = {cosf(angle), sinf(angle)};
		ثابت uint16_t bin_واټن = obstacle_map_body_چوکاټ.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		إذا (bin_واټن < UINT16_MAX
		    && (get_وخت() - obstacle_map_body_چوکاټ.timestamp) < RANGE_بهیر_TIMEOUT_US) {
			obstacle_معلومات_present = ریښتیا;
		}

		إذا (bin_واټن * 0.01f < _closest_dist) {
			_closest_dist = bin_واټن * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

فراغ CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, ثابت Vector2f &setpoint_vel)
{
	ثابت عائم setpoint_اوږدتیا = setpoint_accel.norm();
	min_dist_منزل_keep = math::max(obstacle_map_body_چوکاټ.min_واټن / 100.0f, _param_cp_dist.get());

	ثابت hrt_abstime now = get_وخت();

	ثابت منطقي is_stick_deflected = setpoint_اوږدتیا > 0.001f;

	إذا (obstacle_معلومات_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		عائم vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		إذا (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_اوږدتیا);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} وإلا إذا (!obstacle_معلومات_present) {
		// allow no movement
		setpoint_accel.سېټ_Zero();

		// if distance data is stale, switch to Loiter
		إذا (get_Elapsed_وخت(&وروستی_timeout_warning) > 1_s && get_Elapsed_وخت(&وخت_activated) > 1_s) {
			إذا ((now - obstacle_map_body_چوکاټ.timestamp) > TIMEOUT_HOLD_US &&
			    get_Elapsed_وخت(&وخت_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			وروستی_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
فراغ CollisionPrevention::جمع_کړه_Obstacle_Sensor_معلومات(ثابت obstacle_واټن_s &obstacle, ثابت عائم vehicle_yaw)
{

	عائم vehicle_orientation_deg = math::degrees(vehicle_yaw);


	إذا (obstacle.چوکاټ == obstacle.MAV_چوکاټ_GLOBAL || obstacle.چوکاټ == obstacle.MAV_چوکاټ_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		لكل (صحيح i = 0; i < BIN_شمېر; i++) {
			لكل (صحيح j = 0; (j < 360 / obstacle.increment) && (j < BIN_شمېر); j++) {
				عائم bin_ښکتنی_angle = ObstacleMath::get_ښکتنی_bound_angle(i, obstacle_map_body_چوکاټ.increment,
							obstacle_map_body_چوکاټ.angle_دننه_واټن);
				عائم bin_پاسنی_angle = ObstacleMath::get_ښکتنی_bound_angle(i + 1, obstacle_map_body_چوکاټ.increment,
							obstacle_map_body_چوکاټ.angle_دننه_واټن);
				عائم پیغام_ښکتنی_angle = ObstacleMath::get_ښکتنی_bound_angle(j, obstacle.increment,
							obstacle.angle_دننه_واټن - vehicle_orientation_deg);
				عائم پیغام_پاسنی_angle = ObstacleMath::get_ښکتنی_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_دننه_واټن - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				إذا (bin_ښکتنی_angle > bin_پاسنی_angle) {
					bin_ښکتنی_angle -= 360;
				}

				إذا (پیغام_ښکتنی_angle > پیغام_پاسنی_angle) {
					پیغام_ښکتنی_angle -= 360;
				}

				// Check for overlaps.
				إذا ((پیغام_ښکتنی_angle > bin_ښکتنی_angle && پیغام_ښکتنی_angle < bin_پاسنی_angle) ||
				    (پیغام_پاسنی_angle > bin_ښکتنی_angle && پیغام_پاسنی_angle < bin_پاسنی_angle) ||
				    (پیغام_ښکتنی_angle <= bin_ښکتنی_angle && پیغام_پاسنی_angle >= bin_پاسنی_angle) ||
				    (پیغام_ښکتنی_angle >= bin_ښکتنی_angle && پیغام_پاسنی_angle <= bin_پاسنی_angle)) {
					إذا (obstacle.distances[j] != UINT16_MAX) {
						إذا (enter_معلومات(i, obstacle.max_واټن * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_چوکاټ.distances[i] = obstacle.distances[j];
							معلومات_timestamps[i] = obstacle_map_body_چوکاټ.timestamp;
							معلومات_maxranges[i] = obstacle.max_واټن;
							معلومات_fov[i] = 1;
						}
					}
				}

			}
		}

	} وإلا إذا (obstacle.چوکاټ == obstacle.MAV_چوکاټ_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		لكل (صحيح i = 0; i < BIN_شمېر; i++) {
			لكل (صحيح j = 0; (j < 360 / obstacle.increment) && (j < BIN_شمېر); j++) {
				عائم bin_ښکتنی_angle = ObstacleMath::get_ښکتنی_bound_angle(i, obstacle_map_body_چوکاټ.increment,
							obstacle_map_body_چوکاټ.angle_دننه_واټن);
				عائم bin_پاسنی_angle = ObstacleMath::get_ښکتنی_bound_angle(i + 1, obstacle_map_body_چوکاټ.increment,
							obstacle_map_body_چوکاټ.angle_دننه_واټن);
				عائم پیغام_ښکتنی_angle = ObstacleMath::get_ښکتنی_bound_angle(j, obstacle.increment, obstacle.angle_دننه_واټن);
				عائم پیغام_پاسنی_angle = ObstacleMath::get_ښکتنی_bound_angle(j + 1, obstacle.increment, obstacle.angle_دننه_واټن);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				إذا (bin_ښکتنی_angle > bin_پاسنی_angle) {
					bin_ښکتنی_angle -= 360;
				}

				إذا (پیغام_ښکتنی_angle > پیغام_پاسنی_angle) {
					پیغام_ښکتنی_angle -= 360;
				}

				// Check for overlaps.
				إذا ((پیغام_ښکتنی_angle > bin_ښکتنی_angle && پیغام_ښکتنی_angle < bin_پاسنی_angle) ||
				    (پیغام_پاسنی_angle > bin_ښکتنی_angle && پیغام_پاسنی_angle < bin_پاسنی_angle) ||
				    (پیغام_ښکتنی_angle <= bin_ښکتنی_angle && پیغام_پاسنی_angle >= bin_پاسنی_angle) ||
				    (پیغام_ښکتنی_angle >= bin_ښکتنی_angle && پیغام_پاسنی_angle <= bin_پاسنی_angle)) {
					إذا (obstacle.distances[j] != UINT16_MAX) {

						إذا (enter_معلومات(i, obstacle.max_واټن * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_چوکاټ.distances[i] = obstacle.distances[j];
							معلومات_timestamps[i] = obstacle_map_body_چوکاټ.timestamp;
							معلومات_maxranges[i] = obstacle.max_واټن;
							معلومات_fov[i] = 1;
						}
					}
				}

			}
		}

	} وإلا {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.چوکاټ);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.چوکاټ);
	}
}

منطقي
CollisionPrevention::enter_معلومات(صحيح map_شاخص, عائم sensor_range, عائم sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	إذا (sensor_reading < sensor_range) {
		إذا ((obstacle_map_body_چوکاټ.distances[map_شاخص] < معلومات_maxranges[map_شاخص]
		     && sensor_range_cm <= معلومات_maxranges[map_شاخص])
		    || obstacle_map_body_چوکاټ.distances[map_شاخص] >= معلومات_maxranges[map_شاخص]) {

			أرجع ریښتیا;
		}

	} وإلا {
		إذا ((obstacle_map_body_چوکاټ.distances[map_شاخص] >= معلومات_maxranges[map_شاخص]
		     && sensor_range_cm >= معلومات_maxranges[map_شاخص])
		    || (obstacle_map_body_چوکاټ.distances[map_شاخص] < معلومات_maxranges[map_شاخص]
			&& sensor_range_cm == معلومات_maxranges[map_شاخص])) {

			أرجع ریښتیا;
		}
	}

	أرجع خطأ;
}

منطقي
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	إذا (setpoint_شاخص < 0 || setpoint_شاخص >= BIN_شمېر) {
		أرجع خطأ; // treat out-of-bounds as unsafe
	}

	ثابت منطقي no_معلومات = (obstacle_map_body_چوکاټ.distances[setpoint_شاخص] == UINT16_MAX);
	ثابت منطقي allow_movement_towards_no_معلومات = param_cp_go_no_معلومات.get();
	ثابت منطقي fov_at_setpoint = معلومات_fov[setpoint_شاخص];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	أرجع !no_معلومات || (allow_movement_towards_no_معلومات && !fov_at_setpoint);
}

فراغ
CollisionPrevention::_transformSetpoint(ثابت Vector2f &setpoint)
{
	ثابت عائم sp_angle_body_چوکاټ = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	ثابت عائم sp_angle_with_دننه_واټن_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_چوکاټ) -
					       obstacle_map_body_چوکاټ.angle_دننه_واټن);
	setpoint_شاخص = floor(sp_angle_with_دننه_واټن_deg / BIN_اندازه);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, setpoint_شاخص, _vehicle_yaw);
}

فراغ
CollisionPrevention::جمع_کړه_واټن_Sensor_معلومات(واټن_sensor_s &واټن_sensor, ثابت Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	عائم واټن_reading = math::min(واټن_sensor.current_واټن, واټن_sensor.max_واټن);

	// negative values indicate out of range but valid measurements.
	إذا (fabsf(واټن_sensor.current_واټن - -1.f) < FLT_EPSILON && واټن_sensor.signal_quality == 0) {
		واټن_reading = واټن_sensor.max_واټن;
	}

	// discard values below min range
	إذا (واټن_reading > واټن_sensor.min_واټن) {
		عائم sensor_yaw_body_rad = ObstacleMath::sensor_orientation_منزل_yaw_دننه_واټن(static_cast<ObstacleMath::SensorOrientation>
					    (واټن_sensor.orientation), واټن_sensor.q);
		عائم sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		صحيح ښکتنی_bound = (صحيح)round((sensor_yaw_body_deg  - math::degrees(واټن_sensor.h_fov / 2.0f)) / BIN_اندازه);
		صحيح پاسنی_bound = (صحيح)round((sensor_yaw_body_deg  + math::degrees(واټن_sensor.h_fov / 2.0f)) / BIN_اندازه);

		إذا (واټن_reading < واټن_sensor.max_واټن) {
			ObstacleMath::project_واټن_on_horizontal_plane(واټن_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * واټن_sensor.max_واټن)); // convert to cm

		لكل (صحيح bin = ښکتنی_bound; bin <= پاسنی_bound; ++bin) {
			صحيح wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_شمېر);

			إذا (enter_معلومات(wrapped_bin, واټن_sensor.max_واټن, واټن_reading)) {
				obstacle_map_body_چوکاټ.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * واټن_reading));
				معلومات_timestamps[wrapped_bin] = obstacle_map_body_چوکاټ.timestamp;
				معلومات_maxranges[wrapped_bin] = sensor_range;
				معلومات_fov[wrapped_bin] = 1;
			}
		}
	}
}

فراغ
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, صحيح &setpoint_شاخص_2, عائم vehicle_yaw_angle_rad)
{
	ثابت صحيح guidance_bins = floor(_param_cp_guide_ang.get() / BIN_اندازه);
	ثابت صحيح sp_شاخص_original = setpoint_شاخص_2;
	عائم غوره_لګښت = 9999.f;
	صحيح new_sp_شاخص = setpoint_شاخص_2;

	لكل (صحيح i = sp_شاخص_original - guidance_bins; i <= sp_شاخص_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		ثابت صحيح filter_اندازه = 1;
		عائم منځنۍ_dist = 0;

		لكل (صحيح j = i - filter_اندازه; j <= i + filter_اندازه; j++) {
			صحيح bin = ObstacleMath::wrap_bin(j, BIN_شمېر);

			إذا (obstacle_map_body_چوکاټ.distances[bin] == UINT16_MAX) {
				منځنۍ_dist += _param_cp_dist.get() * 100.f;

			} وإلا {
				منځنۍ_dist += obstacle_map_body_چوکاټ.distances[bin];
			}
		}

		ثابت صحيح bin = ObstacleMath::wrap_bin(i, BIN_شمېر);
		منځنۍ_dist = منځنۍ_dist / (2.f * filter_اندازه + 1.f);
		ثابت عائم deviation_لګښت = _param_cp_dist.get() * 50.f * مطلق_قیمت(i - sp_شاخص_original);
		ثابت عائم bin_لګښت = deviation_لګښت - منځنۍ_dist - obstacle_map_body_چوکاټ.distances[bin];

		إذا (bin_لګښت < غوره_لګښت && obstacle_map_body_چوکاټ.distances[bin] != UINT16_MAX) {
			غوره_لګښت = bin_لګښت;
			new_sp_شاخص = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	إذا (new_sp_شاخص != setpoint_شاخص_2) {
		عائم angle = math::radians((عائم)new_sp_شاخص * BIN_اندازه + obstacle_map_body_چوکاټ.angle_دننه_واټن);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_شاخص_2 = new_sp_شاخص;
	}
}

عائم CollisionPrevention::get_Obstacle_واټن(ثابت Vector2f &direction)
{
	عائم obstacle_واټن = 0.f;
	ثابت عائم direction_norm = direction.norm();

	إذا (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		ثابت عائم sp_angle_body_چوکاټ = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		ثابت عائم sp_angle_with_دننه_واټن_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_چوکاټ) - obstacle_map_body_چوکاټ.angle_دننه_واټن);

		ثابت صحيح dir_شاخص = ObstacleMath::get_bin_at_angle(BIN_اندازه, sp_angle_with_دننه_واټن_deg);
		obstacle_واټن   = obstacle_map_body_چوکاټ.distances[dir_شاخص] * 0.01f;
	}

	أرجع obstacle_واټن;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(ثابت عائم &setpoint_اوږدتیا)
{
	Vector2f new_setpoint{};
	ثابت Vector2f نورمال_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	ثابت Vector2f tangential_component = _setpoint_dir - نورمال_component;

	ثابت عائم نورمال_scale = _getScale(_closest_dist);


	ثابت عائم closest_dist_tangential = get_Obstacle_واټن(tangential_component);
	ثابت عائم tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	إذا (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + نورمال_component * نورمال_scale) * setpoint_اوږدتیا;

	} وإلا {
		new_setpoint = _setpoint_dir * setpoint_اوږدتیا;
	}

	أرجع new_setpoint;
}

عائم
CollisionPrevention::_getScale(ثابت عائم &reference_واټن)
{
	عائم scale = (reference_واټن - min_dist_منزل_keep);
	ثابت عائم scale_واټن = math::max(min_dist_منزل_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_واټن, 2) : scale;
	scale = math::min(scale, 1.0f);
	أرجع scale;
}

فراغ CollisionPrevention::_getVelocityCompensationAcceleration(ثابت عائم vehicle_yaw_angle_rad,
		ثابت Vector2f &setpoint_vel,
		ثابت hrt_abstime now, عائم &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	لكل (صحيح i = 0; i < BIN_شمېر; i++) {
		ثابت عائم max_range = معلومات_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		عائم bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((عائم)i * BIN_اندازه + obstacle_map_body_چوکاټ.angle_دننه_واټن));

		ثابت Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		عائم bin_واټن = obstacle_map_body_چوکاټ.distances[i];

		// only consider bins which are between min and max values
		إذا (bin_واټن > obstacle_map_body_چوکاټ.min_واټن && bin_واټن < UINT16_MAX) {
			ثابت عائم واټن = bin_واټن * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			ثابت عائم curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			عائم delay_واټن = curr_vel_parallel * _param_cp_delay.get();

			ثابت hrt_abstime معلومات_age = now - معلومات_timestamps[i];

			إذا (واټن < max_range) {
				delay_واټن += curr_vel_parallel * (معلومات_age * 1e-6f);
			}

			ثابت عائم stop_واټن = واټن - min_dist_منزل_keep - delay_واټن;

			عائم curr_acc_vel_constraint;

			إذا (stop_واټن >= 0.f) {
				ثابت عائم max_vel = math::trajectory::compute_Max_چټکتیا_From_واټن(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_واټن, 0.f);
				curr_acc_vel_constraint = _param_mpc_xy_vel_p_acc.get() * math::min(max_vel - curr_vel_parallel, 0.f);

			} وإلا {
				curr_acc_vel_constraint = -1.f * _param_mpc_xy_vel_p_acc.get() * curr_vel_parallel;
			}

			إذا (curr_acc_vel_constraint < vel_comp_accel) {
				vel_comp_accel = curr_acc_vel_constraint;
				vel_comp_accel_dir = bin_direction;
			}
		}
	}
}

فراغ CollisionPrevention::_publishVehicleCmdDoLoiter()
{
	vehicle_command_s command{};
	command.command = vehicle_command_s::VEHICLE_CMD_DO_سېټ_ډېرپېښ;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (عائم)PX4_CUSTOM_MAIN_ډېرپېښ_AUTO;
	command.param3 = (عائم)PX4_CUSTOM_SUB_ډېرپېښ_AUTO_LOITER;
	command.موخه_system = 1;
	command.موخه_component = 1;
	command.سرچینه_system = 1;
	command.سرچینه_component = 1;
	command.confirmation = خطأ;
	command.from_external = خطأ;
	command.timestamp = get_وخت();
	_vehicle_command_pub.publish(command);
}
