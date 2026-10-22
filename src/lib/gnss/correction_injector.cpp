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


#ಅನ್ತರ್ಭಾವ "correction_injector.h"

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/posix.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/time.h>

ಬಳಸಿ ನಾಮಾಕಾಶ ಸಮಯ_literals;

ನಾಮಾಕಾಶ gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ಸ್ಥಿರ constexpr hrt_abstime k_ಮೂಲ_Timeout = 5_s;
// The injection rate is frames per second over windows this long
ಸ್ಥಿರ constexpr hrt_abstime k_ದರ_ಕಿಟಕಿ = 5_s;
// Start bit, eight data bits, stop bit
ಸ್ಥಿರ constexpr uint32_t k_ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_Byte_On_The_Wire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ಸ್ಥಿರ constexpr uint32_t kMinRetryDelayUs = 1_ms;
ಸ್ಥಿರ constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೋರ್ಟ್) :
	Scheduled_Work_ಅಂಶ(name, px4::serial_ಪೋರ್ಟ್_ಗಮ್ಯ_wq(ಪೋರ್ಟ್))
{
	strncpy(ಪೋರ್ಟ್_2, ಪೋರ್ಟ್, ಗಾತ್ರ(ಪೋರ್ಟ್_2) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	ವೇಳಾಪಟ್ಟಿ_Clear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_ಬಫರ್_ತುಂಬಿದ);
	perf_free(framer_ತುಂಬಿದ);
	perf_free(_unsupported);
	perf_free(_short_write);
}

ಶೂನ್ಯ CorrectionInjector::ಆರಂಭ(ಸ್ಥಿರಾಂಕ Config &config)
{
	ದರ_ಕಿಟಕಿ_ಆರಂಭ = hrt_absolute_ಸಮಯ();
	ದರ_ಕಿಟಕಿ_ಎಣಿಕೆ = perf_event_ಎಣಿಕೆ(_injected);
	injection_ದರ_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	ಆದರೆ (config.ಹರಿವು == ಹರಿವು_2::MovingBaseline) {
		_moving_baseline_sub.ರಿಜಿಸ್ಟರ್_Callback();

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sub : _corrections_sub) {
			sub.ರಿಜಿಸ್ಟರ್_Callback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	ಸಕ್ರಿಯ = ನಿಜ;
	_write_error_reported = ಸುಳ್ಳು;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ವೇಳಾಪಟ್ಟಿ_Now();
}

ಶೂನ್ಯ CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	ಸಕ್ರಿಯ = ಸುಳ್ಳು;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ವೇಳಾಪಟ್ಟಿ_Now();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		ಆದರೆ (closed) {
			ನಿಲ್ಲಿಸು;
		}

		px4_usleep(5_ms);
	}
}

ದಶಮಾಂಶ CorrectionInjector::injection_ದರ_hz_2()
{
	ಸ್ಥಿರಾಂಕ hrt_abstime now = hrt_absolute_ಸಮಯ();

	ಆದರೆ (now - ದರ_ಕಿಟಕಿ_ಆರಂಭ >= k_ದರ_ಕಿಟಕಿ) {
		ಸ್ಥಿರಾಂಕ uint64_t ಎಣಿಕೆ_2 = perf_event_ಎಣಿಕೆ(_injected);
		injection_ದರ_hz = (ದಶಮಾಂಶ)(ಎಣಿಕೆ_2 - ದರ_ಕಿಟಕಿ_ಎಣಿಕೆ) * 1e6f / (ದಶಮಾಂಶ)(now - ದರ_ಕಿಟಕಿ_ಆರಂಭ);
		ದರ_ಕಿಟಕಿ_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
		ದರ_ಕಿಟಕಿ_ಆರಂಭ = now;
	}

	ಹಿಂತಿರುಗಿ injection_ದರ_hz;
}

ಶೂನ್ಯ CorrectionInjector::ಸತತ()
{
	pthread_mutex_lock(&_mutex);

	ಆದರೆ (!ಸಕ್ರಿಯ) {
		ಆದರೆ (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (_fd < 0) {
			_fd = ::open(ಪೋರ್ಟ್_2, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			ಆದರೆ (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", ಪೋರ್ಟ್_2, errno);
				_write_error_reported = ನಿಜ;
			}
		}

		ಆದರೆ (_fd >= 0) {
			ಆದರೆ (_config.ಹರಿವು == ಹರಿವು_2::MovingBaseline) {
				drain_moving_baseline();

			} ಇಲ್ಲದಿದ್ದರೆ {
				drain_corrections();
			}

			inject_ಚೌಕಟ್ಟುಗಳು();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

ಶೂನ್ಯ CorrectionInjector::drain_corrections()
{
	rtcm_ದತ್ತಾಂಶ_s chunk;

	ಆದರೆ (select_corrections_instance(chunk)) {
		ಕೂಡಿಸು_chunk(chunk);
	}

	ಸ್ಥಿರಾಂಕ int8_t selected = _selected_instance.ತುಂಬು();

	ಆದರೆ (selected < 0) {
		ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ n = 0; n < rtcm_ದತ್ತಾಂಶ_s::ORB_ಸರತಿ_ಉದ್ದಳತೆ && _corrections_sub[selected].update(&chunk); n++) {
		ಆದರೆ (chunk.device_id != _config.own_device_id) {
			ಕೂಡಿಸು_chunk(chunk);
		}
	}
}

ಶೂನ್ಯ CorrectionInjector::drain_moving_baseline()
{
	rtcm_ದತ್ತಾಂಶ_s chunk;

	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ n = 0; n < rtcm_ದತ್ತಾಂಶ_s::ORB_ಸರತಿ_ಉದ್ದಳತೆ && _moving_baseline_sub.update(&chunk); n++) {
		ಆದರೆ (chunk.device_id != _config.own_device_id) {
			ಕೂಡಿಸು_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
ತಾರ್ಕಿಕ CorrectionInjector::select_corrections_instance(rtcm_ದತ್ತಾಂಶ_s &chunk)
{
	ಆದರೆ (_selected_instance.ತುಂಬು() >= 0 && hrt_elapsed_ಸಮಯ(&ಕೊನೆಯ_chunk_ಸಮಯ) < k_ಮೂಲ_Timeout) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	_selected_instance.store(-1);

	ಗಾಗಿ (int8_t instance = 0; instance < rtcm_ದತ್ತಾಂಶ_s::MAX_INSTANCES; instance++) {
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ n = 0; n < rtcm_ದತ್ತಾಂಶ_s::ORB_ಸರತಿ_ಉದ್ದಳತೆ && _corrections_sub[instance].update(&chunk); n++) {
			ಆದರೆ (chunk.device_id != _config.own_device_id && hrt_elapsed_ಸಮಯ(&chunk.timestamp) < k_ಮೂಲ_Timeout) {
				_selected_instance.store(instance);
				ಹಿಂತಿರುಗಿ ನಿಜ;
			}
		}
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ CorrectionInjector::ಕೂಡಿಸು_chunk(ಸ್ಥಿರಾಂಕ rtcm_ದತ್ತಾಂಶ_s &chunk)
{
	ಸ್ಥಿರಾಂಕ size_t len = math::min<size_t>(chunk.len, ಗಾತ್ರ(chunk.ದತ್ತಾಂಶ));

	ಆದರೆ (_framer.ಕೂಡಿಸು_ದತ್ತಾಂಶ(chunk.ದತ್ತಾಂಶ, len) < len) {
		perf_ಎಣಿಕೆ(framer_ತುಂಬಿದ);
	}

	ಕೊನೆಯ_chunk_ಸಮಯ = hrt_absolute_ಸಮಯ();
}

ಶೂನ್ಯ CorrectionInjector::inject_ಚೌಕಟ್ಟುಗಳು()
{
	size_t ಚೌಕಟ್ಟು_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ಸ್ಥಿರಾಂಕ uint8_t *ಚೌಕಟ್ಟು = ಶೂನ್ಯಸೂಚಕ;

	ಆಗ ((ಚೌಕಟ್ಟು = _framer.get_ಮುಂದಿನ_ಸಂದೇಶ(&ಚೌಕಟ್ಟು_len, &protocol)) != ಶೂನ್ಯಸೂಚಕ) {
		ಆದರೆ ((_config.protocols & protocol_ಬಿಟ್(protocol)) == 0) {
			_framer.consume_ಸಂದೇಶ(ಚೌಕಟ್ಟು_len);
			perf_ಎಣಿಕೆ(_unsupported);
			ಮುಂದುವರಿ;
		}

		ಸ್ಥಿರಾಂಕ ssize_t tx_space = tx_space_ಲಭ್ಯ();

		// Unknown on POSIX, where the write itself is the check
		ಆದರೆ (tx_space >= 0 && tx_space < (ssize_t)ಚೌಕಟ್ಟು_len) {
			perf_ಎಣಿಕೆ(tx_ಬಫರ್_ತುಂಬಿದ);
			ವೇಳಾಪಟ್ಟಿ_Delayed(drain_ಸಮಯ_us(ಚೌಕಟ್ಟು_len - tx_space));
			ನಿಲ್ಲಿಸು;
		}

		ಸ್ಥಿರಾಂಕ ssize_t written = ::ಲಿಖ(_fd, ಚೌಕಟ್ಟು, ಚೌಕಟ್ಟು_len);

		ಆದರೆ (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_ಎಣಿಕೆ(tx_ಬಫರ್_ತುಂಬಿದ);
			ವೇಳಾಪಟ್ಟಿ_Delayed(drain_ಸಮಯ_us(ಚೌಕಟ್ಟು_len));
			ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ (written != (ssize_t)ಚೌಕಟ್ಟು_len) {
			perf_ಎಣಿಕೆ(_short_write);

			ಆದರೆ (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", ಪೋರ್ಟ್_2, errno);
				_write_error_reported = ನಿಜ;
			}
		}

		_framer.consume_ಸಂದೇಶ(ಚೌಕಟ್ಟು_len);
		perf_ಎಣಿಕೆ(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_ಲಭ್ಯ() ಸ್ಥಿರಾಂಕ
{
#ಆದರೆ defined(FIONSPACE)
	ಪೂರ್ಣಾಂಕ space = 0;

	ಆದರೆ (::ioctl(_fd, FIONSPACE, &space) == 0) {
		ಹಿಂತಿರುಗಿ space;
	}

#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ -1;
}

uint32_t CorrectionInjector::drain_ಸಮಯ_us(size_t ಬೈಟುಗಳು) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ uint32_t baudrate = _config.baudrate;

	ಆದರೆ (baudrate == 0) {
		ಹಿಂತಿರುಗಿ kFallbackRetryDelayUs;
	}

	ಹಿಂತಿರುಗಿ math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)ಬೈಟುಗಳು * k_ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_Byte_On_The_Wire * 1_s / baudrate);
}

ಶೂನ್ಯ CorrectionInjector::print_ಸ್ಥಿತಿ() ಸ್ಥಿರಾಂಕ
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.ಹರಿವು == ಹರಿವು_2::MovingBaseline ? "moving baseline" : "corrections", (ದ್ವಿಗುಣ)injection_ದರ_hz,
		 (ಪೂರ್ಣಾಂಕ)_selected_instance.ತುಂಬು());
	perf_print_counter(_injected);
	perf_print_counter(tx_ಬಫರ್_ತುಂಬಿದ);
	perf_print_counter(framer_ತುಂಬಿದ);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ಸ್ಥಿರಾಂಕ CorrectionFramerStats stats = _framer.getStats();

	ಆದರೆ (stats.messages_parsed > 0 || stats.ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2 > 0 || stats.ಬೈಟುಗಳು_discarded_2 > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (ಚಿಹ್ನೆರಹಿತ)stats.rtcm3_messages, (ಚಿಹ್ನೆರಹಿತ)stats.spartn_messages, (ಚಿಹ್ನೆರಹಿತ)stats.ubx_messages,
			 (ಚಿಹ್ನೆರಹಿತ)stats.ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, (ಚಿಹ್ನೆರಹಿತ)stats.ಬೈಟುಗಳು_discarded_2);
	}
}

} // namespace gnss
