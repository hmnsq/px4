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

#అన్తర్భావ "CollisionPrevention.hpp"
#అన్తర్భావ "ObstacleMath.hpp"
#అన్తర్భావ <px4_platform_common/events.h>
#అన్తర్భావ <math.h>


ఉపయోగించి నామాకాశ మాతృక;

CollisionPrevention::CollisionPrevention(ModuleParams *మాతృ) :
	ModuleParams(మాతృ)
{
	static_assert(BIN_కొలత >= 5, "BIN_SIZE must be at least 5");
	static_assert(360 % BIN_కొలత == 0, "BIN_SIZE must divide 360 evenly");

	// initialize internal obstacle map
	obstacle_map_body_చట్రం.చట్రం = obstacle_దూరం_s::MAV_చట్రం_BODY_FRD;
	obstacle_map_body_చట్రం.increment = BIN_కొలత;
	obstacle_map_body_చట్రం.min_దూరం = UINT16_MAX;

	కొరకు (uint32_t i = 0 ; i < BIN_లెక్క; i++) {
		obstacle_map_body_చట్రం.distances[i] = UINT16_MAX;
	}
}

hrt_abstime CollisionPrevention::get_సమయం()
{
	తిరిగి hrt_absolute_సమయం();
}

hrt_abstime CollisionPrevention::get_Elapsed_సమయం(స్థిరరాశి hrt_abstime *ptr)
{
	తిరిగి hrt_absolute_సమయం() - *ptr;
}

తార్కిక CollisionPrevention::is_చురుకైన()
{
	తార్కిక activated = _param_cp_dist.get() > 0;

	ఒకవేళ (activated && !was_చురుకైన) {
		సమయం_activated = get_సమయం();
	}

	was_చురుకైన = activated;
	తిరిగి activated;
}

శూన్య CollisionPrevention::modifySetpoint(Vector2f &setpoint_accel, స్థిరరాశి Vector2f &setpoint_vel)
{
	ఒకవేళ (_vehicle_attitude_sub.updated()) {
		vehicle_attitude_s vehicle_attitude;

		ఒకవేళ (_vehicle_attitude_sub.copy(&vehicle_attitude)) {
			_vehicle_attitude = Quatf(vehicle_attitude.q);
			_vehicle_yaw = Eulerf(_vehicle_attitude).psi();
		}
	}

	//calculate movement constraints based on range data
	స్థిరరాశి Vector2f original_setpoint = setpoint_accel;
	_updateObstacleMap();
	update_Obstacle_దత్తాంశం();
	_calculateConstrainedSetpoint(setpoint_accel, setpoint_vel);

	// publish constraints
	collision_constraints_s	constraints{};
	original_setpoint.copy_గమ్యం(constraints.original_setpoint);
	setpoint_accel.copy_గమ్యం(constraints.adapted_setpoint);
	constraints.timestamp = get_సమయం();
	_constraints_pub.publish(constraints);
}

శూన్య CollisionPrevention::_updateObstacleMap()
{
	// add distance sensor data
	కొరకు (స్వయంచాలక &dist_sens_sub : దూరం_sensor_subs) {
		దూరం_sensor_s దూరం_sensor;

		ఒకవేళ (dist_sens_sub.update(&దూరం_sensor)) {
			// consider only instances with valid data and orientations useful for collision prevention
			ఒకవేళ ((get_Elapsed_సమయం(&దూరం_sensor.timestamp) < RANGE_ప్రవాహం_TIMEOUT_US) &&
			    (దూరం_sensor.orientation != దూరం_sensor_s::ROTATION_DOWNWARD_FACING) &&
			    (దూరం_sensor.orientation != దూరం_sensor_s::ROTATION_UPWARD_FACING)) {

				// update message description
				obstacle_map_body_చట్రం.timestamp = math::max(obstacle_map_body_చట్రం.timestamp, దూరం_sensor.timestamp);
				obstacle_map_body_చట్రం.max_దూరం = math::max(obstacle_map_body_చట్రం.max_దూరం,
									(uint16_t)(దూరం_sensor.max_దూరం * 100.0f));
				obstacle_map_body_చట్రం.min_దూరం = math::min(obstacle_map_body_చట్రం.min_దూరం,
									(uint16_t)(దూరం_sensor.min_దూరం * 100.0f));

				కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, _vehicle_attitude);
			}
		}
	}

	// add obstacle distance data
	ఒకవేళ (sub_obstacle_దూరం.update()) {
		స్థిరరాశి obstacle_దూరం_s &obstacle_దూరం = sub_obstacle_దూరం.get();

		// Update map with obstacle data if the data is not stale
		ఒకవేళ (get_Elapsed_సమయం(&obstacle_దూరం.timestamp) < RANGE_ప్రవాహం_TIMEOUT_US && obstacle_దూరం.increment > 0.f) {
			//update message description
			obstacle_map_body_చట్రం.timestamp = math::max(obstacle_map_body_చట్రం.timestamp, obstacle_దూరం.timestamp);
			obstacle_map_body_చట్రం.max_దూరం = math::max(obstacle_map_body_చట్రం.max_దూరం,
								obstacle_దూరం.max_దూరం);
			obstacle_map_body_చట్రం.min_దూరం = math::min(obstacle_map_body_చట్రం.min_దూరం,
								obstacle_దూరం.min_దూరం);
			కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_దూరం, _vehicle_yaw);
		}
	}

	// publish fused obtacle distance message with data from offboard obstacle_distance and distance sensor
	obstacle_దూరం_fused_pub.publish(obstacle_map_body_చట్రం);
}

శూన్య CollisionPrevention::update_Obstacle_దత్తాంశం()
{
	obstacle_దత్తాంశం_present = అబద్ధం;
	_closest_dist = UINT16_MAX;
	_closest_dist_dir.సమితి_Zero();

	కొరకు (పూర్ణాంకం i = 0; i < BIN_లెక్క; i++) {
		// if the data is stale, reset the bin
		ఒకవేళ (get_సమయం() - దత్తాంశం_timestamps[i] > RANGE_ప్రవాహం_TIMEOUT_US) {
			obstacle_map_body_చట్రం.distances[i] = UINT16_MAX;
		}

		దశాంశం angle = wrap_2pi(_vehicle_yaw + math::radians((దశాంశం)i * BIN_కొలత +
				       obstacle_map_body_చట్రం.angle_స్థానభ్రంశం));
		స్థిరరాశి Vector2f bin_direction = {cosf(angle), sinf(angle)};
		స్థిరరాశి uint16_t bin_దూరం = obstacle_map_body_చట్రం.distances[i];

		// check if there is avaliable data and the data of the map is not stale
		ఒకవేళ (bin_దూరం < UINT16_MAX
		    && (get_సమయం() - obstacle_map_body_చట్రం.timestamp) < RANGE_ప్రవాహం_TIMEOUT_US) {
			obstacle_దత్తాంశం_present = నిజం;
		}

		ఒకవేళ (bin_దూరం * 0.01f < _closest_dist) {
			_closest_dist = bin_దూరం * 0.01f;
			_closest_dist_dir = bin_direction;
		}
	}
}

శూన్య CollisionPrevention::_calculateConstrainedSetpoint(Vector2f &setpoint_accel, స్థిరరాశి Vector2f &setpoint_vel)
{
	స్థిరరాశి దశాంశం setpoint_నిడివి = setpoint_accel.norm();
	min_dist_గమ్యం_keep = math::max(obstacle_map_body_చట్రం.min_దూరం / 100.0f, _param_cp_dist.get());

	స్థిరరాశి hrt_abstime now = get_సమయం();

	స్థిరరాశి తార్కిక is_stick_deflected = setpoint_నిడివి > 0.001f;

	ఒకవేళ (obstacle_దత్తాంశం_present && is_stick_deflected) {

		_transformSetpoint(setpoint_accel);

		దశాంశం vel_comp_accel = INFINITY;
		Vector2f vel_comp_accel_dir{};

		_getVelocityCompensationAcceleration(_vehicle_yaw, setpoint_vel, now,
						     vel_comp_accel, vel_comp_accel_dir);

		Vector2f constr_accel_setpoint{};

		ఒకవేళ (_checkSetpointDirectionFeasability()) {
			constr_accel_setpoint = _constrainAccelerationSetpoint(setpoint_నిడివి);
		}

		setpoint_accel = constr_accel_setpoint + vel_comp_accel * vel_comp_accel_dir;

	} లేకపోతే ఒకవేళ (!obstacle_దత్తాంశం_present) {
		// allow no movement
		setpoint_accel.సమితి_Zero();

		// if distance data is stale, switch to Loiter
		ఒకవేళ (get_Elapsed_సమయం(&చివరి_timeout_warning) > 1_s && get_Elapsed_సమయం(&సమయం_activated) > 1_s) {
			ఒకవేళ ((now - obstacle_map_body_చట్రం.timestamp) > TIMEOUT_HOLD_US &&
			    get_Elapsed_సమయం(&సమయం_activated) > TIMEOUT_HOLD_US) {
				_publishVehicleCmdDoLoiter();
			}

			PX4_WARN("No obstacle data, not moving...");
			చివరి_timeout_warning = now;
		}
	}
}

// TODO this gives false output if the offset is not a multiple of the resolution. to be fixed...
శూన్య CollisionPrevention::కూడు_Obstacle_Sensor_దత్తాంశం(స్థిరరాశి obstacle_దూరం_s &obstacle, స్థిరరాశి దశాంశం vehicle_yaw)
{

	దశాంశం vehicle_orientation_deg = math::degrees(vehicle_yaw);


	ఒకవేళ (obstacle.చట్రం == obstacle.MAV_చట్రం_GLOBAL || obstacle.చట్రం == obstacle.MAV_చట్రం_LOCAL_NED) {
		// Obstacle message arrives in local_origin frame (north aligned)
		// corresponding data index (convert to world frame and shift by msg offset)
		కొరకు (పూర్ణాంకం i = 0; i < BIN_లెక్క; i++) {
			కొరకు (పూర్ణాంకం j = 0; (j < 360 / obstacle.increment) && (j < BIN_లెక్క); j++) {
				దశాంశం bin_క్రింది_angle = ObstacleMath::get_క్రింది_bound_angle(i, obstacle_map_body_చట్రం.increment,
							obstacle_map_body_చట్రం.angle_స్థానభ్రంశం);
				దశాంశం bin_పై_angle = ObstacleMath::get_క్రింది_bound_angle(i + 1, obstacle_map_body_చట్రం.increment,
							obstacle_map_body_చట్రం.angle_స్థానభ్రంశం);
				దశాంశం సందేశం_క్రింది_angle = ObstacleMath::get_క్రింది_bound_angle(j, obstacle.increment,
							obstacle.angle_స్థానభ్రంశం - vehicle_orientation_deg);
				దశాంశం సందేశం_పై_angle = ObstacleMath::get_క్రింది_bound_angle(j + 1, obstacle.increment,
							obstacle.angle_స్థానభ్రంశం - vehicle_orientation_deg);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				ఒకవేళ (bin_క్రింది_angle > bin_పై_angle) {
					bin_క్రింది_angle -= 360;
				}

				ఒకవేళ (సందేశం_క్రింది_angle > సందేశం_పై_angle) {
					సందేశం_క్రింది_angle -= 360;
				}

				// Check for overlaps.
				ఒకవేళ ((సందేశం_క్రింది_angle > bin_క్రింది_angle && సందేశం_క్రింది_angle < bin_పై_angle) ||
				    (సందేశం_పై_angle > bin_క్రింది_angle && సందేశం_పై_angle < bin_పై_angle) ||
				    (సందేశం_క్రింది_angle <= bin_క్రింది_angle && సందేశం_పై_angle >= bin_పై_angle) ||
				    (సందేశం_క్రింది_angle >= bin_క్రింది_angle && సందేశం_పై_angle <= bin_పై_angle)) {
					ఒకవేళ (obstacle.distances[j] != UINT16_MAX) {
						ఒకవేళ (enter_దత్తాంశం(i, obstacle.max_దూరం * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_చట్రం.distances[i] = obstacle.distances[j];
							దత్తాంశం_timestamps[i] = obstacle_map_body_చట్రం.timestamp;
							దత్తాంశం_maxranges[i] = obstacle.max_దూరం;
							దత్తాంశం_fov[i] = 1;
						}
					}
				}

			}
		}

	} లేకపోతే ఒకవేళ (obstacle.చట్రం == obstacle.MAV_చట్రం_BODY_FRD) {
		// Obstacle message arrives in body frame (front aligned)
		// corresponding data index (shift by msg offset)
		కొరకు (పూర్ణాంకం i = 0; i < BIN_లెక్క; i++) {
			కొరకు (పూర్ణాంకం j = 0; (j < 360 / obstacle.increment) && (j < BIN_లెక్క); j++) {
				దశాంశం bin_క్రింది_angle = ObstacleMath::get_క్రింది_bound_angle(i, obstacle_map_body_చట్రం.increment,
							obstacle_map_body_చట్రం.angle_స్థానభ్రంశం);
				దశాంశం bin_పై_angle = ObstacleMath::get_క్రింది_bound_angle(i + 1, obstacle_map_body_చట్రం.increment,
							obstacle_map_body_చట్రం.angle_స్థానభ్రంశం);
				దశాంశం సందేశం_క్రింది_angle = ObstacleMath::get_క్రింది_bound_angle(j, obstacle.increment, obstacle.angle_స్థానభ్రంశం);
				దశాంశం సందేశం_పై_angle = ObstacleMath::get_క్రింది_bound_angle(j + 1, obstacle.increment, obstacle.angle_స్థానభ్రంశం);

				// if a bin stretches over the 0/360 degree line, adjust the angles
				ఒకవేళ (bin_క్రింది_angle > bin_పై_angle) {
					bin_క్రింది_angle -= 360;
				}

				ఒకవేళ (సందేశం_క్రింది_angle > సందేశం_పై_angle) {
					సందేశం_క్రింది_angle -= 360;
				}

				// Check for overlaps.
				ఒకవేళ ((సందేశం_క్రింది_angle > bin_క్రింది_angle && సందేశం_క్రింది_angle < bin_పై_angle) ||
				    (సందేశం_పై_angle > bin_క్రింది_angle && సందేశం_పై_angle < bin_పై_angle) ||
				    (సందేశం_క్రింది_angle <= bin_క్రింది_angle && సందేశం_పై_angle >= bin_పై_angle) ||
				    (సందేశం_క్రింది_angle >= bin_క్రింది_angle && సందేశం_పై_angle <= bin_పై_angle)) {
					ఒకవేళ (obstacle.distances[j] != UINT16_MAX) {

						ఒకవేళ (enter_దత్తాంశం(i, obstacle.max_దూరం * 0.01f, obstacle.distances[j] * 0.01f)) {
							obstacle_map_body_చట్రం.distances[i] = obstacle.distances[j];
							దత్తాంశం_timestamps[i] = obstacle_map_body_చట్రం.timestamp;
							దత్తాంశం_maxranges[i] = obstacle.max_దూరం;
							దత్తాంశం_fov[i] = 1;
						}
					}
				}

			}
		}

	} లేకపోతే {
		mavlink_log_critical(&_mavlink_log_pub, "Obstacle message received in unsupported frame %i\t",
				     obstacle.చట్రం);
		events::send<uint8_t>(events::ID("col_prev_unsup_frame"), events::Log::Error,
				      "Obstacle message received in unsupported frame {1}", obstacle.చట్రం);
	}
}

తార్కిక
CollisionPrevention::enter_దత్తాంశం(పూర్ణాంకం map_సూచీ, దశాంశం sensor_range, దశాంశం sensor_reading)
{
	//use data from this sensor if:
	//1. this sensor data is in range, the bin contains already valid data and this data is coming from the same or less range sensor
	//2. this sensor data is in range, and the last reading was out of range
	//3. this sensor data is out of range, the last reading was as well and this is the sensor with longest range
	//4. this sensor data is out of range, the last reading was valid and coming from the same sensor

	uint16_t sensor_range_cm = static_cast<uint16_t>(lroundf(100.0f * sensor_range)); //convert to cm

	ఒకవేళ (sensor_reading < sensor_range) {
		ఒకవేళ ((obstacle_map_body_చట్రం.distances[map_సూచీ] < దత్తాంశం_maxranges[map_సూచీ]
		     && sensor_range_cm <= దత్తాంశం_maxranges[map_సూచీ])
		    || obstacle_map_body_చట్రం.distances[map_సూచీ] >= దత్తాంశం_maxranges[map_సూచీ]) {

			తిరిగి నిజం;
		}

	} లేకపోతే {
		ఒకవేళ ((obstacle_map_body_చట్రం.distances[map_సూచీ] >= దత్తాంశం_maxranges[map_సూచీ]
		     && sensor_range_cm >= దత్తాంశం_maxranges[map_సూచీ])
		    || (obstacle_map_body_చట్రం.distances[map_సూచీ] < దత్తాంశం_maxranges[map_సూచీ]
			&& sensor_range_cm == దత్తాంశం_maxranges[map_సూచీ])) {

			తిరిగి నిజం;
		}
	}

	తిరిగి అబద్ధం;
}

తార్కిక
CollisionPrevention::_checkSetpointDirectionFeasability()
{
	ఒకవేళ (setpoint_సూచీ < 0 || setpoint_సూచీ >= BIN_లెక్క) {
		తిరిగి అబద్ధం; // treat out-of-bounds as unsafe
	}

	స్థిరరాశి తార్కిక no_దత్తాంశం = (obstacle_map_body_చట్రం.distances[setpoint_సూచీ] == UINT16_MAX);
	స్థిరరాశి తార్కిక allow_movement_towards_no_దత్తాంశం = param_cp_go_no_దత్తాంశం.get();
	స్థిరరాశి తార్కిక fov_at_setpoint = దత్తాంశం_fov[setpoint_సూచీ];

	// The setpoint is feasible if:
	// 1. There is actual data at the setpoint (no_data == false), OR
	// 2. There is no data, but movement into no-data bins is allowed and the setpoint is outside the sensor FOV.
	తిరిగి !no_దత్తాంశం || (allow_movement_towards_no_దత్తాంశం && !fov_at_setpoint);
}

శూన్య
CollisionPrevention::_transformSetpoint(స్థిరరాశి Vector2f &setpoint)
{
	స్థిరరాశి దశాంశం sp_angle_body_చట్రం = atan2f(setpoint(1), setpoint(0)) - _vehicle_yaw;
	స్థిరరాశి దశాంశం sp_angle_with_స్థానభ్రంశం_deg = ObstacleMath::wrap_360(math::degrees(sp_angle_body_చట్రం) -
					       obstacle_map_body_చట్రం.angle_స్థానభ్రంశం);
	setpoint_సూచీ = floor(sp_angle_with_స్థానభ్రంశం_deg / BIN_కొలత);
	// change setpoint direction slightly (max by _param_cp_guide_ang degrees) to help guide through narrow gaps
	_setpoint_dir = setpoint.unit_or_zero();
	_adaptSetpointDirection(_setpoint_dir, setpoint_సూచీ, _vehicle_yaw);
}

శూన్య
CollisionPrevention::కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor_s &దూరం_sensor, స్థిరరాశి Quatf &vehicle_attitude)
{
	// clamp at maximum sensor range
	దశాంశం దూరం_reading = math::min(దూరం_sensor.current_దూరం, దూరం_sensor.max_దూరం);

	// negative values indicate out of range but valid measurements.
	ఒకవేళ (fabsf(దూరం_sensor.current_దూరం - -1.f) < FLT_EPSILON && దూరం_sensor.signal_quality == 0) {
		దూరం_reading = దూరం_sensor.max_దూరం;
	}

	// discard values below min range
	ఒకవేళ (దూరం_reading > దూరం_sensor.min_దూరం) {
		దశాంశం sensor_yaw_body_rad = ObstacleMath::sensor_orientation_గమ్యం_yaw_స్థానభ్రంశం(static_cast<ObstacleMath::SensorOrientation>
					    (దూరం_sensor.orientation), దూరం_sensor.q);
		దశాంశం sensor_yaw_body_deg = math::degrees(wrap_2pi(sensor_yaw_body_rad));

		// calculate the field of view boundary bin indices
		పూర్ణాంకం క్రింది_bound = (పూర్ణాంకం)round((sensor_yaw_body_deg  - math::degrees(దూరం_sensor.h_fov / 2.0f)) / BIN_కొలత);
		పూర్ణాంకం పై_bound = (పూర్ణాంకం)round((sensor_yaw_body_deg  + math::degrees(దూరం_sensor.h_fov / 2.0f)) / BIN_కొలత);

		ఒకవేళ (దూరం_reading < దూరం_sensor.max_దూరం) {
			ObstacleMath::project_దూరం_on_horizontal_plane(దూరం_reading, sensor_yaw_body_rad, vehicle_attitude);
		}

		uint16_t sensor_range = static_cast<uint16_t>(lroundf(100.0f * దూరం_sensor.max_దూరం)); // convert to cm

		కొరకు (పూర్ణాంకం bin = క్రింది_bound; bin <= పై_bound; ++bin) {
			పూర్ణాంకం wrapped_bin = ObstacleMath::wrap_bin(bin, BIN_లెక్క);

			ఒకవేళ (enter_దత్తాంశం(wrapped_bin, దూరం_sensor.max_దూరం, దూరం_reading)) {
				obstacle_map_body_చట్రం.distances[wrapped_bin] = static_cast<uint16_t>(lroundf(100.0f * దూరం_reading));
				దత్తాంశం_timestamps[wrapped_bin] = obstacle_map_body_చట్రం.timestamp;
				దత్తాంశం_maxranges[wrapped_bin] = sensor_range;
				దత్తాంశం_fov[wrapped_bin] = 1;
			}
		}
	}
}

శూన్య
CollisionPrevention::_adaptSetpointDirection(Vector2f &setpoint_dir, పూర్ణాంకం &setpoint_సూచీ_2, దశాంశం vehicle_yaw_angle_rad)
{
	స్థిరరాశి పూర్ణాంకం guidance_bins = floor(_param_cp_guide_ang.get() / BIN_కొలత);
	స్థిరరాశి పూర్ణాంకం sp_సూచీ_original = setpoint_సూచీ_2;
	దశాంశం ఉత్తమ_ఖర్చు = 9999.f;
	పూర్ణాంకం new_sp_సూచీ = setpoint_సూచీ_2;

	కొరకు (పూర్ణాంకం i = sp_సూచీ_original - guidance_bins; i <= sp_సూచీ_original + guidance_bins; i++) {

		// apply moving average filter to the distance array to be able to center in larger gaps
		స్థిరరాశి పూర్ణాంకం filter_కొలత = 1;
		దశాంశం సగటు_dist = 0;

		కొరకు (పూర్ణాంకం j = i - filter_కొలత; j <= i + filter_కొలత; j++) {
			పూర్ణాంకం bin = ObstacleMath::wrap_bin(j, BIN_లెక్క);

			ఒకవేళ (obstacle_map_body_చట్రం.distances[bin] == UINT16_MAX) {
				సగటు_dist += _param_cp_dist.get() * 100.f;

			} లేకపోతే {
				సగటు_dist += obstacle_map_body_చట్రం.distances[bin];
			}
		}

		స్థిరరాశి పూర్ణాంకం bin = ObstacleMath::wrap_bin(i, BIN_లెక్క);
		సగటు_dist = సగటు_dist / (2.f * filter_కొలత + 1.f);
		స్థిరరాశి దశాంశం deviation_ఖర్చు = _param_cp_dist.get() * 50.f * నిరపేక్ష(i - sp_సూచీ_original);
		స్థిరరాశి దశాంశం bin_ఖర్చు = deviation_ఖర్చు - సగటు_dist - obstacle_map_body_చట్రం.distances[bin];

		ఒకవేళ (bin_ఖర్చు < ఉత్తమ_ఖర్చు && obstacle_map_body_చట్రం.distances[bin] != UINT16_MAX) {
			ఉత్తమ_ఖర్చు = bin_ఖర్చు;
			new_sp_సూచీ = bin;
		}
	}

	//only change setpoint direction if it was moved to a different bin
	ఒకవేళ (new_sp_సూచీ != setpoint_సూచీ_2) {
		దశాంశం angle = math::radians((దశాంశం)new_sp_సూచీ * BIN_కొలత + obstacle_map_body_చట్రం.angle_స్థానభ్రంశం);
		angle = wrap_2pi(vehicle_yaw_angle_rad + angle);
		setpoint_dir = {cosf(angle), sinf(angle)};
		setpoint_సూచీ_2 = new_sp_సూచీ;
	}
}

దశాంశం CollisionPrevention::get_Obstacle_దూరం(స్థిరరాశి Vector2f &direction)
{
	దశాంశం obstacle_దూరం = 0.f;
	స్థిరరాశి దశాంశం direction_norm = direction.norm();

	ఒకవేళ (direction_norm > FLT_EPSILON) {
		Vector2f dir = direction / direction_norm;
		స్థిరరాశి దశాంశం sp_angle_body_చట్రం = atan2f(dir(1), dir(0)) - _vehicle_yaw;
		స్థిరరాశి దశాంశం sp_angle_with_స్థానభ్రంశం_deg =
			ObstacleMath::wrap_360(math::degrees(sp_angle_body_చట్రం) - obstacle_map_body_చట్రం.angle_స్థానభ్రంశం);

		స్థిరరాశి పూర్ణాంకం dir_సూచీ = ObstacleMath::get_bin_at_angle(BIN_కొలత, sp_angle_with_స్థానభ్రంశం_deg);
		obstacle_దూరం   = obstacle_map_body_చట్రం.distances[dir_సూచీ] * 0.01f;
	}

	తిరిగి obstacle_దూరం;
}

Vector2f
CollisionPrevention::_constrainAccelerationSetpoint(స్థిరరాశి దశాంశం &setpoint_నిడివి)
{
	Vector2f new_setpoint{};
	స్థిరరాశి Vector2f సాధారణ_component = _closest_dist_dir * (_setpoint_dir.dot(_closest_dist_dir));
	స్థిరరాశి Vector2f tangential_component = _setpoint_dir - సాధారణ_component;

	స్థిరరాశి దశాంశం సాధారణ_scale = _getScale(_closest_dist);


	స్థిరరాశి దశాంశం closest_dist_tangential = get_Obstacle_దూరం(tangential_component);
	స్థిరరాశి దశాంశం tangential_scale = _getScale(closest_dist_tangential);


	// only scale accelerations towards the obstacle
	ఒకవేళ (_closest_dist_dir.dot(_setpoint_dir) > 0) {
		new_setpoint = (tangential_component * tangential_scale + సాధారణ_component * సాధారణ_scale) * setpoint_నిడివి;

	} లేకపోతే {
		new_setpoint = _setpoint_dir * setpoint_నిడివి;
	}

	తిరిగి new_setpoint;
}

దశాంశం
CollisionPrevention::_getScale(స్థిరరాశి దశాంశం &reference_దూరం)
{
	దశాంశం scale = (reference_దూరం - min_dist_గమ్యం_keep);
	స్థిరరాశి దశాంశం scale_దూరం = math::max(min_dist_గమ్యం_keep, _param_mpc_vel_manual.get() / _param_mpc_xy_p.get());

	// if scale is positive, square it and scale it with the scale_distance
	scale = scale > 0 ? powf(scale / scale_దూరం, 2) : scale;
	scale = math::min(scale, 1.0f);
	తిరిగి scale;
}

శూన్య CollisionPrevention::_getVelocityCompensationAcceleration(స్థిరరాశి దశాంశం vehicle_yaw_angle_rad,
		స్థిరరాశి Vector2f &setpoint_vel,
		స్థిరరాశి hrt_abstime now, దశాంశం &vel_comp_accel, Vector2f &vel_comp_accel_dir)
{
	కొరకు (పూర్ణాంకం i = 0; i < BIN_లెక్క; i++) {
		స్థిరరాశి దశాంశం max_range = దత్తాంశం_maxranges[i] * 0.01f;

		// get the vector pointing into the direction of current bin
		దశాంశం bin_angle = wrap_2pi(vehicle_yaw_angle_rad
					   + math::radians((దశాంశం)i * BIN_కొలత + obstacle_map_body_చట్రం.angle_స్థానభ్రంశం));

		స్థిరరాశి Vector2f bin_direction = { cosf(bin_angle), sinf(bin_angle) };
		దశాంశం bin_దూరం = obstacle_map_body_చట్రం.distances[i];

		// only consider bins which are between min and max values
		ఒకవేళ (bin_దూరం > obstacle_map_body_చట్రం.min_దూరం && bin_దూరం < UINT16_MAX) {
			స్థిరరాశి దశాంశం దూరం = bin_దూరం * 0.01f;

			// Assume current velocity is sufficiently close to the setpoint velocity, this breaks down if flying high
			// acceleration maneuvers
			స్థిరరాశి దశాంశం curr_vel_parallel = math::max(0.f, setpoint_vel.dot(bin_direction));
			దశాంశం delay_దూరం = curr_vel_parallel * _param_cp_delay.get();

			స్థిరరాశి hrt_abstime దత్తాంశం_age = now - దత్తాంశం_timestamps[i];

			ఒకవేళ (దూరం < max_range) {
				delay_దూరం += curr_vel_parallel * (దత్తాంశం_age * 1e-6f);
			}

			స్థిరరాశి దశాంశం stop_దూరం = దూరం - min_dist_గమ్యం_keep - delay_దూరం;

			దశాంశం curr_acc_vel_constraint;

			ఒకవేళ (stop_దూరం >= 0.f) {
				స్థిరరాశి దశాంశం max_vel = math::trajectory::compute_Max_వేగం_From_దూరం(_param_mpc_jerk_max.get(),
						      _param_mpc_acc_hor.get(), stop_దూరం, 0.f);
				curr_acc_vel_constraint = _param_mpc_xy_vel_p_acc.get() * math::min(max_vel - curr_vel_parallel, 0.f);

			} లేకపోతే {
				curr_acc_vel_constraint = -1.f * _param_mpc_xy_vel_p_acc.get() * curr_vel_parallel;
			}

			ఒకవేళ (curr_acc_vel_constraint < vel_comp_accel) {
				vel_comp_accel = curr_acc_vel_constraint;
				vel_comp_accel_dir = bin_direction;
			}
		}
	}
}

శూన్య CollisionPrevention::_publishVehicleCmdDoLoiter()
{
	vehicle_command_s command{};
	command.command = vehicle_command_s::VEHICLE_CMD_DO_సమితి_బహుళకం;
	command.param1 = 1.f; // base mode VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
	command.param2 = (దశాంశం)PX4_CUSTOM_MAIN_బహుళకం_AUTO;
	command.param3 = (దశాంశం)PX4_CUSTOM_SUB_బహుళకం_AUTO_LOITER;
	command.లక్ష్యం_system = 1;
	command.లక్ష్యం_component = 1;
	command.ఆధారం_system = 1;
	command.ఆధారం_component = 1;
	command.confirmation = అబద్ధం;
	command.from_external = అబద్ధం;
	command.timestamp = get_సమయం();
	_vehicle_command_pub.publish(command);
}
