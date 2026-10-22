
#अन्तर्भाव "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	प्रति (पूर्णाङ्क i = 0; i < 8; ++i) {
		यदि (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} अन्यथा {
			crc = crc << 1;
		}
	}

	प्रत्यागम crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, पूर्णाङ्क len)
{
	uint8_t crc = 0;

	प्रति (पूर्णाङ्क i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	प्रत्यागम crc;
}
