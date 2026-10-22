
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	cho (số_nguyên i = 0; i < 8; ++i) {
		nếu (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} không_thì {
			crc = crc << 1;
		}
	}

	trả_về crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, số_nguyên len)
{
	uint8_t crc = 0;

	cho (số_nguyên i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	trả_về crc;
}
