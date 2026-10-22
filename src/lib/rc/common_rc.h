
#pragma once

#અન્તર્ભાવ <stdint.h>

#અન્તર્ભાવ "crsf.h"
#અન્તર્ભાવ "dsm.h"
#અન્તર્ભાવ "ghst.hpp"
#અન્તર્ભાવ "sbus.h"
#અન્તર્ભાવ "st24.h"
#અન્તર્ભાવ "sumd.h"

#pragma pack(ધકેલો, 1)
પ્રકારવ્યાખ્યા  સંરચના rc_decode_buf_ {
	સંઘ {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(કાઢો)

બાહ્ય rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, પૂર્ણાંક len);
