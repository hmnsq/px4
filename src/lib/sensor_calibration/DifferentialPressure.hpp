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

#ہدایت once

#شامل <px4_platform_common/px4_config.h>
#شامل <px4_platform_common/defines.h>
#شامل <px4_platform_common/log.h>

#شامل <inttypes.h>
#شامل <math.h>

نامخلا calibration
{
جماعت DifferentialPressure
{
عوامی:
	ساکن constexpr صحیح MAX_SENSOR_گنتی = 3; ///< also the airspeed selector's sensor limit

	ساکن constexpr ثابت حرف *SensorString() { واپس "DPRES"; }

	DifferentialPressure();
	صریح DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = طےشدہ;

	خالی Print_حالت();

	منطقی سیٹ_calibration_اشاریہ(صحیح calibration_اشاریہ);
	خالی سیٹ_device_id(uint32_t device_id);
	منطقی سیٹ_انحراف(ثابت اعشاری &انحراف);

	منطقی calibrated() ثابت { واپس (_device_id != 0) && (calibration_اشاریہ_2 >= 0); }
	uint8_t calibration_گنتی_2() ثابت { واپس calibration_گنتی; }
	int8_t calibration_اشاریہ() ثابت { واپس calibration_اشاریہ_2; }
	uint32_t device_id() ثابت { واپس _device_id; }
	منطقی external() ثابت { واپس _external; }
	ثابت اعشاری &انحراف() ثابت { واپس انحراف_2; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	اندرونی اعشاری Correct(ثابت اعشاری &ڈیٹا) ثابت { واپس ڈیٹا - انحراف_2; }

	اندرونی اعشاری Uncorrect(ثابت اعشاری &corrected_ڈیٹا) ثابت { واپس corrected_ڈیٹا + انحراف_2; }

	منطقی Parameters_بھرو();
	منطقی ParametersSave(صحیح desired_calibration_اشاریہ = -1, منطقی force = جھوٹ);
	خالی ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	منطقی Adopt_Legacy_انحراف();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	خالی Update_Legacy_انحراف() ثابت;

	خالی Reset();

نجی:
	ساکن constexpr صحیح k_Legacy_اشاریہ{0};
	ساکن constexpr ثابت حرف *k_Legacy_انحراف_Param{"SENS_DPRES_OFF"};

	اعشاری انحراف_2{0};

	int8_t calibration_اشاریہ_2{-1};
	uint32_t _device_id{0};
	uint8_t calibration_گنتی{0};

	منطقی _external{جھوٹ};
};
} // namespace calibration
