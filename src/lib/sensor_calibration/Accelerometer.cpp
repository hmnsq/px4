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

#ଅନ୍ତର୍ଭାଵ "Accelerometer.hpp"

#ଅନ୍ତର୍ଭାଵ "Utilities.hpp"

#ଅନ୍ତର୍ଭାଵ <lib/drivers/device/Device.hpp>
#ଅନ୍ତର୍ଭାଵ <lib/parameters/param.h>

using ନାମାକାଶ matrix;
using ନାମାକାଶ time_literals;

ନାମାକାଶ calibration
{

Accelerometer::Accelerometer()
{
	Reset();
}

Accelerometer::Accelerometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Accelerometer::Accelerometer(uint32_t device_id, ତାର୍କିକ external)
{
	set_device_id(device_id, external);
}

ଶୂନ୍ୟ Accelerometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

ଶୂନ୍ୟ Accelerometer::set_device_id(uint32_t device_id, ତାର୍କିକ external)
{
	ଯଦି (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(ସତ୍ୟ);
	}
}

ଶୂନ୍ୟ Accelerometer::SensorCorrectionsUpdate(ତାର୍କିକ force)
{
	// check if the selected sensor has updated
	ଯଦି (_sensor_correction_sub.updated() || force) {

		// valid device id required
		ଯଦି (_device_id == 0) {
			ଫେରନ୍ତୁ;
		}

		sensor_correction_s corrections;

		ଯଦି (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_SENSOR_ଗଣନା; i++) {
				ଯଦି (corrections.accel_device_ids[i] == _device_id) {
					ବାଛ (i) {
					ସ୍ଥିତି 0:
						_thermal_offset = Vector3f{corrections.accel_offset_0};
						ଫେରନ୍ତୁ;
					ସ୍ଥିତି 1:
						_thermal_offset = Vector3f{corrections.accel_offset_1};
						ଫେରନ୍ତୁ;
					ସ୍ଥିତି 2:
						_thermal_offset = Vector3f{corrections.accel_offset_2};
						ଫେରନ୍ତୁ;
					ସ୍ଥିତି 3:
						_thermal_offset = Vector3f{corrections.accel_offset_3};
						ଫେରନ୍ତୁ;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

ତାର୍କିକ Accelerometer::set_offset(ଅଚଳ Vector3f &offset)
{
	ଯଦି (Vector3f(_offset - offset).longerThan(0.01f)) {
		ଯଦି (offset.isAllFinite()) {
			_offset = offset;
			calibration_ଗଣନା++;
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ Accelerometer::set_scale(ଅଚଳ Vector3f &scale)
{
	ଯଦି (Vector3f(_scale - scale).longerThan(0.01f)) {
		ଯଦି (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale = scale;
			calibration_ଗଣନା++;
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Accelerometer::set_rotation(Rotation rotation)
{
	_rotation_enum = rotation;

	// always apply board level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(rotation);
}

ତାର୍କିକ Accelerometer::set_calibration_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା calibration_index)
{
	ଯଦି ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_ଗଣନା)) {
		_calibration_index = calibration_index;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Accelerometer::ParametersUpdate()
{
	ଯଦି (_device_id == 0) {
		ଫେରନ୍ତୁ;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	ଯଦି (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} ଅନ୍ୟଥା {
		ParametersLoad();
	}
}

ତାର୍କିକ Accelerometer::ParametersLoad()
{
	ଯଦି (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ଗଣନା) {
		// CAL_ACCx_ROT
		int32_t rotation_ମୂଲ୍ୟ = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		ଯଦି (_external) {
			ଯଦି ((rotation_ମୂଲ୍ୟ >= ROTATION_MAX) || (rotation_ମୂଲ୍ୟ < 0)) {
				// invalid rotation, resetting
				rotation_ମୂଲ୍ୟ = ROTATION_NONE;
			}

			set_rotation(static_cast<Rotation>(rotation_ମୂଲ୍ୟ));

		} ଅନ୍ୟଥା {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_ACCx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		ଯଦି ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			ସ୍ଥିର constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			ଯଦି (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_ACCx_OFF{X,Y,Z}
		set_offset(GetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index));

		// CAL_ACCx_SCALE{X,Y,Z}
		set_scale(GetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index));

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Accelerometer::Reset()
{
	ଯଦି (_external) {
		set_rotation(ROTATION_NONE);

	} ଅନ୍ୟଥା {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();
	_scale = Vector3f{1.f, 1.f, 1.f};

	_thermal_offset.zero();

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_ଗଣନା = 0;
}

ତାର୍କିକ Accelerometer::ParametersSave(ପୂର୍ଣ୍ଣସଂଖ୍ୟା desired_calibration_index, ତାର୍କିକ force)
{
	ଯଦି (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_ଗଣନା) {
		_calibration_index = desired_calibration_index;

	} ଅନ୍ୟଥା ଯଦି (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		ଯଦି (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	ଯଦି (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ଗଣନା) {
		// save calibration
		ତାର୍କିକ success = ସତ୍ୟ;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index, _offset);
		success &= SetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index, _scale);

		ଯଦି (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} ଅନ୍ୟଥା {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		ଫେରନ୍ତୁ success;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Accelerometer::PrintStatus()
{
	ଯଦି (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (ଦ୍ୱିଗୁଣ)_offset(0), (ଦ୍ୱିଗୁଣ)_offset(1), (ଦ୍ୱିଗୁଣ)_offset(2),
			     (ଦ୍ୱିଗୁଣ)_scale(0), (ଦ୍ୱିଗୁଣ)_scale(1), (ଦ୍ୱିଗୁଣ)_scale(2),
			     rotation_enum());

	} ଅନ୍ୟଥା {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (ଦ୍ୱିଗୁଣ)_offset(0), (ଦ୍ୱିଗୁଣ)_offset(1), (ଦ୍ୱିଗୁଣ)_offset(2),
			     (ଦ୍ୱିଗୁଣ)_scale(0), (ଦ୍ୱିଗୁଣ)_scale(1), (ଦ୍ୱିଗୁଣ)_scale(2));
	}

	ଯଦି (_thermal_offset.norm() > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: [%.4f %.4f %.4f]\n", SensorString(), _device_id,
			     (ଦ୍ୱିଗୁଣ)_thermal_offset(0), (ଦ୍ୱିଗୁଣ)_thermal_offset(1), (ଦ୍ୱିଗୁଣ)_thermal_offset(2));
	}
}

} // namespace calibration
