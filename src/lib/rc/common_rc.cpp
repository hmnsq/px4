
#அந்தர்பாவ "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	க்கு (முழு i = 0; i < 8; ++i) {
		எனில் (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} இல்லையெனில் {
			crc = crc << 1;
		}
	}

	திரும்பு crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, முழு len)
{
	uint8_t crc = 0;

	க்கு (முழு i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	திரும்பு crc;
}
