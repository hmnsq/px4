
#pragma once

#അന്തര്ഭാവ <stdint.h>

#അന്തര്ഭാവ "crsf.h"
#അന്തര്ഭാവ "dsm.h"
#അന്തര്ഭാവ "ghst.hpp"
#അന്തര്ഭാവ "sbus.h"
#അന്തര്ഭാവ "st24.h"
#അന്തര്ഭാവ "sumd.h"

#pragma pack(തള്ളുക, 1)
തരനിർവചനം  ഘടന rc_decode_buf_ {
	സംഘം {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(എടുക്കുക)

ബാഹ്യ rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, പൂർണ്ണസംഖ്യ len);
