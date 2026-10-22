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

#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव "sumd.h"
#अन्तर्भाव "common_rc.h"

गणना SUMD_DECODE_STATE {
	SUMD_DECODE_STATE_UNSYNCED = 0,
	SUMD_DECODE_STATE_GOT_शीर्षकम्,
	SUMD_DECODE_STATE_GOT_STATE,
	SUMD_DECODE_STATE_GOT_LEN,
	SUMD_DECODE_STATE_GOT_दत्त,
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
तार्किक 		_sumd 		= सत्य;
तार्किक		_crcOK		= असत्य;
तार्किक		_debug		= असत्य;


/* define range mapping here, -+100% -> 1000..2000 */
#परिभाषय SUMD_RANGE_MIN 0.0f
#परिभाषय SUMD_RANGE_MAX 4096.0f

#परिभाषय SUMD_TARGET_MIN 1000.0f
#परिभाषय SUMD_TARGET_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#परिभाषय SUMD_SCALE_गुणकः ((SUMD_TARGET_MAX - SUMD_TARGET_MIN) / (SUMD_RANGE_MAX - SUMD_RANGE_MIN))
#परिभाषय SUMD_SCALE_विस्थापनम् (पूर्णाङ्क)(SUMD_TARGET_MIN - (SUMD_SCALE_गुणकः * SUMD_RANGE_MIN + 0.5f))

स्थिर गणना SUMD_DECODE_STATE _decode_state = SUMD_DECODE_STATE_UNSYNCED;
स्थिर uint8_t _rxlen;

स्थिर ReceiverFcPacketHoTT &_rxpacket = rc_decode_buf._hottrxpacket;

uint16_t sumd_crc16(uint16_t crc, uint8_t value)
{
	पूर्णाङ्क i;
	crc ^= (uint16_t)value << 8;

	प्रति (i = 0; i < 8; i++) {
		crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
	}

	प्रत्यागम crc;
}

uint8_t sumd_crc8(uint8_t crc, uint8_t value)
{
	crc += value;
	प्रत्यागम crc;
}

पूर्णाङ्क sumd_decode(uint8_t byte, uint8_t *rssi, uint8_t *rx_गणनम्, uint16_t *channel_गणनम्, uint16_t *channels,
		uint16_t max_chan_गणनम्, तार्किक *failsafe)
{

	पूर्णाङ्क ret = 1;

	विकल्प (_decode_state) {
	पक्ष SUMD_DECODE_STATE_UNSYNCED:
		यदि (_debug) {
			प्रारूप_लिख(" SUMD_DECODE_STATE_UNSYNCED \n") ;
		}

		यदि (byte == SUMD_शीर्षकम्_ID) {
			_rxpacket.शीर्षकम् = byte;
			_sumd = सत्य;
			_rxlen = 0;
			_crc16 = 0x0000;
			_crc8 = 0x00;
			_crcOK = असत्य;
			_crc16 = sumd_crc16(_crc16, byte);
			_crc8 = sumd_crc8(_crc8, byte);
			_decode_state = SUMD_DECODE_STATE_GOT_शीर्षकम्;

			यदि (_debug) {
				प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_HEADER: %x \n", byte) ;
			}

		} अन्यथा {
			ret = 3;
		}

		भङ्ग;

	पक्ष SUMD_DECODE_STATE_GOT_शीर्षकम्:
		यदि (byte == SUMD_ID_SUMD || byte == SUMD_ID_FAILSAFE || byte == SUMD_ID_SUMH) {
			_rxpacket.स्थितिः = byte;

			यदि (byte == SUMD_ID_SUMH) {
				_sumd = असत्य;
			}

			यदि (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} अन्यथा {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_decode_state = SUMD_DECODE_STATE_GOT_STATE;

			यदि (_debug) {
				प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_STATE: %x \n", byte) ;
			}

		} अन्यथा {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		भङ्ग;

	पक्ष SUMD_DECODE_STATE_GOT_STATE:
		यदि (byte >= 2 && byte <= SUMD_MAX_CHANNELS) {
			_rxpacket.दीर्घता_2 = byte;

			यदि (_sumd) {
				_crc16 = sumd_crc16(_crc16, byte);

			} अन्यथा {
				_crc8 = sumd_crc8(_crc8, byte);
			}

			_rxlen++;
			_decode_state = SUMD_DECODE_STATE_GOT_LEN;

			यदि (_debug) {
				प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_LEN: %x (%d) \n", byte, byte) ;
			}

		} अन्यथा {
			_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		}

		भङ्ग;

	पक्ष SUMD_DECODE_STATE_GOT_LEN:
		_rxpacket.sumd_दत्त[_rxlen] = byte;

		यदि (_sumd) {
			_crc16 = sumd_crc16(_crc16, byte);

		} अन्यथा {
			_crc8 = sumd_crc8(_crc8, byte);
		}

		_rxlen++;

		यदि (_rxlen <= ((_rxpacket.दीर्घता_2 * 2))) {
			यदि (_debug) {
				प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_DATA[%d]: %x\n", _rxlen - 2, byte) ;
			}

		} अन्यथा {
			_decode_state = SUMD_DECODE_STATE_GOT_दत्त;

			यदि (_debug) {
				प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_DATA -- finish --\n") ;
			}

		}

		भङ्ग;

	पक्ष SUMD_DECODE_STATE_GOT_दत्त:
		_rxpacket.crc16_उच्चः = byte;

		यदि (_debug) {
			प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_CRC16[1]: %x   [%x]\n", byte, ((_crc16 >> 8) & 0xff)) ;
		}

		यदि (_sumd) {
			_decode_state = SUMD_DECODE_STATE_GOT_CRC;

		} अन्यथा {
			_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_1;
		}

		भङ्ग;

	पक्ष SUMD_DECODE_STATE_GOT_CRC16_BYTE_1:
		_rxpacket.crc16_नीचः = byte;

		यदि (_debug) {
			प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_CRC16[2]: %x   [%x]\n", byte, (_crc16 & 0xff)) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_CRC16_BYTE_2;

		भङ्ग;

	पक्ष SUMD_DECODE_STATE_GOT_CRC16_BYTE_2:
		_rxpacket.telemetry = byte;

		यदि (_debug) {
			प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_SUMH_TELEMETRY: %x\n", byte) ;
		}

		_decode_state = SUMD_DECODE_STATE_GOT_CRC;

		भङ्ग;

	पक्ष SUMD_DECODE_STATE_GOT_CRC:
		यदि (_sumd) {
			_rxpacket.crc16_नीचः = byte;

			यदि (_debug) {
				प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_CRC[2]: %x   [%x]\n\n", byte, (_crc16 & 0xff)) ;
			}

			यदि (_crc16 == (uint16_t)(_rxpacket.crc16_उच्चः << 8) + _rxpacket.crc16_नीचः) {
				_crcOK = सत्य;
			}

		} अन्यथा {
			_rxpacket.crc8 = byte;

			यदि (_debug) {
				प्रारूप_लिख(" SUMD_DECODE_STATE_GOT_CRC8_SUMH: %x   [%x]\n\n", byte, _crc8) ;
			}

			यदि (_crc8 == _rxpacket.crc8) {
				_crcOK = सत्य;
			}
		}

		यदि (_crcOK) {
			यदि (_debug) {
				प्रारूप_लिख(" CRC - OK \n") ;
			}

			यदि (_sumd) {
				यदि (_debug) {
					प्रारूप_लिख(" Got valid SUMD Packet\n") ;
				}

			} अन्यथा {
				यदि (_debug) {
					प्रारूप_लिख(" Got valid SUMH Packet\n") ;
				}

			}

			यदि (_debug) {
				प्रारूप_लिख(" RXLEN: %d  [Chans: %d] \n\n", _rxlen - 1, (_rxlen - 1) / 2) ;
			}

			ret = 0;
			अचिह्नित i;
			uint8_t _cnt = *rx_गणनम् + 1;
			*rx_गणनम् = _cnt;

			*rssi = 100;

			/* failsafe flag */
			*failsafe = (_rxpacket.स्थितिः == SUMD_ID_FAILSAFE);

			/* received Channels */
			यदि ((uint16_t)_rxpacket.दीर्घता_2 > max_chan_गणनम्) {
				_rxpacket.दीर्घता_2 = (uint8_t) max_chan_गणनम्;
			}

			*channel_गणनम् = (uint16_t)_rxpacket.दीर्घता_2;

			/* decode the actual packet */
			/* reorder first 4 channels */

			/* ch1 = roll -> sumd = ch2 */
			channels[0] = (uint16_t)((_rxpacket.sumd_दत्त[1 * 2 + 1] << 8) | _rxpacket.sumd_दत्त[1 * 2 + 2]) >> 3;
			/* ch2 = pitch -> sumd = ch2 */
			channels[1] = (uint16_t)((_rxpacket.sumd_दत्त[2 * 2 + 1] << 8) | _rxpacket.sumd_दत्त[2 * 2 + 2]) >> 3;
			/* ch3 = throttle -> sumd = ch2 */
			channels[2] = (uint16_t)((_rxpacket.sumd_दत्त[0 * 2 + 1] << 8) | _rxpacket.sumd_दत्त[0 * 2 + 2]) >> 3;
			/* ch4 = yaw -> sumd = ch2 */
			channels[3] = (uint16_t)((_rxpacket.sumd_दत्त[3 * 2 + 1] << 8) | _rxpacket.sumd_दत्त[3 * 2 + 2]) >> 3;

			/* we start at channel 5(index 4) */
			अचिह्नित chan_सूचकांक = 4;

			प्रति (i = 4; i < _rxpacket.दीर्घता_2; i++) {
				यदि (_debug) {
					प्रारूप_लिख("ch[%d] : %x %x [ %x    %d ]\n", i + 1, _rxpacket.sumd_दत्त[i * 2 + 1], _rxpacket.sumd_दत्त[i * 2 + 2],
					       ((_rxpacket.sumd_दत्त[i * 2 + 1] << 8) | _rxpacket.sumd_दत्त[i * 2 + 2]) >> 3,
					       ((_rxpacket.sumd_दत्त[i * 2 + 1] << 8) | _rxpacket.sumd_दत्त[i * 2 + 2]) >> 3);
				}

				channels[chan_सूचकांक] = (uint16_t)((_rxpacket.sumd_दत्त[i * 2 + 1] << 8) | _rxpacket.sumd_दत्त[i * 2 + 2]) >> 3;
				/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
				//channels[chan_index] = (uint16_t)(channels[chan_index] * SUMD_SCALE_FACTOR + .5f) + SUMD_SCALE_OFFSET;

				chan_सूचकांक++;
			}

		} अन्यथा {
			/* decoding failed */
			ret = 4;

			यदि (_debug) {
				प्रारूप_लिख(" CRC - fail \n") ;
			}

		}

		_decode_state = SUMD_DECODE_STATE_UNSYNCED;
		भङ्ग;
	}

	प्रत्यागम ret;
}
