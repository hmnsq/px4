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


استعمال نطاق ميٽرڪس;

CollisionPrevention::CollisionPrevention(ModuleParams *والد) :
	ModuleParams(والد)
{
	static_assert(BIN_سائيز >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_سائيز == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	obstacle_map_body_چوکٺو.چوکٺو = obstacle_مفاصلو_s::MAV_چوکٺو_BODY_FRD;
	obstacle_map_body_چوکٺو.increment = BIN_سائيز;
	obstacle_map_body_چوکٺو.min_مفاصلو = UINT16_MAX;

	لكل (uint32_t i = 0 ; i < BIN_ڳڻپ; i++) {
		obstacle_map_body_چوکٺو.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::get_وقت()
{
	أرجع hrt_absolute_وقت();
}

hrt_abstime CollisionPrevention::get_Elapsed_وقت(ثابت hrt_abstime *ptr)
{
	أرجع hrt_absolute_وقت() - *ptr;
}

منطقي CollisionPrevention::is_سرگرم()
{
	منطقي activated = _param_cp_dist.get() > 0;

	إذا (activated && !was_سرگرم) {
		وقت_activated = get_وقت();
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
	update_Obstacle_معلومات();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copy_منزل(constraints.original_setpoint);
	setpoint_accel.copy_منزل(constraints.adapted_setpoint);
	constraints.timestamp = get_وقت();
	_constraints_pub.publish(constraints);
}

فراغ CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	لكل (خودڪار &dist_sens_sub : مفاصلو_sensor_subs) {
		مفاصلو_sensor_s مفاصلو_sensor;

		إذا (dist_sens_sub.update(&مفاصلو_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			إذا ((get_Elapsed_وقت(&مفاصلو_sensor.timestamp) < RANGE_وهڪرو_TIMEOUT_US) &&
			    (مفاصلو_sensor.orientation != مفاصلو_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (مفاصلو_sensor.orientation != مفاصلو_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				obstacle_map_body_چوکٺو.timestamp = math::max(obstacle_map_body_چوکٺو.timestamp, مفاصلو_sensor.timestamp);
				obstacle_map_body_چوکٺو.max_مفاصلو = math::max(obstacle_map_body_چوکٺو.max_مفاصلو,
									(uint16_t)(مفاصلو_sensor.max_مفاصلو * 100.0f));
				obstacle_map_body_چوکٺو.min_مفاصلو = math::min(obstacle_map_body_چوکٺو.min_مفاصلو,
									(uint16_t)(مفاصلو_sensor.min_مفاصلو * 100.0f));

				جوڙيو_مفاصلو_Sensor_معلومات(مفاصلو_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	إذا (sub_obstacle_مفاصلو.update()) {
		ثابت obstacle_مفاصلو_s &obstacle_مفاصلو = sub_obstacle_مفاصلو.get();

		// Update map with obstacle data if the data is not stale
		إذا (get_Elapsed_وقت(&obstacle_مفاصلو.timestamp) < RANGE_وهڪرو_TIMEOUT_US && obstacle_مفاصلو.increment > 0.f) {
			//update message description
			obstacle_map_body_چوکٺو.timestamp = math::max(obstacle_map_body_چوکٺو.timestamp, obstacle_مفاصلو.timestamp);
			obstacle_map_body_چوکٺو.max_مفاصلو = math::max(obstacle_map_body_چوکٺو.max_مفاصلو,
								obstacle_مفاصلو.max_مفاصلو);
			obstacle_map_body_چوکٺو.min_مفاصلو = math::min(obstacle_map_body_چوکٺو.min_مفاصلو,
								obstacle_مفاصلو.min_مفاصلو);
			جوڙيو_Obstacle_Sensor_معلومات(obstacle_مفاصلو, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	obstacle_مفاصلو_fused_pub.publish(obstacle_map_body_چوکٺو);
}

فراغ CollisionPrevention::update_Obstacle_معلومات()
{
	obstacle_معلومات_present = خطأ;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.سيٽ_Zero();

	لكل (صحيح i = 0; i < BIN_ڳڻپ; i++) {
		// if the data is stale, reset the bin
		إذا (get_وقت() - معلومات_timestamps[i] > RANGE_وهڪرو_TIMEOUT_US) {
			obstacle_map_body_چوکٺو.distances[i] = UINT16_MAX;
		}

		عائم angle = wrap_2pi(_vehicle_yaw + math::radians((عائم)i * BIN_سائيز +
				       obstacle_map_body_چوکٺو.angle_وٿي));
		ثابت Vector2f bin_direction = {cosf(angle), sinf(angle)};
		ثابت uint16_t bin_مفاصلو = obstacle_map_body_چوکٺو.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		إذا (bin_مفاصلو < UINT16_MAX
		    && (get_وقت() - obstacle_map_body_چوکٺو.timestamp) < RANGE_وهڪرو_TIMEOUT_US) {
			obstacle_معلومات_present = سچ;
		}

		إذا (bin_مفاصلو * 0.01f < _closest_dist) {
			_closest_dist = bin_مفاصلو * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

فراغ CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, ثابت Vector2f &setpoint_vel)
{
	ثابت عائم setpoint_ڊيگهائي = setpoint_accel.norm();
	min_dist_منزل_keep = math::max(obstacle_map_body_چوکٺو.min_مفاصلو / 100.0f, _param_cp_dist.get());

	ثابت hrt_abstime now = get_وقت();

	ثابت منطقي is_stick_deflected = setpoint_ڊيگهائي > 0.001f;

	إذا (obstacle_معلومات_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		عائم vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		إذا (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_ڊيگهائي);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} وإلا إذا (!obstacle_معلومات_present) {
		// allow no movement
		setpoint_accel.سيٽ_Zero();

		// if distance data is stale, switch to Loiter
		إذا (get_Elapsed_وقت(&آخري_timeout_warning) > 1_s && get_Elapsed_وقت(&وقت_activated) > 1_s) {
			إذا ((now - obstacle_map_body_چوکٺو.timestamp) > TIMEOUT_HOLD_US &&
			    get_Elapsed_وقت(&وقت_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			آخري_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
فراغ CollisionPrevention::جوڙيو_Obstacle_Sensor_معلومات(ثابت obstacle_مفاصلو_s &obstacle, ثابت عائم vehicle_yaw)
{

	عائم vehicle_orientation_deg = math::degrees(vehicle_yaw);


	إذا (obstacle.چوکٺو == obstacle.MAV_چوکٺو_GLOBAL || obstacle.چوکٺو == obstacle.MAV_چوکٺو_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		لكل (صحيح i = 0; i < BIN_ڳڻپ; i++) {
			لكل (صحيح j = 0; (j < 360 / obstacle.increment) && (j < BIN_ڳڻپ); j++) {
				عائم bin_هيٺاهون_angle = ObstacleMath::get_هيٺاهون_bound_angle(i, obstacle_map_body_چوکٺو.increment,
							obstacle_map_body_چوکٺو.angle_وٿي);
				عائم bin_مٿانهون_angle = ObstacleMath::get_هيٺاهون_bound_angle(i + 1, obstacle_map_body_چوکٺو.increment,
							obstacle_map_body_چوکٺو.angle_وٿي);
				عائم نياپو_هيٺاهون_angle = ObstacleMath::get_هيٺاهون_bound_angle(j, obstacle.increment,
							obstacle.angle_وٿي - vehicle_orientation_deg);
				عائم نياپو_مٿانهون_angle = ObstacleMath::get_هيٺاهون_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_وٿي - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				إذا (bin_هيٺاهون_angle > bin_مٿانهون_angle) {
					bin_هيٺاهون_angle -= 360;
				}

				إذا (نياپو_هيٺاهون_angle > نياپو_مٿانهون_angle) {
					نياپو_هيٺاهون_angle -= 360;
				}

				// Check for overlaps.
				إذا ((نياپو_هيٺاهون_angle > bin_هيٺاهون_angle && نياپو_هيٺاهون_angle < bin_مٿانهون_angle) ||
				    (نياپو_مٿانهون_angle > bin_هيٺاهون_angle && نياپو_مٿانهون_angle < bin_مٿانهون_angle) ||
				    (نياپو_هيٺاهون_angle <= bin_هيٺاهون_angle && نياپو_مٿانهون_angle >= bin_مٿانهون_angle) ||
				    (نياپو_هيٺاهون_angle >= bin_هيٺاهون_angle && نياپو_مٿانهون_angle <= bin_مٿانهون_angle)) {
					إذا (obstacle.distances[j] != UINT16_MAX) {
						إذا (enter_معلومات(i, obstacle.max_مفاصلو * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_چوکٺو.distances[i] = obstacle.distances[j];
							معلومات_timestamps[i] = obstacle_map_body_چوکٺو.timestamp;
							معلومات_maxranges[i] = obstacle.max_مفاصلو;
							معلومات_fov[i] = 1;
						}
					}
				}

			}
		}

	} وإلا إذا (obstacle.چوکٺو == obstacle.MAV_چوکٺو_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		لكل (صحيح i = 0; i < BIN_ڳڻپ; i++) {
			لكل (صحيح j = 0; (j < 360 / obstacle.increment) && (j < BIN_ڳڻپ); j++) {
				عائم bin_هيٺاهون_angle = ObstacleMath::get_هيٺاهون_bound_angle(i, obstacle_map_body_چوکٺو.increment,
							obstacle_map_body_چوکٺو.angle_وٿي);
				عائم bin_مٿانهون_angle = ObstacleMath::get_هيٺاهون_bound_angle(i + 1, obstacle_map_body_چوکٺو.increment,
							obstacle_map_body_چوکٺو.angle_وٿي);
				عائم نياپو_هيٺاهون_angle = ObstacleMath::get_هيٺاهون_bound_angle(j, obstacle.increment, obstacle.angle_وٿي);
				عائم نياپو_مٿانهون_angle = ObstacleMath::get_هيٺاهون_bound_angle(j + 1, obstacle.increment, obstacle.angle_وٿي);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				إذا (bin_هيٺاهون_angle > bin_مٿانهون_angle) {
					bin_هيٺاهون_angle -= 360;
				}

				إذا (نياپو_هيٺاهون_angle > نياپو_مٿانهون_angle) {
					نياپو_هيٺاهون_angle -= 360;
				}

				// Check for overlaps.
				إذا ((نياپو_هيٺاهون_angle > bin_هيٺاهون_angle && نياپو_هيٺاهون_angle < bin_مٿانهون_angle) ||
				    (نياپو_مٿانهون_angle > bin_هيٺاهون_angle && نياپو_مٿانهون_angle < bin_مٿانهون_angle) ||
				    (نياپو_هيٺاهون_angle <= bin_هيٺاهون_angle && نياپو_مٿانهون_angle >= bin_مٿانهون_angle) ||
				    (نياپو_هيٺاهون_angle >= bin_هيٺاهون_angle && نياپو_مٿانهون_angle <= bin_مٿانهون_angle)) {
					إذا (obstacle.distances[j] != UINT16_MAX) {

						إذا (enter_معلومات(i, obstacle.max_مفاصلو * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_چوکٺو.distances[i] = obstacle.distances[j];
							معلومات_timestamps[i] = obstacle_map_body_چوکٺو.timestamp;
							معلومات_maxranges[i] = obstacle.max_مفاصلو;
							معلومات_fov[i] = 1;
						}
					}
				}

			}
		}

	} وإلا {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.چوکٺو);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.چوکٺو);
	}
}

منطقي
CollisionPrevention::enter_معلومات(صحيح map_اشاريو, عائم sensor_range, عائم sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	إذا (sensor_reading < sensor_range) {
		إذا ((obstacle_map_body_چوکٺو.distances[map_اشاريو] < معلومات_maxranges[map_اشاريو]
		     && sensor_range_cm <= معلومات_maxranges[map_اشاريو])
		    || obstacle_map_body_چوکٺو.distances[map_اشاريو] >= معلومات_maxranges[map_اشاريو]) {

			أرجع سچ;
		}

	} وإلا {
		إذا ((obstacle_map_body_چوکٺو.distances[map_اشاريو] >= معلومات_maxranges[map_اشاريو]
		     && sensor_range_cm >= معلومات_maxranges[map_اشاريو])
		    || (obstacle_map_body_چوکٺو.distances[map_اشاريو] < معلومات_maxranges[map_اشاريو]
			&& sensor_range_cm == معلومات_maxranges[map_اشاريو])) {

			أرجع سچ;
		}
	}

	أرجع خطأ;
}

منطقي
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	إذا (setpoint_اشاريو < 0 || setpoint_اشاريو >= BIN_ڳڻپ) {
		أرجع خطأ; // treat out-of-bounds as unsafe
	}

	ثابت منطقي no_معلومات = (obstacle_map_body_چوکٺو.distances[setpoint_اشاريو] == UINT16_MAX);
	ثابت منطقي allow_movement_towards_no_معلومات = param_cp_go_no_معلومات.get();
	ثابت منطقي fov_at_setpoint = معلومات_fov[setpoint_اشاريو];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	أرجع !no_معلومات || (allow_movement_towards_no_معلومات && !fov_at_setpoint);
}

فراغ
CollisionPrevention::_transformSetpoint(ثابت Vector2f &setpoint)
{
	ثابت عائم sp_angle_body_چوکٺو = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	ثابت عائم sp_angle_with_وٿي_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_چوکٺو) -
					       obstacle_map_body_چوکٺو.angle_وٿي);
	setpoint_اشاريو = floor(sp_angle_with_وٿي_deg / BIN_سائيز);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, setpoint_اشاريو, _vehicle_yaw);
}

فراغ
CollisionPrevention::جوڙيو_مفاصلو_Sensor_معلومات(مفاصلو_sensor_s &مفاصلو_sensor, ثابت Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	عائم مفاصلو_reading = math::min(مفاصلو_sensor.current_مفاصلو, مفاصلو_sensor.max_مفاصلو);

	// negative values indicate out of range but valid measurements.
	إذا (fabsf(مفاصلو_sensor.current_مفاصلو - -1.f) < FLT_EPSILON && مفاصلو_sensor.signal_quality == 0) {
		مفاصلو_reading = مفاصلو_sensor.max_مفاصلو;
	}

	// discard values below min range
	إذا (مفاصلو_reading > مفاصلو_sensor.min_مفاصلو) {
		عائم sensor_yaw_body_rad = ObstacleMath::sensor_orientation_منزل_yaw_وٿي(static_cast<ObstacleMath::SensorOrientation>
					    (مفاصلو_sensor.orientation), مفاصلو_sensor.q);
		عائم sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		صحيح هيٺاهون_bound = (صحيح)round((sensor_yaw_body_deg  - math::degrees(مفاصلو_sensor.h_fov / 2.0f)) / BIN_سائيز);
		صحيح مٿانهون_bound = (صحيح)round((sensor_yaw_body_deg  + math::degrees(مفاصلو_sensor.h_fov / 2.0f)) / BIN_سائيز);

		إذا (مفاصلو_reading < مفاصلو_sensor.max_مفاصلو) {
			ObstacleMath::project_مفاصلو_on_horizontal_plane(مفاصلو_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * مفاصلو_sensor.max_مفاصلو)); // convert to cm

		لكل (صحيح bin = هيٺاهون_bound; bin <= مٿانهون_bound; ++bin) {
			صحيح wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_ڳڻپ);

			إذا (enter_معلومات(wrapped_bin, مفاصلو_sensor.max_مفاصلو, مفاصلو_reading)) {
				obstacle_map_body_چوکٺو.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * مفاصلو_reading));
				معلومات_timestamps[wrapped_bin] = obstacle_map_body_چوکٺو.timestamp;
				معلومات_maxranges[wrapped_bin] = sensor_range;
				معلومات_fov[wrapped_bin] = 1;
			}
		}
	}
}

فراغ
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, صحيح &setpoint_اشاريو_2, عائم vehicle_yaw_angle_rad)
{
	ثابت صحيح guidance_bins = floor(_param_cp_guide_ang.get() / BIN_سائيز);
	ثابت صحيح sp_اشاريو_original = setpoint_اشاريو_2;
	عائم بهترين_لاڳت = 9999.f;
	صحيح new_sp_اشاريو = setpoint_اشاريو_2;

	لكل (صحيح i = sp_اشاريو_original - guidance_bins; i <= sp_اشاريو_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		ثابت صحيح filter_سائيز = 1;
		عائم سراسري_dist = 0;

		لكل (صحيح j = i - filter_سائيز; j <= i + filter_سائيز; j++) {
			صحيح bin = ObstacleMath::wrap_bin(j, BIN_ڳڻپ);

			إذا (obstacle_map_body_چوکٺو.distances[bin] == UINT16_MAX) {
				سراسري_dist += _param_cp_dist.get() * 100.f;

			} وإلا {
				سراسري_dist += obstacle_map_body_چوکٺو.distances[bin];
			}
		}

		ثابت صحيح bin = ObstacleMath::wrap_bin(i, BIN_ڳڻپ);
		سراسري_dist = سراسري_dist / (2.f * filter_سائيز + 1.f);
		ثابت عائم deviation_لاڳت = _param_cp_dist.get() * 50.f * مطلق_قدر(i - sp_اشاريو_original);
		ثابت عائم bin_لاڳت = deviation_لاڳت - سراسري_dist - obstacle_map_body_چوکٺو.distances[bin];

		إذا (bin_لاڳت < بهترين_لاڳت && obstacle_map_body_چوکٺو.distances[bin] != UINT16_MAX) {
			بهترين_لاڳت = bin_لاڳت;
			new_sp_اشاريو = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	إذا (new_sp_اشاريو != setpoint_اشاريو_2) {
		عائم angle = math::radians((عائم)new_sp_اشاريو * BIN_سائيز + obstacle_map_body_چوکٺو.angle_وٿي);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_اشاريو_2 = new_sp_اشاريو;
	}
}

عائم CollisionPrevention::get_Obstacle_مفاصلو(ثابت Vector2f &direction)
{
	عائم obstacle_مفاصلو = 0.f;
	ثابت عائم direction_norm = direction.norm();

	إذا (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		ثابت عائم sp_angle_body_چوکٺو = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		ثابت عائم sp_angle_with_وٿي_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_چوکٺو) - obstacle_map_body_چوکٺو.angle_وٿي);

		ثابت صحيح dir_اشاريو = ObstacleMath::get_bin_at_angle(BIN_سائيز, sp_angle_with_وٿي_deg);
		obstacle_مفاصلو   = obstacle_map_body_چوکٺو.distances[dir_اشاريو] * 0.01f;
	}

	أرجع obstacle_مفاصلو;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(ثابت عائم &setpoint_ڊيگهائي)
{
	Vector2f new_setpoint{};
	ثابت Vector2f عام_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	ثابت Vector2f tangential_component = _setpoint_dir - عام_component;

	ثابت عائم عام_scale = _getScale(_closest_dist);


	ثابت عائم closest_dist_tangential = get_Obstacle_مفاصلو(tangential_component);
	ثابت عائم tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	إذا (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + عام_component * عام_scale) * setpoint_ڊيگهائي;

	} وإلا {
		new_setpoint = _setpoint_dir * setpoint_ڊيگهائي;
	}

	أرجع new_setpoint;
}

عائم
CollisionPrevention::_getScale(ثابت عائم &reference_مفاصلو)
{
	عائم scale = (reference_مفاصلو - min_dist_منزل_keep);
	ثابت عائم scale_مفاصلو = math::max(min_dist_منزل_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_مفاصلو, 2) : scale;
	scale = math::min(scale, 1.0f);
	أرجع scale;
}

فراغ CollisionPrevention::_getVelocityCompensationAcceleration(ثابت عائم vehicle_yaw_angle_rad,
		ثابت Vector2f &setpoint_vel,
		ثابت hrt_abstime now, عائم &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	لكل (صحيح i = 0; i < BIN_ڳڻپ; i++) {
		ثابت عائم max_range = معلومات_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		عائم bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((عائم)i * BIN_سائيز + obstacle_map_body_چوکٺو.angle_وٿي));

		ثابت Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		عائم bin_مفاصلو = obstacle_map_body_چوکٺو.distances[i];

		// only consider bins which are between min and max values
		إذا (bin_مفاصلو > obstacle_map_body_چوکٺو.min_مفاصلو && bin_مفاصلو < UINT16_MAX) {
			ثابت عائم مفاصلو = bin_مفاصلو * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			ثابت عائم curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			عائم delay_مفاصلو = curr_vel_parallel * _param_cp_delay.get();

			ثابت hrt_abstime معلومات_age = now - معلومات_timestamps[i];

			إذا (مفاصلو < max_range) {
				delay_مفاصلو += curr_vel_parallel * (معلومات_age * 1e-6f);
			}

			ثابت عائم stop_مفاصلو = مفاصلو - min_dist_منزل_keep - delay_مفاصلو;

			عائم curr_acc_vel_constraint;

			إذا (stop_مفاصلو >= 0.f) {
				ثابت عائم max_vel = math::trajectory::compute_Max_رفتار_From_مفاصلو(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_مفاصلو, 0.f);
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
	command.command = vehicle_command_s::VEHICLE_CMD_DO_سيٽ_گهڻائي;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (عائم)PX4_CUSTOM_MAIN_گهڻائي_AUTO;
	command.param3 = (عائم)PX4_CUSTOM_SUB_گهڻائي_AUTO_LOITER;
	command.هدف_system = 1;
	command.هدف_component = 1;
	command.ذريعو_system = 1;
	command.ذريعو_component = 1;
	command.confirmation = خطأ;
	command.from_external = خطأ;
	command.timestamp = get_وقت();
	_vehicle_command_pub.publish(command);
}
