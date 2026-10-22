/****************************************************************************
 *
 *   Copyright (c) 2020-2022 PX4 Development Team. All rights reserved.
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

#pragma once

#include <lib/conversion/rotation.h>
#include <lib/matrix/matrix/math.hpp>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/sensor_correction.h>

namespace calibration
{
class Magnetometer
{
public:
	static constexpr 整数 MAX_SENSOR_COUNT = 4;

	static constexpr uint8_t DEFAULT_PRIORITY = 50;
	static constexpr uint8_t DEFAULT_EXTERNAL_PRIORITY = 75;

	static constexpr const char *SensorString() { 戻る "MAG"; }

	Magnetometer();
	explicit Magnetometer(uint32_t device_id);
	Magnetometer(uint32_t device_id, bool external);

	~Magnetometer() = default;

	void PrintStatus();

	bool set_calibration_index(整数 calibration_index);
	void set_device_id(uint32_t device_id); ///< classification falls back to the device id's bus
	void set_device_id(uint32_t device_id, bool external);
	bool set_offset(const matrix::Vector3f &offset);
	bool set_scale(const matrix::Vector3f &scale);
	bool set_offdiagonal(const matrix::Vector3f &offdiagonal);

	/**
	 * @brief Set the rotation enum & corresponding rotation matrix for Magnetometer
	 *
	 * @param rotation Rotation enum
	 */
	void set_rotation(Rotation rotation);

	/**
	 * @brief Set the custom rotation & rotation enum to ROTATION_CUSTOM for Magnetometer
	 *
	 * @param rotation Rotation euler angles
	 */
	void set_custom_rotation(const matrix::Eulerf &rotation);

	bool calibrated() const { 戻る (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_count() const { 戻る _calibration_count; }
	int8_t calibration_index() const { 戻る _calibration_index; }
	uint32_t device_id() const { 戻る _device_id; }
	bool enabled() const { 戻る (_priority > 0); }
	void disable() { _priority = 0; }
	bool external() const { 戻る _external; }
	const matrix::Vector3f &offset() const { 戻る _offset; }
	const int32_t &priority() const { 戻る _priority; }
	const matrix::Dcmf &rotation() const { 戻る _rotation; }
	const Rotation &rotation_enum() const { 戻る _rotation_enum; }
	const matrix::Matrix3f &scale() const { 戻る _scale; }

	// apply offsets and scale
	// rotate corrected measurements from sensor to body frame
	inline matrix::Vector3f Correct(const matrix::Vector3f &data) const
	{
		戻る _rotation * (_scale * ((data + _power * _power_compensation) - _offset));
	}

	// Compute sensor offset from bias (board frame)
	matrix::Vector3f BiasCorrectedSensorOffset(const matrix::Vector3f &bias) const
	{
		// updated calibration offset = existing offset + bias rotated to sensor frame and unscaled
		戻る _offset + (_scale.I() * _rotation.T() * bias);
	}

	bool ParametersLoad();
	bool ParametersSave(整数 desired_calibration_index = -1, bool force = false);
	void ParametersUpdate();

	void Reset();

	void SensorCorrectionsUpdate(bool force = false);

	void UpdatePower(float power) { _power = power; }

private:
	uORB::Subscription _sensor_correction_sub{ORB_ID(sensor_correction)};

	Rotation _rotation_enum{ROTATION_NONE};

	/**
	 * @brief 3 x 3 Rotation matrix that translates from sensor frame (XYZ) to vehicle body frame (FRD)
	 */
	matrix::Dcmf _rotation;

	matrix::Eulerf _rotation_custom_euler{0.f, 0.f, 0.f}; // custom rotation euler angles (optional)

	matrix::Vector3f _offset;
	matrix::Matrix3f _scale;
	matrix::Vector3f _thermal_offset;
	matrix::Vector3f _power_compensation;

	float _power{0.f};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	int32_t _priority{-1};

	bool _external{false};

	uint8_t _calibration_count{0};
};
} // namespace calibration
