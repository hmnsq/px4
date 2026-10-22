/****************************************************************************
 *
 *   Copyright (c) 2012-2021 PX4 Development Team. All rights reserved.
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

/**
 * @file perf_counter.cpp
 *
 * @brief Performance measuring tools.
 */

#அந்தர்பாவ <inttypes.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <drivers/drv_hrt.h>
#அந்தர்பாவ <math.h>
#அந்தர்பாவ <pthread.h>
#அந்தர்பாவ <systemlib/err.h>

#அந்தர்பாவ "perf_counter.h"

#அந்தர்பாவ <px4_platform_common/atomic.h>

// perf counters are written on the hot path (perf_begin/perf_end/perf_count)
// and read concurrently by the 'perf' command and the logger.
//
// On POSIX/SITL we always use real atomics: ThreadSanitizer needs them to stay
// clean, native 64-bit atomics are cheap there, and we don't care about the
// overhead. On flight hardware (NuttX/QURT) these are single-writer diagnostic
// counters, and a 64-bit atomic lowers to a locked (interrupt-disabling)
// sequence on Cortex-M (~2x cost on this hot path). There a concurrent reader
// can at worst observe an off-by-one torn read of a 64-bit field, which is
// benign and self-correcting, so a plain scalar is used instead.
//
// PerfVar mirrors the subset of the px4::atomic API used below so the call
// sites are identical in both configurations; on embedded the accessors are
// trivial and inline to bare loads/stores.
#எனில் defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
template <typename T> using PerfVar = px4::atomic<T>;
#இல்லையெனில்
template <typename T>
வகுப்பு PerfVar
{
பொது:
	PerfVar() = இயல்பு;
	T load() மாறிலி { திரும்பு மதிப்பு_2; }
	வெறுமை store(T value) { மதிப்பு_2 = value; }
	T fetch_add(T value) { மாறிலி T previous = மதிப்பு_2; மதிப்பு_2 += value; திரும்பு previous; }
தனிப்பட்ட:
	T மதிப்பு_2 {};
};
#endif

/**
 * Header common to all counters.
 */
கட்டமைப்பு perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	எண்ணிக்கை perf_counter_type	type;	/**< counter type */
	தருக்க			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	மாறிலி எழுத்து		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
கட்டமைப்பு perf_ctr_மொத்தம் : பொது perf_ctr_header {
	PerfVar<uint64_t>	event_மொத்தம்;
};

/**
 * PC_ELAPSED counter.
 */
கட்டமைப்பு perf_ctr_elapsed : பொது perf_ctr_header {
	PerfVar<uint64_t>	event_மொத்தம்;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_total;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<தசம>	mean;
	PerfVar<தசம>	M2;
};

/**
 * PC_INTERVAL counter.
 */
கட்டமைப்பு perf_ctr_interval : பொது perf_ctr_header {
	PerfVar<uint64_t>	event_மொத்தம்;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<தசம>	mean;
	PerfVar<தசம>	M2;
};

/**
 * List of all known counters.
 */
நிலையான sq_queue_t	perf_counters = { nullptr, nullptr };

/**
 * mutex protecting access to the perf_counters linked list (which is read from & written to by different threads)
 */
pthread_mutex_t perf_counters_mutex = PTHREAD_MUTEX_INITIALIZER;
// Note: the mutex only protects the linked list, not individual counter data.
// Counter fields go through PerfVar (atomic on POSIX/TSan, plain on embedded).

/**
 * Number of perf_iterate_all() calls currently in progress (guarded by perf_counters_mutex).
 * While non-zero, perf_free() only marks entries as pending_free; the last
 * iteration to finish deletes them.
 */
நிலையான முழு perf_iterate_active = 0;

நிலையான வெறுமை
perf_delete(perf_counter_t handle)
{
	தேர்வு (handle->type) {
	நிலை PC_மொத்தம்:
		delete (கட்டமைப்பு perf_ctr_மொத்தம் *)handle;
		நிறுத்து;

	நிலை PC_ELAPSED:
		delete (கட்டமைப்பு perf_ctr_elapsed *)handle;
		நிறுத்து;

	நிலை PC_INTERVAL:
		delete (கட்டமைப்பு perf_ctr_interval *)handle;
		நிறுத்து;

	இயல்பு:
		நிறுத்து;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
நிலையான வெறுமை
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	போது (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		எனில் (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(எண்ணிக்கை perf_counter_type type, மாறிலி எழுத்து *name)
{
	perf_counter_t ctr = nullptr;

	தேர்வு (type) {
	நிலை PC_மொத்தம்:
		ctr = புதிய perf_ctr_மொத்தம்();
		நிறுத்து;

	நிலை PC_ELAPSED:
		ctr = புதிய perf_ctr_elapsed();
		நிறுத்து;

	நிலை PC_INTERVAL:
		ctr = புதிய perf_ctr_interval();
		நிறுத்து;

	இயல்பு:
		நிறுத்து;
	}

	எனில் (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = பொய்;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	திரும்பு ctr;
}

perf_counter_t
perf_alloc_once(எண்ணிக்கை perf_counter_type type, மாறிலி எழுத்து *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	போது (handle != nullptr) {
		எனில் (!handle->pending_free && !strcmp(handle->name, name)) {
			எனில் (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				திரும்பு handle;

			} இல்லையெனில் {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				திரும்பு nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	திரும்பு perf_alloc(type, name);
}

வெறுமை
perf_free(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	எனில் (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = உண்மை;
		pthread_mutex_unlock(&perf_counters_mutex);
		திரும்பு;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

வெறுமை
perf_மொத்தம்(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_மொத்தம்:
		((கட்டமைப்பு perf_ctr_மொத்தம் *)handle)->event_மொத்தம்.fetch_add(1);
		நிறுத்து;

	நிலை PC_INTERVAL:
		perf_மொத்தம்_interval(handle, hrt_absolute_time());
		நிறுத்து;

	இயல்பு:
		நிறுத்து;
	}
}

வெறுமை
perf_begin(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_ELAPSED:
		((கட்டமைப்பு perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		நிறுத்து;

	இயல்பு:
		நிறுத்து;
	}
}

வெறுமை
perf_end(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_ELAPSED: {
			கட்டமைப்பு perf_ctr_elapsed *pce = (கட்டமைப்பு perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			எனில் (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		நிறுத்து;

	இயல்பு:
		நிறுத்து;
	}
}

வெறுமை
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_ELAPSED: {
			கட்டமைப்பு perf_ctr_elapsed *pce = (கட்டமைப்பு perf_ctr_elapsed *)handle;

			எனில் (elapsed >= 0) {
				uint64_t ec = pce->event_மொத்தம்.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				எனில் ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.load();

				எனில் (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				தசம dt = elapsed / 1e6f;
				தசம mean_val = pce->mean.load();
				தசம delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				தசம m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		நிறுத்து;

	இயல்பு:
		நிறுத்து;
	}
}

வெறுமை
perf_மொத்தம்_interval(perf_counter_t handle, hrt_abstime now)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_INTERVAL: {
			கட்டமைப்பு perf_ctr_interval *pci = (கட்டமைப்பு perf_ctr_interval *)handle;
			uint64_t ec = pci->event_மொத்தம்.load();

			தேர்வு (ec) {
			நிலை 0:
				pci->time_first.store(now);
				நிறுத்து;

			நிலை 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					நிறுத்து;
				}

			இயல்பு: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					எனில் ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					எனில் ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					தசம dt = interval / 1e6f;
					தசம mean_val = pci->mean.load();
					தசம delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					தசம m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					நிறுத்து;
				}
			}

			pci->time_last.store(now);
			pci->event_மொத்தம்.fetch_add(1);
			நிறுத்து;
		}

	இயல்பு:
		நிறுத்து;
	}
}

வெறுமை
perf_set_மொத்தம்(perf_counter_t handle, uint64_t மொத்தம்)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_மொத்தம்: {
			((கட்டமைப்பு perf_ctr_மொத்தம் *)handle)->event_மொத்தம்.store(மொத்தம்);
		}
		நிறுத்து;

	இயல்பு:
		நிறுத்து;
	}

}

வெறுமை
perf_cancel(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_ELAPSED: {
			கட்டமைப்பு perf_ctr_elapsed *pce = (கட்டமைப்பு perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		நிறுத்து;

	இயல்பு:
		நிறுத்து;
	}
}

வெறுமை
perf_reset(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_மொத்தம்:
		((கட்டமைப்பு perf_ctr_மொத்தம் *)handle)->event_மொத்தம்.store((uint64_t)0);
		நிறுத்து;

	நிலை PC_ELAPSED: {
			கட்டமைப்பு perf_ctr_elapsed *pce = (கட்டமைப்பு perf_ctr_elapsed *)handle;
			pce->event_மொத்தம்.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			நிறுத்து;
		}

	நிலை PC_INTERVAL: {
			கட்டமைப்பு perf_ctr_interval *pci = (கட்டமைப்பு perf_ctr_interval *)handle;
			pci->event_மொத்தம்.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			நிறுத்து;
		}
	}
}

வெறுமை
perf_print_counter(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு;
	}

	தேர்வு (handle->type) {
	நிலை PC_மொத்தம்: {
			uint64_t ec = ((கட்டமைப்பு perf_ctr_மொத்தம் *)handle)->event_மொத்தம்.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			நிறுத்து;
		}

	நிலை PC_ELAPSED: {
			கட்டமைப்பு perf_ctr_elapsed *pce = (கட்டமைப்பு perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_மொத்தம்.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			தசம m2 = pce->M2.load();
			தசம rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (இரட்டை)tt / (இரட்டை)ec,
				     tl, tm, (இரட்டை)(1e6f * rms));
			நிறுத்து;
		}

	நிலை PC_INTERVAL: {
			கட்டமைப்பு perf_ctr_interval *pci = (கட்டமைப்பு perf_ctr_interval *)handle;
			uint64_t ec = pci->event_மொத்தம்.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			தசம m2 = pci->M2.load();
			தசம rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (இரட்டை)(tla - tf) / (இரட்டை)ec,
				     tl, tm, (இரட்டை)(1e6f * rms));
			நிறுத்து;
		}

	இயல்பு:
		நிறுத்து;
	}
}


முழு
perf_print_counter_buffer(எழுத்து *buffer, முழு length, perf_counter_t handle)
{
	முழு இலக்கம்_written = 0;

	எனில் (handle == nullptr) {
		திரும்பு 0;
	}

	தேர்வு (handle->type) {
	நிலை PC_மொத்தம்:
		இலக்கம்_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((கட்டமைப்பு perf_ctr_மொத்தம் *)handle)->event_மொத்தம்.load());
		நிறுத்து;

	நிலை PC_ELAPSED: {
			கட்டமைப்பு perf_ctr_elapsed *pce = (கட்டமைப்பு perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_மொத்தம்.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			தசம m2 = pce->M2.load();
			தசம rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			இலக்கம்_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (இரட்டை)tt / (இரட்டை)ec,
					       tl,
					       tm,
					       (இரட்டை)(1e6f * rms));
			நிறுத்து;
		}

	நிலை PC_INTERVAL: {
			கட்டமைப்பு perf_ctr_interval *pci = (கட்டமைப்பு perf_ctr_interval *)handle;
			uint64_t ec = pci->event_மொத்தம்.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			தசம m2 = pci->M2.load();
			தசம rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			இலக்கம்_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (இரட்டை)(pci->time_last.load() - pci->time_first.load()) / (இரட்டை)ec,
					       tl,
					       tm,
					       (இரட்டை)(1e6f * rms));
			நிறுத்து;
		}

	இயல்பு:
		நிறுத்து;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	திரும்பு இலக்கம்_written;
}

uint64_t
perf_event_மொத்தம்(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு 0;
	}

	தேர்வு (handle->type) {
	நிலை PC_மொத்தம்:
		திரும்பு ((கட்டமைப்பு perf_ctr_மொத்தம் *)handle)->event_மொத்தம்.load();

	நிலை PC_ELAPSED: {
			கட்டமைப்பு perf_ctr_elapsed *pce = (கட்டமைப்பு perf_ctr_elapsed *)handle;
			திரும்பு pce->event_மொத்தம்.load();
		}

	நிலை PC_INTERVAL: {
			கட்டமைப்பு perf_ctr_interval *pci = (கட்டமைப்பு perf_ctr_interval *)handle;
			திரும்பு pci->event_மொத்தம்.load();
		}

	இயல்பு:
		நிறுத்து;
	}

	திரும்பு 0;
}

தசம
perf_mean(perf_counter_t handle)
{
	எனில் (handle == nullptr) {
		திரும்பு 0;
	}

	தேர்வு (handle->type) {
	நிலை PC_ELAPSED: {
			கட்டமைப்பு perf_ctr_elapsed *pce = (கட்டமைப்பு perf_ctr_elapsed *)handle;
			திரும்பு pce->mean.load();
		}

	நிலை PC_INTERVAL: {
			கட்டமைப்பு perf_ctr_interval *pci = (கட்டமைப்பு perf_ctr_interval *)handle;
			திரும்பு pci->mean.load();
		}

	இயல்பு:
		நிறுத்து;
	}

	திரும்பு 0.0f;
}

வெறுமை
perf_iterate_all(perf_callback cb, வெறுமை *user)
{
	நிலையான constexpr size_t PERF_COUNTER_வரி_BUFFER_SIZE = 220;
	எழுத்து buffer[PERF_COUNTER_வரி_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	போது (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		எனில் (!handle->pending_free) {
			perf_print_counter_buffer(buffer, அளவு(buffer), handle);

			// the callback may block (the logger waits for MAVLink acks here), so run
			// it without the global mutex: perf_alloc()/perf_free() on other threads
			// keep working, and 'next' stays valid because perf_free() defers deletion
			// while perf_iterate_active > 0
			pthread_mutex_unlock(&perf_counters_mutex);
			cb(buffer, user);
			pthread_mutex_lock(&perf_counters_mutex);
		}

		handle = next;
	}

	எனில் (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

வெறுமை
perf_print_all(வெறுமை)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	போது (handle != nullptr) {
		எனில் (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

வெறுமை
perf_print_latency(வெறுமை)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	க்கு (முழு i = 0; i < get_latency_bucket_மொத்தம்(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long முழு)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_மொத்தம்() - 1, get_latency_bucket_மொத்தம்());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.counter);
}

வெறுமை
perf_reset_all(வெறுமை)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	போது (handle != nullptr) {
		எனில் (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
