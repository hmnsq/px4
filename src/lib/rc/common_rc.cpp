
#include "common_rc.h"

__EXPORT rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
	crc ^= a;

	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 8; ++i) {
		ຖ້າ (crc & 0x80) {
			crc = (crc << 1) ^ 0xD5;

		} ບໍ່ດັ່ງນັ້ນ {
			crc = crc << 1;
		}
	}

	ສົ່ງຄືນ crc;
}

uint8_t crc8_dvb_s2_buf(uint8_t *buf, ຈຳນວນເຕັມ len)
{
	uint8_t crc = 0;

	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < len; ++i) {
		crc = crc8_dvb_s2(crc, buf[i]);
	}

	ສົ່ງຄືນ crc;
}
