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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

ਨਾਮਾਕਾਸ਼ open_drone_id
{

ਸਥਿਰ constexpr uint8_t UAS_ID_ਮਾਪ = 20;
ਸਥਿਰ constexpr uint8_t UAS_ID_TYPE_SERIAL_ਸੰਖਿਆ = 1;
ਸਥਿਰ constexpr int32_t UAS_ID_ਬਹੁਲਕ_ਤਾਲਾ_ਪਹਿਲਾ_BASIC_ID = 1;

ਬਣਤਰ UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_ਮਾਪ] {};
};

ਗਿਣਤੀ ਵਰਗ Bid_ਤਾਲਾ_State {
	ਖਾਲੀ_ਥਾਵਾਂ_2,
	Locked,
	Invalid_ਰਿਕਾਰਡ,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

ਬਣਤਰ Bid_ਤਾਲਾ_Info {
	Bid_ਤਾਲਾ_State state{Bid_ਤਾਲਾ_State::Unsupported};
	UasId id{};

	ਤਾਰਕਿਕ locked() ਅਚਲ { ਵਾਪਸ state == Bid_ਤਾਲਾ_State::Locked; }
};

ਗਿਣਤੀ ਵਰਗ Bid_Store_ਨਤੀਜਾ {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	Invalid_ਰਿਕਾਰਡ,
	WriteError,
	Unsupported,
};

ਵਰਗ BidStorage
{
ਜਨਤਕ:
	ਸਥਿਰ Bid_ਤਾਲਾ_Info get();
	ਸਥਿਰ Bid_Store_ਨਤੀਜਾ checkOrStore(uint8_t ua_type, ਅਚਲ uint8_t uas_id[UAS_ID_ਮਾਪ]);
};

ਵਰਗ Bid_ਤਾਲਾ
{
ਜਨਤਕ:
	Bid_Store_ਨਤੀਜਾ resolve(uint8_t id_type, uint8_t ua_type, ਅਚਲ uint8_t uas_id[UAS_ID_ਮਾਪ], UasId &locked_id);

ਨਿਜੀ:
	Bid_ਤਾਲਾ_Info _info{};
	ਤਾਰਕਿਕ _loaded{ਝੂਠ};
};

} // namespace open_drone_id
