
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	위해 (정수 i = 0; i < 8; ++i) {
		만약 (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} 아니면 {
			crc = crc << 1;
		}
	}

	반환 crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, 정수 len)
{
	uint8_t crc = 0;

	위해 (정수 i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	반환 crc;
}
