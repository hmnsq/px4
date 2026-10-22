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

#ifdef TIOCSSINGLEWIRE
#ضمن <sys/ioctl.h>
#endif

#ضمن "sbus.h"
#ضمن "common_rc.h"
#ضمن <drivers/drv_hrt.h>
#ضمن <lib/mathlib/mathlib.h>

using نطاق time_literals;

#define SBUS_DEBUG_LEVEL 	0 /* Set debug output level */

#إذا defined(__PX4_LINUX)
#ضمن <sys/ioctl.h>
#ضمن <asm-generic/termbits.h>
#وإلا
#ضمن <termios.h>
#endif

#define SBUS_START_رمز	0x0f

#define SBUS_إدخال_CHANNELS	16
#define SBUS_FLAGS_BYTE		23
#define SBUS_FAILSAFE_BIT	3
#define SBUS_FRAMELOST_BIT	2

// testing with a SBUS->PWM adapter shows that
// above 300Hz SBUS becomes unreliable. 333 would
// be the theoretical achievable, but at 333Hz some
// frames are lost
#define SBUS1_MAX_RATE_HZ	300
#define SBUS1_MIN_RATE_HZ	50

// this is the rate of the old code
#define SBUS1_DEFAULT_RATE_HZ	72

/*
  Measured values with Futaba FX-30/R6108SB:
    -+100% on TX:  PCM 1.100/1.520/1.950ms -> SBus raw values: 350/1024/1700  (100% ATV)
    -+140% on TX:  PCM 0.930/1.520/2.112ms -> SBus raw values:  78/1024/1964  (140% ATV)
    -+152% on TX:  PCM 0.884/1.520/2.160ms -> SBus raw values:   1/1024/2047  (140% ATV plus dirty tricks)
*/

/* define range mapping here, -+100% -> 1000..2000 */
#define SBUS_RANGE_MIN 200.0f
#define SBUS_RANGE_MAX 1800.0f

#define SBUS_TARGET_MIN 1000.0f
#define SBUS_TARGET_MAX 2000.0f

#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
#  include <stdio.h>
#endif

/* pre-calculate the floating point stuff as far as possible at compile time */
#define SBUS_SCALE_FACTOR ((SBUS_TARGET_MAX - SBUS_TARGET_MIN) / (SBUS_RANGE_MAX - SBUS_RANGE_MIN))
#define SBUS_SCALE_OFFSET (صحيح)(SBUS_TARGET_MIN - (SBUS_SCALE_FACTOR * SBUS_RANGE_MIN + 0.5f))

ساكن hrt_abstime last_rx_time;
ساكن hrt_abstime last_txframe_time = 0;

#define SBUS2_FRAME_SIZE_RX_VOLTAGE	3
#define SBUS2_FRAME_SIZE_GPS_DIGIT	3

ساكن تعداد SBUS2_DECODE_STATE {
	SBUS2_DECODE_STATE_DESYNC = 0xFFF,
	SBUS2_DECODE_STATE_SBUS_START = 0x2FF,
	SBUS2_DECODE_STATE_SBUS1_SYNC = 0x00,
	SBUS2_DECODE_STATE_SBUS2_SYNC = 0x1FF,
	SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE = 0x04,
	SBUS2_DECODE_STATE_SBUS2_GPS = 0x14,
	SBUS2_DECODE_STATE_SBUS2_DATA1 = 0x24,
	SBUS2_DECODE_STATE_SBUS2_DATA2 = 0x34
} sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;

ساكن sbus_frame_t	 &sbus_frame = rc_decode_buf.sbus_frame;

ساكن unsigned partial_frame_تعداد;
ساكن unsigned sbus1_frame_delay = (1000U * 1000U) / SBUS1_DEFAULT_RATE_HZ;

ساكن unsigned sbus_frame_drops;

unsigned
sbus_dropped_frames()
{
	إرجاع sbus_frame_drops;
}

ساكن منطقي
sbus_decode(uint64_t frame_time, uint8_t *frame, uint16_t *قيم_2, uint16_t *رقم_قيم,
	    منطقي *sbus_failsafe, منطقي *sbus_frame_drop, uint16_t max_قيم);

صحيح
sbus_init(ثابت حرف *device, منطقي singlewire)
{
	صحيح sbus_fd = open(device, O_RDWR | O_NONBLOCK);

	صحيح ret = sbus_config(sbus_fd, singlewire);

	إذا (!ret) {
		إرجاع sbus_fd;

	} وإلا {
		إرجاع -1;
	}
}

صحيح
sbus_config(صحيح sbus_fd, منطقي singlewire)
{
	صحيح ret = -1;

#إذا defined(__PX4_LINUX)

	بنية termios2 tio = {};

	إذا (0 != ioctl(sbus_fd, TCGETS2, &tio)) {
		إرجاع ret;
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
		إرجاع ret;
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
#ifdef TIOCSSINGLEWIRE
			ioctl(sbus_fd, TIOCSSINGLEWIRE, SER_SINGLEWIRE_ENABLED);
#endif
		}

		ret = 0;
	}

#endif
	/* initialise the decoder */
	partial_frame_تعداد = 0;
	last_rx_time = hrt_absolute_time();
	sbus_frame_drops = 0;

	إرجاع ret;
}

فراغ
sbus1_output(صحيح sbus_fd, uint16_t *قيم_2, uint16_t رقم_قيم)
{
	uint8_t byteindex = 1; /*Data starts one byte into the sbus frame. */
	uint8_t offset = 0;
	uint16_t value;
	hrt_abstime now;

	now = hrt_absolute_time();

	إذا ((now - last_txframe_time) > sbus1_frame_delay) {
		last_txframe_time = now;
		uint8_t	oframe[SBUS_FRAME_SIZE] = { 0x0f };

		/* 16 is sbus number of servos/channels minus 2 single bit channels.
		* currently ignoring single bit channels.  */

		لكل (unsigned i = 0; (i < رقم_قيم) && (i < 16); ++i) {
			long scaled = lround((قيم_2[i] - SBUS_SCALE_OFFSET) / SBUS_SCALE_FACTOR);

			/* limit to 11 bits before the cast: a value below the scale offset (an output with no
			 * function is 0) is negative and would wrap to full scale */
			إذا (scaled < 0) {
				scaled = 0;

			} وإلا إذا (scaled > 0x07ff) {
				scaled = 0x07ff;
			}

			value = static_cast<uint16_t>(scaled);

			بينما (offset >= 8) {
				++byteindex;
				offset -= 8;
			}

			oframe[byteindex] |= (value << (offset)) & 0xff;
			oframe[byteindex + 1] |= (value >> (8 - offset)) & 0xff;
			oframe[byteindex + 2] |= (value >> (16 - offset)) & 0xff;
			offset += 11;
		}

		اكتب(sbus_fd, oframe, SBUS_FRAME_SIZE);
	}
}
فراغ
sbus2_output(صحيح sbus_fd, uint16_t *قيم_2, uint16_t رقم_قيم)
{
	sbus1_output(sbus_fd, قيم_2, رقم_قيم);
}

منطقي
sbus_إدخال(صحيح sbus_fd, uint16_t *قيم_2, uint16_t *رقم_قيم, منطقي *sbus_failsafe, منطقي *sbus_frame_drop,
	   uint16_t max_channels)
{
	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */
	uint8_t buf[SBUS_FRAME_SIZE * 2];

	صحيح ret = اقرأ(sbus_fd, &buf[0], SBUS_FRAME_SIZE);

	/* if the read failed for any reason, just give up here */
	إذا (ret < 1) {
		إرجاع خطأ;
	}

	ثابت hrt_abstime now = hrt_absolute_time();
#ifdef __PX4_NUTTX /* limit time-based hardening to RTOS's where we have reliable timing */

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
	إذا (now - last_rx_time > 4_ms) {
		إذا (partial_frame_تعداد > 0) {
			partial_frame_تعداد = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			printf("SBUS: RESET (TIME LIM)\n");
#endif
		}
	}

#endif /* __PX4_NUTTX */

	/*
	 * Try to decode something with what we got
	 */
	إرجاع sbus_parse(now, &buf[0], ret, قيم_2, رقم_قيم, sbus_failsafe,
			  sbus_frame_drop, &sbus_frame_drops, max_channels);
}

منطقي
sbus_parse(uint64_t now, uint8_t *frame, unsigned len, uint16_t *قيم_2,
	   uint16_t *رقم_قيم, منطقي *sbus_failsafe, منطقي *sbus_frame_drop, unsigned *frame_drops, uint16_t max_channels)
{

	last_rx_time = now;

	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	منطقي decode_ret = خطأ;

	/* keep decoding until we have consumed the buffer */
	لكل (unsigned d = 0; d < len; d++) {

		/* overflow check */
		إذا (partial_frame_تعداد == حجم(sbus_frame) / حجم(sbus_frame[0])) {
			partial_frame_تعداد = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			printf("SBUS2: RESET (BUF LIM)\n");
#endif
		}

		إذا (partial_frame_تعداد == SBUS_FRAME_SIZE) {
			partial_frame_تعداد = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			printf("SBUS2: RESET (PACKET LIM)\n");
#endif
		}

#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 1
		printf("sbus state: %s%s%s%s%s%s, count: %d, val: %02x\n",
		       (sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) ? "SBUS2_DECODE_STATE_DESYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS_START) ? "SBUS2_DECODE_STATE_SBUS_START" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS1_SYNC) ? "SBUS2_DECODE_STATE_SBUS1_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_SYNC) ? "SBUS2_DECODE_STATE_SBUS2_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE) ? "SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_GPS) ? "SBUS2_DECODE_STATE_SBUS2_GPS" : "",
		       partial_frame_تعداد,
		       (unsigned)frame[d]);
#endif

		بدل (sbus_decode_state) {
		حالة SBUS2_DECODE_STATE_DESYNC:

		/* fall through */
		حالة SBUS2_DECODE_STATE_SBUS_START:

			/* we are de-synced and only interested in the frame marker */
			إذا (frame[d] == SBUS_START_رمز) {
				sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				partial_frame_تعداد = 0;
				sbus_frame[partial_frame_تعداد++] = frame[d];
			}

			اكسر;

		حالة SBUS2_DECODE_STATE_SBUS1_SYNC:

		/* fall through */
		حالة SBUS2_DECODE_STATE_SBUS2_SYNC: {
				sbus_frame[partial_frame_تعداد++] = frame[d];

				/* decode whatever we got and expect */
				إذا (partial_frame_تعداد < SBUS_FRAME_SIZE) {
					اكسر;
				}

				/*
				 * Great, it looks like we might have a frame. Go ahead and decode it.
				 */
				decode_ret = sbus_decode(now, sbus_frame, قيم_2, رقم_قيم, sbus_failsafe, sbus_frame_drop, max_channels);

				/*
				 * Offset recovery: If decoding failed, check if there is a second
				 * start marker in the packet.
				 */
				unsigned start_index = 0;

				إذا (!decode_ret && sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) {

					لكل (unsigned i = 1; i < partial_frame_تعداد; i++) {
						إذا (sbus_frame[i] == SBUS_START_رمز) {
							start_index = i;
							اكسر;
						}
					}

					/* we found a second start marker */
					إذا (start_index != 0) {
						/* shift everything in the buffer and reset the state machine */
						لكل (unsigned i = 0; i < partial_frame_تعداد - start_index; i++) {
							sbus_frame[i] = sbus_frame[i + start_index];
						}

						partial_frame_تعداد -= start_index;
						sbus_decode_state = SBUS2_DECODE_STATE_SBUS_START;

#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						printf("DECODE RECOVERY: %d\n", start_index);
#endif
					}
				}

				/* if there has been no successful attempt at saving a failed
				 * decoding run, reset the frame count for successful and
				 * unsuccessful decode runs.
				 */
				إذا (start_index == 0) {
					partial_frame_تعداد = 0;
				}

				إذا (decode_ret) {
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS_START;
				}

			}
			اكسر;

		حالة SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE: {
				sbus_frame[partial_frame_تعداد++] = frame[d];

				إذا (partial_frame_تعداد == 1 && sbus_frame[0] == SBUS_START_رمز) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				إذا (partial_frame_تعداد < SBUS2_FRAME_SIZE_RX_VOLTAGE) {
					اكسر;
				}

				/* find out which payload we're dealing with in this slot */
				بدل (sbus_frame[0]) {
				حالة 0x03: {
						// Observed values:
						// (frame[0] == 0x3 && frame[1] == 0x84 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0xc4 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0x80 && frame[2] == 0x2f)
						// (frame[0] == 0x3 && frame[1] == 0xc0 && frame[2] == 0x2f)
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 2
						uint16_t rx_voltage = (sbus_frame[1] << 8) | sbus_frame[2];
						printf("rx_voltage %d\n", (صحيح)rx_voltage);
#endif
					}

					partial_frame_تعداد = 0;
					اكسر;

				افتراضي:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_frame_drops++;
				}

			}
			اكسر;

		حالة SBUS2_DECODE_STATE_SBUS2_GPS: {
				sbus_frame[partial_frame_تعداد++] = frame[d];

				إذا (partial_frame_تعداد == 1 && sbus_frame[0] == SBUS_START_رمز) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				إذا (partial_frame_تعداد < 24) {
					اكسر;
				}

				/* find out which payload we're dealing with in this slot */
				بدل (sbus_frame[0]) {
				حالة 0x13: {
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						uint16_t gps_something = (frame[1] << 8) | frame[2];
						printf("gps_something %d\n", (صحيح)gps_something);
#endif
					}

					partial_frame_تعداد = 0;
					اكسر;

				افتراضي:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_frame_drops++;
					/* throw unknown bytes away */
				}
			}
			اكسر;

		افتراضي:
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			printf("UNKNOWN PROTO STATE");
#endif
			decode_ret = خطأ;
		}


	}

	إذا (frame_drops) {
		*frame_drops = sbus_frame_drops;
	}

	/* return false as default */
	إرجاع decode_ret;
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
بنية sbus_bit_pick {
	uint8_t byte;
	uint8_t rshift;
	uint8_t mask;
	uint8_t lshift;
};
ساكن ثابت بنية sbus_bit_pick sbus_decoder[SBUS_إدخال_CHANNELS][3] = {
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
sbus_decode(uint64_t frame_time, uint8_t *frame, uint16_t *قيم_2, uint16_t *رقم_قيم,
	    منطقي *sbus_failsafe, منطقي *sbus_frame_drop, uint16_t max_قيم)
{

	/* check frame boundary markers to avoid out-of-sync cases */
	إذا (frame[0] != SBUS_START_رمز) {
		sbus_frame_drops++;
#إذا defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		printf("DECODE FAIL: ");

		لكل (unsigned i = 0; i < SBUS_FRAME_SIZE; i++) {
			printf("%0x ", frame[i]);
		}

		printf("\n");
#endif
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		إرجاع خطأ;
	}

	/* the last byte in the frame indicates what frame will follow after this one */
	بدل (frame[24]) {
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
		printf("DECODE FAIL: END MARKER\n");
#endif
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		إرجاع خطأ;
	}

	/* we have received something we think is a frame */
	unsigned chancount = (max_قيم > SBUS_إدخال_CHANNELS) ?
			     SBUS_إدخال_CHANNELS : max_قيم;

	/* use the decoder matrix to extract channel data */
	لكل (unsigned channel = 0; channel < chancount; channel++) {
		unsigned value = 0;

		لكل (unsigned pick = 0; pick < 3; pick++) {
			ثابت بنية sbus_bit_pick *decode = &sbus_decoder[channel][pick];

			إذا (decode->mask != 0) {
				unsigned piece = frame[1 + decode->byte];
				piece >>= decode->rshift;
				piece &= decode->mask;
				piece <<= decode->lshift;

				value |= piece;
			}
		}


		/* convert 0-2048 values to 1000-2000 ppm encoding in a not too sloppy fashion */
		قيم_2[channel] = static_cast<uint16_t>(lround(value * SBUS_SCALE_FACTOR) + SBUS_SCALE_OFFSET);
	}

	/* decode switch channels if data fields are wide enough */
	إذا (max_قيم > 17 && chancount > 15) {
		chancount = 18;

		/* channel 17 (index 16) */
		قيم_2[16] = ((frame[SBUS_FLAGS_BYTE] & (1 << 0)) ? 1000 : 0) + 998;
		/* channel 18 (index 17) */
		قيم_2[17] = ((frame[SBUS_FLAGS_BYTE] & (1 << 1)) ? 1000 : 0) + 998;
	}

	/* note the number of channels decoded */
	*رقم_قيم = chancount;

	/* decode and handle failsafe and frame-lost flags */
	إذا (frame[SBUS_FLAGS_BYTE] & (1 << SBUS_FAILSAFE_BIT)) { /* failsafe */
		/* report that we failed to read anything valid off the receiver */
		*sbus_failsafe = true;
		*sbus_frame_drop = true;

	} وإلا إذا (frame[SBUS_FLAGS_BYTE] & (1 << SBUS_FRAMELOST_BIT)) { /* a frame was lost */
		/* set a special warning flag
		 *
		 * Attention! This flag indicates a skipped frame only, not a total link loss! Handling this
		 * condition as fail-safe greatly reduces the reliability and range of the radio link,
		 * e.g. by prematurely issuing return-to-launch!!! */

		*sbus_failsafe = خطأ;
		*sbus_frame_drop = true;

	} وإلا {
		*sbus_failsafe = خطأ;
		*sbus_frame_drop = خطأ;
	}

	إرجاع true;
}

/*
  set output rate of SBUS in Hz
 */
فراغ sbus1_set_output_rate_hz(uint16_t rate_hz)
{
	sbus1_frame_delay = (1000U * 1000U) / math::constrain(rate_hz, (uint16_t)SBUS1_MIN_RATE_HZ,
			    (uint16_t)SBUS1_MAX_RATE_HZ);
}
