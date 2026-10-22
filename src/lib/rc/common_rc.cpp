
#ضمن "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	لكل (صحيح i = 0; i < 8; ++i) {
		ܐܢ (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} ܐܚܪܢܐ {
			crc = crc << 1;
		}
	}

	ܦܢܐ crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحيح len)
{
	uint8_t crc = 0;

	لكل (صحيح i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ܦܢܐ crc;
}
