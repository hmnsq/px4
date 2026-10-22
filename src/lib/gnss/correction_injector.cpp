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


#অন্তর্ভাব "correction_injector.h"

#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/ioctl.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব <mathlib/mathlib.h>
#অন্তর্ভাব <px4_platform_common/log.h>
#অন্তর্ভাব <px4_platform_common/posix.h>
#অন্তর্ভাব <px4_platform_common/time.h>

using নামাকাশ time_literals;

নামাকাশ gnss
{

// Without a chunk from the selected source for this long, another source may be taken
স্থির constexpr hrt_abstime kSourceTimeout = 5_s;
// The injection rate is frames per second over windows this long
স্থির constexpr hrt_abstime kRateWindow = 5_s;
// Start bit, eight data bits, stop bit
স্থির constexpr uint32_t kBitsPerByteOnTheWire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
স্থির constexpr uint32_t kMinRetryDelayUs = 1_ms;
স্থির constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *port) :
	ScheduledWorkItem(name, px4::serial_port_to_wq(port))
{
	strncpy(_port, port, আকার(_port) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	জন্য (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	ScheduleClear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_বাফার_full);
	perf_free(_framer_full);
	perf_free(_unsupported);
	perf_free(_short_write);
}

শূন্য CorrectionInjector::start(ধ্রুবক Config &config)
{
	_rate_window_start = hrt_absolute_time();
	rate_window_গুনতি = perf_event_গুনতি(_injected);
	_injection_rate_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	যদি (config.stream == Stream::MovingBaseline) {
		_moving_baseline_sub.registerCallback();

	} অন্যথা {
		জন্য (auto &sub : _corrections_sub) {
			sub.registerCallback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	_active = সত্য;
	_write_error_reported = মিথ্যা;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ScheduleNow();
}

শূন্য CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	_active = মিথ্যা;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	জন্য (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ScheduleNow();

	জন্য (পূর্ণসংখ্যা i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		ধ্রুবক যৌক্তিক closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		যদি (closed) {
			বিরতি;
		}

		px4_usleep(5_ms);
	}
}

দশমিক CorrectionInjector::injection_rate_hz()
{
	ধ্রুবক hrt_abstime now = hrt_absolute_time();

	যদি (now - _rate_window_start >= kRateWindow) {
		ধ্রুবক uint64_t গুনতি = perf_event_গুনতি(_injected);
		_injection_rate_hz = (দশমিক)(গুনতি - rate_window_গুনতি) * 1e6f / (দশমিক)(now - _rate_window_start);
		rate_window_গুনতি = গুনতি;
		_rate_window_start = now;
	}

	ফেরত _injection_rate_hz;
}

শূন্য CorrectionInjector::Run()
{
	pthread_mutex_lock(&_mutex);

	যদি (!_active) {
		যদি (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} অন্যথা {
		যদি (_fd < 0) {
			_fd = ::open(_port, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			যদি (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", _port, errno);
				_write_error_reported = সত্য;
			}
		}

		যদি (_fd >= 0) {
			যদি (_config.stream == Stream::MovingBaseline) {
				drain_moving_baseline();

			} অন্যথা {
				drain_corrections();
			}

			inject_frames();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

শূন্য CorrectionInjector::drain_corrections()
{
	rtcm_তথ্য_s chunk;

	যদি (select_corrections_instance(chunk)) {
		add_chunk(chunk);
	}

	ধ্রুবক int8_t selected = _selected_instance.load();

	যদি (selected < 0) {
		ফেরত;
	}

	জন্য (unsigned n = 0; n < rtcm_তথ্য_s::ORB_QUEUE_দৈর্ঘ্য && _corrections_sub[selected].update(&chunk); n++) {
		যদি (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

শূন্য CorrectionInjector::drain_moving_baseline()
{
	rtcm_তথ্য_s chunk;

	জন্য (unsigned n = 0; n < rtcm_তথ্য_s::ORB_QUEUE_দৈর্ঘ্য && _moving_baseline_sub.update(&chunk); n++) {
		যদি (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
যৌক্তিক CorrectionInjector::select_corrections_instance(rtcm_তথ্য_s &chunk)
{
	যদি (_selected_instance.load() >= 0 && hrt_elapsed_time(&_last_chunk_time) < kSourceTimeout) {
		ফেরত মিথ্যা;
	}

	_selected_instance.store(-1);

	জন্য (int8_t instance = 0; instance < rtcm_তথ্য_s::MAX_INSTANCES; instance++) {
		জন্য (unsigned n = 0; n < rtcm_তথ্য_s::ORB_QUEUE_দৈর্ঘ্য && _corrections_sub[instance].update(&chunk); n++) {
			যদি (chunk.device_id != _config.own_device_id && hrt_elapsed_time(&chunk.timestamp) < kSourceTimeout) {
				_selected_instance.store(instance);
				ফেরত সত্য;
			}
		}
	}

	ফেরত মিথ্যা;
}

শূন্য CorrectionInjector::add_chunk(ধ্রুবক rtcm_তথ্য_s &chunk)
{
	ধ্রুবক size_t len = math::min<size_t>(chunk.len, আকার(chunk.তথ্য));

	যদি (_framer.add_তথ্য(chunk.তথ্য, len) < len) {
		perf_গুনতি(_framer_full);
	}

	_last_chunk_time = hrt_absolute_time();
}

শূন্য CorrectionInjector::inject_frames()
{
	size_t frame_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ধ্রুবক uint8_t *frame = nullptr;

	যখন ((frame = _framer.get_পরবর্তী_Message(&frame_len, &protocol)) != nullptr) {
		যদি ((_config.protocols & protocol_bit(protocol)) == 0) {
			_framer.consumeMessage(frame_len);
			perf_গুনতি(_unsupported);
			চালু;
		}

		ধ্রুবক ssize_t tx_space = tx_space_available();

		// Unknown on POSIX, where the write itself is the check
		যদি (tx_space >= 0 && tx_space < (ssize_t)frame_len) {
			perf_গুনতি(tx_বাফার_full);
			ScheduleDelayed(drain_time_us(frame_len - tx_space));
			বিরতি;
		}

		ধ্রুবক ssize_t written = ::লিখ(_fd, frame, frame_len);

		যদি (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_গুনতি(tx_বাফার_full);
			ScheduleDelayed(drain_time_us(frame_len));
			বিরতি;
		}

		যদি (written != (ssize_t)frame_len) {
			perf_গুনতি(_short_write);

			যদি (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", _port, errno);
				_write_error_reported = সত্য;
			}
		}

		_framer.consumeMessage(frame_len);
		perf_গুনতি(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_available() ধ্রুবক
{
#যদি defined(FIONSPACE)
	পূর্ণসংখ্যা space = 0;

	যদি (::ioctl(_fd, FIONSPACE, &space) == 0) {
		ফেরত space;
	}

#endif
	ফেরত -1;
}

uint32_t CorrectionInjector::drain_time_us(size_t bytes) ধ্রুবক
{
	ধ্রুবক uint32_t baudrate = _config.baudrate;

	যদি (baudrate == 0) {
		ফেরত kFallbackRetryDelayUs;
	}

	ফেরত math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)bytes * kBitsPerByteOnTheWire * 1_s / baudrate);
}

শূন্য CorrectionInjector::print_status() ধ্রুবক
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.stream == Stream::MovingBaseline ? "moving baseline" : "corrections", (দ্বিগুণ)_injection_rate_hz,
		 (পূর্ণসংখ্যা)_selected_instance.load());
	perf_print_গণক(_injected);
	perf_print_গণক(tx_বাফার_full);
	perf_print_গণক(_framer_full);
	perf_print_গণক(_unsupported);
	perf_print_গণক(_short_write);

	ধ্রুবক CorrectionFramerStats stats = _framer.getStats();

	যদি (stats.messages_parsed > 0 || stats.crc_errors > 0 || stats.bytes_discarded > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (unsigned)stats.rtcm3_messages, (unsigned)stats.spartn_messages, (unsigned)stats.ubx_messages,
			 (unsigned)stats.crc_errors, (unsigned)stats.bytes_discarded);
	}
}

} // namespace gnss
