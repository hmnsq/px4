
#pragma once

#অন্তর্ভাৱ <stdint.h>

#অন্তর্ভাৱ "crsf.h"
#অন্তর্ভাৱ "dsm.h"
#অন্তর্ভাৱ "ghst.hpp"
#অন্তর্ভাৱ "sbus.h"
#অন্তর্ভাৱ "st24.h"
#অন্তর্ভাৱ "sumd.h"

#pragma pack(ঠেলো, 1)
প্রকারনির্দেশ  সংরচনা rc_decode_buf_ {
	সঙ্ঘ {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(বেৰ_কৰো)

বাহ্য rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, পূর্ণাঙ্ক len);
