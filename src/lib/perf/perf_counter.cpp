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

#ضمن <inttypes.h>
#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <drivers/drv_hrt.h>
#ضمن <math.h>
#ضمن <pthread.h>
#ضمن <systemlib/err.h>

#ضمن "perf_counter.h"

#ضمن <px4_platform_common/atomic.h>

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
#ܐܢ defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
template <typename T> using PerfVar = px4::atomic<T>;
#ܐܚܪܢܐ
template <typename T>
صنف PerfVar
{
عام:
	PerfVar() = افتراضي;
	T load() ثابت { ܦܢܐ _value; }
	فراغ store(T value) { _value = value; }
	T fetch_add(T value) { ثابت T previous = _value; _value += value; ܦܢܐ previous; }
خاص:
	T _value {};
};
#endif

/**
 * Header common to all counters.
 */
بنية perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	تعداد perf_counter_type	type;	/**< counter type */
	منطقي			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ثابت حرف		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
بنية perf_ctr_count : عام perf_ctr_header {
	PerfVar<uint64_t>	event_count;
};

/**
 * PC_ELAPSED counter.
 */
بنية perf_ctr_elapsed : عام perf_ctr_header {
	PerfVar<uint64_t>	event_count;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_total;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<عائم>	mean;
	PerfVar<عائم>	M2;
};

/**
 * PC_INTERVAL counter.
 */
بنية perf_ctr_interval : عام perf_ctr_header {
	PerfVar<uint64_t>	event_count;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<عائم>	mean;
	PerfVar<عائم>	M2;
};

/**
 * List of all known counters.
 */
ساكن sq_queue_t	perf_counters = { nullptr, nullptr };

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
ساكن صحيح perf_iterate_active = 0;

ساكن فراغ
perf_delete(perf_counter_t handle)
{
	بدل (handle->type) {
	حالة PC_COUNT:
		delete (بنية perf_ctr_count *)handle;
		اكسر;

	حالة PC_ELAPSED:
		delete (بنية perf_ctr_elapsed *)handle;
		اكسر;

	حالة PC_INTERVAL:
		delete (بنية perf_ctr_interval *)handle;
		اكسر;

	افتراضي:
		اكسر;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
ساكن فراغ
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ܟܡܐ (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		ܐܢ (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(تعداد perf_counter_type type, ثابت حرف *name)
{
	perf_counter_t ctr = nullptr;

	بدل (type) {
	حالة PC_COUNT:
		ctr = جديد perf_ctr_count();
		اكسر;

	حالة PC_ELAPSED:
		ctr = جديد perf_ctr_elapsed();
		اكسر;

	حالة PC_INTERVAL:
		ctr = جديد perf_ctr_interval();
		اكسر;

	افتراضي:
		اكسر;
	}

	ܐܢ (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = ܕܓܠܐ;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	ܦܢܐ ctr;
}

perf_counter_t
perf_alloc_once(تعداد perf_counter_type type, ثابت حرف *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ܟܡܐ (handle != nullptr) {
		ܐܢ (!handle->pending_free && !strcmp(handle->name, name)) {
			ܐܢ (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				ܦܢܐ handle;

			} ܐܚܪܢܐ {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				ܦܢܐ nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	ܦܢܐ perf_alloc(type, name);
}

فراغ
perf_free(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	ܐܢ (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = ܫܪܪܐ;
		pthread_mutex_unlock(&perf_counters_mutex);
		ܦܢܐ;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

فراغ
perf_count(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_COUNT:
		((بنية perf_ctr_count *)handle)->event_count.fetch_add(1);
		اكسر;

	حالة PC_INTERVAL:
		perf_count_interval(handle, hrt_absolute_time());
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_begin(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED:
		((بنية perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_end(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			ܐܢ (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;

			ܐܢ (elapsed >= 0) {
				uint64_t ec = pce->event_count.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				ܐܢ ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.load();

				ܐܢ (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				عائم dt = elapsed / 1e6f;
				عائم mean_val = pce->mean.load();
				عائم delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				عائم m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_count_interval(perf_counter_t handle, hrt_abstime now)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			uint64_t ec = pci->event_count.load();

			بدل (ec) {
			حالة 0:
				pci->time_first.store(now);
				اكسر;

			حالة 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					اكسر;
				}

			افتراضي: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					ܐܢ ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					ܐܢ ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					عائم dt = interval / 1e6f;
					عائم mean_val = pci->mean.load();
					عائم delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					عائم m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					اكسر;
				}
			}

			pci->time_last.store(now);
			pci->event_count.fetch_add(1);
			اكسر;
		}

	افتراضي:
		اكسر;
	}
}

فراغ
perf_set_count(perf_counter_t handle, uint64_t count)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_COUNT: {
			((بنية perf_ctr_count *)handle)->event_count.store(count);
		}
		اكسر;

	افتراضي:
		اكسر;
	}

}

فراغ
perf_cancel(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_reset(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_COUNT:
		((بنية perf_ctr_count *)handle)->event_count.store((uint64_t)0);
		اكسر;

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			pce->event_count.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			اكسر;
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			pci->event_count.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			اكسر;
		}
	}
}

فراغ
perf_print_counter(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ;
	}

	بدل (handle->type) {
	حالة PC_COUNT: {
			uint64_t ec = ((بنية perf_ctr_count *)handle)->event_count.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			اكسر;
		}

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_count.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			عائم m2 = pce->M2.load();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (مزدوج)tt / (مزدوج)ec,
				     tl, tm, (مزدوج)(1e6f * rms));
			اكسر;
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			uint64_t ec = pci->event_count.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			عائم m2 = pci->M2.load();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (مزدوج)(tla - tf) / (مزدوج)ec,
				     tl, tm, (مزدوج)(1e6f * rms));
			اكسر;
		}

	افتراضي:
		اكسر;
	}
}


صحيح
perf_print_counter_buffer(حرف *buffer, صحيح length, perf_counter_t handle)
{
	صحيح num_written = 0;

	ܐܢ (handle == nullptr) {
		ܦܢܐ 0;
	}

	بدل (handle->type) {
	حالة PC_COUNT:
		num_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((بنية perf_ctr_count *)handle)->event_count.load());
		اكسر;

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_count.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			عائم m2 = pce->M2.load();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			num_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (مزدوج)tt / (مزدوج)ec,
					       tl,
					       tm,
					       (مزدوج)(1e6f * rms));
			اكسر;
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			uint64_t ec = pci->event_count.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			عائم m2 = pci->M2.load();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			num_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (مزدوج)(pci->time_last.load() - pci->time_first.load()) / (مزدوج)ec,
					       tl,
					       tm,
					       (مزدوج)(1e6f * rms));
			اكسر;
		}

	افتراضي:
		اكسر;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	ܦܢܐ num_written;
}

uint64_t
perf_event_count(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ 0;
	}

	بدل (handle->type) {
	حالة PC_COUNT:
		ܦܢܐ ((بنية perf_ctr_count *)handle)->event_count.load();

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			ܦܢܐ pce->event_count.load();
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			ܦܢܐ pci->event_count.load();
		}

	افتراضي:
		اكسر;
	}

	ܦܢܐ 0;
}

عائم
perf_mean(perf_counter_t handle)
{
	ܐܢ (handle == nullptr) {
		ܦܢܐ 0;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			ܦܢܐ pce->mean.load();
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			ܦܢܐ pci->mean.load();
		}

	افتراضي:
		اكسر;
	}

	ܦܢܐ 0.0f;
}

فراغ
perf_iterate_all(perf_callback cb, فراغ *user)
{
	ساكن constexpr size_t PERF_COUNTER_LINE_BUFFER_SIZE = 220;
	حرف buffer[PERF_COUNTER_LINE_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ܟܡܐ (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		ܐܢ (!handle->pending_free) {
			perf_print_counter_buffer(buffer, حجم(buffer), handle);

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

	ܐܢ (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

فراغ
perf_print_all(فراغ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ܟܡܐ (handle != nullptr) {
		ܐܢ (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

فراغ
perf_print_latency(فراغ)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	لكل (صحيح i = 0; i < get_latency_bucket_count(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long صحيح)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_count() - 1, get_latency_bucket_count());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.counter);
}

فراغ
perf_reset_all(فراغ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ܟܡܐ (handle != nullptr) {
		ܐܢ (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
