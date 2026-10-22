
#هدايت once

#ضمن <stdint.h>

#ضمن "crsf.h"
#ضمن "dsm.h"
#ضمن "ghst.hpp"
#ضمن "sbus.h"
#ضمن "st24.h"
#ضمن "sumd.h"

#هدايت pack(ڌڪو, 1)
تعريفنوع  بنية rc_decode_buf_ {
	اتحاد {
		crsf_چوکٺو_t crsf_چوکٺو;
		ghst_چوکٺو_t ghst_چوکٺو;
		dsm_decode_t dsm;
		sbus_چوکٺو_t sbus_چوکٺو;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#هدايت pack(ڪڍو)

خارجي rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t چڪري_جاچ, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحيح len);
