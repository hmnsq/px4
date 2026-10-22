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

using ನಾಮಾಕಾಶ time_literals;

ನಾಮಾಕಾಶ gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ಸ್ಥಿರ constexpr hrt_abstime kSourceTimeout = 5_s;
// The injection rate is frames per second over windows this long
ಸ್ಥಿರ constexpr hrt_abstime kRateWindow = 5_s;
// Start bit, eight data bits, stop bit
ಸ್ಥಿರ constexpr uint32_t kBitsPerByteOnTheWire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ಸ್ಥಿರ constexpr uint32_t kMinRetryDelayUs = 1_ms;
ಸ್ಥಿರ constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *port) :
	ScheduledWorkItem(name, px4::serial_port_to_wq(port))
{
	strncpy(_port, port, ಪರಿಮಾಣ(_port) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	ಪ್ರತಿ (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	ScheduleClear();
	Deinit();

	perf_free(_injected);
	perf_free(_tx_buffer_full);
	perf_free(_framer_full);
	perf_free(_unsupported);
	perf_free(_short_write);
}

ಶೂನ್ಯ CorrectionInjector::start(ಧ್ರುವ Config &config)
{
	_rate_window_start = hrt_absolute_time();
	rate_window_ಎಣಿಕೆ = perf_event_ಎಣಿಕೆ(_injected);
	_injection_rate_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	ಯದಿ (config.stream == Stream::MovingBaseline) {
		_moving_baseline_sub.registerCallback();

	} ಅನ್ಯಥಾ {
		ಪ್ರತಿ (auto &sub : _corrections_sub) {
			sub.registerCallback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	_active = ಸತ್ಯ;
	_write_error_reported = ಅಸತ್ಯ;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ScheduleNow();
}

ಶೂನ್ಯ CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	_active = ಅಸತ್ಯ;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	ಪ್ರತಿ (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ScheduleNow();

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		ಧ್ರುವ ತಾರ್ಕಿಕ closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		ಯದಿ (closed) {
			ಭಙ್ಗ;
		}

		px4_usleep(5_ms);
	}
}

ಭಿನ್ನ CorrectionInjector::injection_rate_hz()
{
	ಧ್ರುವ hrt_abstime now = hrt_absolute_time();

	ಯದಿ (now - _rate_window_start >= kRateWindow) {
		ಧ್ರುವ uint64_t ಎಣಿಕೆ = perf_event_ಎಣಿಕೆ(_injected);
		_injection_rate_hz = (ಭಿನ್ನ)(ಎಣಿಕೆ - rate_window_ಎಣಿಕೆ) * 1e6f / (ಭಿನ್ನ)(now - _rate_window_start);
		rate_window_ಎಣಿಕೆ = ಎಣಿಕೆ;
		_rate_window_start = now;
	}

	ಪ್ರತ್ಯಾಗಮ _injection_rate_hz;
}

ಶೂನ್ಯ CorrectionInjector::Run()
{
	pthread_mutex_lock(&_mutex);

	ಯದಿ (!_active) {
		ಯದಿ (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} ಅನ್ಯಥಾ {
		ಯದಿ (_fd < 0) {
			_fd = ::open(_port, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			ಯದಿ (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", _port, errno);
				_write_error_reported = ಸತ್ಯ;
			}
		}

		ಯದಿ (_fd >= 0) {
			ಯದಿ (_config.stream == Stream::MovingBaseline) {
				drain_moving_baseline();

			} ಅನ್ಯಥಾ {
				drain_corrections();
			}

			inject_frames();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

ಶೂನ್ಯ CorrectionInjector::drain_corrections()
{
	rtcm_data_s chunk;

	ಯದಿ (select_corrections_instance(chunk)) {
		add_chunk(chunk);
	}

	ಧ್ರುವ int8_t selected = _selected_instance.load();

	ಯದಿ (selected < 0) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[selected].update(&chunk); n++) {
		ಯದಿ (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

ಶೂನ್ಯ CorrectionInjector::drain_moving_baseline()
{
	rtcm_data_s chunk;

	ಪ್ರತಿ (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _moving_baseline_sub.update(&chunk); n++) {
		ಯದಿ (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
ತಾರ್ಕಿಕ CorrectionInjector::select_corrections_instance(rtcm_data_s &chunk)
{
	ಯದಿ (_selected_instance.load() >= 0 && hrt_elapsed_time(&_last_chunk_time) < kSourceTimeout) {
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	_selected_instance.store(-1);

	ಪ್ರತಿ (int8_t instance = 0; instance < rtcm_data_s::MAX_INSTANCES; instance++) {
		ಪ್ರತಿ (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[instance].update(&chunk); n++) {
			ಯದಿ (chunk.device_id != _config.own_device_id && hrt_elapsed_time(&chunk.timestamp) < kSourceTimeout) {
				_selected_instance.store(instance);
				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ CorrectionInjector::add_chunk(ಧ್ರುವ rtcm_data_s &chunk)
{
	ಧ್ರುವ size_t len = math::min<size_t>(chunk.len, ಪರಿಮಾಣ(chunk.data));

	ಯದಿ (_framer.addData(chunk.data, len) < len) {
		perf_ಎಣಿಕೆ(_framer_full);
	}

	_last_chunk_time = hrt_absolute_time();
}

ಶೂನ್ಯ CorrectionInjector::inject_frames()
{
	size_t frame_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ಧ್ರುವ uint8_t *frame = nullptr;

	ಯಾವತ್ ((frame = _framer.getNextMessage(&frame_len, &protocol)) != nullptr) {
		ಯದಿ ((_config.protocols & protocol_bit(protocol)) == 0) {
			_framer.consumeMessage(frame_len);
			perf_ಎಣಿಕೆ(_unsupported);
			ಅನುವರ್ತ;
		}

		ಧ್ರುವ ssize_t tx_space = tx_space_available();

		// Unknown on POSIX, where the write itself is the check
		ಯದಿ (tx_space >= 0 && tx_space < (ssize_t)frame_len) {
			perf_ಎಣಿಕೆ(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len - tx_space));
			ಭಙ್ಗ;
		}

		ಧ್ರುವ ssize_t written = ::ಲಿಖ(_fd, frame, frame_len);

		ಯದಿ (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_ಎಣಿಕೆ(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len));
			ಭಙ್ಗ;
		}

		ಯದಿ (written != (ssize_t)frame_len) {
			perf_ಎಣಿಕೆ(_short_write);

			ಯದಿ (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", _port, errno);
				_write_error_reported = ಸತ್ಯ;
			}
		}

		_framer.consumeMessage(frame_len);
		perf_ಎಣಿಕೆ(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_available() ಧ್ರುವ
{
#ಯದಿ defined(FIONSPACE)
	ಪೂರ್ಣಾಙ್ಕ space = 0;

	ಯದಿ (::ioctl(_fd, FIONSPACE, &space) == 0) {
		ಪ್ರತ್ಯಾಗಮ space;
	}

#endif
	ಪ್ರತ್ಯಾಗಮ -1;
}

uint32_t CorrectionInjector::drain_time_us(size_t bytes) ಧ್ರುವ
{
	ಧ್ರುವ uint32_t baudrate = _config.baudrate;

	ಯದಿ (baudrate == 0) {
		ಪ್ರತ್ಯಾಗಮ kFallbackRetryDelayUs;
	}

	ಪ್ರತ್ಯಾಗಮ math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)bytes * kBitsPerByteOnTheWire * 1_s / baudrate);
}

ಶೂನ್ಯ CorrectionInjector::print_status() ಧ್ರುವ
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.stream == Stream::MovingBaseline ? "moving baseline" : "corrections", (ದ್ವಿಗುಣ)_injection_rate_hz,
		 (ಪೂರ್ಣಾಙ್ಕ)_selected_instance.load());
	perf_print_counter(_injected);
	perf_print_counter(_tx_buffer_full);
	perf_print_counter(_framer_full);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ಧ್ರುವ CorrectionFramerStats stats = _framer.getStats();

	ಯದಿ (stats.messages_parsed > 0 || stats.crc_errors > 0 || stats.bytes_discarded > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (unsigned)stats.rtcm3_messages, (unsigned)stats.spartn_messages, (unsigned)stats.ubx_messages,
			 (unsigned)stats.crc_errors, (unsigned)stats.bytes_discarded);
	}
}

} // namespace gnss
