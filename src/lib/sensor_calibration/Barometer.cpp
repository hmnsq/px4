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

استفاده نطاق ماتریس;
استفاده نطاق زمان_literals;

نطاق calibration
{

Barometer::Barometer()
{
	Reset();
}

Barometer::Barometer(uint32_t device_id)
{
	مجموعه_device_id(device_id);
}

Barometer::Barometer(uint32_t device_id, منطقي external)
{
	مجموعه_device_id(device_id, external);
}

فراغ Barometer::مجموعه_device_id(uint32_t device_id)
{
	مجموعه_device_id(device_id, device::device_is_external(device_id));
}

فراغ Barometer::مجموعه_device_id(uint32_t device_id, منطقي external)
{
	إذا (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(درست);
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
			لكل (صحيح i = 0; i < MAX_SENSOR_شمار; i++) {
				إذا (corrections.baro_device_ids[i] == _device_id) {
					بدل (i) {
					حالة 0:
						thermal_جابه_جایی = corrections.baro_جابه_جایی_0;
						أرجع;

					حالة 1:
						thermal_جابه_جایی = corrections.baro_جابه_جایی_1;
						أرجع;

					حالة 2:
						thermal_جابه_جایی = corrections.baro_جابه_جایی_2;
						أرجع;

					حالة 3:
						thermal_جابه_جایی = corrections.baro_جابه_جایی_3;
						أرجع;
					}
				}
			}
		}

		// zero thermal offset if not found
		thermal_جابه_جایی = 0;
	}
}

منطقي Barometer::مجموعه_جابه_جایی(ثابت عائم &جابه_جایی)
{
	إذا (fabsf(جابه_جایی_2 - جابه_جایی) > 0.01f) {
		إذا (PX4_ISFINITE(جابه_جایی)) {
			جابه_جایی_2 = جابه_جایی;
			calibration_شمار++;
			أرجع درست;
		}
	}

	أرجع خطأ;
}

منطقي Barometer::مجموعه_calibration_شاخص(صحيح calibration_شاخص)
{
	إذا ((calibration_شاخص >= 0) && (calibration_شاخص < MAX_SENSOR_شمار)) {
		calibration_شاخص_2 = calibration_شاخص;
		أرجع درست;
	}

	أرجع خطأ;
}

فراغ Barometer::ParametersUpdate()
{
	إذا (_device_id == 0) {
		أرجع;
	}

	calibration_شاخص_2 = پیدا_کن_Current_Calibration_شاخص(SensorString(), _device_id);

	إذا (calibration_شاخص_2 == -1) {
		// no saved calibration available
		Reset();

	} وإلا {
		Parameters_بارگیری();
	}
}

منطقي Barometer::Parameters_بارگیری()
{
	إذا (calibration_شاخص_2 >= 0 && calibration_شاخص_2 < MAX_SENSOR_شمار) {
		// CAL_BAROx_PRIO
		اولویت_2 = GetCalibrationParamInt32(SensorString(), "PRIO", calibration_شاخص_2);

		إذا ((اولویت_2 < 0) || (اولویت_2 > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			ساكن constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			إذا (اولویت_2 != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					calibration_شاخص_2, اولویت_2);

				مجموعه_Calibration_Param(SensorString(), "PRIO", calibration_شاخص_2, CAL_PRIO_UNINITIALIZED);
			}

			اولویت_2 = _external ? DEFAULT_EXTERNAL_اولویت : DEFAULT_اولویت;
		}

		// CAL_BAROx_OFF
		مجموعه_جابه_جایی(GetCalibrationParamFloat(SensorString(), "OFF", calibration_شاخص_2));

		أرجع درست;
	}

	أرجع خطأ;
}

فراغ Barometer::Reset()
{
	جابه_جایی_2 = 0;

	thermal_جابه_جایی = 0;

	اولویت_2 = _external ? DEFAULT_EXTERNAL_اولویت : DEFAULT_اولویت;

	calibration_شاخص_2 = -1;

	calibration_شمار = 0;
}

منطقي Barometer::ParametersSave(صحيح desired_calibration_شاخص, منطقي force)
{
	إذا (force && desired_calibration_شاخص >= 0 && desired_calibration_شاخص < MAX_SENSOR_شمار) {
		calibration_شاخص_2 = desired_calibration_شاخص;

	} وإلا إذا (!force || (calibration_شاخص_2 < 0)
		   || (desired_calibration_شاخص != -1 && desired_calibration_شاخص != calibration_شاخص_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_شاخص_prev = calibration_شاخص_2;
		calibration_شاخص_2 = پیدا_کن_موجود_Calibration_شاخص(SensorString(), _device_id, desired_calibration_شاخص);

		إذا (calibration_شاخص_prev >= 0 && (calibration_شاخص_prev != calibration_شاخص_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_شاخص_prev, calibration_شاخص_2);
		}
	}

	إذا (calibration_شاخص_2 >= 0 && calibration_شاخص_2 < MAX_SENSOR_شمار) {
		// save calibration
		منطقي success = درست;
		success &= مجموعه_Calibration_Param(SensorString(), "ID", calibration_شاخص_2, _device_id);
		success &= مجموعه_Calibration_Param(SensorString(), "PRIO", calibration_شاخص_2, اولویت_2);
		success &= مجموعه_Calibration_Param(SensorString(), "OFF", calibration_شاخص_2, جابه_جایی_2);

		أرجع success;
	}

	أرجع خطأ;
}

فراغ Barometer::Print_وضعیت()
{
	إذا (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: %05.3f, Ext\n",
			     SensorString(), device_id(), enabled(),
			     (مزدوج)جابه_جایی_2);

	} وإلا {
		PX4_INFO_RAW("%s %" PRIu32 " EN: %d, offset: %05.3f, Internal\n",
			     SensorString(), device_id(), enabled(),
			     (مزدوج)جابه_جایی_2);
	}

	إذا (fabsf(thermal_جابه_جایی) > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: %.4f\n", SensorString(), _device_id, (مزدوج)thermal_جابه_جایی);
	}
}

} // namespace calibration
