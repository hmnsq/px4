
#അന്തര്ഭാവ "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 8; ++i) {
		എങ്കിൽ (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} അല്ലെങ്കിൽ {
			crc = crc << 1;
		}
	}

	തിരികെ crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, പൂർണ്ണസംഖ്യ len)
{
	uint8_t crc = 0;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	തിരികെ crc;
}
