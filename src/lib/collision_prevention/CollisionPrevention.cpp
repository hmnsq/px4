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


استفاده نطاق ماتریس;

CollisionPrevention::CollisionPrevention(ModuleParams *والد) :
	ModuleParams(والد)
{
	static_assert(BIN_اندازه >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_اندازه == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	obstacle_map_body_قاب.قاب = obstacle_فاصله_s::MAV_قاب_BODY_FRD;
	obstacle_map_body_قاب.increment = BIN_اندازه;
	obstacle_map_body_قاب.min_فاصله = UINT16_MAX;

	لكل (uint32_t i = 0 ; i < BIN_شمار; i++) {
		obstacle_map_body_قاب.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::get_زمان()
{
	أرجع hrt_absolute_زمان();
}

hrt_abstime CollisionPrevention::get_Elapsed_زمان(ثابت hrt_abstime *ptr)
{
	أرجع hrt_absolute_زمان() - *ptr;
}

منطقي CollisionPrevention::is_فعال()
{
	منطقي activated = _param_cp_dist.get() > 0;

	إذا (activated && !was_فعال) {
		زمان_activated = get_زمان();
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
	update_Obstacle_داده();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copy_مقصد(constraints.original_setpoint);
	setpoint_accel.copy_مقصد(constraints.adapted_setpoint);
	constraints.timestamp = get_زمان();
	_constraints_pub.publish(constraints);
}

فراغ CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	لكل (خودکار &dist_sens_sub : فاصله_sensor_subs) {
		فاصله_sensor_s فاصله_sensor;

		إذا (dist_sens_sub.update(&فاصله_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			إذا ((get_Elapsed_زمان(&فاصله_sensor.timestamp) < RANGE_جریان_TIMEOUT_US) &&
			    (فاصله_sensor.orientation != فاصله_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (فاصله_sensor.orientation != فاصله_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				obstacle_map_body_قاب.timestamp = math::max(obstacle_map_body_قاب.timestamp, فاصله_sensor.timestamp);
				obstacle_map_body_قاب.max_فاصله = math::max(obstacle_map_body_قاب.max_فاصله,
									(uint16_t)(فاصله_sensor.max_فاصله * 100.0f));
				obstacle_map_body_قاب.min_فاصله = math::min(obstacle_map_body_قاب.min_فاصله,
									(uint16_t)(فاصله_sensor.min_فاصله * 100.0f));

				جمع_کن_فاصله_Sensor_داده(فاصله_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	إذا (sub_obstacle_فاصله.update()) {
		ثابت obstacle_فاصله_s &obstacle_فاصله = sub_obstacle_فاصله.get();

		// Update map with obstacle data if the data is not stale
		إذا (get_Elapsed_زمان(&obstacle_فاصله.timestamp) < RANGE_جریان_TIMEOUT_US && obstacle_فاصله.increment > 0.f) {
			//update message description
			obstacle_map_body_قاب.timestamp = math::max(obstacle_map_body_قاب.timestamp, obstacle_فاصله.timestamp);
			obstacle_map_body_قاب.max_فاصله = math::max(obstacle_map_body_قاب.max_فاصله,
								obstacle_فاصله.max_فاصله);
			obstacle_map_body_قاب.min_فاصله = math::min(obstacle_map_body_قاب.min_فاصله,
								obstacle_فاصله.min_فاصله);
			جمع_کن_Obstacle_Sensor_داده(obstacle_فاصله, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	obstacle_فاصله_fused_pub.publish(obstacle_map_body_قاب);
}

فراغ CollisionPrevention::update_Obstacle_داده()
{
	obstacle_داده_present = خطأ;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.مجموعه_Zero();

	لكل (صحيح i = 0; i < BIN_شمار; i++) {
		// if the data is stale, reset the bin
		إذا (get_زمان() - داده_timestamps[i] > RANGE_جریان_TIMEOUT_US) {
			obstacle_map_body_قاب.distances[i] = UINT16_MAX;
		}

		عائم angle = wrap_2pi(_vehicle_yaw + math::radians((عائم)i * BIN_اندازه +
				       obstacle_map_body_قاب.angle_جابه_جایی));
		ثابت Vector2f bin_direction = {cosf(angle), sinf(angle)};
		ثابت uint16_t bin_فاصله = obstacle_map_body_قاب.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		إذا (bin_فاصله < UINT16_MAX
		    && (get_زمان() - obstacle_map_body_قاب.timestamp) < RANGE_جریان_TIMEOUT_US) {
			obstacle_داده_present = درست;
		}

		إذا (bin_فاصله * 0.01f < _closest_dist) {
			_closest_dist = bin_فاصله * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

فراغ CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, ثابت Vector2f &setpoint_vel)
{
	ثابت عائم setpoint_درازی = setpoint_accel.norm();
	min_dist_مقصد_keep = math::max(obstacle_map_body_قاب.min_فاصله / 100.0f, _param_cp_dist.get());

	ثابت hrt_abstime now = get_زمان();

	ثابت منطقي is_stick_deflected = setpoint_درازی > 0.001f;

	إذا (obstacle_داده_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		عائم vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		إذا (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_درازی);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} وإلا إذا (!obstacle_داده_present) {
		// allow no movement
		setpoint_accel.مجموعه_Zero();

		// if distance data is stale, switch to Loiter
		إذا (get_Elapsed_زمان(&آخرین_timeout_warning) > 1_s && get_Elapsed_زمان(&زمان_activated) > 1_s) {
			إذا ((now - obstacle_map_body_قاب.timestamp) > TIMEOUT_HOLD_US &&
			    get_Elapsed_زمان(&زمان_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			آخرین_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
فراغ CollisionPrevention::جمع_کن_Obstacle_Sensor_داده(ثابت obstacle_فاصله_s &obstacle, ثابت عائم vehicle_yaw)
{

	عائم vehicle_orientation_deg = math::degrees(vehicle_yaw);


	إذا (obstacle.قاب == obstacle.MAV_قاب_GLOBAL || obstacle.قاب == obstacle.MAV_قاب_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		لكل (صحيح i = 0; i < BIN_شمار; i++) {
			لكل (صحيح j = 0; (j < 360 / obstacle.increment) && (j < BIN_شمار); j++) {
				عائم bin_پایینی_angle = ObstacleMath::get_پایینی_bound_angle(i, obstacle_map_body_قاب.increment,
							obstacle_map_body_قاب.angle_جابه_جایی);
				عائم bin_فوقانی_angle = ObstacleMath::get_پایینی_bound_angle(i + 1, obstacle_map_body_قاب.increment,
							obstacle_map_body_قاب.angle_جابه_جایی);
				عائم پیام_پایینی_angle = ObstacleMath::get_پایینی_bound_angle(j, obstacle.increment,
							obstacle.angle_جابه_جایی - vehicle_orientation_deg);
				عائم پیام_فوقانی_angle = ObstacleMath::get_پایینی_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_جابه_جایی - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				إذا (bin_پایینی_angle > bin_فوقانی_angle) {
					bin_پایینی_angle -= 360;
				}

				إذا (پیام_پایینی_angle > پیام_فوقانی_angle) {
					پیام_پایینی_angle -= 360;
				}

				// Check for overlaps.
				إذا ((پیام_پایینی_angle > bin_پایینی_angle && پیام_پایینی_angle < bin_فوقانی_angle) ||
				    (پیام_فوقانی_angle > bin_پایینی_angle && پیام_فوقانی_angle < bin_فوقانی_angle) ||
				    (پیام_پایینی_angle <= bin_پایینی_angle && پیام_فوقانی_angle >= bin_فوقانی_angle) ||
				    (پیام_پایینی_angle >= bin_پایینی_angle && پیام_فوقانی_angle <= bin_فوقانی_angle)) {
					إذا (obstacle.distances[j] != UINT16_MAX) {
						إذا (enter_داده(i, obstacle.max_فاصله * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_قاب.distances[i] = obstacle.distances[j];
							داده_timestamps[i] = obstacle_map_body_قاب.timestamp;
							داده_maxranges[i] = obstacle.max_فاصله;
							داده_fov[i] = 1;
						}
					}
				}

			}
		}

	} وإلا إذا (obstacle.قاب == obstacle.MAV_قاب_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		لكل (صحيح i = 0; i < BIN_شمار; i++) {
			لكل (صحيح j = 0; (j < 360 / obstacle.increment) && (j < BIN_شمار); j++) {
				عائم bin_پایینی_angle = ObstacleMath::get_پایینی_bound_angle(i, obstacle_map_body_قاب.increment,
							obstacle_map_body_قاب.angle_جابه_جایی);
				عائم bin_فوقانی_angle = ObstacleMath::get_پایینی_bound_angle(i + 1, obstacle_map_body_قاب.increment,
							obstacle_map_body_قاب.angle_جابه_جایی);
				عائم پیام_پایینی_angle = ObstacleMath::get_پایینی_bound_angle(j, obstacle.increment, obstacle.angle_جابه_جایی);
				عائم پیام_فوقانی_angle = ObstacleMath::get_پایینی_bound_angle(j + 1, obstacle.increment, obstacle.angle_جابه_جایی);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				إذا (bin_پایینی_angle > bin_فوقانی_angle) {
					bin_پایینی_angle -= 360;
				}

				إذا (پیام_پایینی_angle > پیام_فوقانی_angle) {
					پیام_پایینی_angle -= 360;
				}

				// Check for overlaps.
				إذا ((پیام_پایینی_angle > bin_پایینی_angle && پیام_پایینی_angle < bin_فوقانی_angle) ||
				    (پیام_فوقانی_angle > bin_پایینی_angle && پیام_فوقانی_angle < bin_فوقانی_angle) ||
				    (پیام_پایینی_angle <= bin_پایینی_angle && پیام_فوقانی_angle >= bin_فوقانی_angle) ||
				    (پیام_پایینی_angle >= bin_پایینی_angle && پیام_فوقانی_angle <= bin_فوقانی_angle)) {
					إذا (obstacle.distances[j] != UINT16_MAX) {

						إذا (enter_داده(i, obstacle.max_فاصله * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_قاب.distances[i] = obstacle.distances[j];
							داده_timestamps[i] = obstacle_map_body_قاب.timestamp;
							داده_maxranges[i] = obstacle.max_فاصله;
							داده_fov[i] = 1;
						}
					}
				}

			}
		}

	} وإلا {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.قاب);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.قاب);
	}
}

منطقي
CollisionPrevention::enter_داده(صحيح map_شاخص, عائم sensor_range, عائم sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	إذا (sensor_reading < sensor_range) {
		إذا ((obstacle_map_body_قاب.distances[map_شاخص] < داده_maxranges[map_شاخص]
		     && sensor_range_cm <= داده_maxranges[map_شاخص])
		    || obstacle_map_body_قاب.distances[map_شاخص] >= داده_maxranges[map_شاخص]) {

			أرجع درست;
		}

	} وإلا {
		إذا ((obstacle_map_body_قاب.distances[map_شاخص] >= داده_maxranges[map_شاخص]
		     && sensor_range_cm >= داده_maxranges[map_شاخص])
		    || (obstacle_map_body_قاب.distances[map_شاخص] < داده_maxranges[map_شاخص]
			&& sensor_range_cm == داده_maxranges[map_شاخص])) {

			أرجع درست;
		}
	}

	أرجع خطأ;
}

منطقي
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	إذا (setpoint_شاخص < 0 || setpoint_شاخص >= BIN_شمار) {
		أرجع خطأ; // treat out-of-bounds as unsafe
	}

	ثابت منطقي no_داده = (obstacle_map_body_قاب.distances[setpoint_شاخص] == UINT16_MAX);
	ثابت منطقي allow_movement_towards_no_داده = param_cp_go_no_داده.get();
	ثابت منطقي fov_at_setpoint = داده_fov[setpoint_شاخص];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	أرجع !no_داده || (allow_movement_towards_no_داده && !fov_at_setpoint);
}

فراغ
CollisionPrevention::_transformSetpoint(ثابت Vector2f &setpoint)
{
	ثابت عائم sp_angle_body_قاب = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	ثابت عائم sp_angle_with_جابه_جایی_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_قاب) -
					       obstacle_map_body_قاب.angle_جابه_جایی);
	setpoint_شاخص = floor(sp_angle_with_جابه_جایی_deg / BIN_اندازه);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, setpoint_شاخص, _vehicle_yaw);
}

فراغ
CollisionPrevention::جمع_کن_فاصله_Sensor_داده(فاصله_sensor_s &فاصله_sensor, ثابت Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	عائم فاصله_reading = math::min(فاصله_sensor.current_فاصله, فاصله_sensor.max_فاصله);

	// negative values indicate out of range but valid measurements.
	إذا (fabsf(فاصله_sensor.current_فاصله - -1.f) < FLT_EPSILON && فاصله_sensor.signal_quality == 0) {
		فاصله_reading = فاصله_sensor.max_فاصله;
	}

	// discard values below min range
	إذا (فاصله_reading > فاصله_sensor.min_فاصله) {
		عائم sensor_yaw_body_rad = ObstacleMath::sensor_orientation_مقصد_yaw_جابه_جایی(static_cast<ObstacleMath::SensorOrientation>
					    (فاصله_sensor.orientation), فاصله_sensor.q);
		عائم sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		صحيح پایینی_bound = (صحيح)round((sensor_yaw_body_deg  - math::degrees(فاصله_sensor.h_fov / 2.0f)) / BIN_اندازه);
		صحيح فوقانی_bound = (صحيح)round((sensor_yaw_body_deg  + math::degrees(فاصله_sensor.h_fov / 2.0f)) / BIN_اندازه);

		إذا (فاصله_reading < فاصله_sensor.max_فاصله) {
			ObstacleMath::project_فاصله_on_horizontal_plane(فاصله_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * فاصله_sensor.max_فاصله)); // convert to cm

		لكل (صحيح bin = پایینی_bound; bin <= فوقانی_bound; ++bin) {
			صحيح wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_شمار);

			إذا (enter_داده(wrapped_bin, فاصله_sensor.max_فاصله, فاصله_reading)) {
				obstacle_map_body_قاب.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * فاصله_reading));
				داده_timestamps[wrapped_bin] = obstacle_map_body_قاب.timestamp;
				داده_maxranges[wrapped_bin] = sensor_range;
				داده_fov[wrapped_bin] = 1;
			}
		}
	}
}

فراغ
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, صحيح &setpoint_شاخص_2, عائم vehicle_yaw_angle_rad)
{
	ثابت صحيح guidance_bins = floor(_param_cp_guide_ang.get() / BIN_اندازه);
	ثابت صحيح sp_شاخص_original = setpoint_شاخص_2;
	عائم بهترین_هزینه = 9999.f;
	صحيح new_sp_شاخص = setpoint_شاخص_2;

	لكل (صحيح i = sp_شاخص_original - guidance_bins; i <= sp_شاخص_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		ثابت صحيح filter_اندازه = 1;
		عائم میانگین_dist = 0;

		لكل (صحيح j = i - filter_اندازه; j <= i + filter_اندازه; j++) {
			صحيح bin = ObstacleMath::wrap_bin(j, BIN_شمار);

			إذا (obstacle_map_body_قاب.distances[bin] == UINT16_MAX) {
				میانگین_dist += _param_cp_dist.get() * 100.f;

			} وإلا {
				میانگین_dist += obstacle_map_body_قاب.distances[bin];
			}
		}

		ثابت صحيح bin = ObstacleMath::wrap_bin(i, BIN_شمار);
		میانگین_dist = میانگین_dist / (2.f * filter_اندازه + 1.f);
		ثابت عائم deviation_هزینه = _param_cp_dist.get() * 50.f * قدر_مطلق(i - sp_شاخص_original);
		ثابت عائم bin_هزینه = deviation_هزینه - میانگین_dist - obstacle_map_body_قاب.distances[bin];

		إذا (bin_هزینه < بهترین_هزینه && obstacle_map_body_قاب.distances[bin] != UINT16_MAX) {
			بهترین_هزینه = bin_هزینه;
			new_sp_شاخص = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	إذا (new_sp_شاخص != setpoint_شاخص_2) {
		عائم angle = math::radians((عائم)new_sp_شاخص * BIN_اندازه + obstacle_map_body_قاب.angle_جابه_جایی);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_شاخص_2 = new_sp_شاخص;
	}
}

عائم CollisionPrevention::get_Obstacle_فاصله(ثابت Vector2f &direction)
{
	عائم obstacle_فاصله = 0.f;
	ثابت عائم direction_norm = direction.norm();

	إذا (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		ثابت عائم sp_angle_body_قاب = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		ثابت عائم sp_angle_with_جابه_جایی_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_قاب) - obstacle_map_body_قاب.angle_جابه_جایی);

		ثابت صحيح dir_شاخص = ObstacleMath::get_bin_at_angle(BIN_اندازه, sp_angle_with_جابه_جایی_deg);
		obstacle_فاصله   = obstacle_map_body_قاب.distances[dir_شاخص] * 0.01f;
	}

	أرجع obstacle_فاصله;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(ثابت عائم &setpoint_درازی)
{
	Vector2f new_setpoint{};
	ثابت Vector2f نرمال_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	ثابت Vector2f tangential_component = _setpoint_dir - نرمال_component;

	ثابت عائم نرمال_scale = _getScale(_closest_dist);


	ثابت عائم closest_dist_tangential = get_Obstacle_فاصله(tangential_component);
	ثابت عائم tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	إذا (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + نرمال_component * نرمال_scale) * setpoint_درازی;

	} وإلا {
		new_setpoint = _setpoint_dir * setpoint_درازی;
	}

	أرجع new_setpoint;
}

عائم
CollisionPrevention::_getScale(ثابت عائم &reference_فاصله)
{
	عائم scale = (reference_فاصله - min_dist_مقصد_keep);
	ثابت عائم scale_فاصله = math::max(min_dist_مقصد_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_فاصله, 2) : scale;
	scale = math::min(scale, 1.0f);
	أرجع scale;
}

فراغ CollisionPrevention::_getVelocityCompensationAcceleration(ثابت عائم vehicle_yaw_angle_rad,
		ثابت Vector2f &setpoint_vel,
		ثابت hrt_abstime now, عائم &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	لكل (صحيح i = 0; i < BIN_شمار; i++) {
		ثابت عائم max_range = داده_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		عائم bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((عائم)i * BIN_اندازه + obstacle_map_body_قاب.angle_جابه_جایی));

		ثابت Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		عائم bin_فاصله = obstacle_map_body_قاب.distances[i];

		// only consider bins which are between min and max values
		إذا (bin_فاصله > obstacle_map_body_قاب.min_فاصله && bin_فاصله < UINT16_MAX) {
			ثابت عائم فاصله = bin_فاصله * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			ثابت عائم curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			عائم delay_فاصله = curr_vel_parallel * _param_cp_delay.get();

			ثابت hrt_abstime داده_age = now - داده_timestamps[i];

			إذا (فاصله < max_range) {
				delay_فاصله += curr_vel_parallel * (داده_age * 1e-6f);
			}

			ثابت عائم stop_فاصله = فاصله - min_dist_مقصد_keep - delay_فاصله;

			عائم curr_acc_vel_constraint;

			إذا (stop_فاصله >= 0.f) {
				ثابت عائم max_vel = math::trajectory::compute_Max_سرعت_From_فاصله(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_فاصله, 0.f);
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
	command.command = vehicle_command_s::VEHICLE_CMD_DO_مجموعه_نما;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (عائم)PX4_CUSTOM_MAIN_نما_AUTO;
	command.param3 = (عائم)PX4_CUSTOM_SUB_نما_AUTO_LOITER;
	command.هدف_system = 1;
	command.هدف_component = 1;
	command.منبع_system = 1;
	command.منبع_component = 1;
	command.confirmation = خطأ;
	command.from_external = خطأ;
	command.timestamp = get_زمان();
	_vehicle_command_pub.publish(command);
}
