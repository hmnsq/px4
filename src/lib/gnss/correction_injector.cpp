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

استعمال نطاق وقت_literals;

نطاق gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ساكن constexpr hrt_abstime k_ذريعو_Timeout = 5_s;
// The injection rate is frames per second over windows this long
ساكن constexpr hrt_abstime k_شرح_دري = 5_s;
// Start bit, eight data bits, stop bit
ساكن constexpr uint32_t k_بٽ_في_Byte_On_The_Wire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ساكن constexpr uint32_t kMinRetryDelayUs = 1_ms;
ساكن constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ثابت حرف *name, ثابت حرف *پورٽ) :
	Scheduled_Work_شي(name, px4::serial_پورٽ_منزل_wq(پورٽ))
{
	strncpy(پورٽ_2, پورٽ, حجم(پورٽ_2) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	لكل (خودڪار &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	وقت_جدول_Clear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_بفر_ڀريل);
	perf_free(framer_ڀريل);
	perf_free(_unsupported);
	perf_free(_short_write);
}

فراغ CorrectionInjector::شروعات(ثابت Config &config)
{
	شرح_دري_شروعات = hrt_absolute_وقت();
	شرح_دري_ڳڻپ = perf_event_ڳڻپ(_injected);
	injection_شرح_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	إذا (config.وهڪرو == وهڪرو_2::MovingBaseline) {
		_moving_baseline_sub.رجسٽر_Callback();

	} وإلا {
		لكل (خودڪار &sub : _corrections_sub) {
			sub.رجسٽر_Callback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	سرگرم = سچ;
	_write_error_reported = خطأ;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	وقت_جدول_Now();
}

فراغ CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	سرگرم = خطأ;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	لكل (خودڪار &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	وقت_جدول_Now();

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
	ثابت hrt_abstime now = hrt_absolute_وقت();

	إذا (now - شرح_دري_شروعات >= k_شرح_دري) {
		ثابت uint64_t ڳڻپ = perf_event_ڳڻپ(_injected);
		injection_شرح_hz = (عائم)(ڳڻپ - شرح_دري_ڳڻپ) * 1e6f / (عائم)(now - شرح_دري_شروعات);
		شرح_دري_ڳڻپ = ڳڻپ;
		شرح_دري_شروعات = now;
	}

	أرجع injection_شرح_hz;
}

فراغ CorrectionInjector::لاڳيتو()
{
	pthread_mutex_lock(&_mutex);

	إذا (!سرگرم) {
		إذا (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} وإلا {
		إذا (_fd < 0) {
			_fd = ::open(پورٽ_2, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			إذا (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", پورٽ_2, errno);
				_write_error_reported = سچ;
			}
		}

		إذا (_fd >= 0) {
			إذا (_config.وهڪرو == وهڪرو_2::MovingBaseline) {
				drain_moving_baseline();

			} وإلا {
				drain_corrections();
			}

			inject_چوکٺا();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

فراغ CorrectionInjector::drain_corrections()
{
	rtcm_معلومات_s chunk;

	إذا (select_corrections_instance(chunk)) {
		جوڙيو_chunk(chunk);
	}

	ثابت int8_t selected = _selected_instance.ڀريو();

	إذا (selected < 0) {
		أرجع;
	}

	لكل (بنانشان n = 0; n < rtcm_معلومات_s::ORB_قطار_ڊيگهائي && _corrections_sub[selected].update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			جوڙيو_chunk(chunk);
		}
	}
}

فراغ CorrectionInjector::drain_moving_baseline()
{
	rtcm_معلومات_s chunk;

	لكل (بنانشان n = 0; n < rtcm_معلومات_s::ORB_قطار_ڊيگهائي && _moving_baseline_sub.update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			جوڙيو_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
منطقي CorrectionInjector::select_corrections_instance(rtcm_معلومات_s &chunk)
{
	إذا (_selected_instance.ڀريو() >= 0 && hrt_elapsed_وقت(&آخري_chunk_وقت) < k_ذريعو_Timeout) {
		أرجع خطأ;
	}

	_selected_instance.store(-1);

	لكل (int8_t instance = 0; instance < rtcm_معلومات_s::MAX_INSTANCES; instance++) {
		لكل (بنانشان n = 0; n < rtcm_معلومات_s::ORB_قطار_ڊيگهائي && _corrections_sub[instance].update(&chunk); n++) {
			إذا (chunk.device_id != _config.own_device_id && hrt_elapsed_وقت(&chunk.timestamp) < k_ذريعو_Timeout) {
				_selected_instance.store(instance);
				أرجع سچ;
			}
		}
	}

	أرجع خطأ;
}

فراغ CorrectionInjector::جوڙيو_chunk(ثابت rtcm_معلومات_s &chunk)
{
	ثابت size_t len = math::min<size_t>(chunk.len, حجم(chunk.معلومات));

	إذا (_framer.جوڙيو_معلومات(chunk.معلومات, len) < len) {
		perf_ڳڻپ(framer_ڀريل);
	}

	آخري_chunk_وقت = hrt_absolute_وقت();
}

فراغ CorrectionInjector::inject_چوکٺا()
{
	size_t چوکٺو_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *چوکٺو = خالي_اشارو;

	طالما ((چوکٺو = _framer.get_ايندڙ_پيغام(&چوکٺو_len, &protocol)) != خالي_اشارو) {
		إذا ((_config.protocols & protocol_ٻٽو_انگ(protocol)) == 0) {
			_framer.consume_پيغام(چوکٺو_len);
			perf_ڳڻپ(_unsupported);
			استمر;
		}

		ثابت ssize_t tx_space = tx_space_موجود();

		// Unknown on POSIX, where the write itself is the check
		إذا (tx_space >= 0 && tx_space < (ssize_t)چوکٺو_len) {
			perf_ڳڻپ(tx_بفر_ڀريل);
			وقت_جدول_Delayed(drain_وقت_us(چوکٺو_len - tx_space));
			اكسر;
		}

		ثابت ssize_t written = ::اكتب(_fd, چوکٺو, چوکٺو_len);

		إذا (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_ڳڻپ(tx_بفر_ڀريل);
			وقت_جدول_Delayed(drain_وقت_us(چوکٺو_len));
			اكسر;
		}

		إذا (written != (ssize_t)چوکٺو_len) {
			perf_ڳڻپ(_short_write);

			إذا (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", پورٽ_2, errno);
				_write_error_reported = سچ;
			}
		}

		_framer.consume_پيغام(چوکٺو_len);
		perf_ڳڻپ(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_موجود() ثابت
{
#إذا defined(FIONSPACE)
	صحيح space = 0;

	إذا (::ioctl(_fd, FIONSPACE, &space) == 0) {
		أرجع space;
	}

#جيڪڏهن_پورو
	أرجع -1;
}

uint32_t CorrectionInjector::drain_وقت_us(size_t بائيٽون) ثابت
{
	ثابت uint32_t baudrate = _config.baudrate;

	إذا (baudrate == 0) {
		أرجع kFallbackRetryDelayUs;
	}

	أرجع math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)بائيٽون * k_بٽ_في_Byte_On_The_Wire * 1_s / baudrate);
}

فراغ CorrectionInjector::print_حالت() ثابت
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.وهڪرو == وهڪرو_2::MovingBaseline ? "moving baseline" : "corrections", (مزدوج)injection_شرح_hz,
		 (صحيح)_selected_instance.ڀريو());
	perf_print_counter(_injected);
	perf_print_counter(tx_بفر_ڀريل);
	perf_print_counter(framer_ڀريل);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ثابت CorrectionFramerStats stats = _framer.getStats();

	إذا (stats.messages_parsed > 0 || stats.چڪري_جاچ_errors_2 > 0 || stats.بائيٽون_discarded_2 > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (بنانشان)stats.rtcm3_messages, (بنانشان)stats.spartn_messages, (بنانشان)stats.ubx_messages,
			 (بنانشان)stats.چڪري_جاچ_errors_2, (بنانشان)stats.بائيٽون_discarded_2);
	}
}

} // namespace gnss
