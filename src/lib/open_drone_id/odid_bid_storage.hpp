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

#pragma once

#अन्तर्भाव <stdint.h>

नामाकाश open_drone_id
{

स्थिर constexpr uint8_t UAS_ID_SIZE = 20;
स्थिर constexpr uint8_t UAS_ID_TYPE_SERIAL_सङ्ख्या = 1;
स्थिर constexpr int32_t UAS_ID_MODE_LOCK_FIRST_BASIC_ID = 1;

संरचना UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_SIZE] {};
};

गणना वग्ग BidLockState {
	Empty,
	Locked,
	InvalidRecord,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

संरचना BidLockInfo {
	BidLockState state{BidLockState::Unsupported};
	UasId id{};

	तार्किक locked() ध्रुव { पटिनिवत्त state == BidLockState::Locked; }
};

गणना वग्ग BidStoreResult {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	InvalidRecord,
	WriteError,
	Unsupported,
};

वग्ग BidStorage
{
सार्वजनिक:
	स्थिर BidLockInfo get();
	स्थिर BidStoreResult checkOrStore(uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_SIZE]);
};

वग्ग BidLock
{
सार्वजनिक:
	BidStoreResult resolve(uint8_t id_type, uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_SIZE], UasId &locked_id);

गुप्त:
	BidLockInfo _info{};
	तार्किक _loaded{मिच्छा};
};

} // namespace open_drone_id
