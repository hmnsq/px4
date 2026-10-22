/****************************************************************************
 *
 *   Copyright (c) 2014 PX4 Development Team. All rights reserved.
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
 * @file st24.cpp
 *
 * RC protocol implementation for Yuneec ST24 transmitter.
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 */

#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <cmath>
#अन्तर्भाव "st24.h"
#अन्तर्भाव "common_rc.h"

ध्रुव वर्ण *decode_अवस्थाः[] = {"UNSYNCED",
			       "GOT_STX1",
			       "GOT_STX2",
			       "GOT_LEN",
			       "GOT_TYPE",
			       "GOT_DATA"
			      };

/* define range mapping here, -+100% -> 1000..2000 */
#परिभाषय ST24_RANGE_MIN 500.0f
#परिभाषय ST24_RANGE_MAX 3500.0f

#परिभाषय ST24_TARGET_MIN 1000.0f
#परिभाषय ST24_TARGET_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#परिभाषय ST24_SCALE_गुणकः ((ST24_TARGET_MAX - ST24_TARGET_MIN) / (ST24_RANGE_MAX - ST24_RANGE_MIN))
#परिभाषय ST24_SCALE_विस्थापनम् (पूर्णाङ्क)(ST24_TARGET_MIN - (ST24_SCALE_गुणकः * ST24_RANGE_MIN + 0.5f))

स्थिर गणना ST24_DECODE_STATE _decode_state = ST24_DECODE_STATE_UNSYNCED;
स्थिर uint8_t _rxlen;

स्थिर ReceiverFcPacket &_rxpacket = rc_decode_buf._strxpacket;

uint8_t st24_common_crc8(uint8_t *ptr, uint8_t len)
{
	uint8_t i, crc ;
	crc = 0;

	यावत् (len--) {
		प्रति (i = 0x80; i != 0; i >>= 1) {
			यदि ((crc & 0x80) != 0) {
				crc <<= 1;
				crc ^= 0x07;

			} अन्यथा {
				crc <<= 1;
			}

			यदि ((*ptr & i) != 0) {
				crc ^= 0x07;
			}
		}

		ptr++;
	}

	प्रत्यागम (crc);
}


पूर्णाङ्क st24_decode(uint8_t byte, uint8_t *rssi, uint8_t *नष्टम्_गणनम्, uint16_t *channel_गणनम्, uint16_t *channels,
		uint16_t max_chan_गणनम्)
{
	पूर्णाङ्क ret = 1;

	विकल्प (_decode_state) {
	पक्ष ST24_DECODE_STATE_UNSYNCED:
		यदि (byte == ST24_STX1) {
			_decode_state = ST24_DECODE_STATE_GOT_STX1;

		} अन्यथा {
			ret = 3;
		}

		भङ्ग;

	पक्ष ST24_DECODE_STATE_GOT_STX1:
		यदि (byte == ST24_STX2) {
			_decode_state = ST24_DECODE_STATE_GOT_STX2;

		} अन्यथा {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		भङ्ग;

	पक्ष ST24_DECODE_STATE_GOT_STX2:

		/* ensure no data overflow failure or hack is possible */
		यदि ((अचिह्नित)byte >= परिमाण(_rxpacket.type) + परिमाण(_rxpacket.crc8)
		    && (अचिह्नित)byte <= परिमाण(_rxpacket.type) + परिमाण(_rxpacket.st24_दत्त) + परिमाण(_rxpacket.crc8)) {
			_rxpacket.दीर्घता_2 = byte;
			_rxlen = 0;
			_decode_state = ST24_DECODE_STATE_GOT_LEN;

		} अन्यथा {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		भङ्ग;

	पक्ष ST24_DECODE_STATE_GOT_LEN:
		_rxpacket.type = byte;
		_rxlen++;
		_decode_state = (_rxlen == (_rxpacket.दीर्घता_2 - 1)) ? ST24_DECODE_STATE_GOT_दत्त : ST24_DECODE_STATE_GOT_TYPE;
		भङ्ग;

	पक्ष ST24_DECODE_STATE_GOT_TYPE:
		_rxpacket.st24_दत्त[_rxlen - 1] = byte;
		_rxlen++;

		यदि (_rxlen == (_rxpacket.दीर्घता_2 - 1)) {
			_decode_state = ST24_DECODE_STATE_GOT_दत्त;
		}

		भङ्ग;

	पक्ष ST24_DECODE_STATE_GOT_दत्त:
		_rxpacket.crc8 = byte;
		_rxlen++;

		यदि (st24_common_crc8((uint8_t *) & (_rxpacket.दीर्घता_2), _rxlen) == _rxpacket.crc8) {

			ret = 0;

			/* decode the actual packet */

			विकल्प (_rxpacket.type) {

			पक्ष ST24_PACKET_TYPE_CHANNELDATA12: {
					ChannelData12 *d = (ChannelData12 *)_rxpacket.st24_दत्त;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*नष्टम्_गणनम् = d->नष्टम्_गणनम्;

					/* this can lead to rounding of the strides */
					*channel_गणनम् = (max_chan_गणनम् < 12) ? max_chan_गणनम् : 12;

					अचिह्नित stride_गणनम् = (*channel_गणनम् * 3) / 2;
					अचिह्नित chan_सूचकांक = 0;

					प्रति (अचिह्नित i = 0; i < stride_गणनम्; i += 3) {
						channels[chan_सूचकांक] = ((uint16_t)d->channel[i] << 4);
						channels[chan_सूचकांक] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_सूचकांक] = (uint16_t)lroundf(channels[chan_सूचकांक] * ST24_SCALE_गुणकः) + ST24_SCALE_विस्थापनम्;
						chan_सूचकांक++;

						channels[chan_सूचकांक] = ((uint16_t)d->channel[i + 2]);
						channels[chan_सूचकांक] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_सूचकांक] = (uint16_t)lroundf(channels[chan_सूचकांक] * ST24_SCALE_गुणकः) + ST24_SCALE_विस्थापनम्;
						chan_सूचकांक++;
					}
				}
				भङ्ग;

			पक्ष ST24_PACKET_TYPE_CHANNELDATA24: {
					ChannelData24 *d = (ChannelData24 *)&_rxpacket.st24_दत्त;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*नष्टम्_गणनम् = d->नष्टम्_गणनम्;

					/* this can lead to rounding of the strides */
					*channel_गणनम् = (max_chan_गणनम् < 24) ? max_chan_गणनम् : 24;

					अचिह्नित stride_गणनम् = (*channel_गणनम् * 3) / 2;
					अचिह्नित chan_सूचकांक = 0;

					प्रति (अचिह्नित i = 0; i < stride_गणनम्; i += 3) {
						channels[chan_सूचकांक] = ((uint16_t)d->channel[i] << 4);
						channels[chan_सूचकांक] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_सूचकांक] = static_cast<uint16_t>(lroundf(channels[chan_सूचकांक] * ST24_SCALE_गुणकः) + ST24_SCALE_विस्थापनम्);
						chan_सूचकांक++;

						channels[chan_सूचकांक] = ((uint16_t)d->channel[i + 2]);
						channels[chan_सूचकांक] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_सूचकांक] = static_cast<uint16_t>(lroundf(channels[chan_सूचकांक] * ST24_SCALE_गुणकः) + ST24_SCALE_विस्थापनम्);
						chan_सूचकांक++;
					}
				}
				भङ्ग;

			पक्ष ST24_PACKET_TYPE_TRANSMITTERGPSDATA: {

					// ReceiverFcPacket* d = (ReceiverFcPacket*)&_rxpacket.st24_data;
					/* we silently ignore this data for now, as it is unused */
					ret = 5;
				}
				भङ्ग;

			स्वभाव:
				ret = 2;
				भङ्ग;
			}

		} अन्यथा {
			/* decoding failed */
			ret = 4;
		}

		_decode_state = ST24_DECODE_STATE_UNSYNCED;
		भङ्ग;
	}

	प्रत्यागम ret;
}
