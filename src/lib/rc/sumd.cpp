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

#ضمن <stdbool.h>
#ضمن <stdio.h>
#ضمن "sumd.h"
#ضمن "common_rc.h"

تعداد SUMD_DECODE_STATE {
	SUMD_DECODE_STATE_UNSYNCED = 0,
	SUMD_DECODE_STATE_GOT_سرآیند,
	SUMD_DECODE_STATE_GOT_STATE,
	SUMD_DECODE_STATE_GOT_LEN,
	SUMD_DECODE_STATE_GOT_داده,
	SUMD_DECODE_STATE_GOT_بررسی_چرخهای,
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
منطقي 		_sumd 		= درست;
منطقي		بررسی_چرخهای_OK		= خطأ;
منطقي		_debug		= خطأ;


/* define range mapping here, -+100% -> 1000..2000 */
#تعریف_کن SUMD_RANGE_MIN 0.0f
#تعریف_کن SUMD_RANGE_MAX 4096.0f

#تعریف_کن SUMD_هدف_MIN 1000.0f
#تعریف_کن SUMD_هدف_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#تعریف_کن SUMD_SCALE_ضریب ((SUMD_هدف_MAX - SUMD_هدف_MIN) / (SUMD_RANGE_MAX - SUMD_RANGE_MIN))
#تعریف_کن SUMD_SCALE_جابه_جایی (صحيح)(SUMD_هدف_MIN - (SUMD_SCALE_ضریب * SUMD_RANGE_MIN + 0.5f))

ساكن تعداد SUMD_DECODE_STATE _decode_state = SUMD_DECODE_STATE_UNSYNCED;
ساكن uint8_t _rxlen;

ساكن ReceiverFcPacketHoTT &_rxpacket = rc_decode_buf._hottrxpacket;

uint16_t sumd_crc16(uint16_t بررسی_چرخهای, uint8_t value)
{
	صحيح i;
	بررسی_چرخهای ^= (uint16_t)value << 8;

	لكل (i = 0; i < 8; i++) {
		بررسی_چرخهای = (بررسی_چرخهای & 0x8000) ? (بررسی_چرخهای << 1) ^ 0x1021 : (بررسی_چرخهای << 1);
	}

	أرجع بررسی_چرخهای;
}

uint8_t sumd_crc8(uint8_t بررسی_چرخهای, uint8_t value)
{
	بررسی_چرخهای += value;
	أرجع بررسی_چرخهای;
}

صحيح sumd_decode(uint8_t byte, uint8_t *rssi, uint8_t *rx_شمار, uint16_t *channel_شمار, uint16_t *channels,
		uint16_t max_chan_شمار, منطقي *failsafe)
{

	صحيح ret = 1;

	بدل (_decode_state) {
	حالة SUMD_DECODE_STATE_UNSYNCED:
		إذا (_debug) {
			چاپ_کن(" SUMD_DECODE_STATE_UNSYNCED \n") ;
		}

		إذا (byte == SUMD_سرآیند_ID) {
			_rxpacket.سرآیند = byte;
			_sumd = درست;
			_rxlen = 0;
			_crc16 = 0x0000;
			_crc8 = 0x00;
			بررسی_چرخهای_OK = خطأ;
			_crc16 = sumd_crc16(_crc16, byte);
			_crc8 = sumd_crc8(_crc8, byte);
			_decode_state = SUMD_DECODE_STATE_GOT_سرآیند;

			إذا (_debug) {
				چاپ_کن(" SUMD_DECODE_STATE_GOT_HEADER: %x \n", byte) ;
			}

		} وإلا {
			ret = 3;
		}

		اكسر;

	حالة SUMD_DECODE_STATE_GOT_سرآیند:
		إذا (byte == SUMD_ID_SUMD || byte == SUMD_ID_FAILSAFE || byte == SUMD_ID_SUMH) {
			_rxpacket.وضعیت = byte;

			إذا (byte == SUMD_ID_SUMH) {
				_sumd = خطأ;
			}

			إذا (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} وإلا {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_decode_state = SUMD_DECODE_STATE_GOT_STATE;

			إذا (_debug) {
				چاپ_کن(" SUMD_DECODE_STATE_GOT_STATE: %x \n", byte) ;
			}

		} وإلا {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		اكسر;

	حالة SUMD_DECODE_STATE_GOT_STATE:
		إذا (byte >= 2 && byte <= SUMD_MAX_CHANNELS) {
			_rxpacket.درازی = byte;

			إذا (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} وإلا {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_rxlen++;
			_decode_state = SUMD_DECODE_STATE_GOT_LEN;

			إذا (_debug) {
				چاپ_کن(" SUMD_DECODE_STATE_GOT_LEN: %x (%d) \n", byte, byte) ;
			}

		} وإلا {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		اكسر;

	حالة SUMD_DECODE_STATE_GOT_LEN:
		_rxpacket.sumd_داده[_rxlen] = byte;

		إذا (_sumd) {
			_crc16 = sumd_crc16(_crc16, byte);

		} وإلا {
			_crc8 = sumd_crc8(_crc8, byte);
		}

		_rxlen++;

		إذا (_rxlen <= ((_rxpacket.درازی * 2))) {
			إذا (_debug) {
				چاپ_کن(" SUMD_DECODE_STATE_GOT_DATA[%d]: %x\n", _rxlen - 2, byte) ;
			}

		} وإلا {
			_decode_state = SUMD_DECODE_STATE_GOT_داده;

			إذا (_debug) {
				چاپ_کن(" SUMD_DECODE_STATE_GOT_DATA -- finish --\n") ;
			}

		}

		اكسر;

	حالة SUMD_DECODE_STATE_GOT_داده:
		_rxpacket.crc16_بالا = byte;

		إذا (_debug) {
			چاپ_کن(" SUMD_DECODE_STATE_GOT_CRC16[1]: %x   [%x]\n", byte, ((_crc16 >> 8) & 0xff)) ;
		}

		إذا (_sumd) {
			_decode_state = SUMD_DECODE_STATE_GOT_بررسی_چرخهای;

		} وإلا {
			_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_1;
		}

		اكسر;

	حالة SUMD_DECODE_STATE_GOT_CRC16_BYTE_1:
		_rxpacket.crc16_کم = byte;

		إذا (_debug) {
			چاپ_کن(" SUMD_DECODE_STATE_GOT_CRC16[2]: %x   [%x]\n", byte, (_crc16 & 0xff)) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_2;

		اكسر;

	حالة SUMD_DECODE_STATE_GOT_CRC16_BYTE_2:
		_rxpacket.telemetry = byte;

		إذا (_debug) {
			چاپ_کن(" SUMD_DECODE_STATE_GOT_SUMH_TELEMETRY: %x\n", byte) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_بررسی_چرخهای;

		اكسر;

	حالة SUMD_DECODE_STATE_GOT_بررسی_چرخهای:
		إذا (_sumd) {
			_rxpacket.crc16_کم = byte;

			إذا (_debug) {
				چاپ_کن(" SUMD_DECODE_STATE_GOT_CRC[2]: %x   [%x]\n\n", byte, (_crc16 & 0xff)) ;
			}

			إذا (_crc16 == (uint16_t)(_rxpacket.crc16_بالا << 8) + _rxpacket.crc16_کم) {
				بررسی_چرخهای_OK = درست;
			}

		} وإلا {
			_rxpacket.crc8 = byte;

			إذا (_debug) {
				چاپ_کن(" SUMD_DECODE_STATE_GOT_CRC8_SUMH: %x   [%x]\n\n", byte, _crc8) ;
			}

			إذا (_crc8 == _rxpacket.crc8) {
				بررسی_چرخهای_OK = درست;
			}
		}

		إذا (بررسی_چرخهای_OK) {
			إذا (_debug) {
				چاپ_کن(" CRC - OK \n") ;
			}

			إذا (_sumd) {
				إذا (_debug) {
					چاپ_کن(" Got valid SUMD Packet\n") ;
				}

			} وإلا {
				إذا (_debug) {
					چاپ_کن(" Got valid SUMH Packet\n") ;
				}

			}

			إذا (_debug) {
				چاپ_کن(" RXLEN: %d  [Chans: %d] \n\n", _rxlen - 1, (_rxlen - 1) / 2) ;
			}

			ret = 0;
			بیعلامت i;
			uint8_t _cnt = *rx_شمار + 1;
			*rx_شمار = _cnt;

			*rssi = 100;

			/* failsafe flag */
			*failsafe = (_rxpacket.وضعیت == SUMD_ID_FAILSAFE);

			/* received Channels */
			إذا ((uint16_t)_rxpacket.درازی > max_chan_شمار) {
				_rxpacket.درازی = (uint8_t) max_chan_شمار;
			}

			*channel_شمار = (uint16_t)_rxpacket.درازی;

			/* decode the actual packet */
			/* reorder first 4 channels */

			/* ch1 = roll -> sumd = ch2 */
			channels[0] = (uint16_t)((_rxpacket.sumd_داده[1 * 2 + 1] << 8) | _rxpacket.sumd_داده[1 * 2 + 2]) >> 3;
			/* ch2 = pitch -> sumd = ch2 */
			channels[1] = (uint16_t)((_rxpacket.sumd_داده[2 * 2 + 1] << 8) | _rxpacket.sumd_داده[2 * 2 + 2]) >> 3;
			/* ch3 = throttle -> sumd = ch2 */
			channels[2] = (uint16_t)((_rxpacket.sumd_داده[0 * 2 + 1] << 8) | _rxpacket.sumd_داده[0 * 2 + 2]) >> 3;
			/* ch4 = yaw -> sumd = ch2 */
			channels[3] = (uint16_t)((_rxpacket.sumd_داده[3 * 2 + 1] << 8) | _rxpacket.sumd_داده[3 * 2 + 2]) >> 3;

			/* we start at channel 5(index 4) */
			بیعلامت chan_شاخص = 4;

			لكل (i = 4; i < _rxpacket.درازی; i++) {
				إذا (_debug) {
					چاپ_کن("ch[%d] : %x %x [ %x    %d ]\n", i + 1, _rxpacket.sumd_داده[i * 2 + 1], _rxpacket.sumd_داده[i * 2 + 2],
					       ((_rxpacket.sumd_داده[i * 2 + 1] << 8) | _rxpacket.sumd_داده[i * 2 + 2]) >> 3,
					       ((_rxpacket.sumd_داده[i * 2 + 1] << 8) | _rxpacket.sumd_داده[i * 2 + 2]) >> 3);
				}

				channels[chan_شاخص] = (uint16_t)((_rxpacket.sumd_داده[i * 2 + 1] << 8) | _rxpacket.sumd_داده[i * 2 + 2]) >> 3;
				/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
				//channels[chan_index] = (uint16_t)(channels[chan_index] * SUMD_SCALE_FACTOR + .5f) + SUMD_SCALE_OFFSET;

				chan_شاخص++;
			}

		} وإلا {
			/* decoding failed */
			ret = 4;

			إذا (_debug) {
				چاپ_کن(" CRC - fail \n") ;
			}

		}

		_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		اكسر;
	}

	أرجع ret;
}
