
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	အတွက် (ကိန်းပြည့် i = 0; i < 8; ++i) {
		အကယ်၍ (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} မဟုတ်ပါက {
			crc = crc << 1;
		}
	}

	ပြန်ပေး crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, ကိန်းပြည့် len)
{
	uint8_t crc = 0;

	အတွက် (ကိန်းပြည့် i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ပြန်ပေး crc;
}
