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

#अन्तर्भाव "Barometer.hpp"

#अन्तर्भाव "Utilities.hpp"

#अन्तर्भाव <lib/drivers/device/Device.hpp>
#अन्तर्भाव <lib/parameters/param.h>

using नामाकाश matrix;
using नामाकाश time_literals;

नामाकाश calibration
{

Barometer::Barometer()
{
	Reset();
}

Barometer::Barometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Barometer::Barometer(uint32_t device_id, तार्किक external)
{
	set_device_id(device_id, external);
}

शून्य Barometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

शून्य Barometer::set_device_id(uint32_t device_id, तार्किक external)
{
	सचे (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(सच्च);
	}
}

शून्य Barometer::SensorCorrectionsUpdate(तार्किक force)
{
	// check if the selected sensor has updated
	सचे (_sensor_correction_sub.updated() || force) {

		// valid device id required
		सचे (_device_id == 0) {
			पटिनिवत्त;
		}

		sensor_correction_s corrections;

		सचे (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			अत्थाय (पूर्णाङ्क i = 0; i < MAX_SENSOR_गणनम्; i++) {
				सचे (corrections.baro_device_ids[i] == _device_id) {
					विकल्प (i) {
					पक्ष 0:
						_thermal_offset = corrections.baro_offset_0;
						पटिनिवत्त;

					पक्ष 1:
						_thermal_offset = corrections.baro_offset_1;
						पटिनिवत्त;

					पक्ष 2:
						_thermal_offset = corrections.baro_offset_2;
						पटिनिवत्त;

					पक्ष 3:
						_thermal_offset = corrections.baro_offset_3;
						पटिनिवत्त;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset = 0;
	}
}

तार्किक Barometer::set_offset(ध्रुव भिन्न &offset)
{
	सचे (fabsf(_offset - offset) > 0.01f) {
		सचे (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_गणनम्++;
			पटिनिवत्त सच्च;
		}
	}

	पटिनिवत्त मिच्छा;
}

तार्किक Barometer::set_calibration_index(पूर्णाङ्क calibration_index)
{
	सचे ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_गणनम्)) {
		_calibration_index = calibration_index;
		पटिनिवत्त सच्च;
	}

	पटिनिवत्त मिच्छा;
}

शून्य Barometer::ParametersUpdate()
{
	सचे (_device_id == 0) {
		पटिनिवत्त;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	सचे (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} अञ्ञथा {
		ParametersLoad();
	}
}

तार्किक Barometer::ParametersLoad()
{
	सचे (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_गणनम्) {
		// CAL_BAROx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		सचे ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			स्थिर constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			सचे (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_BAROx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		पटिनिवत्त सच्च;
	}

	पटिनिवत्त मिच्छा;
}

शून्य Barometer::Reset()
{
	_offset = 0;

	_thermal_offset = 0;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_गणनम् = 0;
}

तार्किक Barometer::ParametersSave(पूर्णाङ्क desired_calibration_index, तार्किक force)
{
	सचे (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_गणनम्) {
		_calibration_index = desired_calibration_index;

	} अञ्ञथा सचे (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		सचे (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	सचे (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_गणनम्) {
		// save calibration
		तार्किक success = सच्च;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		पटिनिवत्त success;
	}

	पटिनिवत्त मिच्छा;
}

शून्य Barometer::PrintStatus()
{
	सचे (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: %05.3f, Ext\n",
			     SensorString(), device_id(), enabled(),
			     (द्विगुण)_offset);

	} अञ्ञथा {
		PX4_INFO_RAW("%s %" PRIu32 " EN: %d, offset: %05.3f, Internal\n",
			     SensorString(), device_id(), enabled(),
			     (द्विगुण)_offset);
	}

	सचे (fabsf(_thermal_offset) > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: %.4f\n", SensorString(), _device_id, (द्विगुण)_thermal_offset);
	}
}

} // namespace calibration
