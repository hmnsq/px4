
#pragma once

#अन्तर्भाव <stdint.h>

#अन्तर्भाव "crsf.h"
#अन्तर्भाव "dsm.h"
#अन्तर्भाव "ghst.hpp"
#अन्तर्भाव "sbus.h"
#अन्तर्भाव "st24.h"
#अन्तर्भाव "sumd.h"

#pragma pack(ढकला, 1)
प्रकारव्याख्या  संरचना rc_decode_buf_ {
	संघ {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(काढा)

बाह्य rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, पूर्णांक len);
