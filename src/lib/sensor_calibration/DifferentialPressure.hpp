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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>

ਨਾਮਾਕਾਸ਼ calibration
{
ਵਰਗ DifferentialPressure
{
ਜਨਤਕ:
	ਸਥਿਰ constexpr ਪੂਰਨ MAX_SENSOR_ਗਿਣਤੀ = 3; ///< also the airspeed selector's sensor limit

	ਸਥਿਰ constexpr ਅਚਲ ਅੱਖਰ *SensorString() { ਵਾਪਸ "DPRES"; }

	DifferentialPressure();
	ਸਪਸ਼ਟ DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = ਮੂਲ;

	ਖਾਲੀ Print_ਸਥਿਤੀ();

	ਤਾਰਕਿਕ ਸੈੱਟ_calibration_ਸੂਚਕਾਂਕ(ਪੂਰਨ calibration_ਸੂਚਕਾਂਕ);
	ਖਾਲੀ ਸੈੱਟ_device_id(uint32_t device_id);
	ਤਾਰਕਿਕ ਸੈੱਟ_ਵਿੱਥ(ਅਚਲ ਦਸ਼ਮਲਵ &ਵਿੱਥ);

	ਤਾਰਕਿਕ calibrated() ਅਚਲ { ਵਾਪਸ (_device_id != 0) && (calibration_ਸੂਚਕਾਂਕ_2 >= 0); }
	uint8_t calibration_ਗਿਣਤੀ_2() ਅਚਲ { ਵਾਪਸ calibration_ਗਿਣਤੀ; }
	int8_t calibration_ਸੂਚਕਾਂਕ() ਅਚਲ { ਵਾਪਸ calibration_ਸੂਚਕਾਂਕ_2; }
	uint32_t device_id() ਅਚਲ { ਵਾਪਸ _device_id; }
	ਤਾਰਕਿਕ external() ਅਚਲ { ਵਾਪਸ _external; }
	ਅਚਲ ਦਸ਼ਮਲਵ &ਵਿੱਥ() ਅਚਲ { ਵਾਪਸ ਵਿੱਥ_2; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	ਸਤਰਅੰਦਰ ਦਸ਼ਮਲਵ Correct(ਅਚਲ ਦਸ਼ਮਲਵ &ਅੰਕੜੇ) ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ - ਵਿੱਥ_2; }

	ਸਤਰਅੰਦਰ ਦਸ਼ਮਲਵ Uncorrect(ਅਚਲ ਦਸ਼ਮਲਵ &corrected_ਅੰਕੜੇ) ਅਚਲ { ਵਾਪਸ corrected_ਅੰਕੜੇ + ਵਿੱਥ_2; }

	ਤਾਰਕਿਕ Parameters_ਲੱਦੋ();
	ਤਾਰਕਿਕ ParametersSave(ਪੂਰਨ desired_calibration_ਸੂਚਕਾਂਕ = -1, ਤਾਰਕਿਕ force = ਝੂਠ);
	ਖਾਲੀ ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	ਤਾਰਕਿਕ Adopt_Legacy_ਵਿੱਥ();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	ਖਾਲੀ Update_Legacy_ਵਿੱਥ() ਅਚਲ;

	ਖਾਲੀ Reset();

ਨਿਜੀ:
	ਸਥਿਰ constexpr ਪੂਰਨ k_Legacy_ਸੂਚਕਾਂਕ{0};
	ਸਥਿਰ constexpr ਅਚਲ ਅੱਖਰ *k_Legacy_ਵਿੱਥ_Param{"SENS_DPRES_OFF"};

	ਦਸ਼ਮਲਵ ਵਿੱਥ_2{0};

	int8_t calibration_ਸੂਚਕਾਂਕ_2{-1};
	uint32_t _device_id{0};
	uint8_t calibration_ਗਿਣਤੀ{0};

	ਤਾਰਕਿਕ _external{ਝੂਠ};
};
} // namespace calibration
