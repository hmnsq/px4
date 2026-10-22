
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < 8; ++i) {
		නම් (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} නැතහොත් {
			crc = crc << 1;
		}
	}

	ආපසු crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, පූර්ණ_සංඛ්‍යා len)
{
	uint8_t crc = 0;

	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ආපසු crc;
}
