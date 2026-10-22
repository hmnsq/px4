
#ہدایت once

#ضمن <stdint.h>

#ضمن "crsf.h"
#ضمن "dsm.h"
#ضمن "ghst.hpp"
#ضمن "sbus.h"
#ضمن "st24.h"
#ضمن "sumd.h"

#ہدایت pack(ڈالو, 1)
تعريفنوع  بنية rc_decode_buf_ {
	اتحاد {
		crsf_چوکھٹا_t crsf_چوکھٹا;
		ghst_چوکھٹا_t ghst_چوکھٹا;
		dsm_decode_t dsm;
		sbus_چوکھٹا_t sbus_چوکھٹا;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#ہدایت pack(نکالو)

خارجي rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t چکری_جانچ, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحيح len);
