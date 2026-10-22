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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <stdint.h>

ನಾಮಾಕಾಶ open_drone_id
{

ಸ್ಥಿರ constexpr uint8_t UAS_ID_ಗಾತ್ರ = 20;
ಸ್ಥಿರ constexpr uint8_t UAS_ID_TYPE_SERIAL_ಸಂಖ್ಯೆ = 1;
ಸ್ಥಿರ constexpr int32_t UAS_ID_ಬಹುಲಕ_ಬೀಗ_ಮೊದಲ_BASIC_ID = 1;

ರಚನೆ UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_ಗಾತ್ರ] {};
};

ಎಣಿಕೆ ವರ್ಗ Bid_ಬೀಗ_State {
	ಖಾಲಿಗಳು_2,
	Locked,
	Invalid_ದಾಖಲೆ,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

ರಚನೆ Bid_ಬೀಗ_Info {
	Bid_ಬೀಗ_State state{Bid_ಬೀಗ_State::Unsupported};
	UasId id{};

	ತಾರ್ಕಿಕ locked() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ state == Bid_ಬೀಗ_State::Locked; }
};

ಎಣಿಕೆ ವರ್ಗ Bid_Store_ಫಲಿತಾಂಶ {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	Invalid_ದಾಖಲೆ,
	WriteError,
	Unsupported,
};

ವರ್ಗ BidStorage
{
ಸಾರ್ವಜನಿಕ:
	ಸ್ಥಿರ Bid_ಬೀಗ_Info get();
	ಸ್ಥಿರ Bid_Store_ಫಲಿತಾಂಶ checkOrStore(uint8_t ua_type, ಸ್ಥಿರಾಂಕ uint8_t uas_id[UAS_ID_ಗಾತ್ರ]);
};

ವರ್ಗ Bid_ಬೀಗ
{
ಸಾರ್ವಜನಿಕ:
	Bid_Store_ಫಲಿತಾಂಶ resolve(uint8_t id_type, uint8_t ua_type, ಸ್ಥಿರಾಂಕ uint8_t uas_id[UAS_ID_ಗಾತ್ರ], UasId &locked_id);

ಖಾಸಗಿ:
	Bid_ಬೀಗ_Info _info{};
	ತಾರ್ಕಿಕ _loaded{ಸುಳ್ಳು};
};

} // namespace open_drone_id
