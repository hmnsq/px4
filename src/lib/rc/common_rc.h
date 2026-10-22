
#لارښوونه once

#ضمن <stdint.h>

#ضمن "crsf.h"
#ضمن "dsm.h"
#ضمن "ghst.hpp"
#ضمن "sbus.h"
#ضمن "st24.h"
#ضمن "sumd.h"

#لارښوونه pack(ټېل, 1)
تعريفنوع  بنية rc_decode_buf_ {
	اتحاد {
		crsf_چوکاټ_t crsf_چوکاټ;
		ghst_چوکاټ_t ghst_چوکاټ;
		dsm_decode_t dsm;
		sbus_چوکاټ_t sbus_چوکاټ;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#لارښوونه pack(راوباسه)

خارجي rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t دوراني_ازموینه, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحيح len);
