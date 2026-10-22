/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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

#ಅನ್ತರ್ಭಾವ "DifferentialPressure.hpp"

#ಅನ್ತರ್ಭಾವ "Utilities.hpp"

#ಅನ್ತರ್ಭಾವ <lib/drivers/device/Device.hpp>
#ಅನ್ತರ್ಭಾವ <lib/parameters/param.h>

#ಅನ್ತರ್ಭಾವ <float.h>

ನಾಮಾಕಾಶ calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	ಗಣ_device_id(device_id);
}

ಶೂನ್ಯ DifferentialPressure::ಗಣ_device_id(uint32_t device_id)
{
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ external = device::device_is_external(device_id);

	ಆದರೆ (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

ತಾರ್ಕಿಕ DifferentialPressure::ಗಣ_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ &ಸ್ಥಾನಾಂತರ)
{
	ಆದರೆ (fabsf(ಸ್ಥಾನಾಂತರ_2 - ಸ್ಥಾನಾಂತರ) > 0.01f) {
		ಆದರೆ (PX4_ISFINITE(ಸ್ಥಾನಾಂತರ)) {
			ಸ್ಥಾನಾಂತರ_2 = ಸ್ಥಾನಾಂತರ;
			calibration_ಎಣಿಕೆ++;
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ DifferentialPressure::ಗಣ_calibration_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ calibration_ಸೂಚ್ಯಂಕ)
{
	ಆದರೆ ((calibration_ಸೂಚ್ಯಂಕ >= 0) && (calibration_ಸೂಚ್ಯಂಕ < MAX_SENSOR_ಎಣಿಕೆ)) {
		calibration_ಸೂಚ್ಯಂಕ_2 = calibration_ಸೂಚ್ಯಂಕ;
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ DifferentialPressure::ParametersUpdate()
{
	ಆದರೆ (_device_id == 0) {
		ಹಿಂತಿರುಗಿ;
	}

	calibration_ಸೂಚ್ಯಂಕ_2 = ಕಂಡುಹಿಡಿ_Current_Calibration_ಸೂಚ್ಯಂಕ(SensorString(), _device_id);

	ಆದರೆ (calibration_ಸೂಚ್ಯಂಕ_2 == -1) {
		// no saved calibration available
		Reset();

	} ಇಲ್ಲದಿದ್ದರೆ {
		Parameters_ತುಂಬು();
	}
}

ತಾರ್ಕಿಕ DifferentialPressure::Parameters_ತುಂಬು()
{
	ಆದರೆ (calibration_ಸೂಚ್ಯಂಕ_2 >= 0 && calibration_ಸೂಚ್ಯಂಕ_2 < MAX_SENSOR_ಎಣಿಕೆ) {
		// CAL_DPRESx_OFF
		ಗಣ_ಸ್ಥಾನಾಂತರ(GetCalibrationParamFloat(SensorString(), "OFF", calibration_ಸೂಚ್ಯಂಕ_2));

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ DifferentialPressure::Adopt_Legacy_ಸ್ಥಾನಾಂತರ()
{
	ಆದರೆ ((_device_id == 0) || calibrated()) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	ಆದರೆ (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_ಸೂಚ್ಯಂಕ) != 0) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ದಶಮಾಂಶ legacy_ಸ್ಥಾನಾಂತರ = 0.f;

	ಆದರೆ (param_get(param_ಕಂಡುಹಿಡಿ(k_Legacy_ಸ್ಥಾನಾಂತರ_Param), &legacy_ಸ್ಥಾನಾಂತರ) != PX4_OK) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (!PX4_ISFINITE(legacy_ಸ್ಥಾನಾಂತರ) || (fabsf(legacy_ಸ್ಥಾನಾಂತರ) < FLT_EPSILON)) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಸ್ಥಾನಾಂತರ_2 = legacy_ಸ್ಥಾನಾಂತರ;
	calibration_ಸೂಚ್ಯಂಕ_2 = k_Legacy_ಸೂಚ್ಯಂಕ;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, k_Legacy_ಸ್ಥಾನಾಂತರ_Param, (ದ್ವಿಗುಣ)ಸ್ಥಾನಾಂತರ_2);

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ DifferentialPressure::Update_Legacy_ಸ್ಥಾನಾಂತರ() ಸ್ಥಿರಾಂಕ
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	ಆದರೆ (calibration_ಸೂಚ್ಯಂಕ_2 != k_Legacy_ಸೂಚ್ಯಂಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಸ್ಥಿರಾಂಕ param_t handle = param_ಕಂಡುಹಿಡಿ(k_Legacy_ಸ್ಥಾನಾಂತರ_Param);

	ಆದರೆ (handle == PARAM_INVALID) {
		ಹಿಂತಿರುಗಿ;
	}

	ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ_2;
	param_ಗಣ_no_notification(handle, &ಸ್ಥಾನಾಂತರ);
}

ಶೂನ್ಯ DifferentialPressure::Reset()
{
	ಸ್ಥಾನಾಂತರ_2 = 0;

	calibration_ಸೂಚ್ಯಂಕ_2 = -1;

	calibration_ಎಣಿಕೆ = 0;
}

ತಾರ್ಕಿಕ DifferentialPressure::ParametersSave(ಪೂರ್ಣಾಂಕ desired_calibration_ಸೂಚ್ಯಂಕ, ತಾರ್ಕಿಕ force)
{
	ಆದರೆ (force && desired_calibration_ಸೂಚ್ಯಂಕ >= 0 && desired_calibration_ಸೂಚ್ಯಂಕ < MAX_SENSOR_ಎಣಿಕೆ) {
		calibration_ಸೂಚ್ಯಂಕ_2 = desired_calibration_ಸೂಚ್ಯಂಕ;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!force || (calibration_ಸೂಚ್ಯಂಕ_2 < 0)
		   || (desired_calibration_ಸೂಚ್ಯಂಕ != -1 && desired_calibration_ಸೂಚ್ಯಂಕ != calibration_ಸೂಚ್ಯಂಕ_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_ಸೂಚ್ಯಂಕ_prev = calibration_ಸೂಚ್ಯಂಕ_2;
		calibration_ಸೂಚ್ಯಂಕ_2 = ಕಂಡುಹಿಡಿ_ಲಭ್ಯ_Calibration_ಸೂಚ್ಯಂಕ(SensorString(), _device_id, desired_calibration_ಸೂಚ್ಯಂಕ);

		ಆದರೆ (calibration_ಸೂಚ್ಯಂಕ_prev >= 0 && (calibration_ಸೂಚ್ಯಂಕ_prev != calibration_ಸೂಚ್ಯಂಕ_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_ಸೂಚ್ಯಂಕ_prev, calibration_ಸೂಚ್ಯಂಕ_2);
		}
	}

	ಆದರೆ (calibration_ಸೂಚ್ಯಂಕ_2 >= 0 && calibration_ಸೂಚ್ಯಂಕ_2 < MAX_SENSOR_ಎಣಿಕೆ) {
		// save calibration
		ತಾರ್ಕಿಕ success = ನಿಜ;
		success &= ಗಣ_Calibration_Param(SensorString(), "ID", calibration_ಸೂಚ್ಯಂಕ_2, _device_id);
		success &= ಗಣ_Calibration_Param(SensorString(), "OFF", calibration_ಸೂಚ್ಯಂಕ_2, ಸ್ಥಾನಾಂತರ_2);

		Update_Legacy_ಸ್ಥಾನಾಂತರ();

		ಹಿಂತಿರುಗಿ success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_ಸೂಚ್ಯಂಕ_2);

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ DifferentialPressure::Print_ಸ್ಥಿತಿ()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (ದ್ವಿಗುಣ)ಸ್ಥಾನಾಂತರ_2,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
