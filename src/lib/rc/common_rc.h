
#pragma once

#ಅನ್ತರ್ಭಾವ <stdint.h>

#ಅನ್ತರ್ಭಾವ "crsf.h"
#ಅನ್ತರ್ಭಾವ "dsm.h"
#ಅನ್ತರ್ಭಾವ "ghst.hpp"
#ಅನ್ತರ್ಭಾವ "sbus.h"
#ಅನ್ತರ್ಭಾವ "st24.h"
#ಅನ್ತರ್ಭಾವ "sumd.h"

#pragma pack(ತಳ್ಳು, 1)
ಪ್ರಕಾರನಿರ್ದೇಶ  ಸಂರಚನಾ rc_decode_buf_ {
	ಸಙ್ಘ {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(ತೆಗೆ)

ಬಾಹ್ಯ rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, ಪೂರ್ಣಾಙ್ಕ len);
