
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	为 (整数 i = 0; i < 8; ++i) {
		如果 (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} 否则 {
			crc = crc << 1;
		}
	}

	返回 crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, 整数 len)
{
	uint8_t crc = 0;

	为 (整数 i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	返回 crc;
}
