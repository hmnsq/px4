
#অন্তর্ভাব "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	জন্য (পূর্ণসংখ্যা i = 0; i < 8; ++i) {
		যদি (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} অন্যথা {
			crc = crc << 1;
		}
	}

	ফেরত crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, পূর্ণসংখ্যা len)
{
	uint8_t crc = 0;

	জন্য (পূর্ণসংখ্যা i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ফেরত crc;
}
