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

#அந்தர்பாவ <stdint.h>

நாமாகாஶ open_drone_id
{

நிலையான constexpr uint8_t UAS_ID_SIZE = 20;
நிலையான constexpr uint8_t UAS_ID_TYPE_SERIAL_எண் = 1;
நிலையான constexpr int32_t UAS_ID_MODE_LOCK_FIRST_BASIC_ID = 1;

கட்டமைப்பு UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_SIZE] {};
};

எண்ணிக்கை வகுப்பு BidLockState {
	Empty,
	Locked,
	InvalidRecord,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

கட்டமைப்பு BidLockInfo {
	BidLockState state{BidLockState::Unsupported};
	UasId id{};

	தருக்க locked() மாறிலி { திரும்பு state == BidLockState::Locked; }
};

எண்ணிக்கை வகுப்பு BidStoreResult {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	InvalidRecord,
	WriteError,
	Unsupported,
};

வகுப்பு BidStorage
{
பொது:
	நிலையான BidLockInfo get();
	நிலையான BidStoreResult checkOrStore(uint8_t ua_type, மாறிலி uint8_t uas_id[UAS_ID_SIZE]);
};

வகுப்பு BidLock
{
பொது:
	BidStoreResult resolve(uint8_t id_type, uint8_t ua_type, மாறிலி uint8_t uas_id[UAS_ID_SIZE], UasId &locked_id);

தனிப்பட்ட:
	BidLockInfo _info{};
	தருக்க _loaded{பொய்};
};

} // namespace open_drone_id
