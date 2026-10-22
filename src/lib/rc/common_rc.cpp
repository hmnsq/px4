
#ಅನ್ತರ್ಭಾವ "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 8; ++i) {
		ಯದಿ (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} ಅನ್ಯಥಾ {
			crc = crc << 1;
		}
	}

	ಪ್ರತ್ಯಾಗಮ crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, ಪೂರ್ಣಾಙ್ಕ len)
{
	uint8_t crc = 0;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ಪ್ರತ್ಯಾಗಮ crc;
}
