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

#ضمن <px4_platform_common/px4_config.h>

#ضمن <fcntl.h>
#ضمن <unistd.h>
#ضمن <string.h>

#جيڪڏهن_وصف_ٿيل TIOCSSINGLEWIRE
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو

#ضمن "sbus.h"
#ضمن "common_rc.h"
#ضمن <drivers/drv_hrt.h>
#ضمن <lib/mathlib/mathlib.h>

استعمال نطاق وقت_literals;

#وصف_ڪريو SBUS_DEBUG_LEVEL 	0 /* Set debug output level */

#إذا defined(__PX4_LINUX)
#ضمن <sys/ioctl.h>
#ضمن <asm-generic/termbits.h>
#وإلا
#ضمن <termios.h>
#جيڪڏهن_پورو

#وصف_ڪريو SBUS_شروعات_نشان	0x0f

#وصف_ڪريو SBUS_داخلا_CHANNELS	16
#وصف_ڪريو SBUS_FLAGS_BYTE		23
#وصف_ڪريو SBUS_FAILSAFE_ٻٽو_انگ	3
#وصف_ڪريو SBUS_FRAMELOST_ٻٽو_انگ	2

// testing with a SBUS->PWM adapter shows that
// above 300Hz SBUS becomes unreliable. 333 would
// be the theoretical achievable, but at 333Hz some
// frames are lost
#وصف_ڪريو SBUS1_MAX_شرح_HZ	300
#وصف_ڪريو SBUS1_MIN_شرح_HZ	50

// this is the rate of the old code
#وصف_ڪريو SBUS1_DEFAULT_شرح_HZ	72

/*
  Measured values with Futaba FX-30/R6108SB:
    -+100% on TX:  PCM 1.100/1.520/1.950ms -> SBus raw values: 350/1024/1700  (100% ATV)
    -+140% on TX:  PCM 0.930/1.520/2.112ms -> SBus raw values:  78/1024/1964  (140% ATV)
    -+152% on TX:  PCM 0.884/1.520/2.160ms -> SBus raw values:   1/1024/2047  (140% ATV plus dirty tricks)
*/

/* define range mapping here, -+100% -> 1000..2000 */
#وصف_ڪريو SBUS_RANGE_MIN 200.0f
#وصف_ڪريو SBUS_RANGE_MAX 1800.0f

#وصف_ڪريو SBUS_هدف_MIN 1000.0f
#وصف_ڪريو SBUS_هدف_MAX 2000.0f

#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
#  include <stdio.h>
#جيڪڏهن_پورو

/* pre-calculate the floating point stuff as far as possible at compile time */
#وصف_ڪريو SBUS_SCALE_ضرب_ڪندڙ ((SBUS_هدف_MAX - SBUS_هدف_MIN) / (SBUS_RANGE_MAX - SBUS_RANGE_MIN))
#وصف_ڪريو SBUS_SCALE_وٿي (صحيح)(SBUS_هدف_MIN - (SBUS_SCALE_ضرب_ڪندڙ * SBUS_RANGE_MIN + 0.5f))

ساكن hrt_abstime آخري_rx_وقت;
ساكن hrt_abstime آخري_txframe_وقت = 0;

#وصف_ڪريو SBUS2_چوکٺو_سائيز_RX_VOLTAGE	3
#وصف_ڪريو SBUS2_چوکٺو_سائيز_GPS_DIGIT	3

ساكن تعداد SBUS2_DECODE_STATE {
	SBUS2_DECODE_STATE_DESYNC = 0xFFF,
	SBUS2_DECODE_STATE_SBUS_شروعات = 0x2FF,
	SBUS2_DECODE_STATE_SBUS1_SYNC = 0x00,
	SBUS2_DECODE_STATE_SBUS2_SYNC = 0x1FF,
	SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE = 0x04,
	SBUS2_DECODE_STATE_SBUS2_GPS = 0x14,
	SBUS2_DECODE_STATE_SBUS2_DATA1 = 0x24,
	SBUS2_DECODE_STATE_SBUS2_DATA2 = 0x34
} sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;

ساكن sbus_چوکٺو_t	 &sbus_چوکٺو = rc_decode_buf.sbus_چوکٺو;

ساكن بنانشان partial_چوکٺو_ڳڻپ;
ساكن بنانشان sbus1_چوکٺو_delay = (1000U * 1000U) / SBUS1_DEFAULT_شرح_HZ;

ساكن بنانشان sbus_چوکٺو_drops;

بنانشان
sbus_dropped_چوکٺا()
{
	أرجع sbus_چوکٺو_drops;
}

ساكن منطقي
sbus_decode(uint64_t چوکٺو_وقت, uint8_t *چوکٺو, uint16_t *قدرون_2, uint16_t *انگ_قدرون,
	    منطقي *sbus_failsafe, منطقي *sbus_چوکٺو_drop, uint16_t max_قدرون);

صحيح
sbus_init(ثابت حرف *device, منطقي singlewire)
{
	صحيح sbus_fd = open(device, O_RDWR | O_NONBLOCK);

	صحيح ret = sbus_config(sbus_fd, singlewire);

	إذا (!ret) {
		أرجع sbus_fd;

	} وإلا {
		أرجع -1;
	}
}

صحيح
sbus_config(صحيح sbus_fd, منطقي singlewire)
{
	صحيح ret = -1;

#إذا defined(__PX4_LINUX)

	بنية termios2 tio = {};

	إذا (0 != ioctl(sbus_fd, TCGETS2, &tio)) {
		أرجع ret;
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

	إذا (0 != ioctl(sbus_fd, TCSETS2, &tio)) {
		أرجع ret;
	}

	ret = 0;
#وإلا

	إذا (sbus_fd >= 0) {
		بنية termios t;

		/* 100000bps, even parity, two stop bits */
		tcgetattr(sbus_fd, &t);
		cfsetspeed(&t, 100000);
		t.c_cflag |= (CSTOPB | PARENB);
		tcsetattr(sbus_fd, TCSANOW, &t);

		إذا (singlewire) {
			/* only defined in configs capable of IOCTL
			 * Note It is never turned off
			 */
#جيڪڏهن_وصف_ٿيل TIOCSSINGLEWIRE
			ioctl(sbus_fd, TIOCSSINGLEWIRE, SER_SINGLEWIRE_ENABLED);
#جيڪڏهن_پورو
		}

		ret = 0;
	}

#جيڪڏهن_پورو
	/* initialise the decoder */
	partial_چوکٺو_ڳڻپ = 0;
	آخري_rx_وقت = hrt_absolute_وقت();
	sbus_چوکٺو_drops = 0;

	أرجع ret;
}

فراغ
sbus1_output(صحيح sbus_fd, uint16_t *قدرون_2, uint16_t انگ_قدرون)
{
	uint8_t byteindex = 1; /*Data starts one byte into the sbus frame. */
	uint8_t وٿي = 0;
	uint16_t value;
	hrt_abstime now;

	now = hrt_absolute_وقت();

	إذا ((now - آخري_txframe_وقت) > sbus1_چوکٺو_delay) {
		آخري_txframe_وقت = now;
		uint8_t	oframe[SBUS_چوکٺو_سائيز] = { 0x0f };

		/* 16 is sbus number of servos/channels minus 2 single bit channels.
		* currently ignoring single bit channels.  */

		لكل (بنانشان i = 0; (i < انگ_قدرون) && (i < 16); ++i) {
			ڊگهو scaled = lround((قدرون_2[i] - SBUS_SCALE_وٿي) / SBUS_SCALE_ضرب_ڪندڙ);

			/* limit to 11 bits before the cast: a value below the scale offset (an output with no
			 * function is 0) is negative and would wrap to full scale */
			إذا (scaled < 0) {
				scaled = 0;

			} وإلا إذا (scaled > 0x07ff) {
				scaled = 0x07ff;
			}

			value = static_cast<uint16_t>(scaled);

			طالما (وٿي >= 8) {
				++byteindex;
				وٿي -= 8;
			}

			oframe[byteindex] |= (value << (وٿي)) & 0xff;
			oframe[byteindex + 1] |= (value >> (8 - وٿي)) & 0xff;
			oframe[byteindex + 2] |= (value >> (16 - وٿي)) & 0xff;
			وٿي += 11;
		}

		اكتب(sbus_fd, oframe, SBUS_چوکٺو_سائيز);
	}
}
فراغ
sbus2_output(صحيح sbus_fd, uint16_t *قدرون_2, uint16_t انگ_قدرون)
{
	sbus1_output(sbus_fd, قدرون_2, انگ_قدرون);
}

منطقي
sbus_داخلا(صحيح sbus_fd, uint16_t *قدرون_2, uint16_t *انگ_قدرون, منطقي *sbus_failsafe, منطقي *sbus_چوکٺو_drop,
	   uint16_t max_channels)
{
	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */
	uint8_t buf[SBUS_چوکٺو_سائيز * 2];

	صحيح ret = اقرأ(sbus_fd, &buf[0], SBUS_چوکٺو_سائيز);

	/* if the read failed for any reason, just give up here */
	إذا (ret < 1) {
		أرجع خطأ;
	}

	ثابت hrt_abstime now = hrt_absolute_وقت();
#جيڪڏهن_وصف_ٿيل __PX4_NUTTX /* limit time-based hardening to RTOS's where we have reliable timing */

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
	إذا (now - آخري_rx_وقت > 4_ms) {
		إذا (partial_چوکٺو_ڳڻپ > 0) {
			partial_چوکٺو_ڳڻپ = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			لکو("SBUS: RESET (TIME LIM)\n");
#جيڪڏهن_پورو
		}
	}

#جيڪڏهن_پورو /* __PX4_NUTTX */

	/*
	 * Try to decode something with what we got
	 */
	أرجع sbus_parse(now, &buf[0], ret, قدرون_2, انگ_قدرون, sbus_failsafe,
			  sbus_چوکٺو_drop, &sbus_چوکٺو_drops, max_channels);
}

منطقي
sbus_parse(uint64_t now, uint8_t *چوکٺو, بنانشان len, uint16_t *قدرون_2,
	   uint16_t *انگ_قدرون, منطقي *sbus_failsafe, منطقي *sbus_چوکٺو_drop, بنانشان *چوکٺو_drops, uint16_t max_channels)
{

	آخري_rx_وقت = now;

	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	منطقي decode_ret = خطأ;

	/* keep decoding until we have consumed the buffer */
	لكل (بنانشان d = 0; d < len; d++) {

		/* overflow check */
		إذا (partial_چوکٺو_ڳڻپ == حجم(sbus_چوکٺو) / حجم(sbus_چوکٺو[0])) {
			partial_چوکٺو_ڳڻپ = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			لکو("SBUS2: RESET (BUF LIM)\n");
#جيڪڏهن_پورو
		}

		إذا (partial_چوکٺو_ڳڻپ == SBUS_چوکٺو_سائيز) {
			partial_چوکٺو_ڳڻپ = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			لکو("SBUS2: RESET (PACKET LIM)\n");
#جيڪڏهن_پورو
		}

#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 1
		لکو("sbus state: %s%s%s%s%s%s, count: %d, val: %02x\n",
		       (sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) ? "SBUS2_DECODE_STATE_DESYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS_شروعات) ? "SBUS2_DECODE_STATE_SBUS_START" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS1_SYNC) ? "SBUS2_DECODE_STATE_SBUS1_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_SYNC) ? "SBUS2_DECODE_STATE_SBUS2_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE) ? "SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_GPS) ? "SBUS2_DECODE_STATE_SBUS2_GPS" : "",
		       partial_چوکٺو_ڳڻپ,
		       (بنانشان)چوکٺو[d]);
#جيڪڏهن_پورو

		بدل (sbus_decode_state) {
		حالة SBUS2_DECODE_STATE_DESYNC:

		/* fall through */
		حالة SBUS2_DECODE_STATE_SBUS_شروعات:

			/* we are de-synced and only interested in the frame marker */
			إذا (چوکٺو[d] == SBUS_شروعات_نشان) {
				sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				partial_چوکٺو_ڳڻپ = 0;
				sbus_چوکٺو[partial_چوکٺو_ڳڻپ++] = چوکٺو[d];
			}

			اكسر;

		حالة SBUS2_DECODE_STATE_SBUS1_SYNC:

		/* fall through */
		حالة SBUS2_DECODE_STATE_SBUS2_SYNC: {
				sbus_چوکٺو[partial_چوکٺو_ڳڻپ++] = چوکٺو[d];

				/* decode whatever we got and expect */
				إذا (partial_چوکٺو_ڳڻپ < SBUS_چوکٺو_سائيز) {
					اكسر;
				}

				/*
				 * Great, it looks like we might have a frame. Go ahead and decode it.
				 */
				decode_ret = sbus_decode(now, sbus_چوکٺو, قدرون_2, انگ_قدرون, sbus_failsafe, sbus_چوکٺو_drop, max_channels);

				/*
				 * Offset recovery: If decoding failed, check if there is a second
				 * start marker in the packet.
				 */
				بنانشان شروعات_اشاريو = 0;

				إذا (!decode_ret && sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) {

					لكل (بنانشان i = 1; i < partial_چوکٺو_ڳڻپ; i++) {
						إذا (sbus_چوکٺو[i] == SBUS_شروعات_نشان) {
							شروعات_اشاريو = i;
							اكسر;
						}
					}

					/* we found a second start marker */
					إذا (شروعات_اشاريو != 0) {
						/* shift everything in the buffer and reset the state machine */
						لكل (بنانشان i = 0; i < partial_چوکٺو_ڳڻپ - شروعات_اشاريو; i++) {
							sbus_چوکٺو[i] = sbus_چوکٺو[i + شروعات_اشاريو];
						}

						partial_چوکٺو_ڳڻپ -= شروعات_اشاريو;
						sbus_decode_state = SBUS2_DECODE_STATE_SBUS_شروعات;

#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						لکو("DECODE RECOVERY: %d\n", شروعات_اشاريو);
#جيڪڏهن_پورو
					}
				}

				/* if there has been no successful attempt at saving a failed
				 * decoding run, reset the frame count for successful and
				 * unsuccessful decode runs.
				 */
				إذا (شروعات_اشاريو == 0) {
					partial_چوکٺو_ڳڻپ = 0;
				}

				إذا (decode_ret) {
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS_شروعات;
				}

			}
			اكسر;

		حالة SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE: {
				sbus_چوکٺو[partial_چوکٺو_ڳڻپ++] = چوکٺو[d];

				إذا (partial_چوکٺو_ڳڻپ == 1 && sbus_چوکٺو[0] == SBUS_شروعات_نشان) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				إذا (partial_چوکٺو_ڳڻپ < SBUS2_چوکٺو_سائيز_RX_VOLTAGE) {
					اكسر;
				}

				/* find out which payload we're dealing with in this slot */
				بدل (sbus_چوکٺو[0]) {
				حالة 0x03: {
						// Observed values:
						// (frame[0] == 0x3 && frame[1] == 0x84 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0xc4 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0x80 && frame[2] == 0x2f)
						// (frame[0] == 0x3 && frame[1] == 0xc0 && frame[2] == 0x2f)
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 2
						uint16_t rx_voltage = (sbus_چوکٺو[1] << 8) | sbus_چوکٺو[2];
						لکو("rx_voltage %d\n", (صحيح)rx_voltage);
#جيڪڏهن_پورو
					}

					partial_چوکٺو_ڳڻپ = 0;
					اكسر;

				افتراضي:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_چوکٺو_drops++;
				}

			}
			اكسر;

		حالة SBUS2_DECODE_STATE_SBUS2_GPS: {
				sbus_چوکٺو[partial_چوکٺو_ڳڻپ++] = چوکٺو[d];

				إذا (partial_چوکٺو_ڳڻپ == 1 && sbus_چوکٺو[0] == SBUS_شروعات_نشان) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				إذا (partial_چوکٺو_ڳڻپ < 24) {
					اكسر;
				}

				/* find out which payload we're dealing with in this slot */
				بدل (sbus_چوکٺو[0]) {
				حالة 0x13: {
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						uint16_t gps_something = (چوکٺو[1] << 8) | چوکٺو[2];
						لکو("gps_something %d\n", (صحيح)gps_something);
#جيڪڏهن_پورو
					}

					partial_چوکٺو_ڳڻپ = 0;
					اكسر;

				افتراضي:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_چوکٺو_drops++;
					/* throw unknown bytes away */
				}
			}
			اكسر;

		افتراضي:
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			لکو("UNKNOWN PROTO STATE");
#جيڪڏهن_پورو
			decode_ret = خطأ;
		}


	}

	إذا (چوکٺو_drops) {
		*چوکٺو_drops = sbus_چوکٺو_drops;
	}

	/* return false as default */
	أرجع decode_ret;
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
بنية sbus_ٻٽو_انگ_pick {
	uint8_t byte;
	uint8_t rshift;
	uint8_t نقاب;
	uint8_t lshift;
};
ساكن ثابت بنية sbus_ٻٽو_انگ_pick sbus_decoder[SBUS_داخلا_CHANNELS][3] = {
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

منطقي
sbus_decode(uint64_t چوکٺو_وقت, uint8_t *چوکٺو, uint16_t *قدرون_2, uint16_t *انگ_قدرون,
	    منطقي *sbus_failsafe, منطقي *sbus_چوکٺو_drop, uint16_t max_قدرون)
{

	/* check frame boundary markers to avoid out-of-sync cases */
	إذا (چوکٺو[0] != SBUS_شروعات_نشان) {
		sbus_چوکٺو_drops++;
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		لکو("DECODE FAIL: ");

		لكل (بنانشان i = 0; i < SBUS_چوکٺو_سائيز; i++) {
			لکو("%0x ", چوکٺو[i]);
		}

		لکو("\n");
#جيڪڏهن_پورو
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		أرجع خطأ;
	}

	/* the last byte in the frame indicates what frame will follow after this one */
	بدل (چوکٺو[24]) {
	حالة 0x00:
		/* this is S.BUS 1 */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS1_SYNC;
		اكسر;

	حالة 0x04:
		/* receiver voltage */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE;
		اكسر;

	حالة 0x14:
		/* GPS / baro */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_GPS;
		اكسر;

	حالة 0x24:
		/* Unknown SBUS2 data */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
		اكسر;

	حالة 0x34:
		/* Unknown SBUS2 data */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
		اكسر;

	افتراضي:
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		لکو("DECODE FAIL: END MARKER\n");
#جيڪڏهن_پورو
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		أرجع خطأ;
	}

	/* we have received something we think is a frame */
	بنانشان chancount = (max_قدرون > SBUS_داخلا_CHANNELS) ?
			     SBUS_داخلا_CHANNELS : max_قدرون;

	/* use the decoder matrix to extract channel data */
	لكل (بنانشان channel = 0; channel < chancount; channel++) {
		بنانشان value = 0;

		لكل (بنانشان pick = 0; pick < 3; pick++) {
			ثابت بنية sbus_ٻٽو_انگ_pick *decode = &sbus_decoder[channel][pick];

			إذا (decode->نقاب != 0) {
				بنانشان piece = چوکٺو[1 + decode->byte];
				piece >>= decode->rshift;
				piece &= decode->نقاب;
				piece <<= decode->lshift;

				value |= piece;
			}
		}


		/* convert 0-2048 values to 1000-2000 ppm encoding in a not too sloppy fashion */
		قدرون_2[channel] = static_cast<uint16_t>(lround(value * SBUS_SCALE_ضرب_ڪندڙ) + SBUS_SCALE_وٿي);
	}

	/* decode switch channels if data fields are wide enough */
	إذا (max_قدرون > 17 && chancount > 15) {
		chancount = 18;

		/* channel 17 (index 16) */
		قدرون_2[16] = ((چوکٺو[SBUS_FLAGS_BYTE] & (1 << 0)) ? 1000 : 0) + 998;
		/* channel 18 (index 17) */
		قدرون_2[17] = ((چوکٺو[SBUS_FLAGS_BYTE] & (1 << 1)) ? 1000 : 0) + 998;
	}

	/* note the number of channels decoded */
	*انگ_قدرون = chancount;

	/* decode and handle failsafe and frame-lost flags */
	إذا (چوکٺو[SBUS_FLAGS_BYTE] & (1 << SBUS_FAILSAFE_ٻٽو_انگ)) { /* failsafe */
		/* report that we failed to read anything valid off the receiver */
		*sbus_failsafe = سچ;
		*sbus_چوکٺو_drop = سچ;

	} وإلا إذا (چوکٺو[SBUS_FLAGS_BYTE] & (1 << SBUS_FRAMELOST_ٻٽو_انگ)) { /* a frame was lost */
		/* set a special warning flag
		 *
		 * Attention! This flag indicates a skipped frame only, not a total link loss! Handling this
		 * condition as fail-safe greatly reduces the reliability and range of the radio link,
		 * e.g. by prematurely issuing return-to-launch!!! */

		*sbus_failsafe = خطأ;
		*sbus_چوکٺو_drop = سچ;

	} وإلا {
		*sbus_failsafe = خطأ;
		*sbus_چوکٺو_drop = خطأ;
	}

	أرجع سچ;
}

/*
  set output rate of SBUS in Hz
 */
فراغ sbus1_سيٽ_output_شرح_hz(uint16_t شرح_hz)
{
	sbus1_چوکٺو_delay = (1000U * 1000U) / math::constrain(شرح_hz, (uint16_t)SBUS1_MIN_شرح_HZ,
			    (uint16_t)SBUS1_MAX_شرح_HZ);
}
