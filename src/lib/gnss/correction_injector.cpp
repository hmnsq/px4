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

ورتوں نطاق ویلا_literals;

نطاق gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ساكن constexpr hrt_abstime k_سروت_Timeout = 5_s;
// The injection rate is frames per second over windows this long
ساكن constexpr hrt_abstime k_شرح_کھڑکی = 5_s;
// Start bit, eight data bits, stop bit
ساكن constexpr uint32_t k_بٹ_فی_Byte_On_The_Wire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ساكن constexpr uint32_t kMinRetryDelayUs = 1_ms;
ساكن constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ثابت حرف *name, ثابت حرف *پورٹ) :
	Scheduled_Work_شے(name, px4::serial_پورٹ_منزل_wq(پورٹ))
{
	strncpy(پورٹ_2, پورٹ, حجم(پورٹ_2) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	لكل (خودکار &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	ویلا_سارنی_Clear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_بفر_بھریا);
	perf_free(framer_بھریا);
	perf_free(_unsupported);
	perf_free(_short_write);
}

فراغ CorrectionInjector::شروع(ثابت Config &config)
{
	شرح_کھڑکی_شروع = hrt_absolute_ویلا();
	شرح_کھڑکی_گنتی = perf_event_گنتی(_injected);
	injection_شرح_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	إذا (config.دھارا == دھارا_2::MovingBaseline) {
		_moving_baseline_sub.رجسٹر_Callback();

	} وإلا {
		لكل (خودکار &sub : _corrections_sub) {
			sub.رجسٹر_Callback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	سرگرم = سچ;
	_write_error_reported = خطأ;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	ویلا_سارنی_Now();
}

فراغ CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	سرگرم = خطأ;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	لكل (خودکار &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	ویلا_سارنی_Now();

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
	ثابت hrt_abstime now = hrt_absolute_ویلا();

	إذا (now - شرح_کھڑکی_شروع >= k_شرح_کھڑکی) {
		ثابت uint64_t گنتی = perf_event_گنتی(_injected);
		injection_شرح_hz = (عائم)(گنتی - شرح_کھڑکی_گنتی) * 1e6f / (عائم)(now - شرح_کھڑکی_شروع);
		شرح_کھڑکی_گنتی = گنتی;
		شرح_کھڑکی_شروع = now;
	}

	أرجع injection_شرح_hz;
}

فراغ CorrectionInjector::لگاتار()
{
	pthread_mutex_lock(&_mutex);

	إذا (!سرگرم) {
		إذا (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} وإلا {
		إذا (_fd < 0) {
			_fd = ::open(پورٹ_2, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			إذا (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", پورٹ_2, errno);
				_write_error_reported = سچ;
			}
		}

		إذا (_fd >= 0) {
			إذا (_config.دھارا == دھارا_2::MovingBaseline) {
				drain_moving_baseline();

			} وإلا {
				drain_corrections();
			}

			inject_چوکھٹے();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

فراغ CorrectionInjector::drain_corrections()
{
	rtcm_انکڑے_s chunk;

	إذا (select_corrections_instance(chunk)) {
		جوڑو_chunk(chunk);
	}

	ثابت int8_t selected = _selected_instance.لدو();

	إذا (selected < 0) {
		أرجع;
	}

	لكل (بنانشانی n = 0; n < rtcm_انکڑے_s::ORB_قطار_لمائی && _corrections_sub[selected].update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			جوڑو_chunk(chunk);
		}
	}
}

فراغ CorrectionInjector::drain_moving_baseline()
{
	rtcm_انکڑے_s chunk;

	لكل (بنانشانی n = 0; n < rtcm_انکڑے_s::ORB_قطار_لمائی && _moving_baseline_sub.update(&chunk); n++) {
		إذا (chunk.device_id != _config.own_device_id) {
			جوڑو_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
منطقي CorrectionInjector::select_corrections_instance(rtcm_انکڑے_s &chunk)
{
	إذا (_selected_instance.لدو() >= 0 && hrt_elapsed_ویلا(&آخری_chunk_ویلا) < k_سروت_Timeout) {
		أرجع خطأ;
	}

	_selected_instance.store(-1);

	لكل (int8_t instance = 0; instance < rtcm_انکڑے_s::MAX_INSTANCES; instance++) {
		لكل (بنانشانی n = 0; n < rtcm_انکڑے_s::ORB_قطار_لمائی && _corrections_sub[instance].update(&chunk); n++) {
			إذا (chunk.device_id != _config.own_device_id && hrt_elapsed_ویلا(&chunk.timestamp) < k_سروت_Timeout) {
				_selected_instance.store(instance);
				أرجع سچ;
			}
		}
	}

	أرجع خطأ;
}

فراغ CorrectionInjector::جوڑو_chunk(ثابت rtcm_انکڑے_s &chunk)
{
	ثابت size_t len = math::min<size_t>(chunk.len, حجم(chunk.انکڑے));

	إذا (_framer.جوڑو_انکڑے(chunk.انکڑے, len) < len) {
		perf_گنتی(framer_بھریا);
	}

	آخری_chunk_ویلا = hrt_absolute_ویلا();
}

فراغ CorrectionInjector::inject_چوکھٹے()
{
	size_t چوکھٹا_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *چوکھٹا = خالی_سوچک;

	طالما ((چوکھٹا = _framer.get_اگلا_سنیہا(&چوکھٹا_len, &protocol)) != خالی_سوچک) {
		إذا ((_config.protocols & protocol_ثنائی_ہندسہ(protocol)) == 0) {
			_framer.consume_سنیہا(چوکھٹا_len);
			perf_گنتی(_unsupported);
			استمر;
		}

		ثابت ssize_t tx_space = tx_space_دستیاب();

		// Unknown on POSIX, where the write itself is the check
		إذا (tx_space >= 0 && tx_space < (ssize_t)چوکھٹا_len) {
			perf_گنتی(tx_بفر_بھریا);
			ویلا_سارنی_Delayed(drain_ویلا_us(چوکھٹا_len - tx_space));
			اكسر;
		}

		ثابت ssize_t written = ::اكتب(_fd, چوکھٹا, چوکھٹا_len);

		إذا (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_گنتی(tx_بفر_بھریا);
			ویلا_سارنی_Delayed(drain_ویلا_us(چوکھٹا_len));
			اكسر;
		}

		إذا (written != (ssize_t)چوکھٹا_len) {
			perf_گنتی(_short_write);

			إذا (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", پورٹ_2, errno);
				_write_error_reported = سچ;
			}
		}

		_framer.consume_سنیہا(چوکھٹا_len);
		perf_گنتی(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_دستیاب() ثابت
{
#إذا defined(FIONSPACE)
	صحيح space = 0;

	إذا (::ioctl(_fd, FIONSPACE, &space) == 0) {
		أرجع space;
	}

#جے_انت
	أرجع -1;
}

uint32_t CorrectionInjector::drain_ویلا_us(size_t بائٹاں) ثابت
{
	ثابت uint32_t baudrate = _config.baudrate;

	إذا (baudrate == 0) {
		أرجع kFallbackRetryDelayUs;
	}

	أرجع math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)بائٹاں * k_بٹ_فی_Byte_On_The_Wire * 1_s / baudrate);
}

فراغ CorrectionInjector::print_حالت() ثابت
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.دھارا == دھارا_2::MovingBaseline ? "moving baseline" : "corrections", (مزدوج)injection_شرح_hz,
		 (صحيح)_selected_instance.لدو());
	perf_print_counter(_injected);
	perf_print_counter(tx_بفر_بھریا);
	perf_print_counter(framer_بھریا);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	ثابت CorrectionFramerStats stats = _framer.getStats();

	إذا (stats.messages_parsed > 0 || stats.چکری_جانچ_errors_2 > 0 || stats.بائٹاں_discarded_2 > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (بنانشانی)stats.rtcm3_messages, (بنانشانی)stats.spartn_messages, (بنانشانی)stats.ubx_messages,
			 (بنانشانی)stats.چکری_جانچ_errors_2, (بنانشانی)stats.بائٹاں_discarded_2);
	}
}

} // namespace gnss
