
#సూచన once

#అన్తర్భావ <stdint.h>

#అన్తర్భావ "crsf.h"
#అన్తర్భావ "dsm.h"
#అన్తర్భావ "ghst.hpp"
#అన్తర్భావ "sbus.h"
#అన్తర్భావ "st24.h"
#అన్తర్భావ "sumd.h"

#సూచన pack(నెట్టు, 1)
రకనిర్వచనం  నిర్మాణం rc_decode_buf_ {
	సంఘం {
		crsf_చట్రం_t crsf_చట్రం;
		ghst_చట్రం_t ghst_చట్రం;
		dsm_decode_t dsm;
		sbus_చట్రం_t sbus_చట్రం;
		ReceiverFcPacket _strxpacket;
		ReceiverFcPacketHoTT _hottrxpacket;
	};
} rc_decode_buf_t;
#సూచన pack(తీయి)

బాహ్య rc_decode_buf_t rc_decode_buf;

uint8_t crc8_dvb_s2(uint8_t చక్రీయ_తనిఖీ, uint8_t a);
uint8_t crc8_dvb_s2_buf(uint8_t *buf, పూర్ణాంకం len);
