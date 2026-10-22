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

#അന്തര്ഭാവ <stdbool.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ "sumd.h"
#അന്തര്ഭാവ "common_rc.h"

എണ്ണൽ SUMD_DECODE_STATE {
	SUMD_DECODE_STATE_UNSYNCED = 0,
	SUMD_DECODE_STATE_GOT_HEADER,
	SUMD_DECODE_STATE_GOT_STATE,
	SUMD_DECODE_STATE_GOT_LEN,
	SUMD_DECODE_STATE_GOT_DATA,
	SUMD_DECODE_STATE_GOT_CRC,
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
തർക്കം 		_sumd 		= സത്യം;
തർക്കം		_crcOK		= അസത്യം;
തർക്കം		_debug		= അസത്യം;


/* define range mapping here, -+100% -> 1000..2000 */
#define SUMD_RANGE_MIN 0.0f
#define SUMD_RANGE_MAX 4096.0f

#define SUMD_TARGET_MIN 1000.0f
#define SUMD_TARGET_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#define SUMD_SCALE_FACTOR ((SUMD_TARGET_MAX - SUMD_TARGET_MIN) / (SUMD_RANGE_MAX - SUMD_RANGE_MIN))
#define SUMD_SCALE_OFFSET (പൂർണ്ണസംഖ്യ)(SUMD_TARGET_MIN - (SUMD_SCALE_FACTOR * SUMD_RANGE_MIN + 0.5f))

സ്ഥിതം എണ്ണൽ SUMD_DECODE_STATE _decode_state = SUMD_DECODE_STATE_UNSYNCED;
സ്ഥിതം uint8_t _rxlen;

സ്ഥിതം ReceiverFcPacketHoTT &_rxpacket = rc_decode_buf._hottrxpacket;

uint16_t sumd_crc16(uint16_t crc, uint8_t value)
{
	പൂർണ്ണസംഖ്യ i;
	crc ^= (uint16_t)value << 8;

	വേണ്ടി (i = 0; i < 8; i++) {
		crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
	}

	തിരികെ crc;
}

uint8_t sumd_crc8(uint8_t crc, uint8_t value)
{
	crc += value;
	തിരികെ crc;
}

പൂർണ്ണസംഖ്യ sumd_decode(uint8_t byte, uint8_t *rssi, uint8_t *rx_എണ്ണം, uint16_t *channel_എണ്ണം, uint16_t *channels,
		uint16_t max_chan_എണ്ണം, തർക്കം *failsafe)
{

	പൂർണ്ണസംഖ്യ ret = 1;

	തിരഞ്ഞെടുപ്പ് (_decode_state) {
	അവസ്ഥ SUMD_DECODE_STATE_UNSYNCED:
		എങ്കിൽ (_debug) {
			printf(" SUMD_DECODE_STATE_UNSYNCED \n") ;
		}

		എങ്കിൽ (byte == SUMD_HEADER_ID) {
			_rxpacket.header = byte;
			_sumd = സത്യം;
			_rxlen = 0;
			_crc16 = 0x0000;
			_crc8 = 0x00;
			_crcOK = അസത്യം;
			_crc16 = sumd_crc16(_crc16, byte);
			_crc8 = sumd_crc8(_crc8, byte);
			_decode_state = SUMD_DECODE_STATE_GOT_HEADER;

			എങ്കിൽ (_debug) {
				printf(" SUMD_DECODE_STATE_GOT_HEADER: %x \n", byte) ;
			}

		} അല്ലെങ്കിൽ {
			ret = 3;
		}

		നിർത്തുക;

	അവസ്ഥ SUMD_DECODE_STATE_GOT_HEADER:
		എങ്കിൽ (byte == SUMD_ID_SUMD || byte == SUMD_ID_FAILSAFE || byte == SUMD_ID_SUMH) {
			_rxpacket.status = byte;

			എങ്കിൽ (byte == SUMD_ID_SUMH) {
				_sumd = അസത്യം;
			}

			എങ്കിൽ (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} അല്ലെങ്കിൽ {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_decode_state = SUMD_DECODE_STATE_GOT_STATE;

			എങ്കിൽ (_debug) {
				printf(" SUMD_DECODE_STATE_GOT_STATE: %x \n", byte) ;
			}

		} അല്ലെങ്കിൽ {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		നിർത്തുക;

	അവസ്ഥ SUMD_DECODE_STATE_GOT_STATE:
		എങ്കിൽ (byte >= 2 && byte <= SUMD_MAX_CHANNELS) {
			_rxpacket.length = byte;

			എങ്കിൽ (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} അല്ലെങ്കിൽ {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_rxlen++;
			_decode_state = SUMD_DECODE_STATE_GOT_LEN;

			എങ്കിൽ (_debug) {
				printf(" SUMD_DECODE_STATE_GOT_LEN: %x (%d) \n", byte, byte) ;
			}

		} അല്ലെങ്കിൽ {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		നിർത്തുക;

	അവസ്ഥ SUMD_DECODE_STATE_GOT_LEN:
		_rxpacket.sumd_data[_rxlen] = byte;

		എങ്കിൽ (_sumd) {
			_crc16 = sumd_crc16(_crc16, byte);

		} അല്ലെങ്കിൽ {
			_crc8 = sumd_crc8(_crc8, byte);
		}

		_rxlen++;

		എങ്കിൽ (_rxlen <= ((_rxpacket.length * 2))) {
			എങ്കിൽ (_debug) {
				printf(" SUMD_DECODE_STATE_GOT_DATA[%d]: %x\n", _rxlen - 2, byte) ;
			}

		} അല്ലെങ്കിൽ {
			_decode_state = SUMD_DECODE_STATE_GOT_DATA;

			എങ്കിൽ (_debug) {
				printf(" SUMD_DECODE_STATE_GOT_DATA -- finish --\n") ;
			}

		}

		നിർത്തുക;

	അവസ്ഥ SUMD_DECODE_STATE_GOT_DATA:
		_rxpacket.crc16_മുകളിലെ = byte;

		എങ്കിൽ (_debug) {
			printf(" SUMD_DECODE_STATE_GOT_CRC16[1]: %x   [%x]\n", byte, ((_crc16 >> 8) & 0xff)) ;
		}

		എങ്കിൽ (_sumd) {
			_decode_state = SUMD_DECODE_STATE_GOT_CRC;

		} അല്ലെങ്കിൽ {
			_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_1;
		}

		നിർത്തുക;

	അവസ്ഥ SUMD_DECODE_STATE_GOT_CRC16_BYTE_1:
		_rxpacket.crc16_താഴെയുള്ള = byte;

		എങ്കിൽ (_debug) {
			printf(" SUMD_DECODE_STATE_GOT_CRC16[2]: %x   [%x]\n", byte, (_crc16 & 0xff)) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_2;

		നിർത്തുക;

	അവസ്ഥ SUMD_DECODE_STATE_GOT_CRC16_BYTE_2:
		_rxpacket.telemetry = byte;

		എങ്കിൽ (_debug) {
			printf(" SUMD_DECODE_STATE_GOT_SUMH_TELEMETRY: %x\n", byte) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_CRC;

		നിർത്തുക;

	അവസ്ഥ SUMD_DECODE_STATE_GOT_CRC:
		എങ്കിൽ (_sumd) {
			_rxpacket.crc16_താഴെയുള്ള = byte;

			എങ്കിൽ (_debug) {
				printf(" SUMD_DECODE_STATE_GOT_CRC[2]: %x   [%x]\n\n", byte, (_crc16 & 0xff)) ;
			}

			എങ്കിൽ (_crc16 == (uint16_t)(_rxpacket.crc16_മുകളിലെ << 8) + _rxpacket.crc16_താഴെയുള്ള) {
				_crcOK = സത്യം;
			}

		} അല്ലെങ്കിൽ {
			_rxpacket.crc8 = byte;

			എങ്കിൽ (_debug) {
				printf(" SUMD_DECODE_STATE_GOT_CRC8_SUMH: %x   [%x]\n\n", byte, _crc8) ;
			}

			എങ്കിൽ (_crc8 == _rxpacket.crc8) {
				_crcOK = സത്യം;
			}
		}

		എങ്കിൽ (_crcOK) {
			എങ്കിൽ (_debug) {
				printf(" CRC - OK \n") ;
			}

			എങ്കിൽ (_sumd) {
				എങ്കിൽ (_debug) {
					printf(" Got valid SUMD Packet\n") ;
				}

			} അല്ലെങ്കിൽ {
				എങ്കിൽ (_debug) {
					printf(" Got valid SUMH Packet\n") ;
				}

			}

			എങ്കിൽ (_debug) {
				printf(" RXLEN: %d  [Chans: %d] \n\n", _rxlen - 1, (_rxlen - 1) / 2) ;
			}

			ret = 0;
			unsigned i;
			uint8_t _cnt = *rx_എണ്ണം + 1;
			*rx_എണ്ണം = _cnt;

			*rssi = 100;

			/* failsafe flag */
			*failsafe = (_rxpacket.status == SUMD_ID_FAILSAFE);

			/* received Channels */
			എങ്കിൽ ((uint16_t)_rxpacket.length > max_chan_എണ്ണം) {
				_rxpacket.length = (uint8_t) max_chan_എണ്ണം;
			}

			*channel_എണ്ണം = (uint16_t)_rxpacket.length;

			/* decode the actual packet */
			/* reorder first 4 channels */

			/* ch1 = roll -> sumd = ch2 */
			channels[0] = (uint16_t)((_rxpacket.sumd_data[1 * 2 + 1] << 8) | _rxpacket.sumd_data[1 * 2 + 2]) >> 3;
			/* ch2 = pitch -> sumd = ch2 */
			channels[1] = (uint16_t)((_rxpacket.sumd_data[2 * 2 + 1] << 8) | _rxpacket.sumd_data[2 * 2 + 2]) >> 3;
			/* ch3 = throttle -> sumd = ch2 */
			channels[2] = (uint16_t)((_rxpacket.sumd_data[0 * 2 + 1] << 8) | _rxpacket.sumd_data[0 * 2 + 2]) >> 3;
			/* ch4 = yaw -> sumd = ch2 */
			channels[3] = (uint16_t)((_rxpacket.sumd_data[3 * 2 + 1] << 8) | _rxpacket.sumd_data[3 * 2 + 2]) >> 3;

			/* we start at channel 5(index 4) */
			unsigned chan_index = 4;

			വേണ്ടി (i = 4; i < _rxpacket.length; i++) {
				എങ്കിൽ (_debug) {
					printf("ch[%d] : %x %x [ %x    %d ]\n", i + 1, _rxpacket.sumd_data[i * 2 + 1], _rxpacket.sumd_data[i * 2 + 2],
					       ((_rxpacket.sumd_data[i * 2 + 1] << 8) | _rxpacket.sumd_data[i * 2 + 2]) >> 3,
					       ((_rxpacket.sumd_data[i * 2 + 1] << 8) | _rxpacket.sumd_data[i * 2 + 2]) >> 3);
				}

				channels[chan_index] = (uint16_t)((_rxpacket.sumd_data[i * 2 + 1] << 8) | _rxpacket.sumd_data[i * 2 + 2]) >> 3;
				/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
				//channels[chan_index] = (uint16_t)(channels[chan_index] * SUMD_SCALE_FACTOR + .5f) + SUMD_SCALE_OFFSET;

				chan_index++;
			}

		} അല്ലെങ്കിൽ {
			/* decoding failed */
			ret = 4;

			എങ്കിൽ (_debug) {
				printf(" CRC - fail \n") ;
			}

		}

		_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		നിർത്തുക;
	}

	തിരികെ ret;
}
