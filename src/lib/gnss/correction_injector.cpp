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

استعمال نامخلا وقت_literals;

نامخلا gnss
{

// Without a chunk from the selected source for this long, another source may be taken
ساکن constexpr hrt_abstime k_ماخذ_Timeout = 5_s;
// The injection rate is frames per second over windows this long
ساکن constexpr hrt_abstime k_شرح_کھڑکی = 5_s;
// Start bit, eight data bits, stop bit
ساکن constexpr uint32_t k_بٹ_فی_Byte_On_The_Wire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
ساکن constexpr uint32_t kMinRetryDelayUs = 1_ms;
ساکن constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(ثابت حرف *name, ثابت حرف *پورٹ) :
	Scheduled_Work_شے(name, px4::serial_پورٹ_مقصد_wq(پورٹ))
{
	strncpy(پورٹ_2, پورٹ, حجم(پورٹ_2) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	برائے (خودکار &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	جدول_وقت_Clear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_بفر_بھرا);
	perf_free(framer_بھرا);
	perf_free(_unsupported);
	perf_free(_short_write);
}

خالی CorrectionInjector::آغاز(ثابت Config &config)
{
	شرح_کھڑکی_آغاز = hrt_absolute_وقت();
	شرح_کھڑکی_گنتی = perf_event_گنتی(_injected);
	injection_شرح_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	اگر (config.دھارا == دھارا_2::MovingBaseline) {
		_moving_baseline_sub.رجسٹر_Callback();

	} ورنہ {
		برائے (خودکار &sub : _corrections_sub) {
			sub.رجسٹر_Callback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	فعال = سچ;
	_write_error_reported = جھوٹ;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	جدول_وقت_Now();
}

خالی CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	فعال = جھوٹ;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	برائے (خودکار &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	جدول_وقت_Now();

	برائے (صحیح i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		ثابت منطقی closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		اگر (closed) {
			توڑو;
		}

		px4_usleep(5_ms);
	}
}

اعشاری CorrectionInjector::injection_شرح_hz_2()
{
	ثابت hrt_abstime now = hrt_absolute_وقت();

	اگر (now - شرح_کھڑکی_آغاز >= k_شرح_کھڑکی) {
		ثابت uint64_t گنتی = perf_event_گنتی(_injected);
		injection_شرح_hz = (اعشاری)(گنتی - شرح_کھڑکی_گنتی) * 1e6f / (اعشاری)(now - شرح_کھڑکی_آغاز);
		شرح_کھڑکی_گنتی = گنتی;
		شرح_کھڑکی_آغاز = now;
	}

	واپس injection_شرح_hz;
}

خالی CorrectionInjector::تسلسل()
{
	pthread_mutex_lock(&_mutex);

	اگر (!فعال) {
		اگر (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} ورنہ {
		اگر (_fd < 0) {
			_fd = ::open(پورٹ_2, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			اگر (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", پورٹ_2, errno);
				_write_error_reported = سچ;
			}
		}

		اگر (_fd >= 0) {
			اگر (_config.دھارا == دھارا_2::MovingBaseline) {
				drain_moving_baseline();

			} ورنہ {
				drain_corrections();
			}

			inject_چوکھٹے();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

خالی CorrectionInjector::drain_corrections()
{
	rtcm_ڈیٹا_s chunk;

	اگر (select_corrections_instance(chunk)) {
		جوڑو_chunk(chunk);
	}

	ثابت int8_t selected = _selected_instance.بھرو();

	اگر (selected < 0) {
		واپس;
	}

	برائے (غیرعلامتی n = 0; n < rtcm_ڈیٹا_s::ORB_قطار_لمبائی && _corrections_sub[selected].update(&chunk); n++) {
		اگر (chunk.device_id != _config.own_device_id) {
			جوڑو_chunk(chunk);
		}
	}
}

خالی CorrectionInjector::drain_moving_baseline()
{
	rtcm_ڈیٹا_s chunk;

	برائے (غیرعلامتی n = 0; n < rtcm_ڈیٹا_s::ORB_قطار_لمبائی && _moving_baseline_sub.update(&chunk); n++) {
		اگر (chunk.device_id != _config.own_device_id) {
			جوڑو_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
منطقی CorrectionInjector::select_corrections_instance(rtcm_ڈیٹا_s &chunk)
{
	اگر (_selected_instance.بھرو() >= 0 && hrt_elapsed_وقت(&آخری_chunk_وقت) < k_ماخذ_Timeout) {
		واپس جھوٹ;
	}

	_selected_instance.store(-1);

	برائے (int8_t instance = 0; instance < rtcm_ڈیٹا_s::MAX_INSTANCES; instance++) {
		برائے (غیرعلامتی n = 0; n < rtcm_ڈیٹا_s::ORB_قطار_لمبائی && _corrections_sub[instance].update(&chunk); n++) {
			اگر (chunk.device_id != _config.own_device_id && hrt_elapsed_وقت(&chunk.timestamp) < k_ماخذ_Timeout) {
				_selected_instance.store(instance);
				واپس سچ;
			}
		}
	}

	واپس جھوٹ;
}

خالی CorrectionInjector::جوڑو_chunk(ثابت rtcm_ڈیٹا_s &chunk)
{
	ثابت size_t len = math::min<size_t>(chunk.len, حجم(chunk.ڈیٹا));

	اگر (_framer.جوڑو_ڈیٹا(chunk.ڈیٹا, len) < len) {
		perf_گنتی(framer_بھرا);
	}

	آخری_chunk_وقت = hrt_absolute_وقت();
}

خالی CorrectionInjector::inject_چوکھٹے()
{
	size_t چوکھٹا_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *چوکھٹا = خالی_اشاریہ;

	جب ((چوکھٹا = _framer.get_اگلا_پیغام(&چوکھٹا_len, &protocol)) != خالی_اشاریہ) {
		اگر ((_config.protocols & protocol_ثنائی_ہندسہ(protocol)) == 0) {
			_framer.consume_پیغام(چوکھٹا_len);
			perf_گنتی(_unsupported);
			جاری;
		}

		ثابت ssize_t tx_space = tx_space_دستیاب();

		// Unknown on POSIX, where the write itself is the check
		اگر (tx_space >= 0 && tx_space < (ssize_t)چوکھٹا_len) {
			perf_گنتی(tx_بفر_بھرا);
			جدول_وقت_Delayed(drain_وقت_us(چوکھٹا_len - tx_space));
			توڑو;
		}

		ثابت ssize_t written = ::اكتب(_fd, چوکھٹا, چوکھٹا_len);

		اگر (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_گنتی(tx_بفر_بھرا);
			جدول_وقت_Delayed(drain_وقت_us(چوکھٹا_len));
			توڑو;
		}

		اگر (written != (ssize_t)چوکھٹا_len) {
			perf_گنتی(_short_write);

			اگر (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", پورٹ_2, errno);
				_write_error_reported = سچ;
			}
		}

		_framer.consume_پیغام(چوکھٹا_len);
		perf_گنتی(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_دستیاب() ثابت
{
#اگر defined(FIONSPACE)
	صحیح space = 0;

	اگر (::ioctl(_fd, FIONSPACE, &space) == 0) {
		واپس space;
	}

#اگر_ختم
	واپس -1;
}

uint32_t CorrectionInjector::drain_وقت_us(size_t بائٹیں) ثابت
{
	ثابت uint32_t baudrate = _config.baudrate;

	اگر (baudrate == 0) {
		واپس kFallbackRetryDelayUs;
	}

	واپس math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)بائٹیں * k_بٹ_فی_Byte_On_The_Wire * 1_s / baudrate);
}

خالی CorrectionInjector::print_حالت() ثابت
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.دھارا == دھارا_2::MovingBaseline ? "moving baseline" : "corrections", (دوہرا)injection_شرح_hz,
		 (صحیح)_selected_instance.بھرو());
	perf_print_شمار(_injected);
	perf_print_شمار(tx_بفر_بھرا);
	perf_print_شمار(framer_بھرا);
	perf_print_شمار(_unsupported);
	perf_print_شمار(_short_write);

	ثابت CorrectionFramerStats stats = _framer.getStats();

	اگر (stats.messages_parsed > 0 || stats.چکری_جانچ_errors_2 > 0 || stats.بائٹیں_discarded_2 > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (غیرعلامتی)stats.rtcm3_messages, (غیرعلامتی)stats.spartn_messages, (غیرعلامتی)stats.ubx_messages,
			 (غیرعلامتی)stats.چکری_جانچ_errors_2, (غیرعلامتی)stats.بائٹیں_discarded_2);
	}
}

} // namespace gnss
