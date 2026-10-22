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

کارول نطاق وخت_literals;

نطاق gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ساكن constexpr hrt_abstime k_سرچینه_Timeout = 5_s;
// The injection rate is frames per second over windows this long
ساكن constexpr hrt_abstime k_شرح_کړکۍ = 5_s;
// Start bit, eight data bits, stop bit
ساكن constexpr uint32_t k_بټونه_فی_Byte_On_The_Wire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ساكن constexpr uint32_t kMinRetryDelayUs = 1_ms;
ساكن constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ثابت حرف *name, ثابت حرف *پورټ) :
	Scheduled_Work_توکی(name, px4::serial_پورټ_منزل_wq(پورټ))
{
	strncpy(پورټ_2, پورټ, حجم(پورټ_2) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	لكل (خپلکاری &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	مهالوېش_Clear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_بفر_ډک);
	perf_free(framer_ډک);
	perf_free(_unsupported);
	perf_free(_short_write);
}

فراغ CorrectionInjector::پیل(ثابت Config &config)
{
	شرح_کړکۍ_پیل = hrt_absolute_وخت();
	شرح_کړکۍ_شمېر = perf_event_شمېر(_injected);
	injection_شرح_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	إذا (config.بهیر == بهیر_2::MovingBaseline) {
		_moving_baseline_sub.ثبتوونکی_Callback();

	} وإلا {
		لكل (خپلکاری &sub : _corrections_sub) {
			sub.ثبتوونکی_Callback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	فعال = ریښتیا;
	_write_error_reported = خطأ;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	مهالوېش_Now();
}

فراغ CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	فعال = خطأ;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	لكل (خپلکاری &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	مهالوېش_Now();

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

عائم CorrectionInjector::injection_شرح_hz_2()
{
	ثابت hrt_abstime now = hrt_absolute_وخت();

	إذا (now - شرح_کړکۍ_پیل >= k_شرح_کړکۍ) {
		ثابت uint64_t شمېر = perf_event_شمېر(_injected);
		injection_شرح_hz = (عائم)(شمېر - شرح_کړکۍ_شمېر) * 1e6f / (عائم)(now - شرح_کړکۍ_پیل);
		شرح_کړکۍ_شمېر = شمېر;
		شرح_کړکۍ_پیل = now;
	}

	أرجع injection_شرح_hz;
}

فراغ CorrectionInjector::پرلپسې()
{
	pthread_mutex_lock(&_mutex);

	إذا (!فعال) {
		إذا (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} وإلا {
		إذا (_fd < 0) {
			_fd = ::open(پورټ_2, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			إذا (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", پورټ_2, errno);
				_write_error_reported = ریښتیا;
			}
		}

		إذا (_fd >= 0) {
			إذا (_config.بهیر == بهیر_2::MovingBaseline) {
				drain_moving_baseline();

			} وإلا {
				drain_corrections();
			}

			inject_چوکاټونه();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

فراغ CorrectionInjector::drain_corrections()
{
	rtcm_معلومات_s chunk;

	إذا (select_corrections_instance(chunk)) {
		جمع_کړه_chunk(chunk);
	}

	ثابت int8_t selected = _selected_instance.بارول();

	إذا (selected < 0) {
		أرجع;
	}

	لكل (بینښه n = 0; n < rtcm_معلومات_s::ORB_کتار_اوږدتیا && _corrections_sub[selected].update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			جمع_کړه_chunk(chunk);
		}
	}
}

فراغ CorrectionInjector::drain_moving_baseline()
{
	rtcm_معلومات_s chunk;

	لكل (بینښه n = 0; n < rtcm_معلومات_s::ORB_کتار_اوږدتیا && _moving_baseline_sub.update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			جمع_کړه_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
منطقي CorrectionInjector::select_corrections_instance(rtcm_معلومات_s &chunk)
{
	إذا (_selected_instance.بارول() >= 0 && hrt_elapsed_وخت(&وروستی_chunk_وخت) < k_سرچینه_Timeout) {
		أرجع خطأ;
	}

	_selected_instance.store(-1);

	لكل (int8_t instance = 0; instance < rtcm_معلومات_s::MAX_INSTANCES; instance++) {
		لكل (بینښه n = 0; n < rtcm_معلومات_s::ORB_کتار_اوږدتیا && _corrections_sub[instance].update(&chunk); n++) {
			إذا (chunk.device_id != _config.own_device_id && hrt_elapsed_وخت(&chunk.timestamp) < k_سرچینه_Timeout) {
				_selected_instance.store(instance);
				أرجع ریښتیا;
			}
		}
	}

	أرجع خطأ;
}

فراغ CorrectionInjector::جمع_کړه_chunk(ثابت rtcm_معلومات_s &chunk)
{
	ثابت size_t len = math::min<size_t>(chunk.len, حجم(chunk.معلومات));

	إذا (_framer.جمع_کړه_معلومات(chunk.معلومات, len) < len) {
		perf_شمېر(framer_ډک);
	}

	وروستی_chunk_وخت = hrt_absolute_وخت();
}

فراغ CorrectionInjector::inject_چوکاټونه()
{
	size_t چوکاټ_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *چوکاټ = تش_اشاره;

	طالما ((چوکاټ = _framer.get_بل_پیغام(&چوکاټ_len, &protocol)) != تش_اشاره) {
		إذا ((_config.protocols & protocol_بټ(protocol)) == 0) {
			_framer.consume_پیغام(چوکاټ_len);
			perf_شمېر(_unsupported);
			استمر;
		}

		ثابت ssize_t tx_space = tx_space_شته();

		// Unknown on POSIX, where the write itself is the check
		إذا (tx_space >= 0 && tx_space < (ssize_t)چوکاټ_len) {
			perf_شمېر(tx_بفر_ډک);
			مهالوېش_Delayed(drain_وخت_us(چوکاټ_len - tx_space));
			اكسر;
		}

		ثابت ssize_t written = ::اكتب(_fd, چوکاټ, چوکاټ_len);

		إذا (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_شمېر(tx_بفر_ډک);
			مهالوېش_Delayed(drain_وخت_us(چوکاټ_len));
			اكسر;
		}

		إذا (written != (ssize_t)چوکاټ_len) {
			perf_شمېر(_short_write);

			إذا (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", پورټ_2, errno);
				_write_error_reported = ریښتیا;
			}
		}

		_framer.consume_پیغام(چوکاټ_len);
		perf_شمېر(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_شته() ثابت
{
#إذا defined(FIONSPACE)
	صحيح space = 0;

	إذا (::ioctl(_fd, FIONSPACE, &space) == 0) {
		أرجع space;
	}

#د_که_پای
	أرجع -1;
}

uint32_t CorrectionInjector::drain_وخت_us(size_t بایټونه) ثابت
{
	ثابت uint32_t baudrate = _config.baudrate;

	إذا (baudrate == 0) {
		أرجع kFallbackRetryDelayUs;
	}

	أرجع math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)بایټونه * k_بټونه_فی_Byte_On_The_Wire * 1_s / baudrate);
}

فراغ CorrectionInjector::print_حالت() ثابت
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.بهیر == بهیر_2::MovingBaseline ? "moving baseline" : "corrections", (مزدوج)injection_شرح_hz,
		 (صحيح)_selected_instance.بارول());
	perf_print_counter(_injected);
	perf_print_counter(tx_بفر_ډک);
	perf_print_counter(framer_ډک);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ثابت CorrectionFramerStats stats = _framer.getStats();

	إذا (stats.messages_parsed > 0 || stats.دوراني_ازموینه_errors_2 > 0 || stats.بایټونه_discarded_2 > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (بینښه)stats.rtcm3_messages, (بینښه)stats.spartn_messages, (بینښه)stats.ubx_messages,
			 (بینښه)stats.دوراني_ازموینه_errors_2, (بینښه)stats.بایټونه_discarded_2);
	}
}

} // namespace gnss
