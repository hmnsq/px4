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


#అన్తర్భావ "correction_injector.h"

#అన్తర్భావ <errno.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <px4_platform_common/posix.h>
#అన్తర్భావ <px4_platform_common/time.h>

ఉపయోగించి నామాకాశ సమయం_literals;

నామాకాశ gnss
{

// Without a chunk from the selected source for this long, another source may be taken
స్థిర constexpr hrt_abstime k_ఆధారం_Timeout = 5_s;
// The injection rate is frames per second over windows this long
స్థిర constexpr hrt_abstime k_రేటు_కిటికీ = 5_s;
// Start bit, eight data bits, stop bit
స్థిర constexpr uint32_t k_బిట్లు_చొప్పున_Byte_On_The_Wire = 10;
// A full TX buffer is retried no sooner than this, and this late when the baudrate is unknown
స్థిర constexpr uint32_t kMinRetryDelayUs = 1_ms;
స్థిర constexpr uint32_t kFallbackRetryDelayUs = 10_ms;

CorrectionInjector::CorrectionInjector(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *పోర్టు) :
	Scheduled_Work_అంశం(name, px4::serial_పోర్టు_గమ్యం_wq(పోర్టు))
{
	strncpy(పోర్టు_2, పోర్టు, పరిమాణం(పోర్టు_2) - 1);
}

CorrectionInjector::~CorrectionInjector()
{
	_moving_baseline_sub.unregisterCallback();

	కొరకు (స్వయంచాలక &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// Cancels a pending retry, then waits for a Run() in progress, which uses the members destroyed after this body
	కాలపట్టిక_Clear();
	Deinit();

	perf_free(_injected);
	perf_free(tx_బఫరు_నిండిన);
	perf_free(framer_నిండిన);
	perf_free(_unsupported);
	perf_free(_short_write);
}

శూన్య CorrectionInjector::ప్రారంభం(స్థిరరాశి Config &config)
{
	రేటు_కిటికీ_ప్రారంభం = hrt_absolute_సమయం();
	రేటు_కిటికీ_లెక్క = perf_event_లెక్క(_injected);
	injection_రేటు_hz = 0.f;

	// Only the stream in use schedules runs: a base would otherwise wake for each moving-baseline chunk it publishes
	ఒకవేళ (config.ప్రవాహం == ప్రవాహం_2::MovingBaseline) {
		_moving_baseline_sub.రిజిస్టరు_Callback();

	} లేకపోతే {
		కొరకు (స్వయంచాలక &sub : _corrections_sub) {
			sub.రిజిస్టరు_Callback();
		}
	}

	pthread_mutex_lock(&_mutex);
	_config = config;
	చురుకైన = నిజం;
	_write_error_reported = అబద్ధం;
	pthread_mutex_unlock(&_mutex);

	// What queued up during configuration still goes out: a base sends its position (1005/1006) only every few seconds
	కాలపట్టిక_Now();
}

శూన్య CorrectionInjector::stop()
{
	// Returns only after a write in progress, so the owner's next command can't land inside an RTCM frame
	pthread_mutex_lock(&_mutex);
	చురుకైన = అబద్ధం;
	pthread_mutex_unlock(&_mutex);

	// Chunks still queue up until start(), which drains them
	_moving_baseline_sub.unregisterCallback();

	కొరకు (స్వయంచాలక &sub : _corrections_sub) {
		sub.unregisterCallback();
	}

	// The descriptor is closed where it was opened. Bounded: a starved work queue leaks it rather than blocking the owner.
	కాలపట్టిక_Now();

	కొరకు (పూర్ణాంకం i = 0; i < 20; i++) {
		pthread_mutex_lock(&_mutex);
		స్థిరరాశి తార్కిక closed = (_fd < 0);
		pthread_mutex_unlock(&_mutex);

		ఒకవేళ (closed) {
			ఆపు;
		}

		px4_usleep(5_ms);
	}
}

దశాంశం CorrectionInjector::injection_రేటు_hz_2()
{
	స్థిరరాశి hrt_abstime now = hrt_absolute_సమయం();

	ఒకవేళ (now - రేటు_కిటికీ_ప్రారంభం >= k_రేటు_కిటికీ) {
		స్థిరరాశి uint64_t లెక్క = perf_event_లెక్క(_injected);
		injection_రేటు_hz = (దశాంశం)(లెక్క - రేటు_కిటికీ_లెక్క) * 1e6f / (దశాంశం)(now - రేటు_కిటికీ_ప్రారంభం);
		రేటు_కిటికీ_లెక్క = లెక్క;
		రేటు_కిటికీ_ప్రారంభం = now;
	}

	తిరిగి injection_రేటు_hz;
}

శూన్య CorrectionInjector::నిరంతరం()
{
	pthread_mutex_lock(&_mutex);

	ఒకవేళ (!చురుకైన) {
		ఒకవేళ (_fd >= 0) {
			::close(_fd);
			_fd = -1;
		}

	} లేకపోతే {
		ఒకవేళ (_fd < 0) {
			_fd = ::open(పోర్టు_2, O_WRONLY | O_NONBLOCK | O_NOCTTY);

			ఒకవేళ (_fd < 0 && !_write_error_reported) {
				PX4_ERR("%s open for injection failed: %d", పోర్టు_2, errno);
				_write_error_reported = నిజం;
			}
		}

		ఒకవేళ (_fd >= 0) {
			ఒకవేళ (_config.ప్రవాహం == ప్రవాహం_2::MovingBaseline) {
				drain_moving_baseline();

			} లేకపోతే {
				drain_corrections();
			}

			inject_చట్రాలు();
		}
	}

	pthread_mutex_unlock(&_mutex);
}

శూన్య CorrectionInjector::drain_corrections()
{
	rtcm_దత్తాంశం_s chunk;

	ఒకవేళ (select_corrections_instance(chunk)) {
		కూడు_chunk(chunk);
	}

	స్థిరరాశి int8_t selected = _selected_instance.ఎక్కించు();

	ఒకవేళ (selected < 0) {
		తిరిగి;
	}

	కొరకు (గుర్తులేని n = 0; n < rtcm_దత్తాంశం_s::ORB_వరుస_నిడివి && _corrections_sub[selected].update(&chunk); n++) {
		ఒకవేళ (chunk.device_id != _config.own_device_id) {
			కూడు_chunk(chunk);
		}
	}
}

శూన్య CorrectionInjector::drain_moving_baseline()
{
	rtcm_దత్తాంశం_s chunk;

	కొరకు (గుర్తులేని n = 0; n < rtcm_దత్తాంశం_s::ORB_వరుస_నిడివి && _moving_baseline_sub.update(&chunk); n++) {
		ఒకవేళ (chunk.device_id != _config.own_device_id) {
			కూడు_chunk(chunk);
		}
	}
}

// Once the selected source has gone quiet, takes the first instance with a fresh chunk from another device. update()
// hands out the oldest queued chunk, so a backlog from before start() is walked through here in one go rather than
// one chunk per publication. Returns true with the chunk that selected the source, which goes out like any other.
తార్కిక CorrectionInjector::select_corrections_instance(rtcm_దత్తాంశం_s &chunk)
{
	ఒకవేళ (_selected_instance.ఎక్కించు() >= 0 && hrt_elapsed_సమయం(&చివరి_chunk_సమయం) < k_ఆధారం_Timeout) {
		తిరిగి అబద్ధం;
	}

	_selected_instance.store(-1);

	కొరకు (int8_t instance = 0; instance < rtcm_దత్తాంశం_s::MAX_INSTANCES; instance++) {
		కొరకు (గుర్తులేని n = 0; n < rtcm_దత్తాంశం_s::ORB_వరుస_నిడివి && _corrections_sub[instance].update(&chunk); n++) {
			ఒకవేళ (chunk.device_id != _config.own_device_id && hrt_elapsed_సమయం(&chunk.timestamp) < k_ఆధారం_Timeout) {
				_selected_instance.store(instance);
				తిరిగి నిజం;
			}
		}
	}

	తిరిగి అబద్ధం;
}

శూన్య CorrectionInjector::కూడు_chunk(స్థిరరాశి rtcm_దత్తాంశం_s &chunk)
{
	స్థిరరాశి size_t len = math::min<size_t>(chunk.len, పరిమాణం(chunk.దత్తాంశం));

	ఒకవేళ (_framer.కూడు_దత్తాంశం(chunk.దత్తాంశం, len) < len) {
		perf_లెక్క(framer_నిండిన);
	}

	చివరి_chunk_సమయం = hrt_absolute_సమయం();
}

శూన్య CorrectionInjector::inject_చట్రాలు()
{
	size_t చట్రం_len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	స్థిరరాశి uint8_t *చట్రం = శూన్యసూచిక;

	అయితే ((చట్రం = _framer.get_తదుపరి_సందేశం(&చట్రం_len, &protocol)) != శూన్యసూచిక) {
		ఒకవేళ ((_config.protocols & protocol_బిట్టు(protocol)) == 0) {
			_framer.consume_సందేశం(చట్రం_len);
			perf_లెక్క(_unsupported);
			కొనసాగు;
		}

		స్థిరరాశి ssize_t tx_space = tx_space_అందుబాటు();

		// Unknown on POSIX, where the write itself is the check
		ఒకవేళ (tx_space >= 0 && tx_space < (ssize_t)చట్రం_len) {
			perf_లెక్క(tx_బఫరు_నిండిన);
			కాలపట్టిక_Delayed(drain_సమయం_us(చట్రం_len - tx_space));
			ఆపు;
		}

		స్థిరరాశి ssize_t written = ::లిఖ(_fd, చట్రం, చట్రం_len);

		ఒకవేళ (written < 0 && errno == EAGAIN) {
			// The buffer filled between the check and the write: the frame stays for the retry
			perf_లెక్క(tx_బఫరు_నిండిన);
			కాలపట్టిక_Delayed(drain_సమయం_us(చట్రం_len));
			ఆపు;
		}

		ఒకవేళ (written != (ssize_t)చట్రం_len) {
			perf_లెక్క(_short_write);

			ఒకవేళ (written < 0 && !_write_error_reported) {
				PX4_ERR("%s injection write failed: %d", పోర్టు_2, errno);
				_write_error_reported = నిజం;
			}
		}

		_framer.consume_సందేశం(చట్రం_len);
		perf_లెక్క(_injected);
	}
}

ssize_t CorrectionInjector::tx_space_అందుబాటు() స్థిరరాశి
{
#ఒకవేళ defined(FIONSPACE)
	పూర్ణాంకం space = 0;

	ఒకవేళ (::ioctl(_fd, FIONSPACE, &space) == 0) {
		తిరిగి space;
	}

#అయితే_ముగింపు
	తిరిగి -1;
}

uint32_t CorrectionInjector::drain_సమయం_us(size_t బైట్లు) స్థిరరాశి
{
	స్థిరరాశి uint32_t baudrate = _config.baudrate;

	ఒకవేళ (baudrate == 0) {
		తిరిగి kFallbackRetryDelayUs;
	}

	తిరిగి math::max<uint32_t>(kMinRetryDelayUs, (uint64_t)బైట్లు * k_బిట్లు_చొప్పున_Byte_On_The_Wire * 1_s / baudrate);
}

శూన్య CorrectionInjector::print_స్థితి() స్థిరరాశి
{
	PX4_INFO("injecting %s: %6.2f Hz, source instance %d",
		 _config.ప్రవాహం == ప్రవాహం_2::MovingBaseline ? "moving baseline" : "corrections", (ద్విగుణ)injection_రేటు_hz,
		 (పూర్ణాంకం)_selected_instance.ఎక్కించు());
	perf_print_counter(_injected);
	perf_print_counter(tx_బఫరు_నిండిన);
	perf_print_counter(framer_నిండిన);
	perf_print_counter(_unsupported);
	perf_print_counter(_short_write);

	స్థిరరాశి CorrectionFramerStats stats = _framer.getStats();

	ఒకవేళ (stats.messages_parsed > 0 || stats.చక్రీయ_తనిఖీ_errors_2 > 0 || stats.బైట్లు_discarded_2 > 0) {
		PX4_INFO("framed: %u RTCM3, %u SPARTN, %u UBX, %u CRC errors, %u bytes discarded",
			 (గుర్తులేని)stats.rtcm3_messages, (గుర్తులేని)stats.spartn_messages, (గుర్తులేని)stats.ubx_messages,
			 (గుర్తులేని)stats.చక్రీయ_తనిఖీ_errors_2, (గుర్తులేని)stats.బైట్లు_discarded_2);
	}
}

} // namespace gnss
