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

#അന്തര്ഭാവ <stdint.h>

നാമാകാശ open_drone_id
{

സ്ഥിതം constexpr uint8_t UAS_ID_SIZE = 20;
സ്ഥിതം constexpr uint8_t UAS_ID_TYPE_SERIAL_സംഖ്യ = 1;
സ്ഥിതം constexpr int32_t UAS_ID_MODE_LOCK_FIRST_BASIC_ID = 1;

ഘടന UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_SIZE] {};
};

എണ്ണൽ വർഗ്ഗം BidLockState {
	Empty,
	Locked,
	InvalidRecord,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

ഘടന BidLockInfo {
	BidLockState state{BidLockState::Unsupported};
	UasId id{};

	തർക്കം locked() സ്ഥിരം { തിരികെ state == BidLockState::Locked; }
};

എണ്ണൽ വർഗ്ഗം BidStoreResult {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	InvalidRecord,
	WriteError,
	Unsupported,
};

വർഗ്ഗം BidStorage
{
പൊതു:
	സ്ഥിതം BidLockInfo get();
	സ്ഥിതം BidStoreResult checkOrStore(uint8_t ua_type, സ്ഥിരം uint8_t uas_id[UAS_ID_SIZE]);
};

വർഗ്ഗം BidLock
{
പൊതു:
	BidStoreResult resolve(uint8_t id_type, uint8_t ua_type, സ്ഥിരം uint8_t uas_id[UAS_ID_SIZE], UasId &locked_id);

സ്വകാര്യ:
	BidLockInfo _info{};
	തർക്കം _loaded{അസത്യം};
};

} // namespace open_drone_id
