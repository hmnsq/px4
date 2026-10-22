
#pragma once

#ଅନ୍ତର୍ଭାଵ <stdint.h>

#ଅନ୍ତର୍ଭାଵ "crsf.h"
#ଅନ୍ତର୍ଭାଵ "dsm.h"
#ଅନ୍ତର୍ଭାଵ "ghst.hpp"
#ଅନ୍ତର୍ଭାଵ "sbus.h"
#ଅନ୍ତର୍ଭାଵ "st24.h"
#ଅନ୍ତର୍ଭାଵ "sumd.h"

#pragma pack(ଠେଲ, 1)
ପ୍ରକାରବ୍ୟାଖ୍ୟା  ସଂରଚନା rc_decode_buf_ {
	ସଂଘ {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(କାଢ଼)

ବାହ୍ୟ rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
