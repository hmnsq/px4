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


#அந்தர்பாவ "correction_injector.h"

#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ <mathlib/mathlib.h>
#அந்தர்பாவ <px4_platform_common/log.h>
#அந்தர்பாவ <px4_platform_common/posix.h>
#அந்தர்பாவ <px4_platform_common/time.h>

using நாமாகாஶ time_literals;

நாமாகாஶ gnss
{

// Without a chunk from the selected source for this long, another source may be taken
நிலையான constexpr hrt_abstime kSourceTimeout = 5_s;
// The injection rate is frames per second over windows this long
நிலையான constexpr hrt_abstime kRateWindow = 5_s;
// Start bit, eight data bits, stop bit
நிலையான constexpr uint32_t kBitsPerByteOnTheWire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
நிலையான constexpr uint32_t kMinRetryDelayUs = 1_ms;
நிலையான constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(மாறிலி எழுத்து *name, மாறிலி எழுத்து *port) :
	ScheduledWorkItem(name, px4::serial_port_to_wq(port))
{
	strncpy(_port, port, அளவு(_port) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	க்கு (auto &sub : _corrections_sub) {
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

வெறுமை CorrectionInjector::start(மாறிலி Config &config)
{
	_rate_window_start = hrt_absolute_time();
	rate_window_மொத்தம் = perf_event_மொத்தம்(_injected);
	_injection_rate_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	எனில் (config.stream == Stream::MovingBaseline) {
		_moving_baseline_sub.registerCallback();

	} இல்லையெனில் {
		க்கு (auto &sub : _corrections_sub) {
			sub.registerCallback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	_active = உண்மை;
	_write_error_reported = பொய்;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ScheduleNow();
}

வெறுமை CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	_active = பொய்;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	க்கு (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ScheduleNow();

	க்கு (முழு i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		மாறிலி தருக்க closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		எனில் (closed) {
			நிறுத்து;
		}

		px4_usleep(5_ms);
	}
}

தசம CorrectionInjector::injection_rate_hz()
{
	மாறிலி hrt_abstime now = hrt_absolute_time();

	எனில் (now - _rate_window_start >= kRateWindow) {
		மாறிலி uint64_t மொத்தம் = perf_event_மொத்தம்(_injected);
		_injection_rate_hz = (தசம)(மொத்தம் - rate_window_மொத்தம்) * 1e6f / (தசம)(now - _rate_window_start);
		rate_window_மொத்தம் = மொத்தம்;
		_rate_window_start = now;
	}

	திரும்பு _injection_rate_hz;
}

வெறுமை CorrectionInjector::Run()
{
	pthread_mutex_lock(&_mutex);

	எனில் (!_active) {
		எனில் (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} இல்லையெனில் {
		எனில் (_fd < 0) {
			_fd = ::open(_port, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			எனில் (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", _port, errno);
				_write_error_reported = உண்மை;
			}
		}

		எனில் (_fd >= 0) {
			எனில் (_config.stream == Stream::MovingBaseline) {
				drain_moving_baseline();

			} இல்லையெனில் {
				drain_corrections();
			}

			inject_frames();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

வெறுமை CorrectionInjector::drain_corrections()
{
	rtcm_data_s chunk;

	எனில் (select_corrections_instance(chunk)) {
		add_chunk(chunk);
	}

	மாறிலி int8_t selected = _selected_instance.load();

	எனில் (selected < 0) {
		திரும்பு;
	}

	க்கு (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[selected].update(&chunk); n++) {
		எனில் (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

வெறுமை CorrectionInjector::drain_moving_baseline()
{
	rtcm_data_s chunk;

	க்கு (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _moving_baseline_sub.update(&chunk); n++) {
		எனில் (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
தருக்க CorrectionInjector::select_corrections_instance(rtcm_data_s &chunk)
{
	எனில் (_selected_instance.load() >= 0 && hrt_elapsed_time(&_last_chunk_time) < kSourceTimeout) {
		திரும்பு பொய்;
	}

	_selected_instance.store(-1);

	க்கு (int8_t instance = 0; instance < rtcm_data_s::MAX_INSTANCES; instance++) {
		க்கு (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[instance].update(&chunk); n++) {
			எனில் (chunk.device_id != _config.own_device_id && hrt_elapsed_time(&chunk.timestamp) < kSourceTimeout) {
				_selected_instance.store(instance);
				திரும்பு உண்மை;
			}
		}
	}

	திரும்பு பொய்;
}

வெறுமை CorrectionInjector::add_chunk(மாறிலி rtcm_data_s &chunk)
{
	மாறிலி size_t len = math::min<size_t>(chunk.len, அளவு(chunk.data));

	எனில் (_framer.addData(chunk.data, len) < len) {
		perf_மொத்தம்(_framer_full);
	}

	_last_chunk_time = hrt_absolute_time();
}

வெறுமை CorrectionInjector::inject_frames()
{
	size_t frame_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	மாறிலி uint8_t *frame = nullptr;

	போது ((frame = _framer.getNextMessage(&frame_len, &protocol)) != nullptr) {
		எனில் ((_config.protocols & protocol_bit(protocol)) == 0) {
			_framer.consumeMessage(frame_len);
			perf_மொத்தம்(_unsupported);
			தொடர்;
		}

		மாறிலி ssize_t tx_space = tx_space_available();

		// Unknown on POSIX, where the write itself is the check
		எனில் (tx_space >= 0 && tx_space < (ssize_t)frame_len) {
			perf_மொத்தம்(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len - tx_space));
			நிறுத்து;
		}

		மாறிலி ssize_t written = ::லிக(_fd, frame, frame_len);

		எனில் (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_மொத்தம்(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len));
			நிறுத்து;
		}

		எனில் (written != (ssize_t)frame_len) {
			perf_மொத்தம்(_short_write);

			எனில் (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", _port, errno);
				_write_error_reported = உண்மை;
			}
		}

		_framer.consumeMessage(frame_len);
		perf_மொத்தம்(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_available() மாறிலி
{
#எனில் defined(FIONSPACE)
	முழு space = 0;

	எனில் (::ioctl(_fd, FIONSPACE, &space) == 0) {
		திரும்பு space;
	}

#endif
	திரும்பு -1;
}

uint32_t CorrectionInjector::drain_time_us(size_t bytes) மாறிலி
{
	மாறிலி uint32_t baudrate = _config.baudrate;

	எனில் (baudrate == 0) {
		திரும்பு kFallbackRetryDelayUs;
	}

	திரும்பு math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)bytes * kBitsPerByteOnTheWire * 1_s / baudrate);
}

வெறுமை CorrectionInjector::print_status() மாறிலி
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.stream == Stream::MovingBaseline ? "moving baseline" : "corrections", (இரட்டை)_injection_rate_hz,
		 (முழு)_selected_instance.load());
	perf_print_counter(_injected);
	perf_print_counter(_tx_buffer_full);
	perf_print_counter(_framer_full);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	மாறிலி CorrectionFramerStats stats = _framer.getStats();

	எனில் (stats.messages_parsed > 0 || stats.crc_errors > 0 || stats.bytes_discarded > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (unsigned)stats.rtcm3_messages, (unsigned)stats.spartn_messages, (unsigned)stats.ubx_messages,
			 (unsigned)stats.crc_errors, (unsigned)stats.bytes_discarded);
	}
}

} // namespace gnss
