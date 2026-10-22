/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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

#ضمن "Barometer.hpp"

#ضمن "Utilities.hpp"

#ضمن <lib/drivers/device/Device.hpp>
#ضمن <lib/parameters/param.h>

using نطاق matrix;
using نطاق time_literals;

نطاق calibration
{

Barometer::Barometer()
{
	Reset();
}

Barometer::Barometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Barometer::Barometer(uint32_t device_id, منطقي external)
{
	set_device_id(device_id, external);
}

فراغ Barometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

فراغ Barometer::set_device_id(uint32_t device_id, منطقي external)
{
	إذا (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(true);
	}
}

فراغ Barometer::SensorCorrectionsUpdate(منطقي force)
{
	// check if the selected sensor has updated
	إذا (_sensor_correction_sub.updated() || force) {

		// valid device id required
		إذا (_device_id == 0) {
			أرجع;
		}

		sensor_correction_s corrections;

		إذا (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			لكل (صحيح i = 0; i < MAX_SENSOR_عدد; i++) {
				إذا (corrections.baro_device_ids[i] == _device_id) {
					بدل (i) {
					حالة 0:
						_thermal_offset = corrections.baro_offset_0;
						أرجع;

					حالة 1:
						_thermal_offset = corrections.baro_offset_1;
						أرجع;

					حالة 2:
						_thermal_offset = corrections.baro_offset_2;
						أرجع;

					حالة 3:
						_thermal_offset = corrections.baro_offset_3;
						أرجع;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset = 0;
	}
}

منطقي Barometer::set_offset(ثابت عائم &offset)
{
	إذا (fabsf(_offset - offset) > 0.01f) {
		إذا (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_عدد++;
			أرجع true;
		}
	}

	أرجع خطأ;
}

منطقي Barometer::set_calibration_index(صحيح calibration_index)
{
	إذا ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_عدد)) {
		_calibration_index = calibration_index;
		أرجع true;
	}

	أرجع خطأ;
}

فراغ Barometer::ParametersUpdate()
{
	إذا (_device_id == 0) {
		أرجع;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	إذا (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} وإلا {
		ParametersLoad();
	}
}

منطقي Barometer::ParametersLoad()
{
	إذا (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_عدد) {
		// CAL_BAROx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		إذا ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			ساكن constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			إذا (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_BAROx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		أرجع true;
	}

	أرجع خطأ;
}

فراغ Barometer::Reset()
{
	_offset = 0;

	_thermal_offset = 0;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_عدد = 0;
}

منطقي Barometer::ParametersSave(صحيح desired_calibration_index, منطقي force)
{
	إذا (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_عدد) {
		_calibration_index = desired_calibration_index;

	} وإلا إذا (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		إذا (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	إذا (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_عدد) {
		// save calibration
		منطقي success = true;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		أرجع success;
	}

	أرجع خطأ;
}

فراغ Barometer::اطبع_Status_2()
{
	إذا (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: %05.3f, Ext\n",
			     SensorString(), device_id(), enabled(),
			     (مزدوج)_offset);

	} وإلا {
		PX4_INFO_RAW("%s %" PRIu32 " EN: %d, offset: %05.3f, Internal\n",
			     SensorString(), device_id(), enabled(),
			     (مزدوج)_offset);
	}

	إذا (fabsf(_thermal_offset) > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: %.4f\n", SensorString(), _device_id, (مزدوج)_thermal_offset);
	}
}

} // namespace calibration
