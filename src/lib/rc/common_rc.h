
#pragma once

#ضمن <stdint.h>

#ضمن "crsf.h"
#ضمن "dsm.h"
#ضمن "ghst.hpp"
#ضمن "sbus.h"
#ضمن "st24.h"
#ضمن "sumd.h"

#pragma pack(ڈالو, 1)
تعريفنوع  بنية rc_decode_buf_ {
	اتحاد {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(نکالو)

خارجي rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحيح len);
