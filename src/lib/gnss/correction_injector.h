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


#pragma once

#অন্তর্ভাব <pthread.h>

#অন্তর্ভাব <drivers/drv_hrt.h>
#অন্তর্ভাব <lib/perf/perf_counter.h>
#অন্তর্ভাব <px4_platform_common/atomic.h>
#অন্তর্ভাব <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#অন্তর্ভাব <uORB/SubscriptionCallback.hpp>
#অন্তর্ভাব <uORB/topics/rtcm_data.h>

#অন্তর্ভাব "correction_framer.h"

namespace gnss
{

// A protocol's bit in a set of accepted protocols
constexpr uint8_t protocol_bit(CorrectionProtocol protocol) { ফেরত 1u << static_cast<uint8_t>(protocol); }

// Writes corrections to a receiver as they are published, from the serial port's work queue. A driver's reader thread
// blocks on the UART between epochs, so injecting from it would hold corrections back by up to one output interval.
//
// The injector opens its own descriptor on that work queue. On NuttX the work queues are threads of the wq:manager
// task and a descriptor belongs to the task that opened it, so the driver's is not valid there.
//
// Chunks are reassembled into frames, and a frame is written only once the TX buffer has room for all of it: a frame
// cut short would fail its CRC at the receiver. Until then it waits in the framer, and the injector comes back once
// the buffer has drained.
class CorrectionInjector : সর্বজনীন px4::ScheduledWorkItem
{
সর্বজনীন:
	// The one stream a receiver takes. An RTK engine works against one reference station, so a rover offered a fixed
	// base beside its moving base can settle on the fixed base and lose the heading. The fixed-base corrections go to
	// the moving base instead, whose absolute fix the rover inherits (u-blox UBX-19009093 figure 2). A receiver that
	// takes neither, such as a rover wired to its moving base, never starts the injector.
	গণনা class Stream : uint8_t {
		Corrections,	// Fixed-base corrections, rtcm_corrections: every receiver but a moving-base rover
		MovingBaseline,	// A moving base's stream, rtcm_moving_baseline: its rover, when the baseline comes through PX4
	};

	কাঠামো Config {
		uint32_t own_device_id{0};	// Chunks this device published are never written back
		Stream stream{Stream::Corrections};
		// protocol_bit()s the receiver accepts; frames of any other protocol are dropped
		uint8_t protocols{protocol_bit(CorrectionProtocol::Rtcm3)};
		// The port's baudrate, fixed while injecting; paces retries while the TX buffer is full. 0 if unknown.
		uint32_t baudrate{0};
	};

	// name labels the work item; port selects its work queue and is what the injector opens
	CorrectionInjector(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *port);
	~CorrectionInjector() override;

	// Injects only between start() and stop(): outside them the owner configures the receiver on the same port.
	// stop() also has the work queue close the injector's descriptor, so call it before destruction.
	শূন্য start(ধ্রুবক Config &config);
	শূন্য stop();

	// rtcm_corrections instance in use, -1 for none
	int8_t selected_instance() ধ্রুবক { ফেরত _selected_instance.load(); }
	// Frames written per second over the last window. Owner thread only.
	দশমিক injection_rate_hz();
	শূন্য print_status() ধ্রুবক;

ব্যক্তিগত:
	শূন্য Run() override;
	শূন্য drain_corrections();
	শূন্য drain_moving_baseline();
	bool select_corrections_instance(rtcm_তথ্য_s &chunk);
	শূন্য add_chunk(ধ্রুবক rtcm_তথ্য_s &chunk);
	শূন্য inject_frames();
	ssize_t tx_space_available() ধ্রুবক;
	uint32_t drain_time_us(size_t bytes) ধ্রুবক;

	অক্ষর _port[32] {};

	pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
	// Guarded by _mutex
	bool _active{false};
	পূর্ণসংখ্যা _fd{-1};	// opened and closed on the work queue
	bool _write_error_reported{false};
	Config _config{};
	hrt_abstime _last_chunk_time{0};
	CorrectionFramer _framer;

	px4::atomic<int8_t> _selected_instance{-1};

	// Owner thread only
	hrt_abstime _rate_window_start{0};
	uint64_t rate_window_গুনতি{0};
	দশমিক _injection_rate_hz{0.f};

	uORB::SubscriptionCallbackWorkItem _moving_baseline_sub{this, ORB_ID(rtcm_moving_baseline)};
	uORB::SubscriptionCallbackWorkItem _corrections_sub[rtcm_তথ্য_s::MAX_INSTANCES] {
		{this, ORB_ID(rtcm_corrections), 0},
		{this, ORB_ID(rtcm_corrections), 1},
		{this, ORB_ID(rtcm_corrections), 2},
		{this, ORB_ID(rtcm_corrections), 3},
	};

	perf_গণক_t _injected{perf_alloc(PC_গুনতি, "gnss_injector: frames")};
	perf_গণক_t tx_বাফার_full{perf_alloc(PC_গুনতি, "gnss_injector: tx buf full")};
	perf_গণক_t _framer_full{perf_alloc(PC_গুনতি, "gnss_injector: framer full")};
	perf_গণক_t _unsupported{perf_alloc(PC_গুনতি, "gnss_injector: unsupported protocol")};
	perf_গণক_t _short_write{perf_alloc(PC_গুনতি, "gnss_injector: short write")};
};

} // namespace gnss
