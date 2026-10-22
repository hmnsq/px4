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

#شامل <px4_platform_common/px4_config.h>

#شامل <fcntl.h>
#شامل <unistd.h>
#شامل <string.h>

#اگر_تعریف_شدہ TIOCSSINGLEWIRE
#شامل <sys/ioctl.h>
#اگر_ختم

#شامل "sbus.h"
#شامل "common_rc.h"
#شامل <drivers/drv_hrt.h>
#شامل <lib/mathlib/mathlib.h>

استعمال نامخلا وقت_literals;

#تعریف_کرو SBUS_DEBUG_LEVEL 	0 /* Set debug output level */

#اگر defined(__PX4_LINUX)
#شامل <sys/ioctl.h>
#شامل <asm-generic/termbits.h>
#ورنہ
#شامل <termios.h>
#اگر_ختم

#تعریف_کرو SBUS_آغاز_علامت	0x0f

#تعریف_کرو SBUS_داخلہ_CHANNELS	16
#تعریف_کرو SBUS_FLAGS_BYTE		23
#تعریف_کرو SBUS_FAILSAFE_ثنائی_ہندسہ	3
#تعریف_کرو SBUS_FRAMELOST_ثنائی_ہندسہ	2

// testing with a SBUS->PWM adapter shows that
// above 300Hz SBUS becomes unreliable. 333 would
// be the theoretical achievable, but at 333Hz some
// frames are lost
#تعریف_کرو SBUS1_MAX_شرح_HZ	300
#تعریف_کرو SBUS1_MIN_شرح_HZ	50

// this is the rate of the old code
#تعریف_کرو SBUS1_DEFAULT_شرح_HZ	72

/*
  Measured values with Futaba FX-30/R6108SB:
    -+100% on TX:  PCM 1.100/1.520/1.950ms -> SBus raw values: 350/1024/1700  (100% ATV)
    -+140% on TX:  PCM 0.930/1.520/2.112ms -> SBus raw values:  78/1024/1964  (140% ATV)
    -+152% on TX:  PCM 0.884/1.520/2.160ms -> SBus raw values:   1/1024/2047  (140% ATV plus dirty tricks)
*/

/* define range mapping here, -+100% -> 1000..2000 */
#تعریف_کرو SBUS_RANGE_MIN 200.0f
#تعریف_کرو SBUS_RANGE_MAX 1800.0f

#تعریف_کرو SBUS_ہدف_MIN 1000.0f
#تعریف_کرو SBUS_ہدف_MAX 2000.0f

#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
#  include <stdio.h>
#اگر_ختم

/* pre-calculate the floating point stuff as far as possible at compile time */
#تعریف_کرو SBUS_SCALE_ضربی_عدد ((SBUS_ہدف_MAX - SBUS_ہدف_MIN) / (SBUS_RANGE_MAX - SBUS_RANGE_MIN))
#تعریف_کرو SBUS_SCALE_انحراف (صحیح)(SBUS_ہدف_MIN - (SBUS_SCALE_ضربی_عدد * SBUS_RANGE_MIN + 0.5f))

ساکن hrt_abstime آخری_rx_وقت;
ساکن hrt_abstime آخری_txframe_وقت = 0;

#تعریف_کرو SBUS2_چوکھٹا_سائز_RX_VOLTAGE	3
#تعریف_کرو SBUS2_چوکھٹا_سائز_GPS_DIGIT	3

ساکن شمار SBUS2_DECODE_STATE {
	SBUS2_DECODE_STATE_DESYNC = 0xFFF,
	SBUS2_DECODE_STATE_SBUS_آغاز = 0x2FF,
	SBUS2_DECODE_STATE_SBUS1_SYNC = 0x00,
	SBUS2_DECODE_STATE_SBUS2_SYNC = 0x1FF,
	SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE = 0x04,
	SBUS2_DECODE_STATE_SBUS2_GPS = 0x14,
	SBUS2_DECODE_STATE_SBUS2_DATA1 = 0x24,
	SBUS2_DECODE_STATE_SBUS2_DATA2 = 0x34
} sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;

ساکن sbus_چوکھٹا_t	 &sbus_چوکھٹا = rc_decode_buf.sbus_چوکھٹا;

ساکن غیرعلامتی partial_چوکھٹا_گنتی;
ساکن غیرعلامتی sbus1_چوکھٹا_delay = (1000U * 1000U) / SBUS1_DEFAULT_شرح_HZ;

ساکن غیرعلامتی sbus_چوکھٹا_drops;

غیرعلامتی
sbus_dropped_چوکھٹے()
{
	واپس sbus_چوکھٹا_drops;
}

ساکن منطقی
sbus_decode(uint64_t چوکھٹا_وقت, uint8_t *چوکھٹا, uint16_t *اقدار_2, uint16_t *عدد_اقدار,
	    منطقی *sbus_failsafe, منطقی *sbus_چوکھٹا_drop, uint16_t max_اقدار);

صحیح
sbus_init(ثابت حرف *device, منطقی singlewire)
{
	صحیح sbus_fd = open(device, O_RDWR | O_NONBLOCK);

	صحیح ret = sbus_config(sbus_fd, singlewire);

	اگر (!ret) {
		واپس sbus_fd;

	} ورنہ {
		واپس -1;
	}
}

صحیح
sbus_config(صحیح sbus_fd, منطقی singlewire)
{
	صحیح ret = -1;

#اگر defined(__PX4_LINUX)

	ساخت termios2 tio = {};

	اگر (0 != ioctl(sbus_fd, TCGETS2, &tio)) {
		واپس ret;
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

	اگر (0 != ioctl(sbus_fd, TCSETS2, &tio)) {
		واپس ret;
	}

	ret = 0;
#ورنہ

	اگر (sbus_fd >= 0) {
		ساخت termios t;

		/* 100000bps, even parity, two stop bits */
		tcgetattr(sbus_fd, &t);
		cfsetspeed(&t, 100000);
		t.c_cflag |= (CSTOPB | PARENB);
		tcsetattr(sbus_fd, TCSANOW, &t);

		اگر (singlewire) {
			/* only defined in configs capable of IOCTL
			 * Note It is never turned off
			 */
#اگر_تعریف_شدہ TIOCSSINGLEWIRE
			ioctl(sbus_fd, TIOCSSINGLEWIRE, SER_SINGLEWIRE_ENABLED);
#اگر_ختم
		}

		ret = 0;
	}

#اگر_ختم
	/* initialise the decoder */
	partial_چوکھٹا_گنتی = 0;
	آخری_rx_وقت = hrt_absolute_وقت();
	sbus_چوکھٹا_drops = 0;

	واپس ret;
}

خالی
sbus1_output(صحیح sbus_fd, uint16_t *اقدار_2, uint16_t عدد_اقدار)
{
	uint8_t byteindex = 1; /*Data starts one byte into the sbus frame. */
	uint8_t انحراف = 0;
	uint16_t value;
	hrt_abstime now;

	now = hrt_absolute_وقت();

	اگر ((now - آخری_txframe_وقت) > sbus1_چوکھٹا_delay) {
		آخری_txframe_وقت = now;
		uint8_t	oframe[SBUS_چوکھٹا_سائز] = { 0x0f };

		/* 16 is sbus number of servos/channels minus 2 single bit channels.
		* currently ignoring single bit channels.  */

		برائے (غیرعلامتی i = 0; (i < عدد_اقدار) && (i < 16); ++i) {
			طویل scaled = lround((اقدار_2[i] - SBUS_SCALE_انحراف) / SBUS_SCALE_ضربی_عدد);

			/* limit to 11 bits before the cast: a value below the scale offset (an output with no
			 * function is 0) is negative and would wrap to full scale */
			اگر (scaled < 0) {
				scaled = 0;

			} ورنہ اگر (scaled > 0x07ff) {
				scaled = 0x07ff;
			}

			value = static_cast<uint16_t>(scaled);

			جب (انحراف >= 8) {
				++byteindex;
				انحراف -= 8;
			}

			oframe[byteindex] |= (value << (انحراف)) & 0xff;
			oframe[byteindex + 1] |= (value >> (8 - انحراف)) & 0xff;
			oframe[byteindex + 2] |= (value >> (16 - انحراف)) & 0xff;
			انحراف += 11;
		}

		اكتب(sbus_fd, oframe, SBUS_چوکھٹا_سائز);
	}
}
خالی
sbus2_output(صحیح sbus_fd, uint16_t *اقدار_2, uint16_t عدد_اقدار)
{
	sbus1_output(sbus_fd, اقدار_2, عدد_اقدار);
}

منطقی
sbus_داخلہ(صحیح sbus_fd, uint16_t *اقدار_2, uint16_t *عدد_اقدار, منطقی *sbus_failsafe, منطقی *sbus_چوکھٹا_drop,
	   uint16_t max_channels)
{
	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */
	uint8_t buf[SBUS_چوکھٹا_سائز * 2];

	صحیح ret = اقرأ(sbus_fd, &buf[0], SBUS_چوکھٹا_سائز);

	/* if the read failed for any reason, just give up here */
	اگر (ret < 1) {
		واپس جھوٹ;
	}

	ثابت hrt_abstime now = hrt_absolute_وقت();
#اگر_تعریف_شدہ __PX4_NUTTX /* limit time-based hardening to RTOS's where we have reliable timing */

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
	اگر (now - آخری_rx_وقت > 4_ms) {
		اگر (partial_چوکھٹا_گنتی > 0) {
			partial_چوکھٹا_گنتی = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			لکھو("SBUS: RESET (TIME LIM)\n");
#اگر_ختم
		}
	}

#اگر_ختم /* __PX4_NUTTX */

	/*
	 * Try to decode something with what we got
	 */
	واپس sbus_parse(now, &buf[0], ret, اقدار_2, عدد_اقدار, sbus_failsafe,
			  sbus_چوکھٹا_drop, &sbus_چوکھٹا_drops, max_channels);
}

منطقی
sbus_parse(uint64_t now, uint8_t *چوکھٹا, غیرعلامتی len, uint16_t *اقدار_2,
	   uint16_t *عدد_اقدار, منطقی *sbus_failsafe, منطقی *sbus_چوکھٹا_drop, غیرعلامتی *چوکھٹا_drops, uint16_t max_channels)
{

	آخری_rx_وقت = now;

	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	منطقی decode_ret = جھوٹ;

	/* keep decoding until we have consumed the buffer */
	برائے (غیرعلامتی d = 0; d < len; d++) {

		/* overflow check */
		اگر (partial_چوکھٹا_گنتی == حجم(sbus_چوکھٹا) / حجم(sbus_چوکھٹا[0])) {
			partial_چوکھٹا_گنتی = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			لکھو("SBUS2: RESET (BUF LIM)\n");
#اگر_ختم
		}

		اگر (partial_چوکھٹا_گنتی == SBUS_چوکھٹا_سائز) {
			partial_چوکھٹا_گنتی = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			لکھو("SBUS2: RESET (PACKET LIM)\n");
#اگر_ختم
		}

#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 1
		لکھو("sbus state: %s%s%s%s%s%s, count: %d, val: %02x\n",
		       (sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) ? "SBUS2_DECODE_STATE_DESYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS_آغاز) ? "SBUS2_DECODE_STATE_SBUS_START" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS1_SYNC) ? "SBUS2_DECODE_STATE_SBUS1_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_SYNC) ? "SBUS2_DECODE_STATE_SBUS2_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE) ? "SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_GPS) ? "SBUS2_DECODE_STATE_SBUS2_GPS" : "",
		       partial_چوکھٹا_گنتی,
		       (غیرعلامتی)چوکھٹا[d]);
#اگر_ختم

		انتخاب (sbus_decode_state) {
		حالت SBUS2_DECODE_STATE_DESYNC:

		/* fall through */
		حالت SBUS2_DECODE_STATE_SBUS_آغاز:

			/* we are de-synced and only interested in the frame marker */
			اگر (چوکھٹا[d] == SBUS_آغاز_علامت) {
				sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				partial_چوکھٹا_گنتی = 0;
				sbus_چوکھٹا[partial_چوکھٹا_گنتی++] = چوکھٹا[d];
			}

			توڑو;

		حالت SBUS2_DECODE_STATE_SBUS1_SYNC:

		/* fall through */
		حالت SBUS2_DECODE_STATE_SBUS2_SYNC: {
				sbus_چوکھٹا[partial_چوکھٹا_گنتی++] = چوکھٹا[d];

				/* decode whatever we got and expect */
				اگر (partial_چوکھٹا_گنتی < SBUS_چوکھٹا_سائز) {
					توڑو;
				}

				/*
				 * Great, it looks like we might have a frame. Go ahead and decode it.
				 */
				decode_ret = sbus_decode(now, sbus_چوکھٹا, اقدار_2, عدد_اقدار, sbus_failsafe, sbus_چوکھٹا_drop, max_channels);

				/*
				 * Offset recovery: If decoding failed, check if there is a second
				 * start marker in the packet.
				 */
				غیرعلامتی آغاز_اشاریہ = 0;

				اگر (!decode_ret && sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) {

					برائے (غیرعلامتی i = 1; i < partial_چوکھٹا_گنتی; i++) {
						اگر (sbus_چوکھٹا[i] == SBUS_آغاز_علامت) {
							آغاز_اشاریہ = i;
							توڑو;
						}
					}

					/* we found a second start marker */
					اگر (آغاز_اشاریہ != 0) {
						/* shift everything in the buffer and reset the state machine */
						برائے (غیرعلامتی i = 0; i < partial_چوکھٹا_گنتی - آغاز_اشاریہ; i++) {
							sbus_چوکھٹا[i] = sbus_چوکھٹا[i + آغاز_اشاریہ];
						}

						partial_چوکھٹا_گنتی -= آغاز_اشاریہ;
						sbus_decode_state = SBUS2_DECODE_STATE_SBUS_آغاز;

#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						لکھو("DECODE RECOVERY: %d\n", آغاز_اشاریہ);
#اگر_ختم
					}
				}

				/* if there has been no successful attempt at saving a failed
				 * decoding run, reset the frame count for successful and
				 * unsuccessful decode runs.
				 */
				اگر (آغاز_اشاریہ == 0) {
					partial_چوکھٹا_گنتی = 0;
				}

				اگر (decode_ret) {
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS_آغاز;
				}

			}
			توڑو;

		حالت SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE: {
				sbus_چوکھٹا[partial_چوکھٹا_گنتی++] = چوکھٹا[d];

				اگر (partial_چوکھٹا_گنتی == 1 && sbus_چوکھٹا[0] == SBUS_آغاز_علامت) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				اگر (partial_چوکھٹا_گنتی < SBUS2_چوکھٹا_سائز_RX_VOLTAGE) {
					توڑو;
				}

				/* find out which payload we're dealing with in this slot */
				انتخاب (sbus_چوکھٹا[0]) {
				حالت 0x03: {
						// Observed values:
						// (frame[0] == 0x3 && frame[1] == 0x84 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0xc4 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0x80 && frame[2] == 0x2f)
						// (frame[0] == 0x3 && frame[1] == 0xc0 && frame[2] == 0x2f)
#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 2
						uint16_t rx_voltage = (sbus_چوکھٹا[1] << 8) | sbus_چوکھٹا[2];
						لکھو("rx_voltage %d\n", (صحیح)rx_voltage);
#اگر_ختم
					}

					partial_چوکھٹا_گنتی = 0;
					توڑو;

				طےشدہ:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_چوکھٹا_drops++;
				}

			}
			توڑو;

		حالت SBUS2_DECODE_STATE_SBUS2_GPS: {
				sbus_چوکھٹا[partial_چوکھٹا_گنتی++] = چوکھٹا[d];

				اگر (partial_چوکھٹا_گنتی == 1 && sbus_چوکھٹا[0] == SBUS_آغاز_علامت) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				اگر (partial_چوکھٹا_گنتی < 24) {
					توڑو;
				}

				/* find out which payload we're dealing with in this slot */
				انتخاب (sbus_چوکھٹا[0]) {
				حالت 0x13: {
#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						uint16_t gps_something = (چوکھٹا[1] << 8) | چوکھٹا[2];
						لکھو("gps_something %d\n", (صحیح)gps_something);
#اگر_ختم
					}

					partial_چوکھٹا_گنتی = 0;
					توڑو;

				طےشدہ:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_چوکھٹا_drops++;
					/* throw unknown bytes away */
				}
			}
			توڑو;

		طےشدہ:
#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			لکھو("UNKNOWN PROTO STATE");
#اگر_ختم
			decode_ret = جھوٹ;
		}


	}

	اگر (چوکھٹا_drops) {
		*چوکھٹا_drops = sbus_چوکھٹا_drops;
	}

	/* return false as default */
	واپس decode_ret;
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
ساخت sbus_ثنائی_ہندسہ_pick {
	uint8_t byte;
	uint8_t rshift;
	uint8_t نقاب;
	uint8_t lshift;
};
ساکن ثابت ساخت sbus_ثنائی_ہندسہ_pick sbus_decoder[SBUS_داخلہ_CHANNELS][3] = {
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

منطقی
sbus_decode(uint64_t چوکھٹا_وقت, uint8_t *چوکھٹا, uint16_t *اقدار_2, uint16_t *عدد_اقدار,
	    منطقی *sbus_failsafe, منطقی *sbus_چوکھٹا_drop, uint16_t max_اقدار)
{

	/* check frame boundary markers to avoid out-of-sync cases */
	اگر (چوکھٹا[0] != SBUS_آغاز_علامت) {
		sbus_چوکھٹا_drops++;
#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		لکھو("DECODE FAIL: ");

		برائے (غیرعلامتی i = 0; i < SBUS_چوکھٹا_سائز; i++) {
			لکھو("%0x ", چوکھٹا[i]);
		}

		لکھو("\n");
#اگر_ختم
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		واپس جھوٹ;
	}

	/* the last byte in the frame indicates what frame will follow after this one */
	انتخاب (چوکھٹا[24]) {
	حالت 0x00:
		/* this is S.BUS 1 */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS1_SYNC;
		توڑو;

	حالت 0x04:
		/* receiver voltage */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE;
		توڑو;

	حالت 0x14:
		/* GPS / baro */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_GPS;
		توڑو;

	حالت 0x24:
		/* Unknown SBUS2 data */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
		توڑو;

	حالت 0x34:
		/* Unknown SBUS2 data */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
		توڑو;

	طےشدہ:
#اگر defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		لکھو("DECODE FAIL: END MARKER\n");
#اگر_ختم
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		واپس جھوٹ;
	}

	/* we have received something we think is a frame */
	غیرعلامتی chancount = (max_اقدار > SBUS_داخلہ_CHANNELS) ?
			     SBUS_داخلہ_CHANNELS : max_اقدار;

	/* use the decoder matrix to extract channel data */
	برائے (غیرعلامتی channel = 0; channel < chancount; channel++) {
		غیرعلامتی value = 0;

		برائے (غیرعلامتی pick = 0; pick < 3; pick++) {
			ثابت ساخت sbus_ثنائی_ہندسہ_pick *decode = &sbus_decoder[channel][pick];

			اگر (decode->نقاب != 0) {
				غیرعلامتی piece = چوکھٹا[1 + decode->byte];
				piece >>= decode->rshift;
				piece &= decode->نقاب;
				piece <<= decode->lshift;

				value |= piece;
			}
		}


		/* convert 0-2048 values to 1000-2000 ppm encoding in a not too sloppy fashion */
		اقدار_2[channel] = static_cast<uint16_t>(lround(value * SBUS_SCALE_ضربی_عدد) + SBUS_SCALE_انحراف);
	}

	/* decode switch channels if data fields are wide enough */
	اگر (max_اقدار > 17 && chancount > 15) {
		chancount = 18;

		/* channel 17 (index 16) */
		اقدار_2[16] = ((چوکھٹا[SBUS_FLAGS_BYTE] & (1 << 0)) ? 1000 : 0) + 998;
		/* channel 18 (index 17) */
		اقدار_2[17] = ((چوکھٹا[SBUS_FLAGS_BYTE] & (1 << 1)) ? 1000 : 0) + 998;
	}

	/* note the number of channels decoded */
	*عدد_اقدار = chancount;

	/* decode and handle failsafe and frame-lost flags */
	اگر (چوکھٹا[SBUS_FLAGS_BYTE] & (1 << SBUS_FAILSAFE_ثنائی_ہندسہ)) { /* failsafe */
		/* report that we failed to read anything valid off the receiver */
		*sbus_failsafe = سچ;
		*sbus_چوکھٹا_drop = سچ;

	} ورنہ اگر (چوکھٹا[SBUS_FLAGS_BYTE] & (1 << SBUS_FRAMELOST_ثنائی_ہندسہ)) { /* a frame was lost */
		/* set a special warning flag
		 *
		 * Attention! This flag indicates a skipped frame only, not a total link loss! Handling this
		 * condition as fail-safe greatly reduces the reliability and range of the radio link,
		 * e.g. by prematurely issuing return-to-launch!!! */

		*sbus_failsafe = جھوٹ;
		*sbus_چوکھٹا_drop = سچ;

	} ورنہ {
		*sbus_failsafe = جھوٹ;
		*sbus_چوکھٹا_drop = جھوٹ;
	}

	واپس سچ;
}

/*
  set output rate of SBUS in Hz
 */
خالی sbus1_سیٹ_output_شرح_hz(uint16_t شرح_hz)
{
	sbus1_چوکھٹا_delay = (1000U * 1000U) / math::constrain(شرح_hz, (uint16_t)SBUS1_MIN_شرح_HZ,
			    (uint16_t)SBUS1_MAX_شرح_HZ);
}
