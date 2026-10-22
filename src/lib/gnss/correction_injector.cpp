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


#અન્તર્ભાવ "correction_injector.h"

#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ <mathlib/mathlib.h>
#અન્તર્ભાવ <px4_platform_common/log.h>
#અન્તર્ભાવ <px4_platform_common/posix.h>
#અન્તર્ભાવ <px4_platform_common/time.h>

using નામાકાશ time_literals;

નામાકાશ gnss
{

// Without a chunk from the selected source for this long, another source may be taken
સ્થિર constexpr hrt_abstime kSourceTimeout = 5_s;
// The injection rate is frames per second over windows this long
સ્થિર constexpr hrt_abstime kRateWindow = 5_s;
// Start bit, eight data bits, stop bit
સ્થિર constexpr uint32_t kBitsPerByteOnTheWire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
સ્થિર constexpr uint32_t kMinRetryDelayUs = 1_ms;
સ્થિર constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(અચલ અક્ષર *name, અચલ અક્ષર *port) :
	ScheduledWorkItem(name, px4::serial_port_to_wq(port))
{
	strncpy(_port, port, કદ(_port) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	માટે (auto &sub : _corrections_sub) {
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

શૂન્ય CorrectionInjector::start(અચલ Config &config)
{
	_rate_window_start = hrt_absolute_time();
	rate_window_ગણતરી = perf_event_ગણતરી(_injected);
	_injection_rate_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	જો (config.stream == Stream::MovingBaseline) {
		_moving_baseline_sub.registerCallback();

	} નહીંતર {
		માટે (auto &sub : _corrections_sub) {
			sub.registerCallback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	_active = સાચું;
	_write_error_reported = ખોટું;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ScheduleNow();
}

શૂન્ય CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	_active = ખોટું;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	માટે (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ScheduleNow();

	માટે (પૂર્ણાંક i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		અચલ તાર્કિક closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		જો (closed) {
			તોડો;
		}

		px4_usleep(5_ms);
	}
}

દશાંશ CorrectionInjector::injection_rate_hz()
{
	અચલ hrt_abstime now = hrt_absolute_time();

	જો (now - _rate_window_start >= kRateWindow) {
		અચલ uint64_t ગણતરી = perf_event_ગણતરી(_injected);
		_injection_rate_hz = (દશાંશ)(ગણતરી - rate_window_ગણતરી) * 1e6f / (દશાંશ)(now - _rate_window_start);
		rate_window_ગણતરી = ગણતરી;
		_rate_window_start = now;
	}

	પાછા _injection_rate_hz;
}

શૂન્ય CorrectionInjector::Run()
{
	pthread_mutex_lock(&_mutex);

	જો (!_active) {
		જો (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} નહીંતર {
		જો (_fd < 0) {
			_fd = ::open(_port, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			જો (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", _port, errno);
				_write_error_reported = સાચું;
			}
		}

		જો (_fd >= 0) {
			જો (_config.stream == Stream::MovingBaseline) {
				drain_moving_baseline();

			} નહીંતર {
				drain_corrections();
			}

			inject_frames();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

શૂન્ય CorrectionInjector::drain_corrections()
{
	rtcm_data_s chunk;

	જો (select_corrections_instance(chunk)) {
		add_chunk(chunk);
	}

	અચલ int8_t selected = _selected_instance.load();

	જો (selected < 0) {
		પાછા;
	}

	માટે (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[selected].update(&chunk); n++) {
		જો (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

શૂન્ય CorrectionInjector::drain_moving_baseline()
{
	rtcm_data_s chunk;

	માટે (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _moving_baseline_sub.update(&chunk); n++) {
		જો (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
તાર્કિક CorrectionInjector::select_corrections_instance(rtcm_data_s &chunk)
{
	જો (_selected_instance.load() >= 0 && hrt_elapsed_time(&_last_chunk_time) < kSourceTimeout) {
		પાછા ખોટું;
	}

	_selected_instance.store(-1);

	માટે (int8_t instance = 0; instance < rtcm_data_s::MAX_INSTANCES; instance++) {
		માટે (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[instance].update(&chunk); n++) {
			જો (chunk.device_id != _config.own_device_id && hrt_elapsed_time(&chunk.timestamp) < kSourceTimeout) {
				_selected_instance.store(instance);
				પાછા સાચું;
			}
		}
	}

	પાછા ખોટું;
}

શૂન્ય CorrectionInjector::add_chunk(અચલ rtcm_data_s &chunk)
{
	અચલ size_t len = math::min<size_t>(chunk.len, કદ(chunk.data));

	જો (_framer.addData(chunk.data, len) < len) {
		perf_ગણતરી(_framer_full);
	}

	_last_chunk_time = hrt_absolute_time();
}

શૂન્ય CorrectionInjector::inject_frames()
{
	size_t frame_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	અચલ uint8_t *frame = nullptr;

	જ્યારે ((frame = _framer.getNextMessage(&frame_len, &protocol)) != nullptr) {
		જો ((_config.protocols & protocol_bit(protocol)) == 0) {
			_framer.consumeMessage(frame_len);
			perf_ગણતરી(_unsupported);
			ચાલુ;
		}

		અચલ ssize_t tx_space = tx_space_available();

		// Unknown on POSIX, where the write itself is the check
		જો (tx_space >= 0 && tx_space < (ssize_t)frame_len) {
			perf_ગણતરી(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len - tx_space));
			તોડો;
		}

		અચલ ssize_t written = ::લિખ(_fd, frame, frame_len);

		જો (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_ગણતરી(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len));
			તોડો;
		}

		જો (written != (ssize_t)frame_len) {
			perf_ગણતરી(_short_write);

			જો (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", _port, errno);
				_write_error_reported = સાચું;
			}
		}

		_framer.consumeMessage(frame_len);
		perf_ગણતરી(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_available() અચલ
{
#જો defined(FIONSPACE)
	પૂર્ણાંક space = 0;

	જો (::ioctl(_fd, FIONSPACE, &space) == 0) {
		પાછા space;
	}

#endif
	પાછા -1;
}

uint32_t CorrectionInjector::drain_time_us(size_t bytes) અચલ
{
	અચલ uint32_t baudrate = _config.baudrate;

	જો (baudrate == 0) {
		પાછા kFallbackRetryDelayUs;
	}

	પાછા math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)bytes * kBitsPerByteOnTheWire * 1_s / baudrate);
}

શૂન્ય CorrectionInjector::print_status() અચલ
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.stream == Stream::MovingBaseline ? "moving baseline" : "corrections", (દ્વિગુણ)_injection_rate_hz,
		 (પૂર્ણાંક)_selected_instance.load());
	perf_print_counter(_injected);
	perf_print_counter(_tx_buffer_full);
	perf_print_counter(_framer_full);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	અચલ CorrectionFramerStats stats = _framer.getStats();

	જો (stats.messages_parsed > 0 || stats.crc_errors > 0 || stats.bytes_discarded > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (unsigned)stats.rtcm3_messages, (unsigned)stats.spartn_messages, (unsigned)stats.ubx_messages,
			 (unsigned)stats.crc_errors, (unsigned)stats.bytes_discarded);
	}
}

} // namespace gnss
