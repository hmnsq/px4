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

#निर्देशः once

#अन्तर्भाव <stdint.h>

नामाकाश open_drone_id
{

स्थिर constexpr uint8_t UAS_ID_आकार = 20;
स्थिर constexpr uint8_t UAS_ID_TYPE_SERIAL_सङ्ख्या = 1;
स्थिर constexpr int32_t UAS_ID_बहुलकम्_तालकम्_FIRST_BASIC_ID = 1;

संरचना UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_आकार] {};
};

गणना वर्ग Bid_तालकम्_State {
	रिक्तानि_2,
	Locked,
	Invalid_लेखः,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

संरचना Bid_तालकम्_Info {
	Bid_तालकम्_State state{Bid_तालकम्_State::Unsupported};
	UasId id{};

	तार्किक locked() ध्रुव { प्रत्यागम state == Bid_तालकम्_State::Locked; }
};

गणना वर्ग Bid_Store_परिणाम {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	Invalid_लेखः,
	WriteError,
	Unsupported,
};

वर्ग BidStorage
{
सार्वजनिक:
	स्थिर Bid_तालकम्_Info get();
	स्थिर Bid_Store_परिणाम checkOrStore(uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_आकार]);
};

वर्ग Bid_तालकम्
{
सार्वजनिक:
	Bid_Store_परिणाम resolve(uint8_t id_type, uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_आकार], UasId &locked_id);

गुप्त:
	Bid_तालकम्_Info _info{};
	तार्किक _loaded{असत्य};
};

} // namespace open_drone_id
