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

#ਅਨ੍ਤਰ੍ਭਾਵ "CollisionPrevention.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "ObstacleMath.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/events.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>


ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

CollisionPrevention::CollisionPrevention(ModuleParams *ਮਾਪਾ) :
	ModuleParams(ਮਾਪਾ)
{
	static_assert(BIN_ਮਾਪ >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_ਮਾਪ == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	obstacle_map_body_ਚੌਖਟਾ.ਚੌਖਟਾ = obstacle_ਦੂਰੀ_s::MAV_ਚੌਖਟਾ_BODY_FRD;
	obstacle_map_body_ਚੌਖਟਾ.increment = BIN_ਮਾਪ;
	obstacle_map_body_ਚੌਖਟਾ.min_ਦੂਰੀ = UINT16_MAX;

	ਲਈ (uint32_t i = 0 ; i < BIN_ਗਿਣਤੀ; i++) {
		obstacle_map_body_ਚੌਖਟਾ.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::get_ਸਮਾਂ()
{
	ਵਾਪਸ hrt_absolute_ਸਮਾਂ();
}

hrt_abstime CollisionPrevention::get_Elapsed_ਸਮਾਂ(ਅਚਲ hrt_abstime *ptr)
{
	ਵਾਪਸ hrt_absolute_ਸਮਾਂ() - *ptr;
}

ਤਾਰਕਿਕ CollisionPrevention::is_ਸਰਗਰਮ()
{
	ਤਾਰਕਿਕ activated = _param_cp_dist.get() > 0;

	ਜੇ (activated && !was_ਸਰਗਰਮ) {
		ਸਮਾਂ_activated = get_ਸਮਾਂ();
	}

	was_ਸਰਗਰਮ = activated;
	ਵਾਪਸ activated;
}

ਖਾਲੀ CollisionPrevention::modifySetpoint(Vector2f &setpoint_accel, ਅਚਲ Vector2f &setpoint_vel)
{
	ਜੇ (_vehicle_attitude_sub.updated()) {
		vehicle_attitude_s vehicle_attitude;

		ਜੇ (_vehicle_attitude_sub.copy(&vehicle_attitude)) {
			_vehicle_attitude = Quatf(vehicle_attitude.q);
			_vehicle_yaw = Eulerf(_vehicle_attitude).psi();
		}
	}

	//calculate movement constraints based on range data
	ਅਚਲ Vector2f original_setpoint = setpoint_accel;
	_updateObstacleMap();
	update_Obstacle_ਅੰਕੜੇ();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copy_ਮੰਜ਼ਿਲ(constraints.original_setpoint);
	setpoint_accel.copy_ਮੰਜ਼ਿਲ(constraints.adapted_setpoint);
	constraints.timestamp = get_ਸਮਾਂ();
	_constraints_pub.publish(constraints);
}

ਖਾਲੀ CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	ਲਈ (ਸਵੈਚਾਲਿਤ &dist_sens_sub : ਦੂਰੀ_sensor_subs) {
		ਦੂਰੀ_sensor_s ਦੂਰੀ_sensor;

		ਜੇ (dist_sens_sub.update(&ਦੂਰੀ_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			ਜੇ ((get_Elapsed_ਸਮਾਂ(&ਦੂਰੀ_sensor.timestamp) < RANGE_ਧਾਰਾ_TIMEOUT_US) &&
			    (ਦੂਰੀ_sensor.orientation != ਦੂਰੀ_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (ਦੂਰੀ_sensor.orientation != ਦੂਰੀ_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				obstacle_map_body_ਚੌਖਟਾ.timestamp = math::max(obstacle_map_body_ਚੌਖਟਾ.timestamp, ਦੂਰੀ_sensor.timestamp);
				obstacle_map_body_ਚੌਖਟਾ.max_ਦੂਰੀ = math::max(obstacle_map_body_ਚੌਖਟਾ.max_ਦੂਰੀ,
									(uint16_t)(ਦੂਰੀ_sensor.max_ਦੂਰੀ * 100.0f));
				obstacle_map_body_ਚੌਖਟਾ.min_ਦੂਰੀ = math::min(obstacle_map_body_ਚੌਖਟਾ.min_ਦੂਰੀ,
									(uint16_t)(ਦੂਰੀ_sensor.min_ਦੂਰੀ * 100.0f));

				ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	ਜੇ (sub_obstacle_ਦੂਰੀ.update()) {
		ਅਚਲ obstacle_ਦੂਰੀ_s &obstacle_ਦੂਰੀ = sub_obstacle_ਦੂਰੀ.get();

		// Update map with obstacle data if the data is not stale
		ਜੇ (get_Elapsed_ਸਮਾਂ(&obstacle_ਦੂਰੀ.timestamp) < RANGE_ਧਾਰਾ_TIMEOUT_US && obstacle_ਦੂਰੀ.increment > 0.f) {
			//update message description
			obstacle_map_body_ਚੌਖਟਾ.timestamp = math::max(obstacle_map_body_ਚੌਖਟਾ.timestamp, obstacle_ਦੂਰੀ.timestamp);
			obstacle_map_body_ਚੌਖਟਾ.max_ਦੂਰੀ = math::max(obstacle_map_body_ਚੌਖਟਾ.max_ਦੂਰੀ,
								obstacle_ਦੂਰੀ.max_ਦੂਰੀ);
			obstacle_map_body_ਚੌਖਟਾ.min_ਦੂਰੀ = math::min(obstacle_map_body_ਚੌਖਟਾ.min_ਦੂਰੀ,
								obstacle_ਦੂਰੀ.min_ਦੂਰੀ);
			ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਦੂਰੀ, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	obstacle_ਦੂਰੀ_fused_pub.publish(obstacle_map_body_ਚੌਖਟਾ);
}

ਖਾਲੀ CollisionPrevention::update_Obstacle_ਅੰਕੜੇ()
{
	obstacle_ਅੰਕੜੇ_present = ਝੂਠ;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.ਸੈੱਟ_Zero();

	ਲਈ (ਪੂਰਨ i = 0; i < BIN_ਗਿਣਤੀ; i++) {
		// if the data is stale, reset the bin
		ਜੇ (get_ਸਮਾਂ() - ਅੰਕੜੇ_timestamps[i] > RANGE_ਧਾਰਾ_TIMEOUT_US) {
			obstacle_map_body_ਚੌਖਟਾ.distances[i] = UINT16_MAX;
		}

		ਦਸ਼ਮਲਵ angle = wrap_2pi(_vehicle_yaw + math::radians((ਦਸ਼ਮਲਵ)i * BIN_ਮਾਪ +
				       obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ));
		ਅਚਲ Vector2f bin_direction = {cosf(angle), sinf(angle)};
		ਅਚਲ uint16_t bin_ਦੂਰੀ = obstacle_map_body_ਚੌਖਟਾ.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		ਜੇ (bin_ਦੂਰੀ < UINT16_MAX
		    && (get_ਸਮਾਂ() - obstacle_map_body_ਚੌਖਟਾ.timestamp) < RANGE_ਧਾਰਾ_TIMEOUT_US) {
			obstacle_ਅੰਕੜੇ_present = ਸੱਚ;
		}

		ਜੇ (bin_ਦੂਰੀ * 0.01f < _closest_dist) {
			_closest_dist = bin_ਦੂਰੀ * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

ਖਾਲੀ CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, ਅਚਲ Vector2f &setpoint_vel)
{
	ਅਚਲ ਦਸ਼ਮਲਵ setpoint_ਲੰਮਾਈ = setpoint_accel.norm();
	min_dist_ਮੰਜ਼ਿਲ_keep = math::max(obstacle_map_body_ਚੌਖਟਾ.min_ਦੂਰੀ / 100.0f, _param_cp_dist.get());

	ਅਚਲ hrt_abstime now = get_ਸਮਾਂ();

	ਅਚਲ ਤਾਰਕਿਕ is_stick_deflected = setpoint_ਲੰਮਾਈ > 0.001f;

	ਜੇ (obstacle_ਅੰਕੜੇ_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		ਦਸ਼ਮਲਵ vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		ਜੇ (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_ਲੰਮਾਈ);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} ਨਹੀਂਤਾਂ ਜੇ (!obstacle_ਅੰਕੜੇ_present) {
		// allow no movement
		setpoint_accel.ਸੈੱਟ_Zero();

		// if distance data is stale, switch to Loiter
		ਜੇ (get_Elapsed_ਸਮਾਂ(&ਆਖਰੀ_timeout_warning) > 1_s && get_Elapsed_ਸਮਾਂ(&ਸਮਾਂ_activated) > 1_s) {
			ਜੇ ((now - obstacle_map_body_ਚੌਖਟਾ.timestamp) > TIMEOUT_HOLD_US &&
			    get_Elapsed_ਸਮਾਂ(&ਸਮਾਂ_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			ਆਖਰੀ_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
ਖਾਲੀ CollisionPrevention::ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(ਅਚਲ obstacle_ਦੂਰੀ_s &obstacle, ਅਚਲ ਦਸ਼ਮਲਵ vehicle_yaw)
{

	ਦਸ਼ਮਲਵ vehicle_orientation_deg = math::degrees(vehicle_yaw);


	ਜੇ (obstacle.ਚੌਖਟਾ == obstacle.MAV_ਚੌਖਟਾ_GLOBAL || obstacle.ਚੌਖਟਾ == obstacle.MAV_ਚੌਖਟਾ_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		ਲਈ (ਪੂਰਨ i = 0; i < BIN_ਗਿਣਤੀ; i++) {
			ਲਈ (ਪੂਰਨ j = 0; (j < 360 / obstacle.increment) && (j < BIN_ਗਿਣਤੀ); j++) {
				ਦਸ਼ਮਲਵ bin_ਹੇਠਲਾ_angle = ObstacleMath::get_ਹੇਠਲਾ_bound_angle(i, obstacle_map_body_ਚੌਖਟਾ.increment,
							obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ);
				ਦਸ਼ਮਲਵ bin_ਉੱਤਲਾ_angle = ObstacleMath::get_ਹੇਠਲਾ_bound_angle(i + 1, obstacle_map_body_ਚੌਖਟਾ.increment,
							obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ);
				ਦਸ਼ਮਲਵ ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle = ObstacleMath::get_ਹੇਠਲਾ_bound_angle(j, obstacle.increment,
							obstacle.angle_ਵਿੱਥ - vehicle_orientation_deg);
				ਦਸ਼ਮਲਵ ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle = ObstacleMath::get_ਹੇਠਲਾ_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_ਵਿੱਥ - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				ਜੇ (bin_ਹੇਠਲਾ_angle > bin_ਉੱਤਲਾ_angle) {
					bin_ਹੇਠਲਾ_angle -= 360;
				}

				ਜੇ (ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle > ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle) {
					ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle -= 360;
				}

				// Check for overlaps.
				ਜੇ ((ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle > bin_ਹੇਠਲਾ_angle && ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle < bin_ਉੱਤਲਾ_angle) ||
				    (ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle > bin_ਹੇਠਲਾ_angle && ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle < bin_ਉੱਤਲਾ_angle) ||
				    (ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle <= bin_ਹੇਠਲਾ_angle && ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle >= bin_ਉੱਤਲਾ_angle) ||
				    (ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle >= bin_ਹੇਠਲਾ_angle && ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle <= bin_ਉੱਤਲਾ_angle)) {
					ਜੇ (obstacle.distances[j] != UINT16_MAX) {
						ਜੇ (enter_ਅੰਕੜੇ(i, obstacle.max_ਦੂਰੀ * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_ਚੌਖਟਾ.distances[i] = obstacle.distances[j];
							ਅੰਕੜੇ_timestamps[i] = obstacle_map_body_ਚੌਖਟਾ.timestamp;
							ਅੰਕੜੇ_maxranges[i] = obstacle.max_ਦੂਰੀ;
							ਅੰਕੜੇ_fov[i] = 1;
						}
					}
				}

			}
		}

	} ਨਹੀਂਤਾਂ ਜੇ (obstacle.ਚੌਖਟਾ == obstacle.MAV_ਚੌਖਟਾ_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		ਲਈ (ਪੂਰਨ i = 0; i < BIN_ਗਿਣਤੀ; i++) {
			ਲਈ (ਪੂਰਨ j = 0; (j < 360 / obstacle.increment) && (j < BIN_ਗਿਣਤੀ); j++) {
				ਦਸ਼ਮਲਵ bin_ਹੇਠਲਾ_angle = ObstacleMath::get_ਹੇਠਲਾ_bound_angle(i, obstacle_map_body_ਚੌਖਟਾ.increment,
							obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ);
				ਦਸ਼ਮਲਵ bin_ਉੱਤਲਾ_angle = ObstacleMath::get_ਹੇਠਲਾ_bound_angle(i + 1, obstacle_map_body_ਚੌਖਟਾ.increment,
							obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ);
				ਦਸ਼ਮਲਵ ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle = ObstacleMath::get_ਹੇਠਲਾ_bound_angle(j, obstacle.increment, obstacle.angle_ਵਿੱਥ);
				ਦਸ਼ਮਲਵ ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle = ObstacleMath::get_ਹੇਠਲਾ_bound_angle(j + 1, obstacle.increment, obstacle.angle_ਵਿੱਥ);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				ਜੇ (bin_ਹੇਠਲਾ_angle > bin_ਉੱਤਲਾ_angle) {
					bin_ਹੇਠਲਾ_angle -= 360;
				}

				ਜੇ (ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle > ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle) {
					ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle -= 360;
				}

				// Check for overlaps.
				ਜੇ ((ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle > bin_ਹੇਠਲਾ_angle && ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle < bin_ਉੱਤਲਾ_angle) ||
				    (ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle > bin_ਹੇਠਲਾ_angle && ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle < bin_ਉੱਤਲਾ_angle) ||
				    (ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle <= bin_ਹੇਠਲਾ_angle && ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle >= bin_ਉੱਤਲਾ_angle) ||
				    (ਸੁਨੇਹਾ_ਹੇਠਲਾ_angle >= bin_ਹੇਠਲਾ_angle && ਸੁਨੇਹਾ_ਉੱਤਲਾ_angle <= bin_ਉੱਤਲਾ_angle)) {
					ਜੇ (obstacle.distances[j] != UINT16_MAX) {

						ਜੇ (enter_ਅੰਕੜੇ(i, obstacle.max_ਦੂਰੀ * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_ਚੌਖਟਾ.distances[i] = obstacle.distances[j];
							ਅੰਕੜੇ_timestamps[i] = obstacle_map_body_ਚੌਖਟਾ.timestamp;
							ਅੰਕੜੇ_maxranges[i] = obstacle.max_ਦੂਰੀ;
							ਅੰਕੜੇ_fov[i] = 1;
						}
					}
				}

			}
		}

	} ਨਹੀਂਤਾਂ {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.ਚੌਖਟਾ);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.ਚੌਖਟਾ);
	}
}

ਤਾਰਕਿਕ
CollisionPrevention::enter_ਅੰਕੜੇ(ਪੂਰਨ map_ਸੂਚਕਾਂਕ, ਦਸ਼ਮਲਵ sensor_range, ਦਸ਼ਮਲਵ sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	ਜੇ (sensor_reading < sensor_range) {
		ਜੇ ((obstacle_map_body_ਚੌਖਟਾ.distances[map_ਸੂਚਕਾਂਕ] < ਅੰਕੜੇ_maxranges[map_ਸੂਚਕਾਂਕ]
		     && sensor_range_cm <= ਅੰਕੜੇ_maxranges[map_ਸੂਚਕਾਂਕ])
		    || obstacle_map_body_ਚੌਖਟਾ.distances[map_ਸੂਚਕਾਂਕ] >= ਅੰਕੜੇ_maxranges[map_ਸੂਚਕਾਂਕ]) {

			ਵਾਪਸ ਸੱਚ;
		}

	} ਨਹੀਂਤਾਂ {
		ਜੇ ((obstacle_map_body_ਚੌਖਟਾ.distances[map_ਸੂਚਕਾਂਕ] >= ਅੰਕੜੇ_maxranges[map_ਸੂਚਕਾਂਕ]
		     && sensor_range_cm >= ਅੰਕੜੇ_maxranges[map_ਸੂਚਕਾਂਕ])
		    || (obstacle_map_body_ਚੌਖਟਾ.distances[map_ਸੂਚਕਾਂਕ] < ਅੰਕੜੇ_maxranges[map_ਸੂਚਕਾਂਕ]
			&& sensor_range_cm == ਅੰਕੜੇ_maxranges[map_ਸੂਚਕਾਂਕ])) {

			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	ਜੇ (setpoint_ਸੂਚਕਾਂਕ < 0 || setpoint_ਸੂਚਕਾਂਕ >= BIN_ਗਿਣਤੀ) {
		ਵਾਪਸ ਝੂਠ; // treat out-of-bounds as unsafe
	}

	ਅਚਲ ਤਾਰਕਿਕ no_ਅੰਕੜੇ = (obstacle_map_body_ਚੌਖਟਾ.distances[setpoint_ਸੂਚਕਾਂਕ] == UINT16_MAX);
	ਅਚਲ ਤਾਰਕਿਕ allow_movement_towards_no_ਅੰਕੜੇ = param_cp_go_no_ਅੰਕੜੇ.get();
	ਅਚਲ ਤਾਰਕਿਕ fov_at_setpoint = ਅੰਕੜੇ_fov[setpoint_ਸੂਚਕਾਂਕ];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	ਵਾਪਸ !no_ਅੰਕੜੇ || (allow_movement_towards_no_ਅੰਕੜੇ && !fov_at_setpoint);
}

ਖਾਲੀ
CollisionPrevention::_transformSetpoint(ਅਚਲ Vector2f &setpoint)
{
	ਅਚਲ ਦਸ਼ਮਲਵ sp_angle_body_ਚੌਖਟਾ = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	ਅਚਲ ਦਸ਼ਮਲਵ sp_angle_with_ਵਿੱਥ_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_ਚੌਖਟਾ) -
					       obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ);
	setpoint_ਸੂਚਕਾਂਕ = floor(sp_angle_with_ਵਿੱਥ_deg / BIN_ਮਾਪ);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, setpoint_ਸੂਚਕਾਂਕ, _vehicle_yaw);
}

ਖਾਲੀ
CollisionPrevention::ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor_s &ਦੂਰੀ_sensor, ਅਚਲ Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	ਦਸ਼ਮਲਵ ਦੂਰੀ_reading = math::min(ਦੂਰੀ_sensor.current_ਦੂਰੀ, ਦੂਰੀ_sensor.max_ਦੂਰੀ);

	// negative values indicate out of range but valid measurements.
	ਜੇ (fabsf(ਦੂਰੀ_sensor.current_ਦੂਰੀ - -1.f) < FLT_EPSILON && ਦੂਰੀ_sensor.signal_quality == 0) {
		ਦੂਰੀ_reading = ਦੂਰੀ_sensor.max_ਦੂਰੀ;
	}

	// discard values below min range
	ਜੇ (ਦੂਰੀ_reading > ਦੂਰੀ_sensor.min_ਦੂਰੀ) {
		ਦਸ਼ਮਲਵ sensor_yaw_body_rad = ObstacleMath::sensor_orientation_ਮੰਜ਼ਿਲ_yaw_ਵਿੱਥ(static_cast<ObstacleMath::SensorOrientation>
					    (ਦੂਰੀ_sensor.orientation), ਦੂਰੀ_sensor.q);
		ਦਸ਼ਮਲਵ sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		ਪੂਰਨ ਹੇਠਲਾ_bound = (ਪੂਰਨ)round((sensor_yaw_body_deg  - math::degrees(ਦੂਰੀ_sensor.h_fov / 2.0f)) / BIN_ਮਾਪ);
		ਪੂਰਨ ਉੱਤਲਾ_bound = (ਪੂਰਨ)round((sensor_yaw_body_deg  + math::degrees(ਦੂਰੀ_sensor.h_fov / 2.0f)) / BIN_ਮਾਪ);

		ਜੇ (ਦੂਰੀ_reading < ਦੂਰੀ_sensor.max_ਦੂਰੀ) {
			ObstacleMath::project_ਦੂਰੀ_on_horizontal_plane(ਦੂਰੀ_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * ਦੂਰੀ_sensor.max_ਦੂਰੀ)); // convert to cm

		ਲਈ (ਪੂਰਨ bin = ਹੇਠਲਾ_bound; bin <= ਉੱਤਲਾ_bound; ++bin) {
			ਪੂਰਨ wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_ਗਿਣਤੀ);

			ਜੇ (enter_ਅੰਕੜੇ(wrapped_bin, ਦੂਰੀ_sensor.max_ਦੂਰੀ, ਦੂਰੀ_reading)) {
				obstacle_map_body_ਚੌਖਟਾ.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * ਦੂਰੀ_reading));
				ਅੰਕੜੇ_timestamps[wrapped_bin] = obstacle_map_body_ਚੌਖਟਾ.timestamp;
				ਅੰਕੜੇ_maxranges[wrapped_bin] = sensor_range;
				ਅੰਕੜੇ_fov[wrapped_bin] = 1;
			}
		}
	}
}

ਖਾਲੀ
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, ਪੂਰਨ &setpoint_ਸੂਚਕਾਂਕ_2, ਦਸ਼ਮਲਵ vehicle_yaw_angle_rad)
{
	ਅਚਲ ਪੂਰਨ guidance_bins = floor(_param_cp_guide_ang.get() / BIN_ਮਾਪ);
	ਅਚਲ ਪੂਰਨ sp_ਸੂਚਕਾਂਕ_original = setpoint_ਸੂਚਕਾਂਕ_2;
	ਦਸ਼ਮਲਵ ਉੱਤਮ_ਲਾਗਤ = 9999.f;
	ਪੂਰਨ new_sp_ਸੂਚਕਾਂਕ = setpoint_ਸੂਚਕਾਂਕ_2;

	ਲਈ (ਪੂਰਨ i = sp_ਸੂਚਕਾਂਕ_original - guidance_bins; i <= sp_ਸੂਚਕਾਂਕ_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		ਅਚਲ ਪੂਰਨ filter_ਮਾਪ = 1;
		ਦਸ਼ਮਲਵ ਔਸਤ_dist = 0;

		ਲਈ (ਪੂਰਨ j = i - filter_ਮਾਪ; j <= i + filter_ਮਾਪ; j++) {
			ਪੂਰਨ bin = ObstacleMath::wrap_bin(j, BIN_ਗਿਣਤੀ);

			ਜੇ (obstacle_map_body_ਚੌਖਟਾ.distances[bin] == UINT16_MAX) {
				ਔਸਤ_dist += _param_cp_dist.get() * 100.f;

			} ਨਹੀਂਤਾਂ {
				ਔਸਤ_dist += obstacle_map_body_ਚੌਖਟਾ.distances[bin];
			}
		}

		ਅਚਲ ਪੂਰਨ bin = ObstacleMath::wrap_bin(i, BIN_ਗਿਣਤੀ);
		ਔਸਤ_dist = ਔਸਤ_dist / (2.f * filter_ਮਾਪ + 1.f);
		ਅਚਲ ਦਸ਼ਮਲਵ deviation_ਲਾਗਤ = _param_cp_dist.get() * 50.f * ਨਿਰਪੇਖ(i - sp_ਸੂਚਕਾਂਕ_original);
		ਅਚਲ ਦਸ਼ਮਲਵ bin_ਲਾਗਤ = deviation_ਲਾਗਤ - ਔਸਤ_dist - obstacle_map_body_ਚੌਖਟਾ.distances[bin];

		ਜੇ (bin_ਲਾਗਤ < ਉੱਤਮ_ਲਾਗਤ && obstacle_map_body_ਚੌਖਟਾ.distances[bin] != UINT16_MAX) {
			ਉੱਤਮ_ਲਾਗਤ = bin_ਲਾਗਤ;
			new_sp_ਸੂਚਕਾਂਕ = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	ਜੇ (new_sp_ਸੂਚਕਾਂਕ != setpoint_ਸੂਚਕਾਂਕ_2) {
		ਦਸ਼ਮਲਵ angle = math::radians((ਦਸ਼ਮਲਵ)new_sp_ਸੂਚਕਾਂਕ * BIN_ਮਾਪ + obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_ਸੂਚਕਾਂਕ_2 = new_sp_ਸੂਚਕਾਂਕ;
	}
}

ਦਸ਼ਮਲਵ CollisionPrevention::get_Obstacle_ਦੂਰੀ(ਅਚਲ Vector2f &direction)
{
	ਦਸ਼ਮਲਵ obstacle_ਦੂਰੀ = 0.f;
	ਅਚਲ ਦਸ਼ਮਲਵ direction_norm = direction.norm();

	ਜੇ (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		ਅਚਲ ਦਸ਼ਮਲਵ sp_angle_body_ਚੌਖਟਾ = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		ਅਚਲ ਦਸ਼ਮਲਵ sp_angle_with_ਵਿੱਥ_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_ਚੌਖਟਾ) - obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ);

		ਅਚਲ ਪੂਰਨ dir_ਸੂਚਕਾਂਕ = ObstacleMath::get_bin_at_angle(BIN_ਮਾਪ, sp_angle_with_ਵਿੱਥ_deg);
		obstacle_ਦੂਰੀ   = obstacle_map_body_ਚੌਖਟਾ.distances[dir_ਸੂਚਕਾਂਕ] * 0.01f;
	}

	ਵਾਪਸ obstacle_ਦੂਰੀ;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(ਅਚਲ ਦਸ਼ਮਲਵ &setpoint_ਲੰਮਾਈ)
{
	Vector2f new_setpoint{};
	ਅਚਲ Vector2f ਸਾਧਾਰਨ_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	ਅਚਲ Vector2f tangential_component = _setpoint_dir - ਸਾਧਾਰਨ_component;

	ਅਚਲ ਦਸ਼ਮਲਵ ਸਾਧਾਰਨ_scale = _getScale(_closest_dist);


	ਅਚਲ ਦਸ਼ਮਲਵ closest_dist_tangential = get_Obstacle_ਦੂਰੀ(tangential_component);
	ਅਚਲ ਦਸ਼ਮਲਵ tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	ਜੇ (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + ਸਾਧਾਰਨ_component * ਸਾਧਾਰਨ_scale) * setpoint_ਲੰਮਾਈ;

	} ਨਹੀਂਤਾਂ {
		new_setpoint = _setpoint_dir * setpoint_ਲੰਮਾਈ;
	}

	ਵਾਪਸ new_setpoint;
}

ਦਸ਼ਮਲਵ
CollisionPrevention::_getScale(ਅਚਲ ਦਸ਼ਮਲਵ &reference_ਦੂਰੀ)
{
	ਦਸ਼ਮਲਵ scale = (reference_ਦੂਰੀ - min_dist_ਮੰਜ਼ਿਲ_keep);
	ਅਚਲ ਦਸ਼ਮਲਵ scale_ਦੂਰੀ = math::max(min_dist_ਮੰਜ਼ਿਲ_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_ਦੂਰੀ, 2) : scale;
	scale = math::min(scale, 1.0f);
	ਵਾਪਸ scale;
}

ਖਾਲੀ CollisionPrevention::_getVelocityCompensationAcceleration(ਅਚਲ ਦਸ਼ਮਲਵ vehicle_yaw_angle_rad,
		ਅਚਲ Vector2f &setpoint_vel,
		ਅਚਲ hrt_abstime now, ਦਸ਼ਮਲਵ &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	ਲਈ (ਪੂਰਨ i = 0; i < BIN_ਗਿਣਤੀ; i++) {
		ਅਚਲ ਦਸ਼ਮਲਵ max_range = ਅੰਕੜੇ_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		ਦਸ਼ਮਲਵ bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((ਦਸ਼ਮਲਵ)i * BIN_ਮਾਪ + obstacle_map_body_ਚੌਖਟਾ.angle_ਵਿੱਥ));

		ਅਚਲ Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		ਦਸ਼ਮਲਵ bin_ਦੂਰੀ = obstacle_map_body_ਚੌਖਟਾ.distances[i];

		// only consider bins which are between min and max values
		ਜੇ (bin_ਦੂਰੀ > obstacle_map_body_ਚੌਖਟਾ.min_ਦੂਰੀ && bin_ਦੂਰੀ < UINT16_MAX) {
			ਅਚਲ ਦਸ਼ਮਲਵ ਦੂਰੀ = bin_ਦੂਰੀ * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			ਅਚਲ ਦਸ਼ਮਲਵ curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			ਦਸ਼ਮਲਵ delay_ਦੂਰੀ = curr_vel_parallel * _param_cp_delay.get();

			ਅਚਲ hrt_abstime ਅੰਕੜੇ_age = now - ਅੰਕੜੇ_timestamps[i];

			ਜੇ (ਦੂਰੀ < max_range) {
				delay_ਦੂਰੀ += curr_vel_parallel * (ਅੰਕੜੇ_age * 1e-6f);
			}

			ਅਚਲ ਦਸ਼ਮਲਵ stop_ਦੂਰੀ = ਦੂਰੀ - min_dist_ਮੰਜ਼ਿਲ_keep - delay_ਦੂਰੀ;

			ਦਸ਼ਮਲਵ curr_acc_vel_constraint;

			ਜੇ (stop_ਦੂਰੀ >= 0.f) {
				ਅਚਲ ਦਸ਼ਮਲਵ max_vel = math::trajectory::compute_Max_ਰਫਤਾਰ_From_ਦੂਰੀ(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_ਦੂਰੀ, 0.f);
				curr_acc_vel_constraint = _param_mpc_xy_vel_p_acc.get() * math::min(max_vel - curr_vel_parallel, 0.f);

			} ਨਹੀਂਤਾਂ {
				curr_acc_vel_constraint = -1.f * _param_mpc_xy_vel_p_acc.get() * curr_vel_parallel;
			}

			ਜੇ (curr_acc_vel_constraint < vel_comp_accel) {
				vel_comp_accel = curr_acc_vel_constraint;
				vel_comp_accel_dir = bin_direction;
			}
		}
	}
}

ਖਾਲੀ CollisionPrevention::_publishVehicleCmdDoLoiter()
{
	vehicle_command_s command{};
	command.command = vehicle_command_s::VEHICLE_CMD_DO_ਸੈੱਟ_ਬਹੁਲਕ;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (ਦਸ਼ਮਲਵ)PX4_CUSTOM_MAIN_ਬਹੁਲਕ_AUTO;
	command.param3 = (ਦਸ਼ਮਲਵ)PX4_CUSTOM_SUB_ਬਹੁਲਕ_AUTO_LOITER;
	command.ਨਿਸ਼ਾਨਾ_system = 1;
	command.ਨਿਸ਼ਾਨਾ_component = 1;
	command.ਸਰੋਤ_system = 1;
	command.ਸਰੋਤ_component = 1;
	command.confirmation = ਝੂਠ;
	command.from_external = ਝੂਠ;
	command.timestamp = get_ਸਮਾਂ();
	_vehicle_command_pub.publish(command);
}
