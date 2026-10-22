
#ଅନ୍ତର୍ଭାଵ "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 8; ++i) {
		ଯଦି (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} ଅନ୍ୟଥା {
			crc = crc << 1;
		}
	}

	ଫେରନ୍ତୁ crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	uint8_t crc = 0;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ଫେରନ୍ତୁ crc;
}
