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

#अन्तर्भाव "DifferentialPressure.hpp"

#अन्तर्भाव "Utilities.hpp"

#अन्तर्भाव <lib/drivers/device/Device.hpp>
#अन्तर्भाव <lib/parameters/param.h>

#अन्तर्भाव <float.h>

नामाकाश calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	समुच्चयः_device_id(device_id);
}

शून्य DifferentialPressure::समुच्चयः_device_id(uint32_t device_id)
{
	ध्रुव तार्किक external = device::device_is_external(device_id);

	यदि (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

तार्किक DifferentialPressure::समुच्चयः_विस्थापनम्(ध्रुव भिन्न &विस्थापनम्)
{
	यदि (fabsf(विस्थापनम्_2 - विस्थापनम्) > 0.01f) {
		यदि (PX4_ISFINITE(विस्थापनम्)) {
			विस्थापनम्_2 = विस्थापनम्;
			calibration_गणनम्++;
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

तार्किक DifferentialPressure::समुच्चयः_calibration_सूचकांक(पूर्णाङ्क calibration_सूचकांक)
{
	यदि ((calibration_सूचकांक >= 0) && (calibration_सूचकांक < MAX_SENSOR_गणनम्)) {
		calibration_सूचकांक_2 = calibration_सूचकांक;
		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

शून्य DifferentialPressure::ParametersUpdate()
{
	यदि (_device_id == 0) {
		प्रत्यागम;
	}

	calibration_सूचकांक_2 = Find_Current_Calibration_सूचकांक(SensorString(), _device_id);

	यदि (calibration_सूचकांक_2 == -1) {
		// no saved calibration available
		Reset();

	} अन्यथा {
		Parameters_आरोपय();
	}
}

तार्किक DifferentialPressure::Parameters_आरोपय()
{
	यदि (calibration_सूचकांक_2 >= 0 && calibration_सूचकांक_2 < MAX_SENSOR_गणनम्) {
		// CAL_DPRESx_OFF
		समुच्चयः_विस्थापनम्(GetCalibrationParamFloat(SensorString(), "OFF", calibration_सूचकांक_2));

		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

तार्किक DifferentialPressure::Adopt_Legacy_विस्थापनम्()
{
	यदि ((_device_id == 0) || calibrated()) {
		प्रत्यागम असत्य;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	यदि (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_सूचकांक) != 0) {
		प्रत्यागम असत्य;
	}

	भिन्न legacy_विस्थापनम् = 0.f;

	यदि (param_get(param_find(k_Legacy_विस्थापनम्_Param), &legacy_विस्थापनम्) != PX4_OK) {
		प्रत्यागम असत्य;
	}

	यदि (!PX4_ISFINITE(legacy_विस्थापनम्) || (fabsf(legacy_विस्थापनम्) < FLT_EPSILON)) {
		प्रत्यागम असत्य;
	}

	विस्थापनम्_2 = legacy_विस्थापनम्;
	calibration_सूचकांक_2 = k_Legacy_सूचकांक;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, k_Legacy_विस्थापनम्_Param, (द्विगुण)विस्थापनम्_2);

	प्रत्यागम सत्य;
}

शून्य DifferentialPressure::Update_Legacy_विस्थापनम्() ध्रुव
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	यदि (calibration_सूचकांक_2 != k_Legacy_सूचकांक) {
		प्रत्यागम;
	}

	ध्रुव param_t handle = param_find(k_Legacy_विस्थापनम्_Param);

	यदि (handle == PARAM_INVALID) {
		प्रत्यागम;
	}

	भिन्न विस्थापनम् = विस्थापनम्_2;
	param_समुच्चयः_no_notification(handle, &विस्थापनम्);
}

शून्य DifferentialPressure::Reset()
{
	विस्थापनम्_2 = 0;

	calibration_सूचकांक_2 = -1;

	calibration_गणनम् = 0;
}

तार्किक DifferentialPressure::ParametersSave(पूर्णाङ्क desired_calibration_सूचकांक, तार्किक force)
{
	यदि (force && desired_calibration_सूचकांक >= 0 && desired_calibration_सूचकांक < MAX_SENSOR_गणनम्) {
		calibration_सूचकांक_2 = desired_calibration_सूचकांक;

	} अन्यथा यदि (!force || (calibration_सूचकांक_2 < 0)
		   || (desired_calibration_सूचकांक != -1 && desired_calibration_सूचकांक != calibration_सूचकांक_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_सूचकांक_पूर्व = calibration_सूचकांक_2;
		calibration_सूचकांक_2 = Find_उपलब्धम्_Calibration_सूचकांक(SensorString(), _device_id, desired_calibration_सूचकांक);

		यदि (calibration_सूचकांक_पूर्व >= 0 && (calibration_सूचकांक_पूर्व != calibration_सूचकांक_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_सूचकांक_पूर्व, calibration_सूचकांक_2);
		}
	}

	यदि (calibration_सूचकांक_2 >= 0 && calibration_सूचकांक_2 < MAX_SENSOR_गणनम्) {
		// save calibration
		तार्किक success = सत्य;
		success &= समुच्चयः_Calibration_Param(SensorString(), "ID", calibration_सूचकांक_2, _device_id);
		success &= समुच्चयः_Calibration_Param(SensorString(), "OFF", calibration_सूचकांक_2, विस्थापनम्_2);

		Update_Legacy_विस्थापनम्();

		प्रत्यागम success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_सूचकांक_2);

	प्रत्यागम असत्य;
}

शून्य DifferentialPressure::Print_स्थितिः()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (द्विगुण)विस्थापनम्_2,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
