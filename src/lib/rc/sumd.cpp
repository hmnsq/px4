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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "sumd.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "common_rc.h"

ਗਿਣਤੀ SUMD_DECODE_STATE {
	SUMD_DECODE_STATE_UNSYNCED = 0,
	SUMD_DECODE_STATE_GOT_ਸਿਰਲੇਖ,
	SUMD_DECODE_STATE_GOT_STATE,
	SUMD_DECODE_STATE_GOT_LEN,
	SUMD_DECODE_STATE_GOT_ਅੰਕੜੇ,
	SUMD_DECODE_STATE_GOT_ਚੱਕਰੀ_ਜਾਂਚ,
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
ਤਾਰਕਿਕ 		_sumd 		= ਸੱਚ;
ਤਾਰਕਿਕ		ਚੱਕਰੀ_ਜਾਂਚ_OK		= ਝੂਠ;
ਤਾਰਕਿਕ		_debug		= ਝੂਠ;


/* define range mapping here, -+100% -> 1000..2000 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUMD_RANGE_MIN 0.0f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUMD_RANGE_MAX 4096.0f

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUMD_ਨਿਸ਼ਾਨਾ_MIN 1000.0f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUMD_ਨਿਸ਼ਾਨਾ_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUMD_SCALE_ਗੁਣਕ ((SUMD_ਨਿਸ਼ਾਨਾ_MAX - SUMD_ਨਿਸ਼ਾਨਾ_MIN) / (SUMD_RANGE_MAX - SUMD_RANGE_MIN))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUMD_SCALE_ਵਿੱਥ (ਪੂਰਨ)(SUMD_ਨਿਸ਼ਾਨਾ_MIN - (SUMD_SCALE_ਗੁਣਕ * SUMD_RANGE_MIN + 0.5f))

ਸਥਿਰ ਗਿਣਤੀ SUMD_DECODE_STATE _decode_state = SUMD_DECODE_STATE_UNSYNCED;
ਸਥਿਰ uint8_t _rxlen;

ਸਥਿਰ ReceiverFcPacketHoTT &_rxpacket = rc_decode_buf._hottrxpacket;

uint16_t sumd_crc16(uint16_t ਚੱਕਰੀ_ਜਾਂਚ, uint8_t value)
{
	ਪੂਰਨ i;
	ਚੱਕਰੀ_ਜਾਂਚ ^= (uint16_t)value << 8;

	ਲਈ (i = 0; i < 8; i++) {
		ਚੱਕਰੀ_ਜਾਂਚ = (ਚੱਕਰੀ_ਜਾਂਚ & 0x8000) ? (ਚੱਕਰੀ_ਜਾਂਚ << 1) ^ 0x1021 : (ਚੱਕਰੀ_ਜਾਂਚ << 1);
	}

	ਵਾਪਸ ਚੱਕਰੀ_ਜਾਂਚ;
}

uint8_t sumd_crc8(uint8_t ਚੱਕਰੀ_ਜਾਂਚ, uint8_t value)
{
	ਚੱਕਰੀ_ਜਾਂਚ += value;
	ਵਾਪਸ ਚੱਕਰੀ_ਜਾਂਚ;
}

ਪੂਰਨ sumd_decode(uint8_t byte, uint8_t *rssi, uint8_t *rx_ਗਿਣਤੀ, uint16_t *channel_ਗਿਣਤੀ, uint16_t *channels,
		uint16_t max_chan_ਗਿਣਤੀ, ਤਾਰਕਿਕ *failsafe)
{

	ਪੂਰਨ ret = 1;

	ਚੋਣ (_decode_state) {
	ਹਾਲਤ SUMD_DECODE_STATE_UNSYNCED:
		ਜੇ (_debug) {
			ਲਿਖੋ(" SUMD_DECODE_STATE_UNSYNCED \n") ;
		}

		ਜੇ (byte == SUMD_ਸਿਰਲੇਖ_ID) {
			_rxpacket.ਸਿਰਲੇਖ = byte;
			_sumd = ਸੱਚ;
			_rxlen = 0;
			_crc16 = 0x0000;
			_crc8 = 0x00;
			ਚੱਕਰੀ_ਜਾਂਚ_OK = ਝੂਠ;
			_crc16 = sumd_crc16(_crc16, byte);
			_crc8 = sumd_crc8(_crc8, byte);
			_decode_state = SUMD_DECODE_STATE_GOT_ਸਿਰਲੇਖ;

			ਜੇ (_debug) {
				ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_HEADER: %x \n", byte) ;
			}

		} ਨਹੀਂਤਾਂ {
			ret = 3;
		}

		ਰੋਕੋ;

	ਹਾਲਤ SUMD_DECODE_STATE_GOT_ਸਿਰਲੇਖ:
		ਜੇ (byte == SUMD_ID_SUMD || byte == SUMD_ID_FAILSAFE || byte == SUMD_ID_SUMH) {
			_rxpacket.ਸਥਿਤੀ = byte;

			ਜੇ (byte == SUMD_ID_SUMH) {
				_sumd = ਝੂਠ;
			}

			ਜੇ (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} ਨਹੀਂਤਾਂ {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_decode_state = SUMD_DECODE_STATE_GOT_STATE;

			ਜੇ (_debug) {
				ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_STATE: %x \n", byte) ;
			}

		} ਨਹੀਂਤਾਂ {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		ਰੋਕੋ;

	ਹਾਲਤ SUMD_DECODE_STATE_GOT_STATE:
		ਜੇ (byte >= 2 && byte <= SUMD_MAX_CHANNELS) {
			_rxpacket.ਲੰਮਾਈ = byte;

			ਜੇ (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} ਨਹੀਂਤਾਂ {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_rxlen++;
			_decode_state = SUMD_DECODE_STATE_GOT_LEN;

			ਜੇ (_debug) {
				ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_LEN: %x (%d) \n", byte, byte) ;
			}

		} ਨਹੀਂਤਾਂ {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		ਰੋਕੋ;

	ਹਾਲਤ SUMD_DECODE_STATE_GOT_LEN:
		_rxpacket.sumd_ਅੰਕੜੇ[_rxlen] = byte;

		ਜੇ (_sumd) {
			_crc16 = sumd_crc16(_crc16, byte);

		} ਨਹੀਂਤਾਂ {
			_crc8 = sumd_crc8(_crc8, byte);
		}

		_rxlen++;

		ਜੇ (_rxlen <= ((_rxpacket.ਲੰਮਾਈ * 2))) {
			ਜੇ (_debug) {
				ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_DATA[%d]: %x\n", _rxlen - 2, byte) ;
			}

		} ਨਹੀਂਤਾਂ {
			_decode_state = SUMD_DECODE_STATE_GOT_ਅੰਕੜੇ;

			ਜੇ (_debug) {
				ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_DATA -- finish --\n") ;
			}

		}

		ਰੋਕੋ;

	ਹਾਲਤ SUMD_DECODE_STATE_GOT_ਅੰਕੜੇ:
		_rxpacket.crc16_ਉੱਚਾ = byte;

		ਜੇ (_debug) {
			ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_CRC16[1]: %x   [%x]\n", byte, ((_crc16 >> 8) & 0xff)) ;
		}

		ਜੇ (_sumd) {
			_decode_state = SUMD_DECODE_STATE_GOT_ਚੱਕਰੀ_ਜਾਂਚ;

		} ਨਹੀਂਤਾਂ {
			_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_1;
		}

		ਰੋਕੋ;

	ਹਾਲਤ SUMD_DECODE_STATE_GOT_CRC16_BYTE_1:
		_rxpacket.crc16_ਨੀਵਾਂ = byte;

		ਜੇ (_debug) {
			ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_CRC16[2]: %x   [%x]\n", byte, (_crc16 & 0xff)) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_2;

		ਰੋਕੋ;

	ਹਾਲਤ SUMD_DECODE_STATE_GOT_CRC16_BYTE_2:
		_rxpacket.telemetry = byte;

		ਜੇ (_debug) {
			ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_SUMH_TELEMETRY: %x\n", byte) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_ਚੱਕਰੀ_ਜਾਂਚ;

		ਰੋਕੋ;

	ਹਾਲਤ SUMD_DECODE_STATE_GOT_ਚੱਕਰੀ_ਜਾਂਚ:
		ਜੇ (_sumd) {
			_rxpacket.crc16_ਨੀਵਾਂ = byte;

			ਜੇ (_debug) {
				ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_CRC[2]: %x   [%x]\n\n", byte, (_crc16 & 0xff)) ;
			}

			ਜੇ (_crc16 == (uint16_t)(_rxpacket.crc16_ਉੱਚਾ << 8) + _rxpacket.crc16_ਨੀਵਾਂ) {
				ਚੱਕਰੀ_ਜਾਂਚ_OK = ਸੱਚ;
			}

		} ਨਹੀਂਤਾਂ {
			_rxpacket.crc8 = byte;

			ਜੇ (_debug) {
				ਲਿਖੋ(" SUMD_DECODE_STATE_GOT_CRC8_SUMH: %x   [%x]\n\n", byte, _crc8) ;
			}

			ਜੇ (_crc8 == _rxpacket.crc8) {
				ਚੱਕਰੀ_ਜਾਂਚ_OK = ਸੱਚ;
			}
		}

		ਜੇ (ਚੱਕਰੀ_ਜਾਂਚ_OK) {
			ਜੇ (_debug) {
				ਲਿਖੋ(" CRC - OK \n") ;
			}

			ਜੇ (_sumd) {
				ਜੇ (_debug) {
					ਲਿਖੋ(" Got valid SUMD Packet\n") ;
				}

			} ਨਹੀਂਤਾਂ {
				ਜੇ (_debug) {
					ਲਿਖੋ(" Got valid SUMH Packet\n") ;
				}

			}

			ਜੇ (_debug) {
				ਲਿਖੋ(" RXLEN: %d  [Chans: %d] \n\n", _rxlen - 1, (_rxlen - 1) / 2) ;
			}

			ret = 0;
			ਬਿਨਾਨਿਸ਼ਾਨੀ i;
			uint8_t _cnt = *rx_ਗਿਣਤੀ + 1;
			*rx_ਗਿਣਤੀ = _cnt;

			*rssi = 100;

			/* failsafe flag */
			*failsafe = (_rxpacket.ਸਥਿਤੀ == SUMD_ID_FAILSAFE);

			/* received Channels */
			ਜੇ ((uint16_t)_rxpacket.ਲੰਮਾਈ > max_chan_ਗਿਣਤੀ) {
				_rxpacket.ਲੰਮਾਈ = (uint8_t) max_chan_ਗਿਣਤੀ;
			}

			*channel_ਗਿਣਤੀ = (uint16_t)_rxpacket.ਲੰਮਾਈ;

			/* decode the actual packet */
			/* reorder first 4 channels */

			/* ch1 = roll -> sumd = ch2 */
			channels[0] = (uint16_t)((_rxpacket.sumd_ਅੰਕੜੇ[1 * 2 + 1] << 8) | _rxpacket.sumd_ਅੰਕੜੇ[1 * 2 + 2]) >> 3;
			/* ch2 = pitch -> sumd = ch2 */
			channels[1] = (uint16_t)((_rxpacket.sumd_ਅੰਕੜੇ[2 * 2 + 1] << 8) | _rxpacket.sumd_ਅੰਕੜੇ[2 * 2 + 2]) >> 3;
			/* ch3 = throttle -> sumd = ch2 */
			channels[2] = (uint16_t)((_rxpacket.sumd_ਅੰਕੜੇ[0 * 2 + 1] << 8) | _rxpacket.sumd_ਅੰਕੜੇ[0 * 2 + 2]) >> 3;
			/* ch4 = yaw -> sumd = ch2 */
			channels[3] = (uint16_t)((_rxpacket.sumd_ਅੰਕੜੇ[3 * 2 + 1] << 8) | _rxpacket.sumd_ਅੰਕੜੇ[3 * 2 + 2]) >> 3;

			/* we start at channel 5(index 4) */
			ਬਿਨਾਨਿਸ਼ਾਨੀ chan_ਸੂਚਕਾਂਕ = 4;

			ਲਈ (i = 4; i < _rxpacket.ਲੰਮਾਈ; i++) {
				ਜੇ (_debug) {
					ਲਿਖੋ("ch[%d] : %x %x [ %x    %d ]\n", i + 1, _rxpacket.sumd_ਅੰਕੜੇ[i * 2 + 1], _rxpacket.sumd_ਅੰਕੜੇ[i * 2 + 2],
					       ((_rxpacket.sumd_ਅੰਕੜੇ[i * 2 + 1] << 8) | _rxpacket.sumd_ਅੰਕੜੇ[i * 2 + 2]) >> 3,
					       ((_rxpacket.sumd_ਅੰਕੜੇ[i * 2 + 1] << 8) | _rxpacket.sumd_ਅੰਕੜੇ[i * 2 + 2]) >> 3);
				}

				channels[chan_ਸੂਚਕਾਂਕ] = (uint16_t)((_rxpacket.sumd_ਅੰਕੜੇ[i * 2 + 1] << 8) | _rxpacket.sumd_ਅੰਕੜੇ[i * 2 + 2]) >> 3;
				/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
				//channels[chan_index] = (uint16_t)(channels[chan_index] * SUMD_SCALE_FACTOR + .5f) + SUMD_SCALE_OFFSET;

				chan_ਸੂਚਕਾਂਕ++;
			}

		} ਨਹੀਂਤਾਂ {
			/* decoding failed */
			ret = 4;

			ਜੇ (_debug) {
				ਲਿਖੋ(" CRC - fail \n") ;
			}

		}

		_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		ਰੋਕੋ;
	}

	ਵਾਪਸ ret;
}
