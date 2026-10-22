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


ورتوں نطاق میٹرکس;

CollisionPrevention::CollisionPrevention(ModuleParams *ماپا) :
	ModuleParams(ماپا)
{
	static_assert(BIN_ناپ >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_ناپ == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	obstacle_map_body_چوکھٹا.چوکھٹا = obstacle_فاصلہ_s::MAV_چوکھٹا_BODY_FRD;
	obstacle_map_body_چوکھٹا.increment = BIN_ناپ;
	obstacle_map_body_چوکھٹا.min_فاصلہ = UINT16_MAX;

	لكل (uint32_t i = 0 ; i < BIN_گنتی; i++) {
		obstacle_map_body_چوکھٹا.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::get_ویلا()
{
	أرجع hrt_absolute_ویلا();
}

hrt_abstime CollisionPrevention::get_Elapsed_ویلا(ثابت hrt_abstime *ptr)
{
	أرجع hrt_absolute_ویلا() - *ptr;
}

منطقي CollisionPrevention::is_سرگرم()
{
	منطقي activated = _param_cp_dist.get() > 0;

	إذا (activated && !was_سرگرم) {
		ویلا_activated = get_ویلا();
	}

	was_سرگرم = activated;
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
	update_Obstacle_انکڑے();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copy_منزل(constraints.original_setpoint);
	setpoint_accel.copy_منزل(constraints.adapted_setpoint);
	constraints.timestamp = get_ویلا();
	_constraints_pub.publish(constraints);
}

فراغ CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	لكل (خودکار &dist_sens_sub : فاصلہ_sensor_subs) {
		فاصلہ_sensor_s فاصلہ_sensor;

		إذا (dist_sens_sub.update(&فاصلہ_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			إذا ((get_Elapsed_ویلا(&فاصلہ_sensor.timestamp) < RANGE_دھارا_TIMEOUT_US) &&
			    (فاصلہ_sensor.orientation != فاصلہ_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (فاصلہ_sensor.orientation != فاصلہ_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				obstacle_map_body_چوکھٹا.timestamp = math::max(obstacle_map_body_چوکھٹا.timestamp, فاصلہ_sensor.timestamp);
				obstacle_map_body_چوکھٹا.max_فاصلہ = math::max(obstacle_map_body_چوکھٹا.max_فاصلہ,
									(uint16_t)(فاصلہ_sensor.max_فاصلہ * 100.0f));
				obstacle_map_body_چوکھٹا.min_فاصلہ = math::min(obstacle_map_body_چوکھٹا.min_فاصلہ,
									(uint16_t)(فاصلہ_sensor.min_فاصلہ * 100.0f));

				جوڑو_فاصلہ_Sensor_انکڑے(فاصلہ_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	إذا (sub_obstacle_فاصلہ.update()) {
		ثابت obstacle_فاصلہ_s &obstacle_فاصلہ = sub_obstacle_فاصلہ.get();

		// Update map with obstacle data if the data is not stale
		إذا (get_Elapsed_ویلا(&obstacle_فاصلہ.timestamp) < RANGE_دھارا_TIMEOUT_US && obstacle_فاصلہ.increment > 0.f) {
			//update message description
			obstacle_map_body_چوکھٹا.timestamp = math::max(obstacle_map_body_چوکھٹا.timestamp, obstacle_فاصلہ.timestamp);
			obstacle_map_body_چوکھٹا.max_فاصلہ = math::max(obstacle_map_body_چوکھٹا.max_فاصلہ,
								obstacle_فاصلہ.max_فاصلہ);
			obstacle_map_body_چوکھٹا.min_فاصلہ = math::min(obstacle_map_body_چوکھٹا.min_فاصلہ,
								obstacle_فاصلہ.min_فاصلہ);
			جوڑو_Obstacle_Sensor_انکڑے(obstacle_فاصلہ, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	obstacle_فاصلہ_fused_pub.publish(obstacle_map_body_چوکھٹا);
}

فراغ CollisionPrevention::update_Obstacle_انکڑے()
{
	obstacle_انکڑے_present = خطأ;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.سیٹ_Zero();

	لكل (صحيح i = 0; i < BIN_گنتی; i++) {
		// if the data is stale, reset the bin
		إذا (get_ویلا() - انکڑے_timestamps[i] > RANGE_دھارا_TIMEOUT_US) {
			obstacle_map_body_چوکھٹا.distances[i] = UINT16_MAX;
		}

		عائم angle = wrap_2pi(_vehicle_yaw + math::radians((عائم)i * BIN_ناپ +
				       obstacle_map_body_چوکھٹا.angle_وتھ));
		ثابت Vector2f bin_direction = {cosf(angle), sinf(angle)};
		ثابت uint16_t bin_فاصلہ = obstacle_map_body_چوکھٹا.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		إذا (bin_فاصلہ < UINT16_MAX
		    && (get_ویلا() - obstacle_map_body_چوکھٹا.timestamp) < RANGE_دھارا_TIMEOUT_US) {
			obstacle_انکڑے_present = سچ;
		}

		إذا (bin_فاصلہ * 0.01f < _closest_dist) {
			_closest_dist = bin_فاصلہ * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

فراغ CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, ثابت Vector2f &setpoint_vel)
{
	ثابت عائم setpoint_لمائی = setpoint_accel.norm();
	min_dist_منزل_keep = math::max(obstacle_map_body_چوکھٹا.min_فاصلہ / 100.0f, _param_cp_dist.get());

	ثابت hrt_abstime now = get_ویلا();

	ثابت منطقي is_stick_deflected = setpoint_لمائی > 0.001f;

	إذا (obstacle_انکڑے_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		عائم vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		إذا (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_لمائی);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} وإلا إذا (!obstacle_انکڑے_present) {
		// allow no movement
		setpoint_accel.سیٹ_Zero();

		// if distance data is stale, switch to Loiter
		إذا (get_Elapsed_ویلا(&آخری_timeout_warning) > 1_s && get_Elapsed_ویلا(&ویلا_activated) > 1_s) {
			إذا ((now - obstacle_map_body_چوکھٹا.timestamp) > TIMEOUT_HOLD_US &&
			    get_Elapsed_ویلا(&ویلا_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			آخری_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
فراغ CollisionPrevention::جوڑو_Obstacle_Sensor_انکڑے(ثابت obstacle_فاصلہ_s &obstacle, ثابت عائم vehicle_yaw)
{

	عائم vehicle_orientation_deg = math::degrees(vehicle_yaw);


	إذا (obstacle.چوکھٹا == obstacle.MAV_چوکھٹا_GLOBAL || obstacle.چوکھٹا == obstacle.MAV_چوکھٹا_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		لكل (صحيح i = 0; i < BIN_گنتی; i++) {
			لكل (صحيح j = 0; (j < 360 / obstacle.increment) && (j < BIN_گنتی); j++) {
				عائم bin_ہیٹھلا_angle = ObstacleMath::get_ہیٹھلا_bound_angle(i, obstacle_map_body_چوکھٹا.increment,
							obstacle_map_body_چوکھٹا.angle_وتھ);
				عائم bin_اتلا_angle = ObstacleMath::get_ہیٹھلا_bound_angle(i + 1, obstacle_map_body_چوکھٹا.increment,
							obstacle_map_body_چوکھٹا.angle_وتھ);
				عائم پیغام_ہیٹھلا_angle = ObstacleMath::get_ہیٹھلا_bound_angle(j, obstacle.increment,
							obstacle.angle_وتھ - vehicle_orientation_deg);
				عائم پیغام_اتلا_angle = ObstacleMath::get_ہیٹھلا_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_وتھ - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				إذا (bin_ہیٹھلا_angle > bin_اتلا_angle) {
					bin_ہیٹھلا_angle -= 360;
				}

				إذا (پیغام_ہیٹھلا_angle > پیغام_اتلا_angle) {
					پیغام_ہیٹھلا_angle -= 360;
				}

				// Check for overlaps.
				إذا ((پیغام_ہیٹھلا_angle > bin_ہیٹھلا_angle && پیغام_ہیٹھلا_angle < bin_اتلا_angle) ||
				    (پیغام_اتلا_angle > bin_ہیٹھلا_angle && پیغام_اتلا_angle < bin_اتلا_angle) ||
				    (پیغام_ہیٹھلا_angle <= bin_ہیٹھلا_angle && پیغام_اتلا_angle >= bin_اتلا_angle) ||
				    (پیغام_ہیٹھلا_angle >= bin_ہیٹھلا_angle && پیغام_اتلا_angle <= bin_اتلا_angle)) {
					إذا (obstacle.distances[j] != UINT16_MAX) {
						إذا (enter_انکڑے(i, obstacle.max_فاصلہ * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_چوکھٹا.distances[i] = obstacle.distances[j];
							انکڑے_timestamps[i] = obstacle_map_body_چوکھٹا.timestamp;
							انکڑے_maxranges[i] = obstacle.max_فاصلہ;
							انکڑے_fov[i] = 1;
						}
					}
				}

			}
		}

	} وإلا إذا (obstacle.چوکھٹا == obstacle.MAV_چوکھٹا_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		لكل (صحيح i = 0; i < BIN_گنتی; i++) {
			لكل (صحيح j = 0; (j < 360 / obstacle.increment) && (j < BIN_گنتی); j++) {
				عائم bin_ہیٹھلا_angle = ObstacleMath::get_ہیٹھلا_bound_angle(i, obstacle_map_body_چوکھٹا.increment,
							obstacle_map_body_چوکھٹا.angle_وتھ);
				عائم bin_اتلا_angle = ObstacleMath::get_ہیٹھلا_bound_angle(i + 1, obstacle_map_body_چوکھٹا.increment,
							obstacle_map_body_چوکھٹا.angle_وتھ);
				عائم پیغام_ہیٹھلا_angle = ObstacleMath::get_ہیٹھلا_bound_angle(j, obstacle.increment, obstacle.angle_وتھ);
				عائم پیغام_اتلا_angle = ObstacleMath::get_ہیٹھلا_bound_angle(j + 1, obstacle.increment, obstacle.angle_وتھ);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				إذا (bin_ہیٹھلا_angle > bin_اتلا_angle) {
					bin_ہیٹھلا_angle -= 360;
				}

				إذا (پیغام_ہیٹھلا_angle > پیغام_اتلا_angle) {
					پیغام_ہیٹھلا_angle -= 360;
				}

				// Check for overlaps.
				إذا ((پیغام_ہیٹھلا_angle > bin_ہیٹھلا_angle && پیغام_ہیٹھلا_angle < bin_اتلا_angle) ||
				    (پیغام_اتلا_angle > bin_ہیٹھلا_angle && پیغام_اتلا_angle < bin_اتلا_angle) ||
				    (پیغام_ہیٹھلا_angle <= bin_ہیٹھلا_angle && پیغام_اتلا_angle >= bin_اتلا_angle) ||
				    (پیغام_ہیٹھلا_angle >= bin_ہیٹھلا_angle && پیغام_اتلا_angle <= bin_اتلا_angle)) {
					إذا (obstacle.distances[j] != UINT16_MAX) {

						إذا (enter_انکڑے(i, obstacle.max_فاصلہ * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_چوکھٹا.distances[i] = obstacle.distances[j];
							انکڑے_timestamps[i] = obstacle_map_body_چوکھٹا.timestamp;
							انکڑے_maxranges[i] = obstacle.max_فاصلہ;
							انکڑے_fov[i] = 1;
						}
					}
				}

			}
		}

	} وإلا {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.چوکھٹا);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.چوکھٹا);
	}
}

منطقي
CollisionPrevention::enter_انکڑے(صحيح map_اشاریہ, عائم sensor_range, عائم sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	إذا (sensor_reading < sensor_range) {
		إذا ((obstacle_map_body_چوکھٹا.distances[map_اشاریہ] < انکڑے_maxranges[map_اشاریہ]
		     && sensor_range_cm <= انکڑے_maxranges[map_اشاریہ])
		    || obstacle_map_body_چوکھٹا.distances[map_اشاریہ] >= انکڑے_maxranges[map_اشاریہ]) {

			أرجع سچ;
		}

	} وإلا {
		إذا ((obstacle_map_body_چوکھٹا.distances[map_اشاریہ] >= انکڑے_maxranges[map_اشاریہ]
		     && sensor_range_cm >= انکڑے_maxranges[map_اشاریہ])
		    || (obstacle_map_body_چوکھٹا.distances[map_اشاریہ] < انکڑے_maxranges[map_اشاریہ]
			&& sensor_range_cm == انکڑے_maxranges[map_اشاریہ])) {

			أرجع سچ;
		}
	}

	أرجع خطأ;
}

منطقي
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	إذا (setpoint_اشاریہ < 0 || setpoint_اشاریہ >= BIN_گنتی) {
		أرجع خطأ; // treat out-of-bounds as unsafe
	}

	ثابت منطقي no_انکڑے = (obstacle_map_body_چوکھٹا.distances[setpoint_اشاریہ] == UINT16_MAX);
	ثابت منطقي allow_movement_towards_no_انکڑے = param_cp_go_no_انکڑے.get();
	ثابت منطقي fov_at_setpoint = انکڑے_fov[setpoint_اشاریہ];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	أرجع !no_انکڑے || (allow_movement_towards_no_انکڑے && !fov_at_setpoint);
}

فراغ
CollisionPrevention::_transformSetpoint(ثابت Vector2f &setpoint)
{
	ثابت عائم sp_angle_body_چوکھٹا = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	ثابت عائم sp_angle_with_وتھ_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_چوکھٹا) -
					       obstacle_map_body_چوکھٹا.angle_وتھ);
	setpoint_اشاریہ = floor(sp_angle_with_وتھ_deg / BIN_ناپ);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, setpoint_اشاریہ, _vehicle_yaw);
}

فراغ
CollisionPrevention::جوڑو_فاصلہ_Sensor_انکڑے(فاصلہ_sensor_s &فاصلہ_sensor, ثابت Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	عائم فاصلہ_reading = math::min(فاصلہ_sensor.current_فاصلہ, فاصلہ_sensor.max_فاصلہ);

	// negative values indicate out of range but valid measurements.
	إذا (fabsf(فاصلہ_sensor.current_فاصلہ - -1.f) < FLT_EPSILON && فاصلہ_sensor.signal_quality == 0) {
		فاصلہ_reading = فاصلہ_sensor.max_فاصلہ;
	}

	// discard values below min range
	إذا (فاصلہ_reading > فاصلہ_sensor.min_فاصلہ) {
		عائم sensor_yaw_body_rad = ObstacleMath::sensor_orientation_منزل_yaw_وتھ(static_cast<ObstacleMath::SensorOrientation>
					    (فاصلہ_sensor.orientation), فاصلہ_sensor.q);
		عائم sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		صحيح ہیٹھلا_bound = (صحيح)round((sensor_yaw_body_deg  - math::degrees(فاصلہ_sensor.h_fov / 2.0f)) / BIN_ناپ);
		صحيح اتلا_bound = (صحيح)round((sensor_yaw_body_deg  + math::degrees(فاصلہ_sensor.h_fov / 2.0f)) / BIN_ناپ);

		إذا (فاصلہ_reading < فاصلہ_sensor.max_فاصلہ) {
			ObstacleMath::project_فاصلہ_on_horizontal_plane(فاصلہ_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * فاصلہ_sensor.max_فاصلہ)); // convert to cm

		لكل (صحيح bin = ہیٹھلا_bound; bin <= اتلا_bound; ++bin) {
			صحيح wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_گنتی);

			إذا (enter_انکڑے(wrapped_bin, فاصلہ_sensor.max_فاصلہ, فاصلہ_reading)) {
				obstacle_map_body_چوکھٹا.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * فاصلہ_reading));
				انکڑے_timestamps[wrapped_bin] = obstacle_map_body_چوکھٹا.timestamp;
				انکڑے_maxranges[wrapped_bin] = sensor_range;
				انکڑے_fov[wrapped_bin] = 1;
			}
		}
	}
}

فراغ
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, صحيح &setpoint_اشاریہ_2, عائم vehicle_yaw_angle_rad)
{
	ثابت صحيح guidance_bins = floor(_param_cp_guide_ang.get() / BIN_ناپ);
	ثابت صحيح sp_اشاریہ_original = setpoint_اشاریہ_2;
	عائم بہترین_لاگت = 9999.f;
	صحيح new_sp_اشاریہ = setpoint_اشاریہ_2;

	لكل (صحيح i = sp_اشاریہ_original - guidance_bins; i <= sp_اشاریہ_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		ثابت صحيح filter_ناپ = 1;
		عائم اوسط_dist = 0;

		لكل (صحيح j = i - filter_ناپ; j <= i + filter_ناپ; j++) {
			صحيح bin = ObstacleMath::wrap_bin(j, BIN_گنتی);

			إذا (obstacle_map_body_چوکھٹا.distances[bin] == UINT16_MAX) {
				اوسط_dist += _param_cp_dist.get() * 100.f;

			} وإلا {
				اوسط_dist += obstacle_map_body_چوکھٹا.distances[bin];
			}
		}

		ثابت صحيح bin = ObstacleMath::wrap_bin(i, BIN_گنتی);
		اوسط_dist = اوسط_dist / (2.f * filter_ناپ + 1.f);
		ثابت عائم deviation_لاگت = _param_cp_dist.get() * 50.f * مطلق_قدر(i - sp_اشاریہ_original);
		ثابت عائم bin_لاگت = deviation_لاگت - اوسط_dist - obstacle_map_body_چوکھٹا.distances[bin];

		إذا (bin_لاگت < بہترین_لاگت && obstacle_map_body_چوکھٹا.distances[bin] != UINT16_MAX) {
			بہترین_لاگت = bin_لاگت;
			new_sp_اشاریہ = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	إذا (new_sp_اشاریہ != setpoint_اشاریہ_2) {
		عائم angle = math::radians((عائم)new_sp_اشاریہ * BIN_ناپ + obstacle_map_body_چوکھٹا.angle_وتھ);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_اشاریہ_2 = new_sp_اشاریہ;
	}
}

عائم CollisionPrevention::get_Obstacle_فاصلہ(ثابت Vector2f &direction)
{
	عائم obstacle_فاصلہ = 0.f;
	ثابت عائم direction_norm = direction.norm();

	إذا (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		ثابت عائم sp_angle_body_چوکھٹا = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		ثابت عائم sp_angle_with_وتھ_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_چوکھٹا) - obstacle_map_body_چوکھٹا.angle_وتھ);

		ثابت صحيح dir_اشاریہ = ObstacleMath::get_bin_at_angle(BIN_ناپ, sp_angle_with_وتھ_deg);
		obstacle_فاصلہ   = obstacle_map_body_چوکھٹا.distances[dir_اشاریہ] * 0.01f;
	}

	أرجع obstacle_فاصلہ;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(ثابت عائم &setpoint_لمائی)
{
	Vector2f new_setpoint{};
	ثابت Vector2f عام_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	ثابت Vector2f tangential_component = _setpoint_dir - عام_component;

	ثابت عائم عام_scale = _getScale(_closest_dist);


	ثابت عائم closest_dist_tangential = get_Obstacle_فاصلہ(tangential_component);
	ثابت عائم tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	إذا (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + عام_component * عام_scale) * setpoint_لمائی;

	} وإلا {
		new_setpoint = _setpoint_dir * setpoint_لمائی;
	}

	أرجع new_setpoint;
}

عائم
CollisionPrevention::_getScale(ثابت عائم &reference_فاصلہ)
{
	عائم scale = (reference_فاصلہ - min_dist_منزل_keep);
	ثابت عائم scale_فاصلہ = math::max(min_dist_منزل_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_فاصلہ, 2) : scale;
	scale = math::min(scale, 1.0f);
	أرجع scale;
}

فراغ CollisionPrevention::_getVelocityCompensationAcceleration(ثابت عائم vehicle_yaw_angle_rad,
		ثابت Vector2f &setpoint_vel,
		ثابت hrt_abstime now, عائم &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	لكل (صحيح i = 0; i < BIN_گنتی; i++) {
		ثابت عائم max_range = انکڑے_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		عائم bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((عائم)i * BIN_ناپ + obstacle_map_body_چوکھٹا.angle_وتھ));

		ثابت Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		عائم bin_فاصلہ = obstacle_map_body_چوکھٹا.distances[i];

		// only consider bins which are between min and max values
		إذا (bin_فاصلہ > obstacle_map_body_چوکھٹا.min_فاصلہ && bin_فاصلہ < UINT16_MAX) {
			ثابت عائم فاصلہ = bin_فاصلہ * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			ثابت عائم curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			عائم delay_فاصلہ = curr_vel_parallel * _param_cp_delay.get();

			ثابت hrt_abstime انکڑے_age = now - انکڑے_timestamps[i];

			إذا (فاصلہ < max_range) {
				delay_فاصلہ += curr_vel_parallel * (انکڑے_age * 1e-6f);
			}

			ثابت عائم stop_فاصلہ = فاصلہ - min_dist_منزل_keep - delay_فاصلہ;

			عائم curr_acc_vel_constraint;

			إذا (stop_فاصلہ >= 0.f) {
				ثابت عائم max_vel = math::trajectory::compute_Max_رفتار_From_فاصلہ(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_فاصلہ, 0.f);
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
	command.command = vehicle_command_s::VEHICLE_CMD_DO_سیٹ_منوال;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (عائم)PX4_CUSTOM_MAIN_منوال_AUTO;
	command.param3 = (عائم)PX4_CUSTOM_SUB_منوال_AUTO_LOITER;
	command.نشانہ_system = 1;
	command.نشانہ_component = 1;
	command.سروت_system = 1;
	command.سروت_component = 1;
	command.confirmation = خطأ;
	command.from_external = خطأ;
	command.timestamp = get_ویلا();
	_vehicle_command_pub.publish(command);
}
