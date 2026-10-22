
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	ཆེད་དུ (ཧྲིལ i = 0; i < 8; ++i) {
		གལ་ཏེ (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} དེ་མིན {
			crc = crc << 1;
		}
	}

	ཕྱིར crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, ཧྲིལ len)
{
	uint8_t crc = 0;

	ཆེད་དུ (ཧྲིལ i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ཕྱིར crc;
}
