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

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TIOCSSINGLEWIRE
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "sbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "common_rc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_DEBUG_LEVEL 	0 /* Set debug output level */

#ਜੇ defined(__PX4_LINUX)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <asm-generic/termbits.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <termios.h>
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_ਸ਼ੁਰੂ_ਚਿੰਨ੍ਹ	0x0f

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_ਨਿਵੇਸ਼_CHANNELS	16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_FLAGS_BYTE		23
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_FAILSAFE_ਦੋਆਧਾਰੀ_ਅੰਕ	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_FRAMELOST_ਦੋਆਧਾਰੀ_ਅੰਕ	2

// testing with a SBUS->PWM adapter shows that
// above 300Hz SBUS becomes unreliable. 333 would
// be the theoretical achievable, but at 333Hz some
// frames are lost
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS1_MAX_ਦਰ_HZ	300
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS1_MIN_ਦਰ_HZ	50

// this is the rate of the old code
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS1_DEFAULT_ਦਰ_HZ	72

/*
  Measured values with Futaba FX-30/R6108SB:
    -+100% on TX:  PCM 1.100/1.520/1.950ms -> SBus raw values: 350/1024/1700  (100% ATV)
    -+140% on TX:  PCM 0.930/1.520/2.112ms -> SBus raw values:  78/1024/1964  (140% ATV)
    -+152% on TX:  PCM 0.884/1.520/2.160ms -> SBus raw values:   1/1024/2047  (140% ATV plus dirty tricks)
*/

/* define range mapping here, -+100% -> 1000..2000 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_RANGE_MIN 200.0f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_RANGE_MAX 1800.0f

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_ਨਿਸ਼ਾਨਾ_MIN 1000.0f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_ਨਿਸ਼ਾਨਾ_MAX 2000.0f

#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
#  include <stdio.h>
#ਜੇ_ਅੰਤ

/* pre-calculate the floating point stuff as far as possible at compile time */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_SCALE_ਗੁਣਕ ((SBUS_ਨਿਸ਼ਾਨਾ_MAX - SBUS_ਨਿਸ਼ਾਨਾ_MIN) / (SBUS_RANGE_MAX - SBUS_RANGE_MIN))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS_SCALE_ਵਿੱਥ (ਪੂਰਨ)(SBUS_ਨਿਸ਼ਾਨਾ_MIN - (SBUS_SCALE_ਗੁਣਕ * SBUS_RANGE_MIN + 0.5f))

ਸਥਿਰ hrt_abstime ਆਖਰੀ_rx_ਸਮਾਂ;
ਸਥਿਰ hrt_abstime ਆਖਰੀ_txframe_ਸਮਾਂ = 0;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS2_ਚੌਖਟਾ_ਮਾਪ_RX_VOLTAGE	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBUS2_ਚੌਖਟਾ_ਮਾਪ_GPS_DIGIT	3

ਸਥਿਰ ਗਿਣਤੀ SBUS2_DECODE_STATE {
	SBUS2_DECODE_STATE_DESYNC = 0xFFF,
	SBUS2_DECODE_STATE_SBUS_ਸ਼ੁਰੂ = 0x2FF,
	SBUS2_DECODE_STATE_SBUS1_SYNC = 0x00,
	SBUS2_DECODE_STATE_SBUS2_SYNC = 0x1FF,
	SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE = 0x04,
	SBUS2_DECODE_STATE_SBUS2_GPS = 0x14,
	SBUS2_DECODE_STATE_SBUS2_DATA1 = 0x24,
	SBUS2_DECODE_STATE_SBUS2_DATA2 = 0x34
} sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;

ਸਥਿਰ sbus_ਚੌਖਟਾ_t	 &sbus_ਚੌਖਟਾ = rc_decode_buf.sbus_ਚੌਖਟਾ;

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ partial_ਚੌਖਟਾ_ਗਿਣਤੀ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ sbus1_ਚੌਖਟਾ_delay = (1000U * 1000U) / SBUS1_DEFAULT_ਦਰ_HZ;

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ sbus_ਚੌਖਟਾ_drops;

ਬਿਨਾਨਿਸ਼ਾਨੀ
sbus_dropped_ਚੌਖਟੇ()
{
	ਵਾਪਸ sbus_ਚੌਖਟਾ_drops;
}

ਸਥਿਰ ਤਾਰਕਿਕ
sbus_decode(uint64_t ਚੌਖਟਾ_ਸਮਾਂ, uint8_t *ਚੌਖਟਾ, uint16_t *ਮੁੱਲਾਂ_2, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ,
	    ਤਾਰਕਿਕ *sbus_failsafe, ਤਾਰਕਿਕ *sbus_ਚੌਖਟਾ_drop, uint16_t max_ਮੁੱਲਾਂ);

ਪੂਰਨ
sbus_init(ਅਚਲ ਅੱਖਰ *device, ਤਾਰਕਿਕ singlewire)
{
	ਪੂਰਨ sbus_fd = open(device, O_RDWR | O_NONBLOCK);

	ਪੂਰਨ ret = sbus_config(sbus_fd, singlewire);

	ਜੇ (!ret) {
		ਵਾਪਸ sbus_fd;

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -1;
	}
}

ਪੂਰਨ
sbus_config(ਪੂਰਨ sbus_fd, ਤਾਰਕਿਕ singlewire)
{
	ਪੂਰਨ ret = -1;

#ਜੇ defined(__PX4_LINUX)

	ਬਣਤਰ termios2 tio = {};

	ਜੇ (0 != ioctl(sbus_fd, TCGETS2, &tio)) {
		ਵਾਪਸ ret;
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

	ਜੇ (0 != ioctl(sbus_fd, TCSETS2, &tio)) {
		ਵਾਪਸ ret;
	}

	ret = 0;
#ਨਹੀਂਤਾਂ

	ਜੇ (sbus_fd >= 0) {
		ਬਣਤਰ termios t;

		/* 100000bps, even parity, two stop bits */
		tcgetattr(sbus_fd, &t);
		cfsetspeed(&t, 100000);
		t.c_cflag |= (CSTOPB | PARENB);
		tcsetattr(sbus_fd, TCSANOW, &t);

		ਜੇ (singlewire) {
			/* only defined in configs capable of IOCTL
			 * Note It is never turned off
			 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TIOCSSINGLEWIRE
			ioctl(sbus_fd, TIOCSSINGLEWIRE, SER_SINGLEWIRE_ENABLED);
#ਜੇ_ਅੰਤ
		}

		ret = 0;
	}

#ਜੇ_ਅੰਤ
	/* initialise the decoder */
	partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
	ਆਖਰੀ_rx_ਸਮਾਂ = hrt_absolute_ਸਮਾਂ();
	sbus_ਚੌਖਟਾ_drops = 0;

	ਵਾਪਸ ret;
}

ਖਾਲੀ
sbus1_output(ਪੂਰਨ sbus_fd, uint16_t *ਮੁੱਲਾਂ_2, uint16_t ਅੰਕ_ਮੁੱਲਾਂ)
{
	uint8_t byteindex = 1; /*Data starts one byte into the sbus frame. */
	uint8_t ਵਿੱਥ = 0;
	uint16_t value;
	hrt_abstime now;

	now = hrt_absolute_ਸਮਾਂ();

	ਜੇ ((now - ਆਖਰੀ_txframe_ਸਮਾਂ) > sbus1_ਚੌਖਟਾ_delay) {
		ਆਖਰੀ_txframe_ਸਮਾਂ = now;
		uint8_t	oframe[SBUS_ਚੌਖਟਾ_ਮਾਪ] = { 0x0f };

		/* 16 is sbus number of servos/channels minus 2 single bit channels.
		* currently ignoring single bit channels.  */

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; (i < ਅੰਕ_ਮੁੱਲਾਂ) && (i < 16); ++i) {
			ਲੰਮਾ scaled = lround((ਮੁੱਲਾਂ_2[i] - SBUS_SCALE_ਵਿੱਥ) / SBUS_SCALE_ਗੁਣਕ);

			/* limit to 11 bits before the cast: a value below the scale offset (an output with no
			 * function is 0) is negative and would wrap to full scale */
			ਜੇ (scaled < 0) {
				scaled = 0;

			} ਨਹੀਂਤਾਂ ਜੇ (scaled > 0x07ff) {
				scaled = 0x07ff;
			}

			value = static_cast<uint16_t>(scaled);

			ਜਦੋਂ (ਵਿੱਥ >= 8) {
				++byteindex;
				ਵਿੱਥ -= 8;
			}

			oframe[byteindex] |= (value << (ਵਿੱਥ)) & 0xff;
			oframe[byteindex + 1] |= (value >> (8 - ਵਿੱਥ)) & 0xff;
			oframe[byteindex + 2] |= (value >> (16 - ਵਿੱਥ)) & 0xff;
			ਵਿੱਥ += 11;
		}

		ਲਿਖ(sbus_fd, oframe, SBUS_ਚੌਖਟਾ_ਮਾਪ);
	}
}
ਖਾਲੀ
sbus2_output(ਪੂਰਨ sbus_fd, uint16_t *ਮੁੱਲਾਂ_2, uint16_t ਅੰਕ_ਮੁੱਲਾਂ)
{
	sbus1_output(sbus_fd, ਮੁੱਲਾਂ_2, ਅੰਕ_ਮੁੱਲਾਂ);
}

ਤਾਰਕਿਕ
sbus_ਨਿਵੇਸ਼(ਪੂਰਨ sbus_fd, uint16_t *ਮੁੱਲਾਂ_2, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, ਤਾਰਕਿਕ *sbus_failsafe, ਤਾਰਕਿਕ *sbus_ਚੌਖਟਾ_drop,
	   uint16_t max_channels)
{
	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */
	uint8_t buf[SBUS_ਚੌਖਟਾ_ਮਾਪ * 2];

	ਪੂਰਨ ret = ਪਠ(sbus_fd, &buf[0], SBUS_ਚੌਖਟਾ_ਮਾਪ);

	/* if the read failed for any reason, just give up here */
	ਜੇ (ret < 1) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਅਚਲ hrt_abstime now = hrt_absolute_ਸਮਾਂ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX /* limit time-based hardening to RTOS's where we have reliable timing */

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
	ਜੇ (now - ਆਖਰੀ_rx_ਸਮਾਂ > 4_ms) {
		ਜੇ (partial_ਚੌਖਟਾ_ਗਿਣਤੀ > 0) {
			partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			ਲਿਖੋ("SBUS: RESET (TIME LIM)\n");
#ਜੇ_ਅੰਤ
		}
	}

#ਜੇ_ਅੰਤ /* __PX4_NUTTX */

	/*
	 * Try to decode something with what we got
	 */
	ਵਾਪਸ sbus_parse(now, &buf[0], ret, ਮੁੱਲਾਂ_2, ਅੰਕ_ਮੁੱਲਾਂ, sbus_failsafe,
			  sbus_ਚੌਖਟਾ_drop, &sbus_ਚੌਖਟਾ_drops, max_channels);
}

ਤਾਰਕਿਕ
sbus_parse(uint64_t now, uint8_t *ਚੌਖਟਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ len, uint16_t *ਮੁੱਲਾਂ_2,
	   uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, ਤਾਰਕਿਕ *sbus_failsafe, ਤਾਰਕਿਕ *sbus_ਚੌਖਟਾ_drop, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਚੌਖਟਾ_drops, uint16_t max_channels)
{

	ਆਖਰੀ_rx_ਸਮਾਂ = now;

	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	ਤਾਰਕਿਕ decode_ret = ਝੂਠ;

	/* keep decoding until we have consumed the buffer */
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ d = 0; d < len; d++) {

		/* overflow check */
		ਜੇ (partial_ਚੌਖਟਾ_ਗਿਣਤੀ == ਆਕਾਰ(sbus_ਚੌਖਟਾ) / ਆਕਾਰ(sbus_ਚੌਖਟਾ[0])) {
			partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			ਲਿਖੋ("SBUS2: RESET (BUF LIM)\n");
#ਜੇ_ਅੰਤ
		}

		ਜੇ (partial_ਚੌਖਟਾ_ਗਿਣਤੀ == SBUS_ਚੌਖਟਾ_ਮਾਪ) {
			partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
			sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			ਲਿਖੋ("SBUS2: RESET (PACKET LIM)\n");
#ਜੇ_ਅੰਤ
		}

#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 1
		ਲਿਖੋ("sbus state: %s%s%s%s%s%s, count: %d, val: %02x\n",
		       (sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) ? "SBUS2_DECODE_STATE_DESYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS_ਸ਼ੁਰੂ) ? "SBUS2_DECODE_STATE_SBUS_START" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS1_SYNC) ? "SBUS2_DECODE_STATE_SBUS1_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_SYNC) ? "SBUS2_DECODE_STATE_SBUS2_SYNC" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE) ? "SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE" : "",
		       (sbus_decode_state == SBUS2_DECODE_STATE_SBUS2_GPS) ? "SBUS2_DECODE_STATE_SBUS2_GPS" : "",
		       partial_ਚੌਖਟਾ_ਗਿਣਤੀ,
		       (ਬਿਨਾਨਿਸ਼ਾਨੀ)ਚੌਖਟਾ[d]);
#ਜੇ_ਅੰਤ

		ਚੋਣ (sbus_decode_state) {
		ਹਾਲਤ SBUS2_DECODE_STATE_DESYNC:

		/* fall through */
		ਹਾਲਤ SBUS2_DECODE_STATE_SBUS_ਸ਼ੁਰੂ:

			/* we are de-synced and only interested in the frame marker */
			ਜੇ (ਚੌਖਟਾ[d] == SBUS_ਸ਼ੁਰੂ_ਚਿੰਨ੍ਹ) {
				sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
				sbus_ਚੌਖਟਾ[partial_ਚੌਖਟਾ_ਗਿਣਤੀ++] = ਚੌਖਟਾ[d];
			}

			ਰੋਕੋ;

		ਹਾਲਤ SBUS2_DECODE_STATE_SBUS1_SYNC:

		/* fall through */
		ਹਾਲਤ SBUS2_DECODE_STATE_SBUS2_SYNC: {
				sbus_ਚੌਖਟਾ[partial_ਚੌਖਟਾ_ਗਿਣਤੀ++] = ਚੌਖਟਾ[d];

				/* decode whatever we got and expect */
				ਜੇ (partial_ਚੌਖਟਾ_ਗਿਣਤੀ < SBUS_ਚੌਖਟਾ_ਮਾਪ) {
					ਰੋਕੋ;
				}

				/*
				 * Great, it looks like we might have a frame. Go ahead and decode it.
				 */
				decode_ret = sbus_decode(now, sbus_ਚੌਖਟਾ, ਮੁੱਲਾਂ_2, ਅੰਕ_ਮੁੱਲਾਂ, sbus_failsafe, sbus_ਚੌਖਟਾ_drop, max_channels);

				/*
				 * Offset recovery: If decoding failed, check if there is a second
				 * start marker in the packet.
				 */
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;

				ਜੇ (!decode_ret && sbus_decode_state == SBUS2_DECODE_STATE_DESYNC) {

					ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 1; i < partial_ਚੌਖਟਾ_ਗਿਣਤੀ; i++) {
						ਜੇ (sbus_ਚੌਖਟਾ[i] == SBUS_ਸ਼ੁਰੂ_ਚਿੰਨ੍ਹ) {
							ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = i;
							ਰੋਕੋ;
						}
					}

					/* we found a second start marker */
					ਜੇ (ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ != 0) {
						/* shift everything in the buffer and reset the state machine */
						ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < partial_ਚੌਖਟਾ_ਗਿਣਤੀ - ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ; i++) {
							sbus_ਚੌਖਟਾ[i] = sbus_ਚੌਖਟਾ[i + ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ];
						}

						partial_ਚੌਖਟਾ_ਗਿਣਤੀ -= ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ;
						sbus_decode_state = SBUS2_DECODE_STATE_SBUS_ਸ਼ੁਰੂ;

#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						ਲਿਖੋ("DECODE RECOVERY: %d\n", ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ);
#ਜੇ_ਅੰਤ
					}
				}

				/* if there has been no successful attempt at saving a failed
				 * decoding run, reset the frame count for successful and
				 * unsuccessful decode runs.
				 */
				ਜੇ (ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ == 0) {
					partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
				}

				ਜੇ (decode_ret) {
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS_ਸ਼ੁਰੂ;
				}

			}
			ਰੋਕੋ;

		ਹਾਲਤ SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE: {
				sbus_ਚੌਖਟਾ[partial_ਚੌਖਟਾ_ਗਿਣਤੀ++] = ਚੌਖਟਾ[d];

				ਜੇ (partial_ਚੌਖਟਾ_ਗਿਣਤੀ == 1 && sbus_ਚੌਖਟਾ[0] == SBUS_ਸ਼ੁਰੂ_ਚਿੰਨ੍ਹ) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				ਜੇ (partial_ਚੌਖਟਾ_ਗਿਣਤੀ < SBUS2_ਚੌਖਟਾ_ਮਾਪ_RX_VOLTAGE) {
					ਰੋਕੋ;
				}

				/* find out which payload we're dealing with in this slot */
				ਚੋਣ (sbus_ਚੌਖਟਾ[0]) {
				ਹਾਲਤ 0x03: {
						// Observed values:
						// (frame[0] == 0x3 && frame[1] == 0x84 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0xc4 && frame[2] == 0x0)
						// (frame[0] == 0x3 && frame[1] == 0x80 && frame[2] == 0x2f)
						// (frame[0] == 0x3 && frame[1] == 0xc0 && frame[2] == 0x2f)
#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 2
						uint16_t rx_voltage = (sbus_ਚੌਖਟਾ[1] << 8) | sbus_ਚੌਖਟਾ[2];
						ਲਿਖੋ("rx_voltage %d\n", (ਪੂਰਨ)rx_voltage);
#ਜੇ_ਅੰਤ
					}

					partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
					ਰੋਕੋ;

				ਮੂਲ:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_ਚੌਖਟਾ_drops++;
				}

			}
			ਰੋਕੋ;

		ਹਾਲਤ SBUS2_DECODE_STATE_SBUS2_GPS: {
				sbus_ਚੌਖਟਾ[partial_ਚੌਖਟਾ_ਗਿਣਤੀ++] = ਚੌਖਟਾ[d];

				ਜੇ (partial_ਚੌਖਟਾ_ਗਿਣਤੀ == 1 && sbus_ਚੌਖਟਾ[0] == SBUS_ਸ਼ੁਰੂ_ਚਿੰਨ੍ਹ) {
					/* this slot is unused and in fact S.BUS2 sync */
					sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
				}

				ਜੇ (partial_ਚੌਖਟਾ_ਗਿਣਤੀ < 24) {
					ਰੋਕੋ;
				}

				/* find out which payload we're dealing with in this slot */
				ਚੋਣ (sbus_ਚੌਖਟਾ[0]) {
				ਹਾਲਤ 0x13: {
#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
						uint16_t gps_something = (ਚੌਖਟਾ[1] << 8) | ਚੌਖਟਾ[2];
						ਲਿਖੋ("gps_something %d\n", (ਪੂਰਨ)gps_something);
#ਜੇ_ਅੰਤ
					}

					partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
					ਰੋਕੋ;

				ਮੂਲ:
					/* this is not what we expect it to be, go back to sync */
					sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
					sbus_ਚੌਖਟਾ_drops++;
					/* throw unknown bytes away */
				}
			}
			ਰੋਕੋ;

		ਮੂਲ:
#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
			ਲਿਖੋ("UNKNOWN PROTO STATE");
#ਜੇ_ਅੰਤ
			decode_ret = ਝੂਠ;
		}


	}

	ਜੇ (ਚੌਖਟਾ_drops) {
		*ਚੌਖਟਾ_drops = sbus_ਚੌਖਟਾ_drops;
	}

	/* return false as default */
	ਵਾਪਸ decode_ret;
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
ਬਣਤਰ sbus_ਦੋਆਧਾਰੀ_ਅੰਕ_pick {
	uint8_t byte;
	uint8_t rshift;
	uint8_t ਨਕਾਬ;
	uint8_t lshift;
};
ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbus_ਦੋਆਧਾਰੀ_ਅੰਕ_pick sbus_decoder[SBUS_ਨਿਵੇਸ਼_CHANNELS][3] = {
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

ਤਾਰਕਿਕ
sbus_decode(uint64_t ਚੌਖਟਾ_ਸਮਾਂ, uint8_t *ਚੌਖਟਾ, uint16_t *ਮੁੱਲਾਂ_2, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ,
	    ਤਾਰਕਿਕ *sbus_failsafe, ਤਾਰਕਿਕ *sbus_ਚੌਖਟਾ_drop, uint16_t max_ਮੁੱਲਾਂ)
{

	/* check frame boundary markers to avoid out-of-sync cases */
	ਜੇ (ਚੌਖਟਾ[0] != SBUS_ਸ਼ੁਰੂ_ਚਿੰਨ੍ਹ) {
		sbus_ਚੌਖਟਾ_drops++;
#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		ਲਿਖੋ("DECODE FAIL: ");

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < SBUS_ਚੌਖਟਾ_ਮਾਪ; i++) {
			ਲਿਖੋ("%0x ", ਚੌਖਟਾ[i]);
		}

		ਲਿਖੋ("\n");
#ਜੇ_ਅੰਤ
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		ਵਾਪਸ ਝੂਠ;
	}

	/* the last byte in the frame indicates what frame will follow after this one */
	ਚੋਣ (ਚੌਖਟਾ[24]) {
	ਹਾਲਤ 0x00:
		/* this is S.BUS 1 */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS1_SYNC;
		ਰੋਕੋ;

	ਹਾਲਤ 0x04:
		/* receiver voltage */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_RX_VOLTAGE;
		ਰੋਕੋ;

	ਹਾਲਤ 0x14:
		/* GPS / baro */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_GPS;
		ਰੋਕੋ;

	ਹਾਲਤ 0x24:
		/* Unknown SBUS2 data */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
		ਰੋਕੋ;

	ਹਾਲਤ 0x34:
		/* Unknown SBUS2 data */
		sbus_decode_state = SBUS2_DECODE_STATE_SBUS2_SYNC;
		ਰੋਕੋ;

	ਮੂਲ:
#ਜੇ defined(SBUS_DEBUG_LEVEL) && SBUS_DEBUG_LEVEL > 0
		ਲਿਖੋ("DECODE FAIL: END MARKER\n");
#ਜੇ_ਅੰਤ
		sbus_decode_state = SBUS2_DECODE_STATE_DESYNC;
		ਵਾਪਸ ਝੂਠ;
	}

	/* we have received something we think is a frame */
	ਬਿਨਾਨਿਸ਼ਾਨੀ chancount = (max_ਮੁੱਲਾਂ > SBUS_ਨਿਵੇਸ਼_CHANNELS) ?
			     SBUS_ਨਿਵੇਸ਼_CHANNELS : max_ਮੁੱਲਾਂ;

	/* use the decoder matrix to extract channel data */
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ channel = 0; channel < chancount; channel++) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ value = 0;

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ pick = 0; pick < 3; pick++) {
			ਅਚਲ ਬਣਤਰ sbus_ਦੋਆਧਾਰੀ_ਅੰਕ_pick *decode = &sbus_decoder[channel][pick];

			ਜੇ (decode->ਨਕਾਬ != 0) {
				ਬਿਨਾਨਿਸ਼ਾਨੀ piece = ਚੌਖਟਾ[1 + decode->byte];
				piece >>= decode->rshift;
				piece &= decode->ਨਕਾਬ;
				piece <<= decode->lshift;

				value |= piece;
			}
		}


		/* convert 0-2048 values to 1000-2000 ppm encoding in a not too sloppy fashion */
		ਮੁੱਲਾਂ_2[channel] = static_cast<uint16_t>(lround(value * SBUS_SCALE_ਗੁਣਕ) + SBUS_SCALE_ਵਿੱਥ);
	}

	/* decode switch channels if data fields are wide enough */
	ਜੇ (max_ਮੁੱਲਾਂ > 17 && chancount > 15) {
		chancount = 18;

		/* channel 17 (index 16) */
		ਮੁੱਲਾਂ_2[16] = ((ਚੌਖਟਾ[SBUS_FLAGS_BYTE] & (1 << 0)) ? 1000 : 0) + 998;
		/* channel 18 (index 17) */
		ਮੁੱਲਾਂ_2[17] = ((ਚੌਖਟਾ[SBUS_FLAGS_BYTE] & (1 << 1)) ? 1000 : 0) + 998;
	}

	/* note the number of channels decoded */
	*ਅੰਕ_ਮੁੱਲਾਂ = chancount;

	/* decode and handle failsafe and frame-lost flags */
	ਜੇ (ਚੌਖਟਾ[SBUS_FLAGS_BYTE] & (1 << SBUS_FAILSAFE_ਦੋਆਧਾਰੀ_ਅੰਕ)) { /* failsafe */
		/* report that we failed to read anything valid off the receiver */
		*sbus_failsafe = ਸੱਚ;
		*sbus_ਚੌਖਟਾ_drop = ਸੱਚ;

	} ਨਹੀਂਤਾਂ ਜੇ (ਚੌਖਟਾ[SBUS_FLAGS_BYTE] & (1 << SBUS_FRAMELOST_ਦੋਆਧਾਰੀ_ਅੰਕ)) { /* a frame was lost */
		/* set a special warning flag
		 *
		 * Attention! This flag indicates a skipped frame only, not a total link loss! Handling this
		 * condition as fail-safe greatly reduces the reliability and range of the radio link,
		 * e.g. by prematurely issuing return-to-launch!!! */

		*sbus_failsafe = ਝੂਠ;
		*sbus_ਚੌਖਟਾ_drop = ਸੱਚ;

	} ਨਹੀਂਤਾਂ {
		*sbus_failsafe = ਝੂਠ;
		*sbus_ਚੌਖਟਾ_drop = ਝੂਠ;
	}

	ਵਾਪਸ ਸੱਚ;
}

/*
  set output rate of SBUS in Hz
 */
ਖਾਲੀ sbus1_ਸੈੱਟ_output_ਦਰ_hz(uint16_t ਦਰ_hz)
{
	sbus1_ਚੌਖਟਾ_delay = (1000U * 1000U) / math::constrain(ਦਰ_hz, (uint16_t)SBUS1_MIN_ਦਰ_HZ,
			    (uint16_t)SBUS1_MAX_ਦਰ_HZ);
}
