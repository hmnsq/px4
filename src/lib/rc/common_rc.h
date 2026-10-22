
#ہدایت once

#شامل <stdint.h>

#شامل "crsf.h"
#شامل "dsm.h"
#شامل "ghst.hpp"
#شامل "sbus.h"
#شامل "st24.h"
#شامل "sumd.h"

#ہدایت pack(ڈالو, 1)
قسمتعریف  ساخت rc_decode_buf_ {
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

خارجی rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t چکری_جانچ, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحیح len);
