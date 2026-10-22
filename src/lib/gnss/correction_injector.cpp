/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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


#ਅਨ੍ਤਰ੍ਭਾਵ "correction_injector.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/time.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

ਨਾਮਾਕਾਸ਼ gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ਸਥਿਰ constexpr hrt_abstime k_ਸਰੋਤ_Timeout = 5_s;
// The injection rate is frames per second over windows this long
ਸਥਿਰ constexpr hrt_abstime k_ਦਰ_ਖਿੜਕੀ = 5_s;
// Start bit, eight data bits, stop bit
ਸਥਿਰ constexpr uint32_t k_ਬਿੱਟ_ਪ੍ਰਤੀ_Byte_On_The_Wire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ਸਥਿਰ constexpr uint32_t kMinRetryDelayUs = 1_ms;
ਸਥਿਰ constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਪੋਰਟ) :
	Scheduled_Work_ਵਸਤੂ(name, px4::serial_ਪੋਰਟ_ਮੰਜ਼ਿਲ_wq(ਪੋਰਟ))
{
	strncpy(ਪੋਰਟ_2, ਪੋਰਟ, ਆਕਾਰ(ਪੋਰਟ_2) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	ਲਈ (ਸਵੈਚਾਲਿਤ &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	ਸਮਾਂ_ਸਾਰਣੀ_Clear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_ਬਫਰ_ਭਰਿਆ);
	perf_free(framer_ਭਰਿਆ);
	perf_free(_unsupported);
	perf_free(_short_write);
}

ਖਾਲੀ CorrectionInjector::ਸ਼ੁਰੂ(ਅਚਲ Config &config)
{
	ਦਰ_ਖਿੜਕੀ_ਸ਼ੁਰੂ = hrt_absolute_ਸਮਾਂ();
	ਦਰ_ਖਿੜਕੀ_ਗਿਣਤੀ = perf_event_ਗਿਣਤੀ(_injected);
	injection_ਦਰ_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	ਜੇ (config.ਧਾਰਾ == ਧਾਰਾ_2::MovingBaseline) {
		_moving_baseline_sub.ਰਜਿਸਟਰ_Callback();

	} ਨਹੀਂਤਾਂ {
		ਲਈ (ਸਵੈਚਾਲਿਤ &sub : _corrections_sub) {
			sub.ਰਜਿਸਟਰ_Callback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	ਸਰਗਰਮ = ਸੱਚ;
	_write_error_reported = ਝੂਠ;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ਸਮਾਂ_ਸਾਰਣੀ_Now();
}

ਖਾਲੀ CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	ਸਰਗਰਮ = ਝੂਠ;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	ਲਈ (ਸਵੈਚਾਲਿਤ &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ਸਮਾਂ_ਸਾਰਣੀ_Now();

	ਲਈ (ਪੂਰਨ i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		ਅਚਲ ਤਾਰਕਿਕ closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		ਜੇ (closed) {
			ਰੋਕੋ;
		}

		px4_usleep(5_ms);
	}
}

ਦਸ਼ਮਲਵ CorrectionInjector::injection_ਦਰ_hz_2()
{
	ਅਚਲ hrt_abstime now = hrt_absolute_ਸਮਾਂ();

	ਜੇ (now - ਦਰ_ਖਿੜਕੀ_ਸ਼ੁਰੂ >= k_ਦਰ_ਖਿੜਕੀ) {
		ਅਚਲ uint64_t ਗਿਣਤੀ_2 = perf_event_ਗਿਣਤੀ(_injected);
		injection_ਦਰ_hz = (ਦਸ਼ਮਲਵ)(ਗਿਣਤੀ_2 - ਦਰ_ਖਿੜਕੀ_ਗਿਣਤੀ) * 1e6f / (ਦਸ਼ਮਲਵ)(now - ਦਰ_ਖਿੜਕੀ_ਸ਼ੁਰੂ);
		ਦਰ_ਖਿੜਕੀ_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
		ਦਰ_ਖਿੜਕੀ_ਸ਼ੁਰੂ = now;
	}

	ਵਾਪਸ injection_ਦਰ_hz;
}

ਖਾਲੀ CorrectionInjector::ਲੜੀਵਾਰ()
{
	pthread_mutex_lock(&_mutex);

	ਜੇ (!ਸਰਗਰਮ) {
		ਜੇ (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} ਨਹੀਂਤਾਂ {
		ਜੇ (_fd < 0) {
			_fd = ::open(ਪੋਰਟ_2, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			ਜੇ (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", ਪੋਰਟ_2, errno);
				_write_error_reported = ਸੱਚ;
			}
		}

		ਜੇ (_fd >= 0) {
			ਜੇ (_config.ਧਾਰਾ == ਧਾਰਾ_2::MovingBaseline) {
				drain_moving_baseline();

			} ਨਹੀਂਤਾਂ {
				drain_corrections();
			}

			inject_ਚੌਖਟੇ();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

ਖਾਲੀ CorrectionInjector::drain_corrections()
{
	rtcm_ਅੰਕੜੇ_s chunk;

	ਜੇ (select_corrections_instance(chunk)) {
		ਜੋੜੋ_chunk(chunk);
	}

	ਅਚਲ int8_t selected = _selected_instance.ਲੱਦੋ();

	ਜੇ (selected < 0) {
		ਵਾਪਸ;
	}

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0; n < rtcm_ਅੰਕੜੇ_s::ORB_ਕਤਾਰ_ਲੰਮਾਈ && _corrections_sub[selected].update(&chunk); n++) {
		ਜੇ (chunk.device_id != _config.own_device_id) {
			ਜੋੜੋ_chunk(chunk);
		}
	}
}

ਖਾਲੀ CorrectionInjector::drain_moving_baseline()
{
	rtcm_ਅੰਕੜੇ_s chunk;

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0; n < rtcm_ਅੰਕੜੇ_s::ORB_ਕਤਾਰ_ਲੰਮਾਈ && _moving_baseline_sub.update(&chunk); n++) {
		ਜੇ (chunk.device_id != _config.own_device_id) {
			ਜੋੜੋ_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
ਤਾਰਕਿਕ CorrectionInjector::select_corrections_instance(rtcm_ਅੰਕੜੇ_s &chunk)
{
	ਜੇ (_selected_instance.ਲੱਦੋ() >= 0 && hrt_elapsed_ਸਮਾਂ(&ਆਖਰੀ_chunk_ਸਮਾਂ) < k_ਸਰੋਤ_Timeout) {
		ਵਾਪਸ ਝੂਠ;
	}

	_selected_instance.store(-1);

	ਲਈ (int8_t instance = 0; instance < rtcm_ਅੰਕੜੇ_s::MAX_INSTANCES; instance++) {
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0; n < rtcm_ਅੰਕੜੇ_s::ORB_ਕਤਾਰ_ਲੰਮਾਈ && _corrections_sub[instance].update(&chunk); n++) {
			ਜੇ (chunk.device_id != _config.own_device_id && hrt_elapsed_ਸਮਾਂ(&chunk.timestamp) < k_ਸਰੋਤ_Timeout) {
				_selected_instance.store(instance);
				ਵਾਪਸ ਸੱਚ;
			}
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ CorrectionInjector::ਜੋੜੋ_chunk(ਅਚਲ rtcm_ਅੰਕੜੇ_s &chunk)
{
	ਅਚਲ size_t len = math::min<size_t>(chunk.len, ਆਕਾਰ(chunk.ਅੰਕੜੇ));

	ਜੇ (_framer.ਜੋੜੋ_ਅੰਕੜੇ(chunk.ਅੰਕੜੇ, len) < len) {
		perf_ਗਿਣਤੀ(framer_ਭਰਿਆ);
	}

	ਆਖਰੀ_chunk_ਸਮਾਂ = hrt_absolute_ਸਮਾਂ();
}

ਖਾਲੀ CorrectionInjector::inject_ਚੌਖਟੇ()
{
	size_t ਚੌਖਟਾ_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ਅਚਲ uint8_t *ਚੌਖਟਾ = ਖਾਲੀ_ਸੂਚਕ;

	ਜਦੋਂ ((ਚੌਖਟਾ = _framer.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&ਚੌਖਟਾ_len, &protocol)) != ਖਾਲੀ_ਸੂਚਕ) {
		ਜੇ ((_config.protocols & protocol_ਦੋਆਧਾਰੀ_ਅੰਕ(protocol)) == 0) {
			_framer.consume_ਸੁਨੇਹਾ(ਚੌਖਟਾ_len);
			perf_ਗਿਣਤੀ(_unsupported);
			ਜਾਰੀ;
		}

		ਅਚਲ ssize_t tx_space = tx_space_ਉਪਲਬਧ();

		// Unknown on POSIX, where the write itself is the check
		ਜੇ (tx_space >= 0 && tx_space < (ssize_t)ਚੌਖਟਾ_len) {
			perf_ਗਿਣਤੀ(tx_ਬਫਰ_ਭਰਿਆ);
			ਸਮਾਂ_ਸਾਰਣੀ_Delayed(drain_ਸਮਾਂ_us(ਚੌਖਟਾ_len - tx_space));
			ਰੋਕੋ;
		}

		ਅਚਲ ssize_t written = ::ਲਿਖ(_fd, ਚੌਖਟਾ, ਚੌਖਟਾ_len);

		ਜੇ (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_ਗਿਣਤੀ(tx_ਬਫਰ_ਭਰਿਆ);
			ਸਮਾਂ_ਸਾਰਣੀ_Delayed(drain_ਸਮਾਂ_us(ਚੌਖਟਾ_len));
			ਰੋਕੋ;
		}

		ਜੇ (written != (ssize_t)ਚੌਖਟਾ_len) {
			perf_ਗਿਣਤੀ(_short_write);

			ਜੇ (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", ਪੋਰਟ_2, errno);
				_write_error_reported = ਸੱਚ;
			}
		}

		_framer.consume_ਸੁਨੇਹਾ(ਚੌਖਟਾ_len);
		perf_ਗਿਣਤੀ(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_ਉਪਲਬਧ() ਅਚਲ
{
#ਜੇ defined(FIONSPACE)
	ਪੂਰਨ space = 0;

	ਜੇ (::ioctl(_fd, FIONSPACE, &space) == 0) {
		ਵਾਪਸ space;
	}

#ਜੇ_ਅੰਤ
	ਵਾਪਸ -1;
}

uint32_t CorrectionInjector::drain_ਸਮਾਂ_us(size_t ਬਾਈਟਾਂ) ਅਚਲ
{
	ਅਚਲ uint32_t baudrate = _config.baudrate;

	ਜੇ (baudrate == 0) {
		ਵਾਪਸ kFallbackRetryDelayUs;
	}

	ਵਾਪਸ math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)ਬਾਈਟਾਂ * k_ਬਿੱਟ_ਪ੍ਰਤੀ_Byte_On_The_Wire * 1_s / baudrate);
}

ਖਾਲੀ CorrectionInjector::print_ਸਥਿਤੀ() ਅਚਲ
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.ਧਾਰਾ == ਧਾਰਾ_2::MovingBaseline ? "moving baseline" : "corrections", (ਦੁੱਗਣਾ)injection_ਦਰ_hz,
		 (ਪੂਰਨ)_selected_instance.ਲੱਦੋ());
	perf_print_counter(_injected);
	perf_print_counter(tx_ਬਫਰ_ਭਰਿਆ);
	perf_print_counter(framer_ਭਰਿਆ);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ਅਚਲ CorrectionFramerStats stats = _framer.getStats();

	ਜੇ (stats.messages_parsed > 0 || stats.ਚੱਕਰੀ_ਜਾਂਚ_errors_2 > 0 || stats.ਬਾਈਟਾਂ_discarded_2 > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (ਬਿਨਾਨਿਸ਼ਾਨੀ)stats.rtcm3_messages, (ਬਿਨਾਨਿਸ਼ਾਨੀ)stats.spartn_messages, (ਬਿਨਾਨਿਸ਼ਾਨੀ)stats.ubx_messages,
			 (ਬਿਨਾਨਿਸ਼ਾਨੀ)stats.ਚੱਕਰੀ_ਜਾਂਚ_errors_2, (ਬਿਨਾਨਿਸ਼ਾਨੀ)stats.ਬਾਈਟਾਂ_discarded_2);
	}
}

} // namespace gnss
