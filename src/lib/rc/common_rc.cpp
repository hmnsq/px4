
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	for (puur_naa_nka i = 0; i < 8; ++i) {
		ᤀᤡ᤺ (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} ᤀᤡ᤺ᤆᤡ {
			crc = crc << 1;
		}
	}

	ᤀᤡᤴ crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, puur_naa_nka len)
{
	uint8_t crc = 0;

	for (puur_naa_nka i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ᤀᤡᤴ crc;
}
