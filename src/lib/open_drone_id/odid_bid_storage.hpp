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

#ضمن <stdint.h>

نطاق open_drone_id
{

ساكن constexpr uint8_t UAS_ID_ناپ = 20;
ساكن constexpr uint8_t UAS_ID_TYPE_SERIAL_نمبر = 1;
ساكن constexpr int32_t UAS_ID_منوال_جندرا_پہلا_BASIC_ID = 1;

بنية UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_ناپ] {};
};

تعداد صنف Bid_جندرا_State {
	خالی_تھاواں_2,
	Locked,
	Invalid_ریکارڈ,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

بنية Bid_جندرا_Info {
	Bid_جندرا_State state{Bid_جندرا_State::Unsupported};
	UasId id{};

	منطقي locked() ثابت { أرجع state == Bid_جندرا_State::Locked; }
};

تعداد صنف Bid_Store_نتیجہ {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	Invalid_ریکارڈ,
	WriteError,
	Unsupported,
};

صنف BidStorage
{
عام:
	ساكن Bid_جندرا_Info get();
	ساكن Bid_Store_نتیجہ checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_ناپ]);
};

صنف Bid_جندرا
{
عام:
	Bid_Store_نتیجہ resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_ناپ], UasId &locked_id);

خاص:
	Bid_جندرا_Info _info{};
	منطقي _loaded{خطأ};
};

} // namespace open_drone_id
