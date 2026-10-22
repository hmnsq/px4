
#અન્તર્ભાવ "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	માટે (પૂર્ણાંક i = 0; i < 8; ++i) {
		જો (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} નહીંતર {
			crc = crc << 1;
		}
	}

	પાછા crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, પૂર્ણાંક len)
{
	uint8_t crc = 0;

	માટે (પૂર્ણાંક i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	પાછા crc;
}
