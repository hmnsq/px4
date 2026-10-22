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


#अन्तर्भाव "correction_injector.h"

#अन्तर्भाव <errno.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <px4_platform_common/log.h>
#अन्तर्भाव <px4_platform_common/posix.h>
#अन्तर्भाव <px4_platform_common/time.h>

using नामाकाश time_literals;

नामाकाश gnss
{

// Without a chunk from the selected source for this long, another source may be taken
स्थिर constexpr hrt_abstime kSourceTimeout = 5_s;
// The injection rate is frames per second over windows this long
स्थिर constexpr hrt_abstime kRateWindow = 5_s;
// Start bit, eight data bits, stop bit
स्थिर constexpr uint32_t kBitsPerByteOnTheWire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
स्थिर constexpr uint32_t kMinRetryDelayUs = 1_ms;
स्थिर constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(अचर वर्ण *name, अचर वर्ण *port) :
	ScheduledWorkItem(name, px4::serial_port_to_wq(port))
{
	म_नकलन(_port, port, परिमाण(_port) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	लागि (स्वतः &sub : _corrections_sub) {
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

शून्य CorrectionInjector::start(अचर Config &config)
{
	_rate_window_start = hrt_absolute_time();
	rate_window_गन्ती = perf_event_गन्ती(_injected);
	_injection_rate_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	यदि (config.stream == Stream::MovingBaseline) {
		_moving_baseline_sub.registerCallback();

	} अन्यथा {
		लागि (स्वतः &sub : _corrections_sub) {
			sub.registerCallback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	_active = सत्य;
	_write_error_reported = असत्य;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ScheduleNow();
}

शून्य CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	_active = असत्य;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	लागि (स्वतः &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ScheduleNow();

	लागि (पूर्णाङ्क i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		अचर तार्किक closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		यदि (closed) {
			रोक;
		}

		px4_usleep(5_ms);
	}
}

भिन्न CorrectionInjector::injection_rate_hz()
{
	अचर hrt_abstime now = hrt_absolute_time();

	यदि (now - _rate_window_start >= kRateWindow) {
		अचर uint64_t गन्ती = perf_event_गन्ती(_injected);
		_injection_rate_hz = (भिन्न)(गन्ती - rate_window_गन्ती) * 1e6f / (भिन्न)(now - _rate_window_start);
		rate_window_गन्ती = गन्ती;
		_rate_window_start = now;
	}

	फर्काऊ _injection_rate_hz;
}

शून्य CorrectionInjector::Run()
{
	pthread_mutex_lock(&_mutex);

	यदि (!_active) {
		यदि (_fd >= 0) {
			::बंद(_fd);
			_fd = -1;
		}

	} अन्यथा {
		यदि (_fd < 0) {
			_fd = ::खोलो(_port, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			यदि (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", _port, त्रुटि_सं);
				_write_error_reported = सत्य;
			}
		}

		यदि (_fd >= 0) {
			यदि (_config.stream == Stream::MovingBaseline) {
				drain_moving_baseline();

			} अन्यथा {
				drain_corrections();
			}

			inject_frames();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

शून्य CorrectionInjector::drain_corrections()
{
	rtcm_data_s chunk;

	यदि (select_corrections_instance(chunk)) {
		add_chunk(chunk);
	}

	अचर int8_t selected = _selected_instance.load();

	यदि (selected < 0) {
		फर्काऊ;
	}

	लागि (अचिन्हित n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[selected].update(&chunk); n++) {
		यदि (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

शून्य CorrectionInjector::drain_moving_baseline()
{
	rtcm_data_s chunk;

	लागि (अचिन्हित n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _moving_baseline_sub.update(&chunk); n++) {
		यदि (chunk.device_id != _config.own_device_id) {
			add_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
तार्किक CorrectionInjector::select_corrections_instance(rtcm_data_s &chunk)
{
	यदि (_selected_instance.load() >= 0 && hrt_elapsed_time(&_last_chunk_time) < kSourceTimeout) {
		फर्काऊ असत्य;
	}

	_selected_instance.store(-1);

	लागि (int8_t instance = 0; instance < rtcm_data_s::MAX_INSTANCES; instance++) {
		लागि (अचिन्हित n = 0; n < rtcm_data_s::ORB_QUEUE_LENGTH && _corrections_sub[instance].update(&chunk); n++) {
			यदि (chunk.device_id != _config.own_device_id && hrt_elapsed_time(&chunk.timestamp) < kSourceTimeout) {
				_selected_instance.store(instance);
				फर्काऊ सत्य;
			}
		}
	}

	फर्काऊ असत्य;
}

शून्य CorrectionInjector::add_chunk(अचर rtcm_data_s &chunk)
{
	अचर माप_प्रकार len = math::min<माप_प्रकार>(chunk.len, परिमाण(chunk.data));

	यदि (_framer.addData(chunk.data, len) < len) {
		perf_गन्ती(_framer_full);
	}

	_last_chunk_time = hrt_absolute_time();
}

शून्य CorrectionInjector::inject_frames()
{
	माप_प्रकार frame_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	अचर uint8_t *frame = nullptr;

	जबसम्म ((frame = _framer.getNextMessage(&frame_len, &protocol)) != nullptr) {
		यदि ((_config.protocols & protocol_bit(protocol)) == 0) {
			_framer.consumeMessage(frame_len);
			perf_गन्ती(_unsupported);
			जारी;
		}

		अचर ssize_t tx_space = tx_space_available();

		// Unknown on POSIX, where the write itself is the check
		यदि (tx_space >= 0 && tx_space < (ssize_t)frame_len) {
			perf_गन्ती(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len - tx_space));
			रोक;
		}

		अचर ssize_t written = ::लिख(_fd, frame, frame_len);

		यदि (written < 0 && त्रुटि_सं == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_गन्ती(_tx_buffer_full);
			ScheduleDelayed(drain_time_us(frame_len));
			रोक;
		}

		यदि (written != (ssize_t)frame_len) {
			perf_गन्ती(_short_write);

			यदि (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", _port, त्रुटि_सं);
				_write_error_reported = सत्य;
			}
		}

		_framer.consumeMessage(frame_len);
		perf_गन्ती(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_available() अचर
{
#यदि defined(FIONSPACE)
	पूर्णाङ्क space = 0;

	यदि (::ioctl(_fd, FIONSPACE, &space) == 0) {
		फर्काऊ space;
	}

#endif
	फर्काऊ -1;
}

uint32_t CorrectionInjector::drain_time_us(माप_प्रकार bytes) अचर
{
	अचर uint32_t baudrate = _config.baudrate;

	यदि (baudrate == 0) {
		फर्काऊ kFallbackRetryDelayUs;
	}

	फर्काऊ math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)bytes * kBitsPerByteOnTheWire * 1_s / baudrate);
}

शून्य CorrectionInjector::print_status() अचर
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.stream == Stream::MovingBaseline ? "moving baseline" : "corrections", (द्विगुण)_injection_rate_hz,
		 (पूर्णाङ्क)_selected_instance.load());
	perf_print_counter(_injected);
	perf_print_counter(_tx_buffer_full);
	perf_print_counter(_framer_full);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	अचर CorrectionFramerStats stats = _framer.getStats();

	यदि (stats.messages_parsed > 0 || stats.crc_errors > 0 || stats.bytes_discarded > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (अचिन्हित)stats.rtcm3_messages, (अचिन्हित)stats.spartn_messages, (अचिन्हित)stats.ubx_messages,
			 (अचिन्हित)stats.crc_errors, (अचिन्हित)stats.bytes_discarded);
	}
}

} // namespace gnss
