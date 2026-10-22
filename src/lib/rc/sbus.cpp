/****************************************************************************
 *
 *   Copyright (c) 2012-2017 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file sbus.cpp
 *
 * Serial protocol decoder for the Futaba S.bus protocol.
 */

#అన్తర్భావ <px4_platform_common/px4_config.h>

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <string.h>

#నిర్వచితమైతే TIOCSSINGLEWIRE
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు

#అన్తర్భావ "sbus.h"
#అన్తర్భావ "common_rc.h"
#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <lib/mathlib/mathlib.h>

ఉపయోగించి నామాకాశ సమయం_literals;

#నిర్వచించు SBUS_DEBUG_LEVEL 	0 /* Set debug output level */

#ఒకవేళ defined(__PX4_LINUX)
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <asm-generic/termbits.h>
#లేకపోతే
#అన్తర్భావ <termios.h>
#అయితే_ముగింపు

#నిర్వచించు SBUS_ప్రారంభం_చిహ్నం	0x0f

#నిర్వచించు SBUS_ఆదానం_CHANNELS	16
#నిర్వచించు SBUS_FLAGS_BYTE		23
#నిర్వచించు SBUS_FAILSAFE_బిట్టు	3
#నిర్వచించు SBUS_FRAMELOST_బిట్టు	2

// testing with a SBUS->PWM adapter shows that
// above 300Hz SBUS becomes unreliable. 333 would
// be the theoretical achievable, but at 333Hz some
// frames are lost
#నిర్వచించు SBUS1_MAX_రేటు_HZ	300
#నిర్వచించు SBUS1_MIN_రేటు_HZ	50

// this is the rate of the old code
#నిర్వచించు SBUS1_DEFAULT_రేటు_HZ	72

/*
  Measured values with Futaba FX-30/R6108SB:
    -+100% on TX:  PCM 1.100/1.520/1.950ms -> SBus raw values: 350/1024/1700  (100% ATV)
    -+140% on TX:  PCM 0.930/1.520/2.112ms -> SBus raw values:  78/1024/1964  (140% ATV)
    -+152% on TX:  PCM 0.884/1.520/2.160ms -> SBus raw values:   1/1024/2047  (140% ATV plus dirty tricks)
*/

/* define range mapping here, -+100% -> 1000..2000 */
#నిర్వచించు SBUS_RANGE_MIN 200.0f
#నిర్వచించు SBUS_RANGE_MAX 1800.0f

#నిర్వచించు SBUS_లక్ష్యం_MIN 1000.0f
#నిర్వచించు SBUS_లక్ష్యం_MAX 2000.0f

#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
#  include <stdio.h>
#అయితే_ముగింపు

/* pre-calculate the floating point stuff as far as possible at compile time */
#నిర్వచించు SBUS_SCALE_గుణకం ((SBUS_లక్ష్యం_MAX - SBUS_లక్ష్యం_MIN) / (SBUS_RANGE_MAX - SBUS_RANGE_MIN))
#నిర్వచించు SBUS_SCALE_స్థానభ్రంశం (పూర్ణాంకం)(SBUS_లక్ష్యం_MIN - (SBUS_SCALE_గుణకం * SBUS_RANGE_MIN + 0.5f))

స్థిర hrt_abstime చివరి_rx_సమయం;
స్థిర hrt_abstime చివరి_txframe_సమయం = 0;

#నిర్వచించు SBUS2_చట్రం_కొలత_RX_VOLTAGE	3
#నిర్వచించు SBUS2_చట్రం_కొలత_GPS_DIGIT	3

స్థిర గణన SBUS2_DECODE_STATE {
	SBUS2_DECODE_STATE_DESYNC = 0xFFF,
	SBUS2_DECODE_STATE_SBUS_ప్రారంభం = 0x2FF,
	SBUS2_DECODE_STATE_SBUS1_SYNC = 0x00,
	SBUS2_DECODE_STATE_SBUS2_SYNC = 0x1FF,
	SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE = 0x04,
	SBUS2_DECODE_STATE_SBUS2_GPS = 0x14,
	SBUS2_DECODE_STATE_SBUS2_DATA1 = 0x24,
	SBUS2_DECODE_STATE_SBUS2_DATA2 = 0x34
} sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;

స్థిర sbus_చట్రం_t	 &sbus_చట్రం = rc_decode_buf.sbus_చట్రం;

స్థిర గుర్తులేని partial_చట్రం_లెక్క;
స్థిర గుర్తులేని sbus1_చట్రం_delay = (1000U * 1000U) / SBUS1_DEFAULT_రేటు_HZ;

స్థిర గుర్తులేని sbus_చట్రం_drops;

గుర్తులేని
sbus_dropped_చట్రాలు()
{
	తిరిగి sbus_చట్రం_drops;
}

స్థిర తార్కిక
sbus_decode(uint64_t చట్రం_సమయం, uint8_t *చట్రం, uint16_t *విలువలు_2, uint16_t *అంకె_విలువలు,
	    తార్కిక *sbus_failsafe, తార్కిక *sbus_చట్రం_drop, uint16_t max_విలువలు);

పూర్ణాంకం
sbus_init(స్థిరరాశి అక్షరం *device, తార్కిక singlewire)
{
	పూర్ణాంకం sbus_fd = open(device, O_RDWR | O_NONBLOCK);

	పూర్ణాంకం ret = sbus_config(sbus_fd, singlewire);

	ఒకవేళ (!ret) {
		తిరిగి sbus_fd;

	} లేకపోతే {
		తిరిగి -1;
	}
}

పూర్ణాంకం
sbus_config(పూర్ణాంకం sbus_fd, తార్కిక singlewire)
{
	పూర్ణాంకం ret = -1;

#ఒకవేళ defined(__PX4_LINUX)

	నిర్మాణం termios2 tio = {};

	ఒకవేళ (0 != ioctl(sbus_fd, TCGETS2, &tio)) {
		తిరిగి ret;
	}

	/**
	 * Setting serial port,8E2, non-blocking.100Kbps
	 */
	tio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL
			 | IXON);
	tio.c_iflag |= (INPCK | IGNPAR);
	tio.c_oflag &= ~OPOST;
	tio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tio.c_cflag &= ~(CSIZE | CRTSCTS | PARODD | CBAUD);
	/**
	 * use BOTHER to specify speed directly in c_[io]speed member
	 */
	tio.c_cflag |= (CS8 | CSTOPB | CLOCAL | PARENB | BOTHER | CREAD);
	tio.c_ispeed = 100000;
	tio.c_ospeed = 100000;
	tio.c_cc[VMIN] = 25;
	tio.c_cc[VTIME] = 0;

	ఒకవేళ (0 != ioctl(sbus_fd, TCSETS2, &tio)) {
		తిరిగి ret;
	}

	ret = 0;
#లేకపోతే

	ఒకవేళ (sbus_fd >= 0) {
		నిర్మాణం termios t;

		/* 100000bps, even parity, two stop bits */
		tcgetattr(sbus_fd, &t);
		cfsetspeed(&t, 100000);
		t.c_cflag |= (CSTOPB | PARENB);
		tcsetattr(sbus_fd, TCSANOW, &t);

		ఒకవేళ (singlewire) {
			/* only defined in configs capable of IOCTL
			 * Note It is never turned off
			 */
#నిర్వచితమైతే TIOCSSINGLEWIRE
			ioctl(sbus_fd, TIOCSSINGLEWIRE, SER_SINGLEWIRE_ENABLED);
#అయితే_ముగింపు
		}

		ret = 0;
	}

#అయితే_ముగింపు
	/* initialise the decoder */
	partial_చట్రం_లెక్క = 0;
	చివరి_rx_సమయం = hrt_absolute_సమయం();
	sbus_చట్రం_drops = 0;

	తిరిగి ret;
}

శూన్య
sbus1_output(పూర్ణాంకం sbus_fd, uint16_t *విలువలు_2, uint16_t అంకె_విలువలు)
{
	uint8_t byteindex = 1; /*Data starts one byte into the sbus frame. */
	uint8_t స్థానభ్రంశం = 0;
	uint16_t value;
	hrt_abstime now;

	now = hrt_absolute_సమయం();

	ఒకవేళ ((now - చివరి_txframe_సమయం) > sbus1_చట్రం_delay) {
		చివరి_txframe_సమయం = now;
		uint8_t	oframe[SBUS_చట్రం_కొలత] = { 0x0f };

		/* 16 is sbus number of servos/channels minus 2 single bit channels.
		* currently ignoring single bit channels.  */

		కొరకు (గుర్తులేని i = 0; (i < అంకె_విలువలు) && (i < 16); ++i) {
			దీర్ఘ scaled = lround((విలువలు_2[i] - SBUS_SCALE_స్థానభ్రంశం) / SBUS_SCALE_గుణకం);

			/* limit to 11 bits before the cast: a value below the scale offset (an output with no
			 * function is 0) is negative and would wrap to full scale */
			ఒకవేళ (scaled < 0) {
				scaled = 0;

			} లేకపోతే ఒకవేళ (scaled > 0x07ff) {
				scaled = 0x07ff;
			}

			value = static_cast<uint16_t>(scaled);

			అయితే (స్థానభ్రంశం >= 8) {
				++byteindex;
				స్థానభ్రంశం -= 8;
			}

			oframe[byteindex] |= (value << (స్థానభ్రంశం)) & 0xff;
			oframe[byteindex + 1] |= (value >> (8 - స్థానభ్రంశం)) & 0xff;
			oframe[byteindex + 2] |= (value >> (16 - స్థానభ్రంశం)) & 0xff;
			స్థానభ్రంశం += 11;
		}

		లిఖ(sbus_fd, oframe, SBUS_చట్రం_కొలత);
	}
}
శూన్య
sbus2_output(పూర్ణాంకం sbus_fd, uint16_t *విలువలు_2, uint16_t అంకె_విలువలు)
{
	sbus1_output(sbus_fd, విలువలు_2, అంకె_విలువలు);
}

తార్కిక
sbus_ఆదానం(పూర్ణాంకం sbus_fd, uint16_t *విలువలు_2, uint16_t *అంకె_విలువలు, తార్కిక *sbus_failsafe, తార్కిక *sbus_చట్రం_drop,
	   uint16_t max_channels)
{
	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */
	uint8_t buf[SBUS_చట్రం_కొలత * 2];

	పూర్ణాంకం ret = పఠ(sbus_fd, &buf[0], SBUS_చట్రం_కొలత);

	/* if the read failed for any reason, just give up here */
	ఒకవేళ (ret < 1) {
		తిరిగి అబద్ధం;
	}

	స్థిరరాశి hrt_abstime now = hrt_absolute_సమయం();
#నిర్వచితమైతే __PX4_NUTTX /* limit time-based hardening to RTOS's where we have reliable timing */

	/*
	 * The S.BUS protocol doesn't provide reliable framing,
	 * so we detect frame boundaries by the inter-frame delay.
	 *
	 * The minimum frame spacing is 7ms; with 25 bytes at 100000bps
	 * frame transmission time is 3ms.
	 *
	 * We expect to only be called when bytes arrive for processing,
	 * and if an interval of more than 3ms passes between calls,
	 * the first byte we read will be the first byte of a frame.
	 *
	 * In the case where byte(s) are dropped from a frame, this also
	 * provides a degree of protection. Of course, it would be better
	 * if we didn't drop bytes...
	 */
	ఒకవేళ (now - చివరి_rx_సమయం > 4_ms) {
		ఒకవేళ (partial_చట్రం_లెక్క > 0) {
			partial_చట్రం_లెక్క = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			రాయి("SBUS: RESET (TIME LIM)\n");
#అయితే_ముగింపు
		}
	}

#అయితే_ముగింపు /* __PX4_NUTTX */

	/*
	 * Try to decode something with what we got
	 */
	తిరిగి sbus_parse(now, &buf[0], ret, విలువలు_2, అంకె_విలువలు, sbus_failsafe,
			  sbus_చట్రం_drop, &sbus_చట్రం_drops, max_channels);
}

తార్కిక
sbus_parse(uint64_t now, uint8_t *చట్రం, గుర్తులేని len, uint16_t *విలువలు_2,
	   uint16_t *అంకె_విలువలు, తార్కిక *sbus_failsafe, తార్కిక *sbus_చట్రం_drop, గుర్తులేని *చట్రం_drops, uint16_t max_channels)
{

	చివరి_rx_సమయం = now;

	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	తార్కిక decode_ret = అబద్ధం;

	/* keep decoding until we have consumed the buffer */
	కొరకు (గుర్తులేని d = 0; d < len; d++) {

		/* overflow check */
		ఒకవేళ (partial_చట్రం_లెక్క == పరిమాణం(sbus_చట్రం) / పరిమాణం(sbus_చట్రం[0])) {
			partial_చట్రం_లెక్క = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			రాయి("SBUS2: RESET (BUF LIM)\n");
#అయితే_ముగింపు
		}

		ఒకవేళ (partial_చట్రం_లెక్క == SBUS_చట్రం_కొలత) {
			partial_చట్రం_లెక్క = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			రాయి("SBUS2: RESET (PACKET LIM)\n");
#అయితే_ముగింపు
		}

#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 1
		రాయి("sbus state: %s%s%s%s%s%s, count: %d, val: %02x\n",
		       (sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) ? "SBUS2_DECODE_STATE_DESYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS_ప్రారంభం) ? "SBUS2_DECODE_STATE_SBUS_START" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS1_SYNC) ? "SBUS2_DECODE_STATE_SBUS1_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_SYNC) ? "SBUS2_DECODE_STATE_SBUS2_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE) ? "SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_GPS) ? "SBUS2_DECODE_STATE_SBUS2_GPS" : "",
		       partial_చట్రం_లెక్క,
		       (గుర్తులేని)చట్రం[d]);
#అయితే_ముగింపు

		ఎంపిక (sbus_decode_state) {
		స్థితి SBUS2_DECODE_STATE_DESYNC:

		/* fall through */
		స్థితి SBUS2_DECODE_STATE_SBUS_ప్రారంభం:

			/* we are de-synced and only interested in the frame marker */
			ఒకవేళ (చట్రం[d] == SBUS_ప్రారంభం_చిహ్నం) {
				sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				partial_చట్రం_లెక్క = 0;
				sbus_చట్రం[partial_చట్రం_లెక్క++] = చట్రం[d];
			}

			ఆపు;

		స్థితి SBUS2_DECODE_STATE_SBUS1_SYNC:

		/* fall through */
		స్థితి SBUS2_DECODE_STATE_SBUS2_SYNC: {
				sbus_చట్రం[partial_చట్రం_లెక్క++] = చట్రం[d];

				/* decode whatever we got and expect */
				ఒకవేళ (partial_చట్రం_లెక్క < SBUS_చట్రం_కొలత) {
					ఆపు;
				}

				/*
				 * Great, it looks like we might have a frame. Go ahead and decode it.
				 */
				decode_ret = sbus_decode(now, sbus_చట్రం, విలువలు_2, అంకె_విలువలు, sbus_failsafe, sbus_చట్రం_drop, max_channels);

				/*
				 * Offset recovery: If decoding failed, check if there is a second
				 * start marker in the packet.
				 */
				గుర్తులేని ప్రారంభం_సూచీ = 0;

				ఒకవేళ (!decode_ret && sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) {

					కొరకు (గుర్తులేని i = 1; i < partial_చట్రం_లెక్క; i++) {
						ఒకవేళ (sbus_చట్రం[i] == SBUS_ప్రారంభం_చిహ్నం) {
							ప్రారంభం_సూచీ = i;
							ఆపు;
						}
					}

					/* we found a second start marker */
					ఒకవేళ (ప్రారంభం_సూచీ != 0) {
						/* shift everything in the buffer and reset the state machine */
						కొరకు (గుర్తులేని i = 0; i < partial_చట్రం_లెక్క - ప్రారంభం_సూచీ; i++) {
							sbus_చట్రం[i] = sbus_చట్రం[i + ప్రారంభం_సూచీ];
						}

						partial_చట్రం_లెక్క -= ప్రారంభం_సూచీ;
						sbus_decode_state = SBUS2_DECODE_STATE_SBUS_ప్రారంభం;

#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						రాయి("DECODE RECOVERY: %d\n", ప్రారంభం_సూచీ);
#అయితే_ముగింపు
					}
				}

				/* if there has been no successful attempt at saving a failed
				 * decoding run, reset the frame count for successful and
				 * unsuccessful decode runs.
				 */
				ఒకవేళ (ప్రారంభం_సూచీ == 0) {
					partial_చట్రం_లెక్క = 0;
				}

				ఒకవేళ (decode_ret) {
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS_ప్రారంభం;
				}

			}
			ఆపు;

		స్థితి SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE: {
				sbus_చట్రం[partial_చట్రం_లెక్క++] = చట్రం[d];

				ఒకవేళ (partial_చట్రం_లెక్క == 1 && sbus_చట్రం[0] == SBUS_ప్రారంభం_చిహ్నం) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				ఒకవేళ (partial_చట్రం_లెక్క < SBUS2_చట్రం_కొలత_RX_VOLTAGE) {
					ఆపు;
				}

				/* find out which payload we're dealing with in this slot */
				ఎంపిక (sbus_చట్రం[0]) {
				స్థితి 0x03: {
						// Observed values:
						// (frame[0] == 0x3 && frame[1] == 0x84 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0xc4 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0x80 && frame[2] == 0x2f)
						// (frame[0] == 0x3 && frame[1] == 0xc0 && frame[2] == 0x2f)
#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 2
						uint16_t rx_voltage = (sbus_చట్రం[1] << 8) | sbus_చట్రం[2];
						రాయి("rx_voltage %d\n", (పూర్ణాంకం)rx_voltage);
#అయితే_ముగింపు
					}

					partial_చట్రం_లెక్క = 0;
					ఆపు;

				అప్రమేయ:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_చట్రం_drops++;
				}

			}
			ఆపు;

		స్థితి SBUS2_DECODE_STATE_SBUS2_GPS: {
				sbus_చట్రం[partial_చట్రం_లెక్క++] = చట్రం[d];

				ఒకవేళ (partial_చట్రం_లెక్క == 1 && sbus_చట్రం[0] == SBUS_ప్రారంభం_చిహ్నం) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				ఒకవేళ (partial_చట్రం_లెక్క < 24) {
					ఆపు;
				}

				/* find out which payload we're dealing with in this slot */
				ఎంపిక (sbus_చట్రం[0]) {
				స్థితి 0x13: {
#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						uint16_t gps_something = (చట్రం[1] << 8) | చట్రం[2];
						రాయి("gps_something %d\n", (పూర్ణాంకం)gps_something);
#అయితే_ముగింపు
					}

					partial_చట్రం_లెక్క = 0;
					ఆపు;

				అప్రమేయ:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_చట్రం_drops++;
					/* throw unknown bytes away */
				}
			}
			ఆపు;

		అప్రమేయ:
#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			రాయి("UNKNOWN PROTO STATE");
#అయితే_ముగింపు
			decode_ret = అబద్ధం;
		}


	}

	ఒకవేళ (చట్రం_drops) {
		*చట్రం_drops = sbus_చట్రం_drops;
	}

	/* return false as default */
	తిరిగి decode_ret;
}

/*
 * S.bus decoder matrix.
 *
 * Each channel value can come from up to 3 input bytes. Each row in the
 * matrix describes up to three bytes, and each entry gives:
 *
 * - byte offset in the data portion of the frame
 * - right shift applied to the data byte
 * - mask for the data byte
 * - left shift applied to the result into the channel value
 */
నిర్మాణం sbus_బిట్టు_pick {
	uint8_t byte;
	uint8_t rshift;
	uint8_t ముసుగు;
	uint8_t lshift;
};
స్థిర స్థిరరాశి నిర్మాణం sbus_బిట్టు_pick sbus_decoder[SBUS_ఆదానం_CHANNELS][3] = {
	/*  0 */ { { 0, 0, 0xff, 0}, { 1, 0, 0x07, 8}, { 0, 0, 0x00,  0} },
	/*  1 */ { { 1, 3, 0x1f, 0}, { 2, 0, 0x3f, 5}, { 0, 0, 0x00,  0} },
	/*  2 */ { { 2, 6, 0x03, 0}, { 3, 0, 0xff, 2}, { 4, 0, 0x01, 10} },
	/*  3 */ { { 4, 1, 0x7f, 0}, { 5, 0, 0x0f, 7}, { 0, 0, 0x00,  0} },
	/*  4 */ { { 5, 4, 0x0f, 0}, { 6, 0, 0x7f, 4}, { 0, 0, 0x00,  0} },
	/*  5 */ { { 6, 7, 0x01, 0}, { 7, 0, 0xff, 1}, { 8, 0, 0x03,  9} },
	/*  6 */ { { 8, 2, 0x3f, 0}, { 9, 0, 0x1f, 6}, { 0, 0, 0x00,  0} },
	/*  7 */ { { 9, 5, 0x07, 0}, {10, 0, 0xff, 3}, { 0, 0, 0x00,  0} },
	/*  8 */ { {11, 0, 0xff, 0}, {12, 0, 0x07, 8}, { 0, 0, 0x00,  0} },
	/*  9 */ { {12, 3, 0x1f, 0}, {13, 0, 0x3f, 5}, { 0, 0, 0x00,  0} },
	/* 10 */ { {13, 6, 0x03, 0}, {14, 0, 0xff, 2}, {15, 0, 0x01, 10} },
	/* 11 */ { {15, 1, 0x7f, 0}, {16, 0, 0x0f, 7}, { 0, 0, 0x00,  0} },
	/* 12 */ { {16, 4, 0x0f, 0}, {17, 0, 0x7f, 4}, { 0, 0, 0x00,  0} },
	/* 13 */ { {17, 7, 0x01, 0}, {18, 0, 0xff, 1}, {19, 0, 0x03,  9} },
	/* 14 */ { {19, 2, 0x3f, 0}, {20, 0, 0x1f, 6}, { 0, 0, 0x00,  0} },
	/* 15 */ { {20, 5, 0x07, 0}, {21, 0, 0xff, 3}, { 0, 0, 0x00,  0} }
};

తార్కిక
sbus_decode(uint64_t చట్రం_సమయం, uint8_t *చట్రం, uint16_t *విలువలు_2, uint16_t *అంకె_విలువలు,
	    తార్కిక *sbus_failsafe, తార్కిక *sbus_చట్రం_drop, uint16_t max_విలువలు)
{

	/* check frame boundary markers to avoid out-of-sync cases */
	ఒకవేళ (చట్రం[0] != SBUS_ప్రారంభం_చిహ్నం) {
		sbus_చట్రం_drops++;
#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		రాయి("DECODE FAIL: ");

		కొరకు (గుర్తులేని i = 0; i < SBUS_చట్రం_కొలత; i++) {
			రాయి("%0x ", చట్రం[i]);
		}

		రాయి("\n");
#అయితే_ముగింపు
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		తిరిగి అబద్ధం;
	}

	/* the last byte in the frame indicates what frame will follow after this one */
	ఎంపిక (చట్రం[24]) {
	స్థితి 0x00:
		/* this is S.BUS 1 */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS1_SYNC;
		ఆపు;

	స్థితి 0x04:
		/* receiver voltage */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE;
		ఆపు;

	స్థితి 0x14:
		/* GPS / baro */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_GPS;
		ఆపు;

	స్థితి 0x24:
		/* Unknown SBUS2 data */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
		ఆపు;

	స్థితి 0x34:
		/* Unknown SBUS2 data */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
		ఆపు;

	అప్రమేయ:
#ఒకవేళ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		రాయి("DECODE FAIL: END MARKER\n");
#అయితే_ముగింపు
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		తిరిగి అబద్ధం;
	}

	/* we have received something we think is a frame */
	గుర్తులేని chancount = (max_విలువలు > SBUS_ఆదానం_CHANNELS) ?
			     SBUS_ఆదానం_CHANNELS : max_విలువలు;

	/* use the decoder matrix to extract channel data */
	కొరకు (గుర్తులేని channel = 0; channel < chancount; channel++) {
		గుర్తులేని value = 0;

		కొరకు (గుర్తులేని pick = 0; pick < 3; pick++) {
			స్థిరరాశి నిర్మాణం sbus_బిట్టు_pick *decode = &sbus_decoder[channel][pick];

			ఒకవేళ (decode->ముసుగు != 0) {
				గుర్తులేని piece = చట్రం[1 + decode->byte];
				piece >>= decode->rshift;
				piece &= decode->ముసుగు;
				piece <<= decode->lshift;

				value |= piece;
			}
		}


		/* convert 0-2048 values to 1000-2000 ppm encoding in a not too sloppy fashion */
		విలువలు_2[channel] = static_cast<uint16_t>(lround(value * SBUS_SCALE_గుణకం) + SBUS_SCALE_స్థానభ్రంశం);
	}

	/* decode switch channels if data fields are wide enough */
	ఒకవేళ (max_విలువలు > 17 && chancount > 15) {
		chancount = 18;

		/* channel 17 (index 16) */
		విలువలు_2[16] = ((చట్రం[SBUS_FLAGS_BYTE] & (1 << 0)) ? 1000 : 0) + 998;
		/* channel 18 (index 17) */
		విలువలు_2[17] = ((చట్రం[SBUS_FLAGS_BYTE] & (1 << 1)) ? 1000 : 0) + 998;
	}

	/* note the number of channels decoded */
	*అంకె_విలువలు = chancount;

	/* decode and handle failsafe and frame-lost flags */
	ఒకవేళ (చట్రం[SBUS_FLAGS_BYTE] & (1 << SBUS_FAILSAFE_బిట్టు)) { /* failsafe */
		/* report that we failed to read anything valid off the receiver */
		*sbus_failsafe = నిజం;
		*sbus_చట్రం_drop = నిజం;

	} లేకపోతే ఒకవేళ (చట్రం[SBUS_FLAGS_BYTE] & (1 << SBUS_FRAMELOST_బిట్టు)) { /* a frame was lost */
		/* set a special warning flag
		 *
		 * Attention! This flag indicates a skipped frame only, not a total link loss! Handling this
		 * condition as fail-safe greatly reduces the reliability and range of the radio link,
		 * e.g. by prematurely issuing return-to-launch!!! */

		*sbus_failsafe = అబద్ధం;
		*sbus_చట్రం_drop = నిజం;

	} లేకపోతే {
		*sbus_failsafe = అబద్ధం;
		*sbus_చట్రం_drop = అబద్ధం;
	}

	తిరిగి నిజం;
}

/*
  set output rate of SBUS in Hz
 */
శూన్య sbus1_సమితి_output_రేటు_hz(uint16_t రేటు_hz)
{
	sbus1_చట్రం_delay = (1000U * 1000U) / math::constrain(రేటు_hz, (uint16_t)SBUS1_MIN_రేటు_HZ,
			    (uint16_t)SBUS1_MAX_రేటు_HZ);
}
