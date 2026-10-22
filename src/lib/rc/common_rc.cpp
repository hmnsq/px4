
#ضمن "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t چکری_جانچ, uint8_t a)
{
	چکری_جانچ ^= a;

	لكل (صحيح i = 0; i < 8; ++i) {
		إذا (چکری_جانچ & 0x80) {
			چکری_جانچ = (چکری_جانچ << 1) ^ 0xD5;

		} وإلا {
			چکری_جانچ = چکری_جانچ << 1;
		}
	}

	أرجع چکری_جانچ;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحيح len)
{
	uint8_t چکری_جانچ = 0;

	لكل (صحيح i = 0; i < len; ++i) {
		چکری_جانچ = crc8_dvb_s2(چکری_جانچ, buf[i]);
	}

	أرجع چکری_جانچ;
}
