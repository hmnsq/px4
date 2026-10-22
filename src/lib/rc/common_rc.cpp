
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	за (цео i = 0; i < 8; ++i) {
		ако (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} инако {
			crc = crc << 1;
		}
	}

	врати crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, цео len)
{
	uint8_t crc = 0;

	за (цео i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	врати crc;
}
