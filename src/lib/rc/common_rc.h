
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "crsf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dsm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ghst.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "sbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "st24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sumd.h"

#ਹਦਾਇਤ pack(ਧੱਕੋ, 1)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ  ਬਣਤਰ rc_decode_buf_ {
	ਸੰਘ {
		crsf_ਚੌਖਟਾ_t crsf_ਚੌਖਟਾ;
		ghst_ਚੌਖਟਾ_t ghst_ਚੌਖਟਾ;
		dsm_decode_t dsm;
		sbus_ਚੌਖਟਾ_t sbus_ਚੌਖਟਾ;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#ਹਦਾਇਤ pack(ਕੱਢੋ)

ਬਾਹਰੀ rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t ਚੱਕਰੀ_ਜਾਂਚ, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, ਪੂਰਨ len);
