/****************************************************************************
 *
 *   Copyright (c) 2015 PX4 Development Team. All rights reserved.
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

/*
 * @file sumd.cpp
 *
 * RC protocol definition for Graupner HoTT transmitter (SUMD/SUMH Protocol)
 *
 * @author Marco Bauer <marco@wtns.de>
 */

#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ "sumd.h"
#ಅನ್ತರ್ಭಾವ "common_rc.h"

ಎಣಿಕೆ SUMD_DECODE_STATE {
	SUMD_DECODE_STATE_UNSYNCED = 0,
	SUMD_DECODE_STATE_GOT_ಶೀರ್ಷಿಕೆ,
	SUMD_DECODE_STATE_GOT_STATE,
	SUMD_DECODE_STATE_GOT_LEN,
	SUMD_DECODE_STATE_GOT_ದತ್ತಾಂಶ,
	SUMD_DECODE_STATE_GOT_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ,
	SUMD_DECODE_STATE_GOT_CRC16_BYTE_1,
	SUMD_DECODE_STATE_GOT_CRC16_BYTE_2
};

/*
const char *decode_states[] = {"UNSYNCED",
			       "GOT_HEADER",
			       "GOT_STATE",
			       "GOT_LEN",
			       "GOT_DATA",
			       "GOT_CRC",
			       "GOT_CRC16_BYTE_1",
			       "GOT_CRC16_BYTE_2"
			      };
*/

uint8_t 	_crc8 	= 0x00;
uint16_t 	_crc16 = 0x0000;
ತಾರ್ಕಿಕ 		_sumd 		= ನಿಜ;
ತಾರ್ಕಿಕ		ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_OK		= ಸುಳ್ಳು;
ತಾರ್ಕಿಕ		_debug		= ಸುಳ್ಳು;


/* define range mapping here, -+100% -> 1000..2000 */
#ವ್ಯಾಖ್ಯಾನಿಸು SUMD_RANGE_MIN 0.0f
#ವ್ಯಾಖ್ಯಾನಿಸು SUMD_RANGE_MAX 4096.0f

#ವ್ಯಾಖ್ಯಾನಿಸು SUMD_ಗುರಿ_MIN 1000.0f
#ವ್ಯಾಖ್ಯಾನಿಸು SUMD_ಗುರಿ_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#ವ್ಯಾಖ್ಯಾನಿಸು SUMD_SCALE_ಗುಣಕ ((SUMD_ಗುರಿ_MAX - SUMD_ಗುರಿ_MIN) / (SUMD_RANGE_MAX - SUMD_RANGE_MIN))
#ವ್ಯಾಖ್ಯಾನಿಸು SUMD_SCALE_ಸ್ಥಾನಾಂತರ (ಪೂರ್ಣಾಂಕ)(SUMD_ಗುರಿ_MIN - (SUMD_SCALE_ಗುಣಕ * SUMD_RANGE_MIN + 0.5f))

ಸ್ಥಿರ ಎಣಿಕೆ SUMD_DECODE_STATE _decode_state = SUMD_DECODE_STATE_UNSYNCED;
ಸ್ಥಿರ uint8_t _rxlen;

ಸ್ಥಿರ ReceiverFcPacketHoTT &_rxpacket = rc_decode_buf._hottrxpacket;

uint16_t sumd_crc16(uint16_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ, uint8_t value)
{
	ಪೂರ್ಣಾಂಕ i;
	ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ ^= (uint16_t)value << 8;

	ಗಾಗಿ (i = 0; i < 8; i++) {
		ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & 0x8000) ? (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ << 1) ^ 0x1021 : (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ << 1);
	}

	ಹಿಂತಿರುಗಿ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;
}

uint8_t sumd_crc8(uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ, uint8_t value)
{
	ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ += value;
	ಹಿಂತಿರುಗಿ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;
}

ಪೂರ್ಣಾಂಕ sumd_decode(uint8_t byte, uint8_t *rssi, uint8_t *rx_ಎಣಿಕೆ, uint16_t *channel_ಎಣಿಕೆ, uint16_t *channels,
		uint16_t max_chan_ಎಣಿಕೆ, ತಾರ್ಕಿಕ *failsafe)
{

	ಪೂರ್ಣಾಂಕ ret = 1;

	ಆಯ್ಕೆ (_decode_state) {
	ಸ್ಥಿತಿ SUMD_DECODE_STATE_UNSYNCED:
		ಆದರೆ (_debug) {
			ಬರೆ(" SUMD_DECODE_STATE_UNSYNCED \n") ;
		}

		ಆದರೆ (byte == SUMD_ಶೀರ್ಷಿಕೆ_ID) {
			_rxpacket.ಶೀರ್ಷಿಕೆ = byte;
			_sumd = ನಿಜ;
			_rxlen = 0;
			_crc16 = 0x0000;
			_crc8 = 0x00;
			ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_OK = ಸುಳ್ಳು;
			_crc16 = sumd_crc16(_crc16, byte);
			_crc8 = sumd_crc8(_crc8, byte);
			_decode_state = SUMD_DECODE_STATE_GOT_ಶೀರ್ಷಿಕೆ;

			ಆದರೆ (_debug) {
				ಬರೆ(" SUMD_DECODE_STATE_GOT_HEADER: %x \n", byte) ;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			ret = 3;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SUMD_DECODE_STATE_GOT_ಶೀರ್ಷಿಕೆ:
		ಆದರೆ (byte == SUMD_ID_SUMD || byte == SUMD_ID_FAILSAFE || byte == SUMD_ID_SUMH) {
			_rxpacket.ಸ್ಥಿತಿ_2 = byte;

			ಆದರೆ (byte == SUMD_ID_SUMH) {
				_sumd = ಸುಳ್ಳು;
			}

			ಆದರೆ (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} ಇಲ್ಲದಿದ್ದರೆ {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_decode_state = SUMD_DECODE_STATE_GOT_STATE;

			ಆದರೆ (_debug) {
				ಬರೆ(" SUMD_DECODE_STATE_GOT_STATE: %x \n", byte) ;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SUMD_DECODE_STATE_GOT_STATE:
		ಆದರೆ (byte >= 2 && byte <= SUMD_MAX_CHANNELS) {
			_rxpacket.ಉದ್ದಳತೆ = byte;

			ಆದರೆ (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} ಇಲ್ಲದಿದ್ದರೆ {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_rxlen++;
			_decode_state = SUMD_DECODE_STATE_GOT_LEN;

			ಆದರೆ (_debug) {
				ಬರೆ(" SUMD_DECODE_STATE_GOT_LEN: %x (%d) \n", byte, byte) ;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SUMD_DECODE_STATE_GOT_LEN:
		_rxpacket.sumd_ದತ್ತಾಂಶ[_rxlen] = byte;

		ಆದರೆ (_sumd) {
			_crc16 = sumd_crc16(_crc16, byte);

		} ಇಲ್ಲದಿದ್ದರೆ {
			_crc8 = sumd_crc8(_crc8, byte);
		}

		_rxlen++;

		ಆದರೆ (_rxlen <= ((_rxpacket.ಉದ್ದಳತೆ * 2))) {
			ಆದರೆ (_debug) {
				ಬರೆ(" SUMD_DECODE_STATE_GOT_DATA[%d]: %x\n", _rxlen - 2, byte) ;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			_decode_state = SUMD_DECODE_STATE_GOT_ದತ್ತಾಂಶ;

			ಆದರೆ (_debug) {
				ಬರೆ(" SUMD_DECODE_STATE_GOT_DATA -- finish --\n") ;
			}

		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SUMD_DECODE_STATE_GOT_ದತ್ತಾಂಶ:
		_rxpacket.crc16_ಮೇಲಿನ = byte;

		ಆದರೆ (_debug) {
			ಬರೆ(" SUMD_DECODE_STATE_GOT_CRC16[1]: %x   [%x]\n", byte, ((_crc16 >> 8) & 0xff)) ;
		}

		ಆದರೆ (_sumd) {
			_decode_state = SUMD_DECODE_STATE_GOT_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;

		} ಇಲ್ಲದಿದ್ದರೆ {
			_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_1;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SUMD_DECODE_STATE_GOT_CRC16_BYTE_1:
		_rxpacket.crc16_ಕೆಳಗಿನ = byte;

		ಆದರೆ (_debug) {
			ಬರೆ(" SUMD_DECODE_STATE_GOT_CRC16[2]: %x   [%x]\n", byte, (_crc16 & 0xff)) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_2;

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SUMD_DECODE_STATE_GOT_CRC16_BYTE_2:
		_rxpacket.telemetry = byte;

		ಆದರೆ (_debug) {
			ಬರೆ(" SUMD_DECODE_STATE_GOT_SUMH_TELEMETRY: %x\n", byte) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SUMD_DECODE_STATE_GOT_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ:
		ಆದರೆ (_sumd) {
			_rxpacket.crc16_ಕೆಳಗಿನ = byte;

			ಆದರೆ (_debug) {
				ಬರೆ(" SUMD_DECODE_STATE_GOT_CRC[2]: %x   [%x]\n\n", byte, (_crc16 & 0xff)) ;
			}

			ಆದರೆ (_crc16 == (uint16_t)(_rxpacket.crc16_ಮೇಲಿನ << 8) + _rxpacket.crc16_ಕೆಳಗಿನ) {
				ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_OK = ನಿಜ;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			_rxpacket.crc8 = byte;

			ಆದರೆ (_debug) {
				ಬರೆ(" SUMD_DECODE_STATE_GOT_CRC8_SUMH: %x   [%x]\n\n", byte, _crc8) ;
			}

			ಆದರೆ (_crc8 == _rxpacket.crc8) {
				ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_OK = ನಿಜ;
			}
		}

		ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_OK) {
			ಆದರೆ (_debug) {
				ಬರೆ(" CRC - OK \n") ;
			}

			ಆದರೆ (_sumd) {
				ಆದರೆ (_debug) {
					ಬರೆ(" Got valid SUMD Packet\n") ;
				}

			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (_debug) {
					ಬರೆ(" Got valid SUMH Packet\n") ;
				}

			}

			ಆದರೆ (_debug) {
				ಬರೆ(" RXLEN: %d  [Chans: %d] \n\n", _rxlen - 1, (_rxlen - 1) / 2) ;
			}

			ret = 0;
			ಚಿಹ್ನೆರಹಿತ i;
			uint8_t _cnt = *rx_ಎಣಿಕೆ + 1;
			*rx_ಎಣಿಕೆ = _cnt;

			*rssi = 100;

			/* failsafe flag */
			*failsafe = (_rxpacket.ಸ್ಥಿತಿ_2 == SUMD_ID_FAILSAFE);

			/* received Channels */
			ಆದರೆ ((uint16_t)_rxpacket.ಉದ್ದಳತೆ > max_chan_ಎಣಿಕೆ) {
				_rxpacket.ಉದ್ದಳತೆ = (uint8_t) max_chan_ಎಣಿಕೆ;
			}

			*channel_ಎಣಿಕೆ = (uint16_t)_rxpacket.ಉದ್ದಳತೆ;

			/* decode the actual packet */
			/* reorder first 4 channels */

			/* ch1 = roll -> sumd = ch2 */
			channels[0] = (uint16_t)((_rxpacket.sumd_ದತ್ತಾಂಶ[1 * 2 + 1] << 8) | _rxpacket.sumd_ದತ್ತಾಂಶ[1 * 2 + 2]) >> 3;
			/* ch2 = pitch -> sumd = ch2 */
			channels[1] = (uint16_t)((_rxpacket.sumd_ದತ್ತಾಂಶ[2 * 2 + 1] << 8) | _rxpacket.sumd_ದತ್ತಾಂಶ[2 * 2 + 2]) >> 3;
			/* ch3 = throttle -> sumd = ch2 */
			channels[2] = (uint16_t)((_rxpacket.sumd_ದತ್ತಾಂಶ[0 * 2 + 1] << 8) | _rxpacket.sumd_ದತ್ತಾಂಶ[0 * 2 + 2]) >> 3;
			/* ch4 = yaw -> sumd = ch2 */
			channels[3] = (uint16_t)((_rxpacket.sumd_ದತ್ತಾಂಶ[3 * 2 + 1] << 8) | _rxpacket.sumd_ದತ್ತಾಂಶ[3 * 2 + 2]) >> 3;

			/* we start at channel 5(index 4) */
			ಚಿಹ್ನೆರಹಿತ chan_ಸೂಚ್ಯಂಕ = 4;

			ಗಾಗಿ (i = 4; i < _rxpacket.ಉದ್ದಳತೆ; i++) {
				ಆದರೆ (_debug) {
					ಬರೆ("ch[%d] : %x %x [ %x    %d ]\n", i + 1, _rxpacket.sumd_ದತ್ತಾಂಶ[i * 2 + 1], _rxpacket.sumd_ದತ್ತಾಂಶ[i * 2 + 2],
					       ((_rxpacket.sumd_ದತ್ತಾಂಶ[i * 2 + 1] << 8) | _rxpacket.sumd_ದತ್ತಾಂಶ[i * 2 + 2]) >> 3,
					       ((_rxpacket.sumd_ದತ್ತಾಂಶ[i * 2 + 1] << 8) | _rxpacket.sumd_ದತ್ತಾಂಶ[i * 2 + 2]) >> 3);
				}

				channels[chan_ಸೂಚ್ಯಂಕ] = (uint16_t)((_rxpacket.sumd_ದತ್ತಾಂಶ[i * 2 + 1] << 8) | _rxpacket.sumd_ದತ್ತಾಂಶ[i * 2 + 2]) >> 3;
				/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
				//channels[chan_index] = (uint16_t)(channels[chan_index] * SUMD_SCALE_FACTOR + .5f) + SUMD_SCALE_OFFSET;

				chan_ಸೂಚ್ಯಂಕ++;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			/* decoding failed */
			ret = 4;

			ಆದರೆ (_debug) {
				ಬರೆ(" CRC - fail \n") ;
			}

		}

		_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ ret;
}
