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

#సూచన once

#అన్తర్భావ <stdint.h>

నామాకాశ open_drone_id
{

స్థిర constexpr uint8_t UAS_ID_కొలత = 20;
స్థిర constexpr uint8_t UAS_ID_TYPE_SERIAL_సంఖ్య = 1;
స్థిర constexpr int32_t UAS_ID_బహుళకం_తాళం_మొదటి_BASIC_ID = 1;

నిర్మాణం UasId {
	uint8_t ua_type{0};
	uint8_t uas_id[UAS_ID_కొలత] {};
};

గణన తరగతి Bid_తాళం_State {
	ఖాళీలు_2,
	Locked,
	Invalid_రికార్డు,
	StorageUnavailable,
	WriteError,
	Unsupported,
};

నిర్మాణం Bid_తాళం_Info {
	Bid_తాళం_State state{Bid_తాళం_State::Unsupported};
	UasId id{};

	తార్కిక locked() స్థిరరాశి { తిరిగి state == Bid_తాళం_State::Locked; }
};

గణన తరగతి Bid_Store_ఫలితం {
	Stored,
	AlreadyLocked,
	AlreadyLockedDifferent,
	InvalidBasicId,
	StorageUnavailable,
	Invalid_రికార్డు,
	WriteError,
	Unsupported,
};

తరగతి BidStorage
{
బహిరంగ:
	స్థిర Bid_తాళం_Info get();
	స్థిర Bid_Store_ఫలితం checkOrStore(uint8_t ua_type, స్థిరరాశి uint8_t uas_id[UAS_ID_కొలత]);
};

తరగతి Bid_తాళం
{
బహిరంగ:
	Bid_Store_ఫలితం resolve(uint8_t id_type, uint8_t ua_type, స్థిరరాశి uint8_t uas_id[UAS_ID_కొలత], UasId &locked_id);

అంతరంగ:
	Bid_తాళం_Info _info{};
	తార్కిక _loaded{అబద్ధం};
};

} // namespace open_drone_id
