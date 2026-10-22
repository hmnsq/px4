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

/**
 * @file correction_framer.h
 *
 * Frame parser for GNSS correction streams: RTCM3, UBX (AssistNow MGA),
 * plus SPARTN when CONFIG_GPS_SPARTN is enabled.
 *
 * Corrections arrive as arbitrary byte chunks (uORB rtcm_corrections, serial),
 * so frame boundaries have to be recovered from the byte stream. All protocols
 * share one buffer: whichever preamble appears first is framed, and a valid
 * frame consumes its own payload. There is no sticky protocol selection — an
 * MGA (UBX) burst followed by SPARTN or RTCM3 is framed frame-by-frame in
 * arrival order.
 *
 * That sharing is what makes the framing safe. A separate framer per protocol
 * fed the same bytes would each scan inside the other's payloads, and SPARTN's
 * header carries no usable integrity check - TF006 is the only candidate and it
 * is 4 bits over a non-byte-aligned field - leaving only the message CRC, which
 * TF005 permits to be 8 bits wide. A stray 0x73 in an RTCM3 payload would then
 * be framed as SPARTN roughly once per 1024 occurrences.
 *
 * Usage:
 *   CorrectionFramer framer;
 *   framer.addData(chunk, len);
 *
 *   size_t frame_len;
 *   CorrectionProtocol protocol;
 *   const uint8_t *frame;
 *   while ((frame = framer.getNextMessage(&frame_len, &protocol)) != nullptr) {
 *       // ... use the frame ...
 *       framer.consumeMessage(frame_len);
 *   }
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_boardconfig.h> // CONFIG_GPS_SPARTN

#ਅਨ੍ਤਰ੍ਭਾਵ <cstdint>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>

#ਅਨ੍ਤਰ੍ਭਾਵ "rtcm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubx.h"

#ਜੇ defined(CONFIG_GPS_SPARTN)
# include "spartn.h"
#ਜੇ_ਅੰਤ

namespace gnss
{

ਗਿਣਤੀ class CorrectionProtocol : uint8_t {
	Rtcm3,
	Spartn,
	Ubx,
};

ਬਣਤਰ CorrectionFramerStats {
	uint32_t messages_parsed;   ///< Frames successfully parsed and consumed
	uint32_t ਚੱਕਰੀ_ਜਾਂਚ_errors_2;        ///< Frames rejected by their CRC / checksum
	uint32_t ਬਾਈਟਾਂ_discarded_2;   ///< Bytes discarded while searching for a frame
	uint32_t ਕੁੱਲ_ਚੌਖਟਾ_ਬਾਈਟਾਂ_2; ///< Total bytes in successfully parsed frames
	uint32_t rtcm3_messages;    ///< Subset of messages_parsed that were RTCM3
	uint32_t spartn_messages;   ///< Subset of messages_parsed that were SPARTN
	uint32_t ubx_messages;      ///< Subset of messages_parsed that were UBX
};

class CorrectionFramer
{
ਜਨਤਕ:
	ਸਥਿਰ constexpr size_t MAX_ਚੌਖਟਾ_LEN =
#ਜੇ defined(CONFIG_GPS_SPARTN)
		(SPARTN_MAX_ਚੌਖਟਾ_LEN > RTCM3_MAX_ਚੌਖਟਾ_LEN)
		? ((SPARTN_MAX_ਚੌਖਟਾ_LEN > UBX_MAX_ਚੌਖਟਾ_LEN) ? SPARTN_MAX_ਚੌਖਟਾ_LEN : UBX_MAX_ਚੌਖਟਾ_LEN)
		: ((RTCM3_MAX_ਚੌਖਟਾ_LEN > UBX_MAX_ਚੌਖਟਾ_LEN) ? RTCM3_MAX_ਚੌਖਟਾ_LEN : UBX_MAX_ਚੌਖਟਾ_LEN);
#ਨਹੀਂਤਾਂ
		(RTCM3_MAX_ਚੌਖਟਾ_LEN > UBX_MAX_ਚੌਖਟਾ_LEN) ? RTCM3_MAX_ਚੌਖਟਾ_LEN : UBX_MAX_ਚੌਖਟਾ_LEN;
#ਜੇ_ਅੰਤ

	// Enough for 2 max-size frames to handle overlap
	ਸਥਿਰ constexpr size_t ਬਫਰ_ਮਾਪ_2 = MAX_ਚੌਖਟਾ_LEN * 2;

	CorrectionFramer() = ਮੂਲ;

	/**
	 * Add data to the framer buffer.
	 *
	 * @param data Pointer to incoming data
	 * @param len  Number of bytes to add
	 * @return     Number of bytes actually added (may be less if the buffer is full)
	 */
	size_t ਜੋੜੋ_ਅੰਕੜੇ(ਅਚਲ uint8_t *ਅੰਕੜੇ, size_t len);

	/**
	 * Get a pointer to the next complete frame without consuming it.
	 *
	 * Returns a pointer into the framer's internal buffer. Invalid bytes at the
	 * buffer start are discarded during the search. The returned pointer remains
	 * valid until the next call to addData() or consumeMessage().
	 *
	 * @param out_len      Set to the total frame length
	 * @param out_protocol If non-null, set to the protocol the frame was framed as
	 * @return             Pointer to the frame, or nullptr if no complete valid
	 *                     frame is available
	 */
	ਅਚਲ uint8_t *get_ਅਗਲਾ_ਸੁਨੇਹਾ(size_t *ਬਾਹਰਲਾ_len, CorrectionProtocol *ਬਾਹਰਲਾ_protocol = nullptr);

	/**
	 * Consume (remove) the frame returned by the preceding getNextMessage().
	 *
	 * @param len Number of bytes to remove, as returned by getNextMessage()
	 */
	ਖਾਲੀ consume_ਸੁਨੇਹਾ(size_t len);

	size_t buffered_ਬਾਈਟਾਂ() ਅਚਲ { ਵਾਪਸ ਬਫਰ_len; }

	CorrectionFramerStats getStats() ਅਚਲ
	{
		ਵਾਪਸ {_messages_parsed, ਚੱਕਰੀ_ਜਾਂਚ_errors, ਬਾਈਟਾਂ_discarded, ਕੁੱਲ_ਚੌਖਟਾ_ਬਾਈਟਾਂ,
			_rtcm3_messages, _spartn_messages, _ubx_messages};
	}

ਨਿਜੀ:
	ਗਿਣਤੀ class Probe {
		Need_ਹੋਰ_ਅੰਕੜੇ,  ///< Could still become a valid frame, wait for more bytes
		Invalid_ਸਿਰਲੇਖ, ///< Cannot be a frame start
		ਚੱਕਰੀ_ਜਾਂਚ_Error,      ///< Frame is complete but failed its CRC / checksum
		Complete,
	};

	ਸਥਿਰ bool isPreamble(uint8_t byte);

	Probe probeRtcm3(size_t *ਚੌਖਟਾ_len) ਅਚਲ;
	Probe probeUbx(size_t *ਚੌਖਟਾ_len) ਅਚਲ;
#ਜੇ defined(CONFIG_GPS_SPARTN)
	Probe probeSpartn(size_t *ਚੌਖਟਾ_len) ਅਚਲ;
#ਜੇ_ਅੰਤ

	ਖਾਲੀ discard_ਬਾਈਟਾਂ(size_t ਗਿਣਤੀ_2);

	uint8_t  ਬਫਰ[ਬਫਰ_ਮਾਪ_2] {};
	size_t   ਬਫਰ_len {0};

	// Protocol of the frame returned by the last getNextMessage(), applied to
	// the per-protocol counters once consumeMessage() commits it.
	CorrectionProtocol _pending_protocol {CorrectionProtocol::Rtcm3};

	// Statistics
	uint32_t _messages_parsed {0};
	uint32_t ਚੱਕਰੀ_ਜਾਂਚ_errors {0};
	uint32_t ਬਾਈਟਾਂ_discarded {0};
	uint32_t ਕੁੱਲ_ਚੌਖਟਾ_ਬਾਈਟਾਂ {0};
	uint32_t _rtcm3_messages {0};
	uint32_t _spartn_messages {0};
	uint32_t _ubx_messages {0};
};

} // namespace gnss
