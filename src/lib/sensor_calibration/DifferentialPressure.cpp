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

#ਅਨ੍ਤਰ੍ਭਾਵ "DifferentialPressure.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ "Utilities.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/drivers/device/Device.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/parameters/param.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>

ਨਾਮਾਕਾਸ਼ calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	ਸੈੱਟ_device_id(device_id);
}

ਖਾਲੀ DifferentialPressure::ਸੈੱਟ_device_id(uint32_t device_id)
{
	ਅਚਲ ਤਾਰਕਿਕ external = device::device_is_external(device_id);

	ਜੇ (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

ਤਾਰਕਿਕ DifferentialPressure::ਸੈੱਟ_ਵਿੱਥ(ਅਚਲ ਦਸ਼ਮਲਵ &ਵਿੱਥ)
{
	ਜੇ (fabsf(ਵਿੱਥ_2 - ਵਿੱਥ) > 0.01f) {
		ਜੇ (PX4_ISFINITE(ਵਿੱਥ)) {
			ਵਿੱਥ_2 = ਵਿੱਥ;
			calibration_ਗਿਣਤੀ++;
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ DifferentialPressure::ਸੈੱਟ_calibration_ਸੂਚਕਾਂਕ(ਪੂਰਨ calibration_ਸੂਚਕਾਂਕ)
{
	ਜੇ ((calibration_ਸੂਚਕਾਂਕ >= 0) && (calibration_ਸੂਚਕਾਂਕ < MAX_SENSOR_ਗਿਣਤੀ)) {
		calibration_ਸੂਚਕਾਂਕ_2 = calibration_ਸੂਚਕਾਂਕ;
		ਵਾਪਸ ਸੱਚ;
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ DifferentialPressure::ParametersUpdate()
{
	ਜੇ (_device_id == 0) {
		ਵਾਪਸ;
	}

	calibration_ਸੂਚਕਾਂਕ_2 = ਲੱਭੋ_Current_Calibration_ਸੂਚਕਾਂਕ(SensorString(), _device_id);

	ਜੇ (calibration_ਸੂਚਕਾਂਕ_2 == -1) {
		// no saved calibration available
		Reset();

	} ਨਹੀਂਤਾਂ {
		Parameters_ਲੱਦੋ();
	}
}

ਤਾਰਕਿਕ DifferentialPressure::Parameters_ਲੱਦੋ()
{
	ਜੇ (calibration_ਸੂਚਕਾਂਕ_2 >= 0 && calibration_ਸੂਚਕਾਂਕ_2 < MAX_SENSOR_ਗਿਣਤੀ) {
		// CAL_DPRESx_OFF
		ਸੈੱਟ_ਵਿੱਥ(GetCalibrationParamFloat(SensorString(), "OFF", calibration_ਸੂਚਕਾਂਕ_2));

		ਵਾਪਸ ਸੱਚ;
	}

	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ DifferentialPressure::Adopt_Legacy_ਵਿੱਥ()
{
	ਜੇ ((_device_id == 0) || calibrated()) {
		ਵਾਪਸ ਝੂਠ;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	ਜੇ (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_ਸੂਚਕਾਂਕ) != 0) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਦਸ਼ਮਲਵ legacy_ਵਿੱਥ = 0.f;

	ਜੇ (param_get(param_ਲੱਭੋ(k_Legacy_ਵਿੱਥ_Param), &legacy_ਵਿੱਥ) != PX4_OK) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (!PX4_ISFINITE(legacy_ਵਿੱਥ) || (fabsf(legacy_ਵਿੱਥ) < FLT_EPSILON)) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਵਿੱਥ_2 = legacy_ਵਿੱਥ;
	calibration_ਸੂਚਕਾਂਕ_2 = k_Legacy_ਸੂਚਕਾਂਕ;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, k_Legacy_ਵਿੱਥ_Param, (ਦੁੱਗਣਾ)ਵਿੱਥ_2);

	ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ DifferentialPressure::Update_Legacy_ਵਿੱਥ() ਅਚਲ
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	ਜੇ (calibration_ਸੂਚਕਾਂਕ_2 != k_Legacy_ਸੂਚਕਾਂਕ) {
		ਵਾਪਸ;
	}

	ਅਚਲ param_t handle = param_ਲੱਭੋ(k_Legacy_ਵਿੱਥ_Param);

	ਜੇ (handle == PARAM_INVALID) {
		ਵਾਪਸ;
	}

	ਦਸ਼ਮਲਵ ਵਿੱਥ = ਵਿੱਥ_2;
	param_ਸੈੱਟ_no_notification(handle, &ਵਿੱਥ);
}

ਖਾਲੀ DifferentialPressure::Reset()
{
	ਵਿੱਥ_2 = 0;

	calibration_ਸੂਚਕਾਂਕ_2 = -1;

	calibration_ਗਿਣਤੀ = 0;
}

ਤਾਰਕਿਕ DifferentialPressure::ParametersSave(ਪੂਰਨ desired_calibration_ਸੂਚਕਾਂਕ, ਤਾਰਕਿਕ force)
{
	ਜੇ (force && desired_calibration_ਸੂਚਕਾਂਕ >= 0 && desired_calibration_ਸੂਚਕਾਂਕ < MAX_SENSOR_ਗਿਣਤੀ) {
		calibration_ਸੂਚਕਾਂਕ_2 = desired_calibration_ਸੂਚਕਾਂਕ;

	} ਨਹੀਂਤਾਂ ਜੇ (!force || (calibration_ਸੂਚਕਾਂਕ_2 < 0)
		   || (desired_calibration_ਸੂਚਕਾਂਕ != -1 && desired_calibration_ਸੂਚਕਾਂਕ != calibration_ਸੂਚਕਾਂਕ_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_ਸੂਚਕਾਂਕ_prev = calibration_ਸੂਚਕਾਂਕ_2;
		calibration_ਸੂਚਕਾਂਕ_2 = ਲੱਭੋ_ਉਪਲਬਧ_Calibration_ਸੂਚਕਾਂਕ(SensorString(), _device_id, desired_calibration_ਸੂਚਕਾਂਕ);

		ਜੇ (calibration_ਸੂਚਕਾਂਕ_prev >= 0 && (calibration_ਸੂਚਕਾਂਕ_prev != calibration_ਸੂਚਕਾਂਕ_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_ਸੂਚਕਾਂਕ_prev, calibration_ਸੂਚਕਾਂਕ_2);
		}
	}

	ਜੇ (calibration_ਸੂਚਕਾਂਕ_2 >= 0 && calibration_ਸੂਚਕਾਂਕ_2 < MAX_SENSOR_ਗਿਣਤੀ) {
		// save calibration
		ਤਾਰਕਿਕ success = ਸੱਚ;
		success &= ਸੈੱਟ_Calibration_Param(SensorString(), "ID", calibration_ਸੂਚਕਾਂਕ_2, _device_id);
		success &= ਸੈੱਟ_Calibration_Param(SensorString(), "OFF", calibration_ਸੂਚਕਾਂਕ_2, ਵਿੱਥ_2);

		Update_Legacy_ਵਿੱਥ();

		ਵਾਪਸ success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_ਸੂਚਕਾਂਕ_2);

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ DifferentialPressure::Print_ਸਥਿਤੀ()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (ਦੁੱਗਣਾ)ਵਿੱਥ_2,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
