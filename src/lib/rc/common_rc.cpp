
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	សម្រាប់ (ចំនួនគត់ i = 0; i < 8; ++i) {
		ប្រសិនបើ (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} បើមិនដូច្នេះ {
			crc = crc << 1;
		}
	}

	ត្រឡប់ crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, ចំនួនគត់ len)
{
	uint8_t crc = 0;

	សម្រាប់ (ចំនួនគត់ i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ត្រឡប់ crc;
}
