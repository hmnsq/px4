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


#شامل "correction_injector.h"

#شامل <errno.h>
#شامل <fcntl.h>
#شامل <string.h>
#شامل <sys/ioctl.h>
#شامل <unistd.h>

#شامل <mathlib/mathlib.h>
#شامل <px4_platform_common/log.h>
#شامل <px4_platform_common/posix.h>
#شامل <px4_platform_common/time.h>

using فضاینام time_literals;

فضاینام gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ایستا constexpr hrt_abstime kSourceTimeout = 5_s;
// The injection rate is frames per second over windows this long
ایستا constexpr hrt_abstime kRateWindow = 5_s;
// Start bit, eight data bits, stop bit
ایستا constexpr uint32_t kBitsPerByteOnTheWire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ایستا constexpr uint32_t kMinRetryDelayUs = 1_ms;
ایستا constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ثابت نویسه *name, ثابت نویسه *port) :
	ScheduledWorkItem(name, px4::serial_port_to_wq(port))
{
	strncpy(_port, port, اندازه(_port) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	برای (auto &sub : _corrections_sub) {
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

تهی CorrectionInjector::start(ثابت Config &config)
{
	_rate_window_start = hrt_absolute_time();
	rate_window_شمار = perf_event_شمار(_injected);
	_injection_rate_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	اگر (config.stream == Stream::MovingBaseline) {
		_moving_baseline_sub.registerCallback();

	} وگرنه {
		برای (auto &sub : _corrections_sub) {
			sub.registerCallback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	_active = درست;
	_write_error_reported = نادرست;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ScheduleNow();
}

تهی CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	_active = نادرست;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	برای (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ScheduleNow();

	برای (صحیح i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		ثابت منطقی closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		اگر (closed) {
			بشکن;
		}

		px4_usleep(5_ms);
	}
}

اعشاری CorrectionInjector::injection_rate_hz()
{
	ثابت hrt_abstime now = hrt_absolute_time();

	اگر (now - _rate_window_start >= kRateWindow) {
		ثابت uint64_t شمار = perf_event_شمار(_injected);
		_injection_rate_hz = (اعشاری)(شمار - rate_window_شمار) * 1e6f / (اعشاری)(now - _rate_window_start);
		rate_window_شمار = شمار;
		_rate_window_start = now;
	}

	بازگشت _injection_rate_hz;
}

تهی CorrectionInjector::Run()
{
	pthread_mutex_lock(&_mutex);

	اگر (!_active) {
		اگر (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} وگرنه {
		اگر (_fd < 0) {
			_fd = ::open(_port, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			اگر (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", _port, errno);
				_write_error_reported = درست;
			}
		}

		اگر (_fd >= 0) {
			اگر (_config.stream == Stream::MovingBaseline) {
				drain_moving_baseline();

			} وگرنه {
				drain_corrections();
			}

			inject_frames();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

تهی CorrectionInjector::drain_corrections()
{
	rtcm_data_s chunk;

	اگر (select_corrections_instance(chunk)) {
		add_chunk(chunk);
	}

	ثابت int8_t selected = _selected_instance.load();

	اگر (selected < 0) {
		بازگشت;
	}

	برای (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[selected].update(&chunk); n++) {
		اگر (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

تهی CorrectionInjector::drain_moving_baseline()
{
	rtcm_data_s chunk;

	برای (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _moving_baseline_sub.update(&chunk); n++) {
		اگر (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
منطقی CorrectionInjector::select_corrections_instance(rtcm_data_s &chunk)
{
	اگر (_selected_instance.load() >= 0 && hrt_elapsed_time(&_last_chunk_time) < kSourceTimeout) {
		بازگشت نادرست;
	}

	_selected_instance.store(-1);

	برای (int8_t instance = 0; instance < rtcm_data_s::MAX_INSTANCES; instance++) {
		برای (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[instance].update(&chunk); n++) {
			اگر (chunk.device_id != _config.own_device_id && hrt_elapsed_time(&chunk.timestamp) < kSourceTimeout) {
				_selected_instance.store(instance);
				بازگشت درست;
			}
		}
	}

	بازگشت نادرست;
}

تهی CorrectionInjector::add_chunk(ثابت rtcm_data_s &chunk)
{
	ثابت size_t len = math::min<size_t>(chunk.len, اندازه(chunk.data));

	اگر (_framer.addData(chunk.data, len) < len) {
		perf_شمار(_framer_full);
	}

	_last_chunk_time = hrt_absolute_time();
}

تهی CorrectionInjector::inject_frames()
{
	size_t frame_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *frame = nullptr;

	وقتی ((frame = _framer.getNextMessage(&frame_len, &protocol)) != nullptr) {
		اگر ((_config.protocols & protocol_bit(protocol)) == 0) {
			_framer.consumeMessage(frame_len);
			perf_شمار(_unsupported);
			ادامه;
		}

		ثابت ssize_t tx_space = tx_space_available();

		// Unknown on POSIX, where the write itself is the check
		اگر (tx_space >= 0 && tx_space < (ssize_t)frame_len) {
			perf_شمار(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len - tx_space));
			بشکن;
		}

		ثابت ssize_t written = ::اكتب(_fd, frame, frame_len);

		اگر (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_شمار(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len));
			بشکن;
		}

		اگر (written != (ssize_t)frame_len) {
			perf_شمار(_short_write);

			اگر (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", _port, errno);
				_write_error_reported = درست;
			}
		}

		_framer.consumeMessage(frame_len);
		perf_شمار(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_available() ثابت
{
#اگر defined(FIONSPACE)
	صحیح space = 0;

	اگر (::ioctl(_fd, FIONSPACE, &space) == 0) {
		بازگشت space;
	}

#endif
	بازگشت -1;
}

uint32_t CorrectionInjector::drain_time_us(size_t bytes) ثابت
{
	ثابت uint32_t baudrate = _config.baudrate;

	اگر (baudrate == 0) {
		بازگشت kFallbackRetryDelayUs;
	}

	بازگشت math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)bytes * kBitsPerByteOnTheWire * 1_s / baudrate);
}

تهی CorrectionInjector::print_status() ثابت
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.stream == Stream::MovingBaseline ? "moving baseline" : "corrections", (دوگانه)_injection_rate_hz,
		 (صحیح)_selected_instance.load());
	perf_print_counter(_injected);
	perf_print_counter(_tx_buffer_full);
	perf_print_counter(_framer_full);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ثابت CorrectionFramerStats stats = _framer.getStats();

	اگر (stats.messages_parsed > 0 || stats.crc_errors > 0 || stats.bytes_discarded > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (unsigned)stats.rtcm3_messages, (unsigned)stats.spartn_messages, (unsigned)stats.ubx_messages,
			 (unsigned)stats.crc_errors, (unsigned)stats.bytes_discarded);
	}
}

} // namespace gnss
