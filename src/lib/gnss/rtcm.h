/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

/**
 * @file rtcm.h
 *
 * RTCM3 protocol definitions and parsing utilities.
 *
 * RTCM3 frame structure:
 *   Byte 0:     Preamble (0xD3)
 *   Byte 1-2:   6 reserved bits (0) + 10-bit payload length
 *   Byte 3..N:  Payload (0-1023 bytes)
 *   Last 3:     CRC-24Q checksum
 *
 * Total frame size: 3 (header) + payload_length + 3 (CRC) = 6 + payload_length
 * Maximum frame size: 6 + 1023 = 1029 bytes
 */

#సూచన once

#అన్తర్భావ <cstdint>
#అన్తర్భావ <cstddef>

namespace gnss
{

// RTCM3 protocol constants
స్థిర constexpr uint8_t  RTCM3_PREAMBLE        = 0xD3;
స్థిర constexpr size_t   RTCM3_శీర్షిక_LEN      = 3;   // Preamble + 2 bytes reserved/length
స్థిర constexpr size_t   RTCM3_చక్రీయ_తనిఖీ_LEN         = 3;
స్థిర constexpr size_t   RTCM3_MAX_PAYLOAD_LEN = 1023;
స్థిర constexpr size_t   RTCM3_MAX_చట్రం_LEN   = RTCM3_శీర్షిక_LEN + RTCM3_MAX_PAYLOAD_LEN + RTCM3_చక్రీయ_తనిఖీ_LEN; // 1029
స్థిర constexpr uint32_t RTCM3_CRC24Q_POLY     = 0x1864CFB;

// MAVLink GPS_RTCM_DATA fragmentation constants.
// Spec: https://mavlink.io/en/messages/common.html#GPS_RTCM_DATA
//
// flags bit layout:
//   bit 0:   fragmented flag
//   bits 1-2: fragment ID
//   bits 3-7: sequence ID
//
// Fragmented payloads must only be flushed once the complete buffer has been
// reconstructed. A buffer is complete once either all 4 fragments are present,
// or the first fragment with a non-full payload has been received and every
// lower fragment ID is present.
స్థిర constexpr size_t   GPS_RTCM_MAX_FRAGMENT_LEN         = 180;
స్థిర constexpr size_t   GPS_RTCM_MAX_FRAGMENTS            = 4;
స్థిర constexpr size_t   GPS_RTCM_MAX_సందేశం_LEN          = GPS_RTCM_MAX_FRAGMENT_LEN * GPS_RTCM_MAX_FRAGMENTS;
స్థిర constexpr uint64_t GPS_RTCM_FRAGMENT_TIMEOUT_US      = 1000000;
స్థిర constexpr uint8_t  GPS_RTCM_FLAG_FRAGMENTED          = 1 << 0;
స్థిర constexpr uint8_t  GPS_RTCM_FLAG_FRAGMENT_ID_జరుపు   = 1;
స్థిర constexpr uint8_t  GPS_RTCM_FLAG_FRAGMENT_ID_ముసుగు    = 0x03;
స్థిర constexpr uint8_t  GPS_RTCM_FLAG_SEQUENCE_ID_జరుపు   = 3;
స్థిర constexpr uint8_t  GPS_RTCM_FLAG_SEQUENCE_ID_ముసుగు    = 0x1f;

/**
 * Calculate CRC-24Q checksum for RTCM3 messages.
 *
 * @param data   Pointer to data buffer
 * @param len    Length of data
 * @return       24-bit CRC value
 */
uint32_t rtcm3_crc24q(స్థిరరాశి uint8_t *దత్తాంశం, size_t len);

/**
 * Extract RTCM3 message type from a frame buffer.
 * The message type is the first 12 bits of the payload.
 *
 * @param frame  Pointer to complete RTCM3 frame (starting with preamble)
 * @return       Message type ID (0 if frame is too short)
 */
అంతఃవరుస uint16_t rtcm3_సందేశం_type(స్థిరరాశి uint8_t *చట్రం)
{
	// Message type is first 12 bits of payload (bytes 3-4)
	తిరిగి (static_cast<uint16_t>(చట్రం[3]) << 4) | (చట్రం[4] >> 4);
}

/**
 * Extract payload length from RTCM3 frame header.
 *
 * @param frame  Pointer to at least 3 bytes of RTCM3 frame header
 * @return       Payload length (0-1023)
 */
అంతఃవరుస size_t rtcm3_payload_నిడివి(స్థిరరాశి uint8_t *చట్రం)
{
	తిరిగి ((static_cast<size_t>(చట్రం[1]) & 0x03) << 8) | చట్రం[2];
}

అంతఃవరుస bool gps_rtcm_is_fragmented(uint8_t flags)
{
	తిరిగి (flags & GPS_RTCM_FLAG_FRAGMENTED) != 0;
}

అంతఃవరుస uint8_t gps_rtcm_fragment_id(uint8_t flags)
{
	తిరిగి (flags >> GPS_RTCM_FLAG_FRAGMENT_ID_జరుపు) & GPS_RTCM_FLAG_FRAGMENT_ID_ముసుగు;
}

అంతఃవరుస uint8_t gps_rtcm_sequence_id(uint8_t flags)
{
	తిరిగి (flags >> GPS_RTCM_FLAG_SEQUENCE_ID_జరుపు) & GPS_RTCM_FLAG_SEQUENCE_ID_ముసుగు;
}

class Gps_Rtcm_సందేశం_Assembler
{
బహిరంగ:
	Gps_Rtcm_సందేశం_Assembler() = అప్రమేయ;

	/**
	 * Add a MAVLink GPS_RTCM_DATA packet.
	 *
	 * Fragmented packets are buffered until the autopilot can reconstruct the
	 * complete payload. The fragment ID selects the slot within the 4-fragment
	 * buffer, while the sequence ID prevents fragments from different buffers
	 * from being mixed together. A fragmented payload is complete once either
	 * all 4 fragments are present, or the first fragment with a non-full
	 * payload has been received and every lower fragment ID is present. If the
	 * RTCM payload length is an exact multiple of 180 bytes and uses fewer than
	 * 4 fragments, the sender must still send a final zero-length fragment to
	 * mark completion.
	 *
	 * As a compatibility fallback for older senders that omit
	 * that terminator, PX4 also flushes a buffered message when a different
	 * sequence ID arrives, but only if the buffered fragments are a gap-free
	 * run of full 180-byte fragments starting at fragment 0 (the start of the
	 * RTCM message). This fallback only works while the older sequence remains
	 * buffered (up to the 1 second fragment timeout).
	 *
	 * If processing a packet completes two messages, addPacket() returns the older one
	 * and queues the newer one for takeDeferredMessage(). The returned pointer
	 * is valid until the next call to addPacket().
	 *
	 * @param flags       MAVLink GPS_RTCM_DATA flags
	 * @param data        Packet payload
	 * @param len         Packet payload length
	 * @param timestamp   Packet arrival timestamp in microseconds
	 * @param out_len     Set to the reassembled message length, or 0 if incomplete/invalid
	 * @return            Pointer to a complete message, or nullptr if the message is incomplete or invalid
	 */
	స్థిరరాశి uint8_t *కూడు_Packet(uint8_t flags, స్థిరరాశి uint8_t *దత్తాంశం, size_t len, uint64_t timestamp, size_t &బహిర్గతం_len);

	/**
	 * Retrieve the single deferred message queued by addPacket().
	 *
	 * This is only needed when processing one input packet completes two RTCM
	 * messages, for example when a legacy exact-multiple sequence is flushed on
	 * sequence rollover and the same packet also completes the new sequence.
	 *
	 * @param out_len     Set to the queued message length, or 0 if none is queued
	 * @return            Pointer to the queued message, or nullptr if none is queued
	 */
	స్థిరరాశి uint8_t *take_Deferred_సందేశం(size_t &బహిర్గతం_len);

	శూన్య reset();

అంతరంగ:
	నిర్మాణం Fragment_స్థానం {
		uint8_t దత్తాంశం[GPS_RTCM_MAX_FRAGMENT_LEN] {};
		size_t len {0};
		bool present {false};
	};

	నిర్మాణం SequenceState {
		uint8_t sequence_id {0};
		int8_t చివరి_fragment_id {-1};
		uint64_t timestamp {0};
		bool చురుకైన_2 {false};
	};

	శూన్య reset_చురుకైన_State();
	bool hasFragmentAfter(uint8_t fragment_id) స్థిరరాశి;

	/**
	 * Compatibility fallback for older QGroundControl builds that omit the
	 * final zero-length fragment: assemble the buffered message only if the
	 * buffered fragments are a gap-free run of full 180-byte fragments
	 * starting at fragment 0.
	 *
	 * @param destination Buffer that receives the assembled message bytes
	 * @return Length of the assembled message, or 0 if the fallback does not apply
	 *
	 */
	size_t assemble_Legacy_Exact_గుణిజం_సందేశం(uint8_t *destination) స్థిరరాశి;

	bool isComplete() స్థిరరాశి;
	size_t చివరి_Fragment_సూచీ() స్థిరరాశి;

	Fragment_స్థానం _fragments[GPS_RTCM_MAX_FRAGMENTS] {};
	uint8_t assembled_సందేశం[GPS_RTCM_MAX_సందేశం_LEN] {};
	uint8_t deferred_సందేశం[GPS_RTCM_MAX_సందేశం_LEN] {};
	size_t deferred_సందేశం_len {0};
	bool deferred_సందేశం_valid {false};
	// State for the sequence that is currently being assembled.
	SequenceState చురుకైన_sequence {};
};

} // namespace gnss
