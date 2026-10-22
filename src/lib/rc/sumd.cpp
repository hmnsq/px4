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

#شامل <stdbool.h>
#شامل <stdio.h>
#شامل "sumd.h"
#شامل "common_rc.h"

شمار SUMD_DECODE_STATE {
	SUMD_DECODE_STATE_UNSYNCED = 0,
	SUMD_DECODE_STATE_GOT_سرخی,
	SUMD_DECODE_STATE_GOT_STATE,
	SUMD_DECODE_STATE_GOT_LEN,
	SUMD_DECODE_STATE_GOT_ڈیٹا,
	SUMD_DECODE_STATE_GOT_چکری_جانچ,
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
منطقی 		_sumd 		= سچ;
منطقی		چکری_جانچ_OK		= جھوٹ;
منطقی		_debug		= جھوٹ;


/* define range mapping here, -+100% -> 1000..2000 */
#تعریف_کرو SUMD_RANGE_MIN 0.0f
#تعریف_کرو SUMD_RANGE_MAX 4096.0f

#تعریف_کرو SUMD_ہدف_MIN 1000.0f
#تعریف_کرو SUMD_ہدف_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#تعریف_کرو SUMD_SCALE_ضربی_عدد ((SUMD_ہدف_MAX - SUMD_ہدف_MIN) / (SUMD_RANGE_MAX - SUMD_RANGE_MIN))
#تعریف_کرو SUMD_SCALE_انحراف (صحیح)(SUMD_ہدف_MIN - (SUMD_SCALE_ضربی_عدد * SUMD_RANGE_MIN + 0.5f))

ساکن شمار SUMD_DECODE_STATE _decode_state = SUMD_DECODE_STATE_UNSYNCED;
ساکن uint8_t _rxlen;

ساکن ReceiverFcPacketHoTT &_rxpacket = rc_decode_buf._hottrxpacket;

uint16_t sumd_crc16(uint16_t چکری_جانچ, uint8_t value)
{
	صحیح i;
	چکری_جانچ ^= (uint16_t)value << 8;

	برائے (i = 0; i < 8; i++) {
		چکری_جانچ = (چکری_جانچ & 0x8000) ? (چکری_جانچ << 1) ^ 0x1021 : (چکری_جانچ << 1);
	}

	واپس چکری_جانچ;
}

uint8_t sumd_crc8(uint8_t چکری_جانچ, uint8_t value)
{
	چکری_جانچ += value;
	واپس چکری_جانچ;
}

صحیح sumd_decode(uint8_t byte, uint8_t *rssi, uint8_t *rx_گنتی, uint16_t *channel_گنتی, uint16_t *channels,
		uint16_t max_chan_گنتی, منطقی *failsafe)
{

	صحیح ret = 1;

	انتخاب (_decode_state) {
	حالت SUMD_DECODE_STATE_UNSYNCED:
		اگر (_debug) {
			لکھو(" SUMD_DECODE_STATE_UNSYNCED \n") ;
		}

		اگر (byte == SUMD_سرخی_ID) {
			_rxpacket.سرخی = byte;
			_sumd = سچ;
			_rxlen = 0;
			_crc16 = 0x0000;
			_crc8 = 0x00;
			چکری_جانچ_OK = جھوٹ;
			_crc16 = sumd_crc16(_crc16, byte);
			_crc8 = sumd_crc8(_crc8, byte);
			_decode_state = SUMD_DECODE_STATE_GOT_سرخی;

			اگر (_debug) {
				لکھو(" SUMD_DECODE_STATE_GOT_HEADER: %x \n", byte) ;
			}

		} ورنہ {
			ret = 3;
		}

		توڑو;

	حالت SUMD_DECODE_STATE_GOT_سرخی:
		اگر (byte == SUMD_ID_SUMD || byte == SUMD_ID_FAILSAFE || byte == SUMD_ID_SUMH) {
			_rxpacket.حالت_2 = byte;

			اگر (byte == SUMD_ID_SUMH) {
				_sumd = جھوٹ;
			}

			اگر (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} ورنہ {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_decode_state = SUMD_DECODE_STATE_GOT_STATE;

			اگر (_debug) {
				لکھو(" SUMD_DECODE_STATE_GOT_STATE: %x \n", byte) ;
			}

		} ورنہ {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		توڑو;

	حالت SUMD_DECODE_STATE_GOT_STATE:
		اگر (byte >= 2 && byte <= SUMD_MAX_CHANNELS) {
			_rxpacket.لمبائی_2 = byte;

			اگر (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} ورنہ {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_rxlen++;
			_decode_state = SUMD_DECODE_STATE_GOT_LEN;

			اگر (_debug) {
				لکھو(" SUMD_DECODE_STATE_GOT_LEN: %x (%d) \n", byte, byte) ;
			}

		} ورنہ {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		توڑو;

	حالت SUMD_DECODE_STATE_GOT_LEN:
		_rxpacket.sumd_ڈیٹا[_rxlen] = byte;

		اگر (_sumd) {
			_crc16 = sumd_crc16(_crc16, byte);

		} ورنہ {
			_crc8 = sumd_crc8(_crc8, byte);
		}

		_rxlen++;

		اگر (_rxlen <= ((_rxpacket.لمبائی_2 * 2))) {
			اگر (_debug) {
				لکھو(" SUMD_DECODE_STATE_GOT_DATA[%d]: %x\n", _rxlen - 2, byte) ;
			}

		} ورنہ {
			_decode_state = SUMD_DECODE_STATE_GOT_ڈیٹا;

			اگر (_debug) {
				لکھو(" SUMD_DECODE_STATE_GOT_DATA -- finish --\n") ;
			}

		}

		توڑو;

	حالت SUMD_DECODE_STATE_GOT_ڈیٹا:
		_rxpacket.crc16_بلند = byte;

		اگر (_debug) {
			لکھو(" SUMD_DECODE_STATE_GOT_CRC16[1]: %x   [%x]\n", byte, ((_crc16 >> 8) & 0xff)) ;
		}

		اگر (_sumd) {
			_decode_state = SUMD_DECODE_STATE_GOT_چکری_جانچ;

		} ورنہ {
			_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_1;
		}

		توڑو;

	حالت SUMD_DECODE_STATE_GOT_CRC16_BYTE_1:
		_rxpacket.crc16_کم = byte;

		اگر (_debug) {
			لکھو(" SUMD_DECODE_STATE_GOT_CRC16[2]: %x   [%x]\n", byte, (_crc16 & 0xff)) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_2;

		توڑو;

	حالت SUMD_DECODE_STATE_GOT_CRC16_BYTE_2:
		_rxpacket.telemetry = byte;

		اگر (_debug) {
			لکھو(" SUMD_DECODE_STATE_GOT_SUMH_TELEMETRY: %x\n", byte) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_چکری_جانچ;

		توڑو;

	حالت SUMD_DECODE_STATE_GOT_چکری_جانچ:
		اگر (_sumd) {
			_rxpacket.crc16_کم = byte;

			اگر (_debug) {
				لکھو(" SUMD_DECODE_STATE_GOT_CRC[2]: %x   [%x]\n\n", byte, (_crc16 & 0xff)) ;
			}

			اگر (_crc16 == (uint16_t)(_rxpacket.crc16_بلند << 8) + _rxpacket.crc16_کم) {
				چکری_جانچ_OK = سچ;
			}

		} ورنہ {
			_rxpacket.crc8 = byte;

			اگر (_debug) {
				لکھو(" SUMD_DECODE_STATE_GOT_CRC8_SUMH: %x   [%x]\n\n", byte, _crc8) ;
			}

			اگر (_crc8 == _rxpacket.crc8) {
				چکری_جانچ_OK = سچ;
			}
		}

		اگر (چکری_جانچ_OK) {
			اگر (_debug) {
				لکھو(" CRC - OK \n") ;
			}

			اگر (_sumd) {
				اگر (_debug) {
					لکھو(" Got valid SUMD Packet\n") ;
				}

			} ورنہ {
				اگر (_debug) {
					لکھو(" Got valid SUMH Packet\n") ;
				}

			}

			اگر (_debug) {
				لکھو(" RXLEN: %d  [Chans: %d] \n\n", _rxlen - 1, (_rxlen - 1) / 2) ;
			}

			ret = 0;
			غیرعلامتی i;
			uint8_t _cnt = *rx_گنتی + 1;
			*rx_گنتی = _cnt;

			*rssi = 100;

			/* failsafe flag */
			*failsafe = (_rxpacket.حالت_2 == SUMD_ID_FAILSAFE);

			/* received Channels */
			اگر ((uint16_t)_rxpacket.لمبائی_2 > max_chan_گنتی) {
				_rxpacket.لمبائی_2 = (uint8_t) max_chan_گنتی;
			}

			*channel_گنتی = (uint16_t)_rxpacket.لمبائی_2;

			/* decode the actual packet */
			/* reorder first 4 channels */

			/* ch1 = roll -> sumd = ch2 */
			channels[0] = (uint16_t)((_rxpacket.sumd_ڈیٹا[1 * 2 + 1] << 8) | _rxpacket.sumd_ڈیٹا[1 * 2 + 2]) >> 3;
			/* ch2 = pitch -> sumd = ch2 */
			channels[1] = (uint16_t)((_rxpacket.sumd_ڈیٹا[2 * 2 + 1] << 8) | _rxpacket.sumd_ڈیٹا[2 * 2 + 2]) >> 3;
			/* ch3 = throttle -> sumd = ch2 */
			channels[2] = (uint16_t)((_rxpacket.sumd_ڈیٹا[0 * 2 + 1] << 8) | _rxpacket.sumd_ڈیٹا[0 * 2 + 2]) >> 3;
			/* ch4 = yaw -> sumd = ch2 */
			channels[3] = (uint16_t)((_rxpacket.sumd_ڈیٹا[3 * 2 + 1] << 8) | _rxpacket.sumd_ڈیٹا[3 * 2 + 2]) >> 3;

			/* we start at channel 5(index 4) */
			غیرعلامتی chan_اشاریہ = 4;

			برائے (i = 4; i < _rxpacket.لمبائی_2; i++) {
				اگر (_debug) {
					لکھو("ch[%d] : %x %x [ %x    %d ]\n", i + 1, _rxpacket.sumd_ڈیٹا[i * 2 + 1], _rxpacket.sumd_ڈیٹا[i * 2 + 2],
					       ((_rxpacket.sumd_ڈیٹا[i * 2 + 1] << 8) | _rxpacket.sumd_ڈیٹا[i * 2 + 2]) >> 3,
					       ((_rxpacket.sumd_ڈیٹا[i * 2 + 1] << 8) | _rxpacket.sumd_ڈیٹا[i * 2 + 2]) >> 3);
				}

				channels[chan_اشاریہ] = (uint16_t)((_rxpacket.sumd_ڈیٹا[i * 2 + 1] << 8) | _rxpacket.sumd_ڈیٹا[i * 2 + 2]) >> 3;
				/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
				//channels[chan_index] = (uint16_t)(channels[chan_index] * SUMD_SCALE_FACTOR + .5f) + SUMD_SCALE_OFFSET;

				chan_اشاریہ++;
			}

		} ورنہ {
			/* decoding failed */
			ret = 4;

			اگر (_debug) {
				لکھو(" CRC - fail \n") ;
			}

		}

		_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		توڑو;
	}

	واپس ret;
}
