
#شامل "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t چکری_جانچ, uint8_t a)
{
	چکری_جانچ ^= a;

	برائے (صحیح i = 0; i < 8; ++i) {
		اگر (چکری_جانچ & 0x80) {
			چکری_جانچ = (چکری_جانچ << 1) ^ 0xD5;

		} ورنہ {
			چکری_جانچ = چکری_جانچ << 1;
		}
	}

	واپس چکری_جانچ;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحیح len)
{
	uint8_t چکری_جانچ = 0;

	برائے (صحیح i = 0; i < len; ++i) {
		چکری_جانچ = crc8_dvb_s2(چکری_جانچ, buf[i]);
	}

	واپس چکری_جانچ;
}
