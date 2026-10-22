
#ضمن "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t چڪري_جاچ, uint8_t a)
{
	چڪري_جاچ ^= a;

	لكل (صحيح i = 0; i < 8; ++i) {
		إذا (چڪري_جاچ & 0x80) {
			چڪري_جاچ = (چڪري_جاچ << 1) ^ 0xD5;

		} وإلا {
			چڪري_جاچ = چڪري_جاچ << 1;
		}
	}

	أرجع چڪري_جاچ;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحيح len)
{
	uint8_t چڪري_جاچ = 0;

	لكل (صحيح i = 0; i < len; ++i) {
		چڪري_جاچ = crc8_dvb_s2(چڪري_جاچ, buf[i]);
	}

	أرجع چڪري_جاچ;
}
