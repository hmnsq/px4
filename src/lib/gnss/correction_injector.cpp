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


#അന്തര്ഭാവ "correction_injector.h"

#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ <mathlib/mathlib.h>
#അന്തര്ഭാവ <px4_platform_common/log.h>
#അന്തര്ഭാവ <px4_platform_common/posix.h>
#അന്തര്ഭാവ <px4_platform_common/time.h>

using നാമാകാശ time_literals;

നാമാകാശ gnss
{

// Without a chunk from the selected source for this long, another source may be taken
സ്ഥിതം constexpr hrt_abstime kSourceTimeout = 5_s;
// The injection rate is frames per second over windows this long
സ്ഥിതം constexpr hrt_abstime kRateWindow = 5_s;
// Start bit, eight data bits, stop bit
സ്ഥിതം constexpr uint32_t kBitsPerByteOnTheWire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
സ്ഥിതം constexpr uint32_t kMinRetryDelayUs = 1_ms;
സ്ഥിതം constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *port) :
	ScheduledWorkItem(name, px4::serial_port_to_wq(port))
{
	strncpy(_port, port, വലുപ്പം(_port) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	വേണ്ടി (auto &sub : _corrections_sub) {
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

ശൂന്യം CorrectionInjector::start(സ്ഥിരം Config &config)
{
	_rate_window_start = hrt_absolute_time();
	rate_window_എണ്ണം = perf_event_എണ്ണം(_injected);
	_injection_rate_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	എങ്കിൽ (config.stream == Stream::MovingBaseline) {
		_moving_baseline_sub.registerCallback();

	} അല്ലെങ്കിൽ {
		വേണ്ടി (auto &sub : _corrections_sub) {
			sub.registerCallback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	_active = സത്യം;
	_write_error_reported = അസത്യം;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ScheduleNow();
}

ശൂന്യം CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	_active = അസത്യം;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	വേണ്ടി (auto &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ScheduleNow();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		സ്ഥിരം തർക്കം closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		എങ്കിൽ (closed) {
			നിർത്തുക;
		}

		px4_usleep(5_ms);
	}
}

ദശാംശം CorrectionInjector::injection_rate_hz()
{
	സ്ഥിരം hrt_abstime now = hrt_absolute_time();

	എങ്കിൽ (now - _rate_window_start >= kRateWindow) {
		സ്ഥിരം uint64_t എണ്ണം = perf_event_എണ്ണം(_injected);
		_injection_rate_hz = (ദശാംശം)(എണ്ണം - rate_window_എണ്ണം) * 1e6f / (ദശാംശം)(now - _rate_window_start);
		rate_window_എണ്ണം = എണ്ണം;
		_rate_window_start = now;
	}

	തിരികെ _injection_rate_hz;
}

ശൂന്യം CorrectionInjector::Run()
{
	pthread_mutex_lock(&_mutex);

	എങ്കിൽ (!_active) {
		എങ്കിൽ (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} അല്ലെങ്കിൽ {
		എങ്കിൽ (_fd < 0) {
			_fd = ::open(_port, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			എങ്കിൽ (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", _port, errno);
				_write_error_reported = സത്യം;
			}
		}

		എങ്കിൽ (_fd >= 0) {
			എങ്കിൽ (_config.stream == Stream::MovingBaseline) {
				drain_moving_baseline();

			} അല്ലെങ്കിൽ {
				drain_corrections();
			}

			inject_frames();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

ശൂന്യം CorrectionInjector::drain_corrections()
{
	rtcm_data_s chunk;

	എങ്കിൽ (select_corrections_instance(chunk)) {
		add_chunk(chunk);
	}

	സ്ഥിരം int8_t selected = _selected_instance.load();

	എങ്കിൽ (selected < 0) {
		തിരികെ;
	}

	വേണ്ടി (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[selected].update(&chunk); n++) {
		എങ്കിൽ (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

ശൂന്യം CorrectionInjector::drain_moving_baseline()
{
	rtcm_data_s chunk;

	വേണ്ടി (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _moving_baseline_sub.update(&chunk); n++) {
		എങ്കിൽ (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
തർക്കം CorrectionInjector::select_corrections_instance(rtcm_data_s &chunk)
{
	എങ്കിൽ (_selected_instance.load() >= 0 && hrt_elapsed_time(&_last_chunk_time) < kSourceTimeout) {
		തിരികെ അസത്യം;
	}

	_selected_instance.store(-1);

	വേണ്ടി (int8_t instance = 0; instance < rtcm_data_s::MAX_INSTANCES; instance++) {
		വേണ്ടി (unsigned n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[instance].update(&chunk); n++) {
			എങ്കിൽ (chunk.device_id != _config.own_device_id && hrt_elapsed_time(&chunk.timestamp) < kSourceTimeout) {
				_selected_instance.store(instance);
				തിരികെ സത്യം;
			}
		}
	}

	തിരികെ അസത്യം;
}

ശൂന്യം CorrectionInjector::add_chunk(സ്ഥിരം rtcm_data_s &chunk)
{
	സ്ഥിരം size_t len = math::min<size_t>(chunk.len, വലുപ്പം(chunk.data));

	എങ്കിൽ (_framer.addData(chunk.data, len) < len) {
		perf_എണ്ണം(_framer_full);
	}

	_last_chunk_time = hrt_absolute_time();
}

ശൂന്യം CorrectionInjector::inject_frames()
{
	size_t frame_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	സ്ഥിരം uint8_t *frame = nullptr;

	ആകുമ്പോൾ ((frame = _framer.getNextMessage(&frame_len, &protocol)) != nullptr) {
		എങ്കിൽ ((_config.protocols & protocol_bit(protocol)) == 0) {
			_framer.consumeMessage(frame_len);
			perf_എണ്ണം(_unsupported);
			തുടരുക;
		}

		സ്ഥിരം ssize_t tx_space = tx_space_available();

		// Unknown on POSIX, where the write itself is the check
		എങ്കിൽ (tx_space >= 0 && tx_space < (ssize_t)frame_len) {
			perf_എണ്ണം(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len - tx_space));
			നിർത്തുക;
		}

		സ്ഥിരം ssize_t written = ::ലിഖ(_fd, frame, frame_len);

		എങ്കിൽ (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_എണ്ണം(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len));
			നിർത്തുക;
		}

		എങ്കിൽ (written != (ssize_t)frame_len) {
			perf_എണ്ണം(_short_write);

			എങ്കിൽ (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", _port, errno);
				_write_error_reported = സത്യം;
			}
		}

		_framer.consumeMessage(frame_len);
		perf_എണ്ണം(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_available() സ്ഥിരം
{
#എങ്കിൽ defined(FIONSPACE)
	പൂർണ്ണസംഖ്യ space = 0;

	എങ്കിൽ (::ioctl(_fd, FIONSPACE, &space) == 0) {
		തിരികെ space;
	}

#endif
	തിരികെ -1;
}

uint32_t CorrectionInjector::drain_time_us(size_t bytes) സ്ഥിരം
{
	സ്ഥിരം uint32_t baudrate = _config.baudrate;

	എങ്കിൽ (baudrate == 0) {
		തിരികെ kFallbackRetryDelayUs;
	}

	തിരികെ math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)bytes * kBitsPerByteOnTheWire * 1_s / baudrate);
}

ശൂന്യം CorrectionInjector::print_status() സ്ഥിരം
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.stream == Stream::MovingBaseline ? "moving baseline" : "corrections", (ഇരട്ട)_injection_rate_hz,
		 (പൂർണ്ണസംഖ്യ)_selected_instance.load());
	perf_print_counter(_injected);
	perf_print_counter(_tx_buffer_full);
	perf_print_counter(_framer_full);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	സ്ഥിരം CorrectionFramerStats stats = _framer.getStats();

	എങ്കിൽ (stats.messages_parsed > 0 || stats.crc_errors > 0 || stats.bytes_discarded > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (unsigned)stats.rtcm3_messages, (unsigned)stats.spartn_messages, (unsigned)stats.ubx_messages,
			 (unsigned)stats.crc_errors, (unsigned)stats.bytes_discarded);
	}
}

} // namespace gnss
