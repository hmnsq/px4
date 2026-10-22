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


#ضمن "correction_injector.h"

#ضمن <errno.h>
#ضمن <fcntl.h>
#ضمن <string.h>
#ضمن <sys/ioctl.h>
#ضمن <unistd.h>

#ضمن <mathlib/mathlib.h>
#ضمن <px4_platform_common/log.h>
#ضمن <px4_platform_common/posix.h>
#ضمن <px4_platform_common/time.h>

using نطاق time_literals;

نطاق gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ساكن constexpr hrt_abstime kSourceTimeout = 5_s;
// The injection rate is frames per second over windows this long
ساكن constexpr hrt_abstime kRateWindow = 5_s;
// Start bit, eight data bits, stop bit
ساكن constexpr uint32_t kBitsPerByteOnTheWire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ساكن constexpr uint32_t kMinRetryDelayUs = 1_ms;
ساكن constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ثابت حرف *name, ثابت حرف *port) :
	ScheduledWorkItem(name, px4::serial_port_to_wq(port))
{
	strncpy(_port, port, حجم(_port) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	لكل (auto &sub : _corrections_sub) {
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

فراغ CorrectionInjector::start(ثابت Config &config)
{
	_rate_window_start = hrt_absolute_time();
	rate_window_تعداد = perf_event_تعداد(_injected);
	_injection_rate_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	إذا (config.stream == Stream::MovingBaseline) {
		_moving_baseline_sub.registerCallback();

	} وإلا {
		لكل (auto &sub : _corrections_sub) {
			sub.registerCallback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	_active = true;
	_write_error_reported = خطأ;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ScheduleNow();
}

فراغ CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	_active = خطأ;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	لكل (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ScheduleNow();

	لكل (صحيح i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		ثابت منطقي closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		إذا (closed) {
			اكسر;
		}

		px4_usleep(5_ms);
	}
}

عائم CorrectionInjector::injection_rate_hz()
{
	ثابت hrt_abstime now = hrt_absolute_time();

	إذا (now - _rate_window_start >= kRateWindow) {
		ثابت uint64_t تعداد_2 = perf_event_تعداد(_injected);
		_injection_rate_hz = (عائم)(تعداد_2 - rate_window_تعداد) * 1e6f / (عائم)(now - _rate_window_start);
		rate_window_تعداد = تعداد_2;
		_rate_window_start = now;
	}

	إرجاع _injection_rate_hz;
}

فراغ CorrectionInjector::Run()
{
	pthread_mutex_lock(&_mutex);

	إذا (!_active) {
		إذا (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} وإلا {
		إذا (_fd < 0) {
			_fd = ::open(_port, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			إذا (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", _port, errno);
				_write_error_reported = true;
			}
		}

		إذا (_fd >= 0) {
			إذا (_config.stream == Stream::MovingBaseline) {
				drain_moving_baseline();

			} وإلا {
				drain_corrections();
			}

			inject_frames();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

فراغ CorrectionInjector::drain_corrections()
{
	rtcm_data_s chunk;

	إذا (select_corrections_instance(chunk)) {
		add_chunk(chunk);
	}

	ثابت int8_t selected = _selected_instance.load();

	إذا (selected < 0) {
		إرجاع;
	}

	لكل (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[selected].update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

فراغ CorrectionInjector::drain_moving_baseline()
{
	rtcm_data_s chunk;

	لكل (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _moving_baseline_sub.update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
منطقي CorrectionInjector::select_corrections_instance(rtcm_data_s &chunk)
{
	إذا (_selected_instance.load() >= 0 && hrt_elapsed_time(&_last_chunk_time) < kSourceTimeout) {
		إرجاع خطأ;
	}

	_selected_instance.store(-1);

	لكل (int8_t instance = 0; instance < rtcm_data_s::MAX_INSTANCES; instance++) {
		لكل (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[instance].update(&chunk); n++) {
			إذا (chunk.device_id != _config.own_device_id && hrt_elapsed_time(&chunk.timestamp) < kSourceTimeout) {
				_selected_instance.store(instance);
				إرجاع true;
			}
		}
	}

	إرجاع خطأ;
}

فراغ CorrectionInjector::add_chunk(ثابت rtcm_data_s &chunk)
{
	ثابت size_t len = math::min<size_t>(chunk.len, حجم(chunk.data));

	إذا (_framer.addData(chunk.data, len) < len) {
		perf_تعداد(_framer_full);
	}

	_last_chunk_time = hrt_absolute_time();
}

فراغ CorrectionInjector::inject_frames()
{
	size_t frame_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *frame = nullptr;

	بينما ((frame = _framer.getNextMessage(&frame_len, &protocol)) != nullptr) {
		إذا ((_config.protocols & protocol_bit(protocol)) == 0) {
			_framer.consumeMessage(frame_len);
			perf_تعداد(_unsupported);
			استمر;
		}

		ثابت ssize_t tx_space = tx_space_available();

		// Unknown on POSIX, where the write itself is the check
		إذا (tx_space >= 0 && tx_space < (ssize_t)frame_len) {
			perf_تعداد(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len - tx_space));
			اكسر;
		}

		ثابت ssize_t written = ::اكتب(_fd, frame, frame_len);

		إذا (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_تعداد(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len));
			اكسر;
		}

		إذا (written != (ssize_t)frame_len) {
			perf_تعداد(_short_write);

			إذا (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", _port, errno);
				_write_error_reported = true;
			}
		}

		_framer.consumeMessage(frame_len);
		perf_تعداد(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_available() ثابت
{
#إذا defined(FIONSPACE)
	صحيح space = 0;

	إذا (::ioctl(_fd, FIONSPACE, &space) == 0) {
		إرجاع space;
	}

#endif
	إرجاع -1;
}

uint32_t CorrectionInjector::drain_time_us(size_t bytes) ثابت
{
	ثابت uint32_t baudrate = _config.baudrate;

	إذا (baudrate == 0) {
		إرجاع kFallbackRetryDelayUs;
	}

	إرجاع math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)bytes * kBitsPerByteOnTheWire * 1_s / baudrate);
}

فراغ CorrectionInjector::print_status() ثابت
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.stream == Stream::MovingBaseline ? "moving baseline" : "corrections", (مزدوج)_injection_rate_hz,
		 (صحيح)_selected_instance.load());
	perf_print_counter(_injected);
	perf_print_counter(_tx_buffer_full);
	perf_print_counter(_framer_full);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ثابت CorrectionFramerStats stats = _framer.getStats();

	إذا (stats.messages_parsed > 0 || stats.crc_errors > 0 || stats.bytes_discarded > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (unsigned)stats.rtcm3_messages, (unsigned)stats.spartn_messages, (unsigned)stats.ubx_messages,
			 (unsigned)stats.crc_errors, (unsigned)stats.bytes_discarded);
	}
}

} // namespace gnss
