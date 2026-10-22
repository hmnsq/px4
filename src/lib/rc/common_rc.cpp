
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	для (целое i = 0; i < 8; ++i) {
		если (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} иначе {
			crc = crc << 1;
		}
	}

	вернуть crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, целое len)
{
	uint8_t crc = 0;

	для (целое i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	вернуть crc;
}
