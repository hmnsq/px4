
#رهنمود once

#ضمن <stdint.h>

#ضمن "crsf.h"
#ضمن "dsm.h"
#ضمن "ghst.hpp"
#ضمن "sbus.h"
#ضمن "st24.h"
#ضمن "sumd.h"

#رهنمود pack(افزودن, 1)
تعريفنوع  بنية rc_decode_buf_ {
	اتحاد {
		crsf_قاب_t crsf_قاب;
		ghst_قاب_t ghst_قاب;
		dsm_decode_t dsm;
		sbus_قاب_t sbus_قاب;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#رهنمود pack(برداشت)

خارجي rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t بررسی_چرخهای, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحيح len);
