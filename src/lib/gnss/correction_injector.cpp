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

استفاده نطاق زمان_literals;

نطاق gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ساكن constexpr hrt_abstime k_منبع_Timeout = 5_s;
// The injection rate is frames per second over windows this long
ساكن constexpr hrt_abstime k_نرخ_پنجره = 5_s;
// Start bit, eight data bits, stop bit
ساكن constexpr uint32_t k_بیتها_در_هر_Byte_On_The_Wire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ساكن constexpr uint32_t kMinRetryDelayUs = 1_ms;
ساكن constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ثابت حرف *name, ثابت حرف *پورت) :
	Scheduled_Work_قلم(name, px4::serial_پورت_مقصد_wq(پورت))
{
	strncpy(پورت_2, پورت, حجم(پورت_2) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	لكل (خودکار &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	زمانبندی_Clear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_میانگیر_پر);
	perf_free(framer_پر);
	perf_free(_unsupported);
	perf_free(_short_write);
}

فراغ CorrectionInjector::شروع(ثابت Config &config)
{
	نرخ_پنجره_شروع = hrt_absolute_زمان();
	نرخ_پنجره_شمار = perf_event_شمار(_injected);
	injection_نرخ_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	إذا (config.جریان == جریان_2::MovingBaseline) {
		_moving_baseline_sub.ثبات_Callback();

	} وإلا {
		لكل (خودکار &sub : _corrections_sub) {
			sub.ثبات_Callback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	فعال = درست;
	_write_error_reported = خطأ;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	زمانبندی_Now();
}

فراغ CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	فعال = خطأ;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	لكل (خودکار &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	زمانبندی_Now();

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

عائم CorrectionInjector::injection_نرخ_hz_2()
{
	ثابت hrt_abstime now = hrt_absolute_زمان();

	إذا (now - نرخ_پنجره_شروع >= k_نرخ_پنجره) {
		ثابت uint64_t شمار = perf_event_شمار(_injected);
		injection_نرخ_hz = (عائم)(شمار - نرخ_پنجره_شمار) * 1e6f / (عائم)(now - نرخ_پنجره_شروع);
		نرخ_پنجره_شمار = شمار;
		نرخ_پنجره_شروع = now;
	}

	أرجع injection_نرخ_hz;
}

فراغ CorrectionInjector::پیاپی()
{
	pthread_mutex_lock(&_mutex);

	إذا (!فعال) {
		إذا (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} وإلا {
		إذا (_fd < 0) {
			_fd = ::open(پورت_2, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			إذا (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", پورت_2, errno);
				_write_error_reported = درست;
			}
		}

		إذا (_fd >= 0) {
			إذا (_config.جریان == جریان_2::MovingBaseline) {
				drain_moving_baseline();

			} وإلا {
				drain_corrections();
			}

			inject_قابها();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

فراغ CorrectionInjector::drain_corrections()
{
	rtcm_داده_s chunk;

	إذا (select_corrections_instance(chunk)) {
		جمع_کن_chunk(chunk);
	}

	ثابت int8_t selected = _selected_instance.بارگیری();

	إذا (selected < 0) {
		أرجع;
	}

	لكل (بیعلامت n = 0; n < rtcm_داده_s::ORB_صف_درازی && _corrections_sub[selected].update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			جمع_کن_chunk(chunk);
		}
	}
}

فراغ CorrectionInjector::drain_moving_baseline()
{
	rtcm_داده_s chunk;

	لكل (بیعلامت n = 0; n < rtcm_داده_s::ORB_صف_درازی && _moving_baseline_sub.update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			جمع_کن_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
منطقي CorrectionInjector::select_corrections_instance(rtcm_داده_s &chunk)
{
	إذا (_selected_instance.بارگیری() >= 0 && hrt_elapsed_زمان(&آخرین_chunk_زمان) < k_منبع_Timeout) {
		أرجع خطأ;
	}

	_selected_instance.store(-1);

	لكل (int8_t instance = 0; instance < rtcm_داده_s::MAX_INSTANCES; instance++) {
		لكل (بیعلامت n = 0; n < rtcm_داده_s::ORB_صف_درازی && _corrections_sub[instance].update(&chunk); n++) {
			إذا (chunk.device_id != _config.own_device_id && hrt_elapsed_زمان(&chunk.timestamp) < k_منبع_Timeout) {
				_selected_instance.store(instance);
				أرجع درست;
			}
		}
	}

	أرجع خطأ;
}

فراغ CorrectionInjector::جمع_کن_chunk(ثابت rtcm_داده_s &chunk)
{
	ثابت size_t len = math::min<size_t>(chunk.len, حجم(chunk.داده));

	إذا (_framer.جمع_کن_داده(chunk.داده, len) < len) {
		perf_شمار(framer_پر);
	}

	آخرین_chunk_زمان = hrt_absolute_زمان();
}

فراغ CorrectionInjector::inject_قابها()
{
	size_t قاب_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *قاب = اشاره_تهی;

	طالما ((قاب = _framer.get_بعدی_پیام(&قاب_len, &protocol)) != اشاره_تهی) {
		إذا ((_config.protocols & protocol_بیت(protocol)) == 0) {
			_framer.consume_پیام(قاب_len);
			perf_شمار(_unsupported);
			استمر;
		}

		ثابت ssize_t tx_space = tx_space_موجود();

		// Unknown on POSIX, where the write itself is the check
		إذا (tx_space >= 0 && tx_space < (ssize_t)قاب_len) {
			perf_شمار(tx_میانگیر_پر);
			زمانبندی_Delayed(drain_زمان_us(قاب_len - tx_space));
			اكسر;
		}

		ثابت ssize_t written = ::اكتب(_fd, قاب, قاب_len);

		إذا (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_شمار(tx_میانگیر_پر);
			زمانبندی_Delayed(drain_زمان_us(قاب_len));
			اكسر;
		}

		إذا (written != (ssize_t)قاب_len) {
			perf_شمار(_short_write);

			إذا (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", پورت_2, errno);
				_write_error_reported = درست;
			}
		}

		_framer.consume_پیام(قاب_len);
		perf_شمار(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_موجود() ثابت
{
#إذا defined(FIONSPACE)
	صحيح space = 0;

	إذا (::ioctl(_fd, FIONSPACE, &space) == 0) {
		أرجع space;
	}

#پایان_اگر
	أرجع -1;
}

uint32_t CorrectionInjector::drain_زمان_us(size_t بایتها) ثابت
{
	ثابت uint32_t baudrate = _config.baudrate;

	إذا (baudrate == 0) {
		أرجع kFallbackRetryDelayUs;
	}

	أرجع math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)بایتها * k_بیتها_در_هر_Byte_On_The_Wire * 1_s / baudrate);
}

فراغ CorrectionInjector::print_وضعیت() ثابت
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.جریان == جریان_2::MovingBaseline ? "moving baseline" : "corrections", (مزدوج)injection_نرخ_hz,
		 (صحيح)_selected_instance.بارگیری());
	perf_print_counter(_injected);
	perf_print_counter(tx_میانگیر_پر);
	perf_print_counter(framer_پر);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ثابت CorrectionFramerStats stats = _framer.getStats();

	إذا (stats.messages_parsed > 0 || stats.بررسی_چرخهای_errors_2 > 0 || stats.بایتها_discarded_2 > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (بیعلامت)stats.rtcm3_messages, (بیعلامت)stats.spartn_messages, (بیعلامت)stats.ubx_messages,
			 (بیعلامت)stats.بررسی_چرخهای_errors_2, (بیعلامت)stats.بایتها_discarded_2);
	}
}

} // namespace gnss
