
#অন্তর্ভাৱ "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	বাবে (পূর্ণাঙ্ক i = 0; i < 8; ++i) {
		যদি (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} অন্যথা {
			crc = crc << 1;
		}
	}

	ঘূৰাই crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, পূর্ণাঙ্ক len)
{
	uint8_t crc = 0;

	বাবে (পূর্ণাঙ্ক i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ঘূৰাই crc;
}
