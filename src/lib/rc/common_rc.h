
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <stdint.h>

#ಅನ್ತರ್ಭಾವ "crsf.h"
#ಅನ್ತರ್ಭಾವ "dsm.h"
#ಅನ್ತರ್ಭಾವ "ghst.hpp"
#ಅನ್ತರ್ಭಾವ "sbus.h"
#ಅನ್ತರ್ಭಾವ "st24.h"
#ಅನ್ತರ್ಭಾವ "sumd.h"

#ಸೂಚನೆ pack(ತಳ್ಳು, 1)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ  ರಚನೆ rc_decode_buf_ {
	ಒಕ್ಕೂಟ {
		crsf_ಚೌಕಟ್ಟು_t crsf_ಚೌಕಟ್ಟು;
		ghst_ಚೌಕಟ್ಟು_t ghst_ಚೌಕಟ್ಟು;
		dsm_decode_t dsm;
		sbus_ಚೌಕಟ್ಟು_t sbus_ಚೌಕಟ್ಟು;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#ಸೂಚನೆ pack(ತೆಗೆ_2)

ಬಾಹ್ಯ rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, ಪೂರ್ಣಾಂಕ len);
