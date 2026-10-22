
#pragma once

#அந்தர்பாவ <stdint.h>

#அந்தர்பாவ "crsf.h"
#அந்தர்பாவ "dsm.h"
#அந்தர்பாவ "ghst.hpp"
#அந்தர்பாவ "sbus.h"
#அந்தர்பாவ "st24.h"
#அந்தர்பாவ "sumd.h"

#pragma pack(தள்ளு, 1)
வகைவரை  கட்டமைப்பு rc_decode_buf_ {
	ஒன்றிப்பு {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(எடு)

வெளி rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, முழு len);
