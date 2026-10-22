
#pragma once

#شامل <stdint.h>

#شامل "crsf.h"
#شامل "dsm.h"
#شامل "ghst.hpp"
#شامل "sbus.h"
#شامل "st24.h"
#شامل "sumd.h"

#pragma pack(افزودن, 1)
تعریفنوع  ساختار rc_decode_buf_ {
	اجتماع {
		crsf_frame_t crsf_frame;
		ghst_frame_t ghst_frame;
		dsm_decode_t dsm;
		sbus_frame_t sbus_frame;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#pragma pack(برداشت)

بیرونی rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, صحیح len);
