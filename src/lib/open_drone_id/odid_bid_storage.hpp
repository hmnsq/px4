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

#لارښوونه once

#ضمن <stdint.h>

نطاق open_drone_id
{

ساكن constexpr uint8_t UAS_ID_اندازه = 20;
ساكن constexpr uint8_t UAS_ID_TYPE_SERIAL_شمېره = 1;
ساكن constexpr int32_t UAS_ID_ډېرپېښ_قلف_لومړی_BASIC_ID = 1;

بنية UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_اندازه] {};
};

تعداد صنف Bid_قلف_State {
	تشه_2,
	Locked,
	Invalid_ریکارډ,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

بنية Bid_قلف_Info {
	Bid_قلف_State state{Bid_قلف_State::Unsupported};
	UasId id{};

	منطقي locked() ثابت { أرجع state == Bid_قلف_State::Locked; }
};

تعداد صنف Bid_Store_پایله {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	Invalid_ریکارډ,
	WriteError,
	Unsupported,
};

صنف BidStorage
{
عام:
	ساكن Bid_قلف_Info get();
	ساكن Bid_Store_پایله checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه]);
};

صنف Bid_قلف
{
عام:
	Bid_Store_پایله resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه], UasId &locked_id);

خاص:
	Bid_قلف_Info _info{};
	منطقي _loaded{خطأ};
};

} // namespace open_drone_id
