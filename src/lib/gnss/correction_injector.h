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


#ہدایت once

#شامل <pthread.h>

#شامل <drivers/drv_hrt.h>
#شامل <lib/perf/perf_counter.h>
#شامل <px4_platform_common/atomic.h>
#شامل <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#شامل <uORB/SubscriptionCallback.hpp>
#شامل <uORB/topics/rtcm_data.h>

#شامل "correction_framer.h"

namespace gnss
{

// A protocol's bit in a set of accepted protocols
constexpr uint8_t protocol_ثنائی_ہندسہ(CorrectionProtocol protocol) { واپس 1u << static_cast<uint8_t>(protocol); }

// Writes corrections to a receiver as they are published, from the serial port's work queue. A driver's reader thread
// blocks on the UART between epochs, so injecting from it would hold corrections back by up to one output interval.
//
// The injector opens its own descriptor on that work queue. On NuttX the work queues are threads of the wq:manager
// task and a descriptor belongs to the task that opened it, so the driver's is not valid there.
//
// Chunks are reassembled into frames, and a frame is written only once the TX buffer has room for all of it: a frame
// cut short would fail its CRC at the receiver. Until then it waits in the framer, and the injector comes back once
// the buffer has drained.
class CorrectionInjector : عوامی px4::Scheduled_Work_شے
{
عوامی:
	// The one stream a receiver takes. An RTK engine works against one reference station, so a rover offered a fixed
	// base beside its moving base can settle on the fixed base and lose the heading. The fixed-base corrections go to
	// the moving base instead, whose absolute fix the rover inherits (u-blox UBX-19009093 figure 2). A receiver that
	// takes neither, such as a rover wired to its moving base, never starts the injector.
	شمار class دھارا_2 : uint8_t {
		Corrections,	// Fixed-base corrections, rtcm_corrections: every receiver but a moving-base rover
		MovingBaseline,	// A moving base's stream, rtcm_moving_baseline: its rover, when the baseline comes through PX4
	};

	ساخت Config {
		uint32_t own_device_id{0};	// Chunks this device published are never written back
		دھارا_2 دھارا{دھارا_2::Corrections};
		// protocol_bit()s the receiver accepts; frames of any other protocol are dropped
		uint8_t protocols{protocol_ثنائی_ہندسہ(CorrectionProtocol::Rtcm3)};
		// The port's baudrate, fixed while injecting; paces retries while the TX buffer is full. 0 if unknown.
		uint32_t baudrate{0};
	};

	// name labels the work item; port selects its work queue and is what the injector opens
	CorrectionInjector(ثابت حرف *name, ثابت حرف *پورٹ);
	~CorrectionInjector() override;

	// Injects only between start() and stop(): outside them the owner configures the receiver on the same port.
	// stop() also has the work queue close the injector's descriptor, so call it before destruction.
	خالی آغاز(ثابت Config &config);
	خالی stop();

	// rtcm_corrections instance in use, -1 for none
	int8_t selected_instance() ثابت { واپس _selected_instance.بھرو(); }
	// Frames written per second over the last window. Owner thread only.
	اعشاری injection_شرح_hz_2();
	خالی print_حالت() ثابت;

نجی:
	خالی تسلسل() override;
	خالی drain_corrections();
	خالی drain_moving_baseline();
	bool select_corrections_instance(rtcm_ڈیٹا_s &chunk);
	خالی جوڑو_chunk(ثابت rtcm_ڈیٹا_s &chunk);
	خالی inject_چوکھٹے();
	ssize_t tx_space_دستیاب() ثابت;
	uint32_t drain_وقت_us(size_t بائٹیں) ثابت;

	حرف پورٹ_2[32] {};

	pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
	// Guarded by _mutex
	bool فعال{false};
	صحیح _fd{-1};	// opened and closed on the work queue
	bool _write_error_reported{false};
	Config _config{};
	hrt_abstime آخری_chunk_وقت{0};
	CorrectionFramer _framer;

	px4::atomic<int8_t> _selected_instance{-1};

	// Owner thread only
	hrt_abstime شرح_کھڑکی_آغاز{0};
	uint64_t شرح_کھڑکی_گنتی{0};
	اعشاری injection_شرح_hz{0.f};

	uORB::Subscription_Callback_Work_شے _moving_baseline_sub{this, ORB_ID(rtcm_moving_baseline)};
	uORB::Subscription_Callback_Work_شے _corrections_sub[rtcm_ڈیٹا_s::MAX_INSTANCES] {
		{this, ORB_ID(rtcm_corrections), 0},
		{this, ORB_ID(rtcm_corrections), 1},
		{this, ORB_ID(rtcm_corrections), 2},
		{this, ORB_ID(rtcm_corrections), 3},
	};

	perf_شمار_t _injected{perf_alloc(PC_گنتی, "gnss_injector: frames")};
	perf_شمار_t tx_بفر_بھرا{perf_alloc(PC_گنتی, "gnss_injector: tx buf full")};
	perf_شمار_t framer_بھرا{perf_alloc(PC_گنتی, "gnss_injector: framer full")};
	perf_شمار_t _unsupported{perf_alloc(PC_گنتی, "gnss_injector: unsupported protocol")};
	perf_شمار_t _short_write{perf_alloc(PC_گنتی, "gnss_injector: short write")};
};

} // namespace gnss
