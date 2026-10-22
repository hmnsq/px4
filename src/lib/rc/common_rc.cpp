
#অন্তর্ভাব "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	প্রতি (পূর্ণাঙ্ক i = 0; i < 8; ++i) {
		যদি (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} অন্যথা {
			crc = crc << 1;
		}
	}

	প্রত্যাগম crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, পূর্ণাঙ্ক len)
{
	uint8_t crc = 0;

	প্রতি (পূর্ণাঙ্ক i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	প্রত্যাগম crc;
}
