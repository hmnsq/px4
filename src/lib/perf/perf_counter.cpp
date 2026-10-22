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

#অন্তর্ভাব <inttypes.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <drivers/drv_hrt.h>
#অন্তর্ভাব <math.h>
#অন্তর্ভাব <pthread.h>
#অন্তর্ভাব <systemlib/err.h>

#অন্তর্ভাব "perf_counter.h"

#অন্তর্ভাব <px4_platform_common/atomic.h>

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
#যদি defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
template <typename T> using PerfVar = px4::atomic<T>;
#অন্যথা
template <typename T>
শ্রেণী PerfVar
{
সর্বজনীন:
	PerfVar() = পূর্বনির্ধারিত;
	T load() ধ্রুবক { ফেরত মান_2; }
	শূন্য store(T value) { মান_2 = value; }
	T fetch_add(T value) { ধ্রুবক T previous = মান_2; মান_2 += value; ফেরত previous; }
ব্যক্তিগত:
	T মান_2 {};
};
#endif

/**
 * Header common to all counters.
 */
কাঠামো perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	গণনা perf_গণক_type	type;	/**< counter type */
	যৌক্তিক			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ধ্রুবক অক্ষর		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
কাঠামো perf_ctr_গুনতি : সর্বজনীন perf_ctr_header {
	PerfVar<uint64_t>	event_গুনতি;
};

/**
 * PC_ELAPSED counter.
 */
কাঠামো perf_ctr_elapsed : সর্বজনীন perf_ctr_header {
	PerfVar<uint64_t>	event_গুনতি;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_যোগ;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<দশমিক>	mean;
	PerfVar<দশমিক>	M2;
};

/**
 * PC_INTERVAL counter.
 */
কাঠামো perf_ctr_interval : সর্বজনীন perf_ctr_header {
	PerfVar<uint64_t>	event_গুনতি;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<দশমিক>	mean;
	PerfVar<দশমিক>	M2;
};

/**
 * List of all known counters.
 */
স্থির sq_queue_t	perf_counters = { nullptr, nullptr };

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
স্থির পূর্ণসংখ্যা perf_iterate_active = 0;

স্থির শূন্য
perf_delete(perf_গণক_t handle)
{
	নির্বাচন (handle->type) {
	ক্ষেত্র PC_গুনতি:
		delete (কাঠামো perf_ctr_গুনতি *)handle;
		বিরতি;

	ক্ষেত্র PC_ELAPSED:
		delete (কাঠামো perf_ctr_elapsed *)handle;
		বিরতি;

	ক্ষেত্র PC_INTERVAL:
		delete (কাঠামো perf_ctr_interval *)handle;
		বিরতি;

	পূর্বনির্ধারিত:
		বিরতি;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
স্থির শূন্য
perf_free_pending_locked()
{
	perf_গণক_t handle = (perf_গণক_t)sq_peek(&perf_counters);

	যখন (handle != nullptr) {
		perf_গণক_t পরবর্তী = (perf_গণক_t)sq_পরবর্তী(&handle->link);

		যদি (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = পরবর্তী;
	}
}

perf_গণক_t
perf_alloc(গণনা perf_গণক_type type, ধ্রুবক অক্ষর *name)
{
	perf_গণক_t ctr = nullptr;

	নির্বাচন (type) {
	ক্ষেত্র PC_গুনতি:
		ctr = নতুন perf_ctr_গুনতি();
		বিরতি;

	ক্ষেত্র PC_ELAPSED:
		ctr = নতুন perf_ctr_elapsed();
		বিরতি;

	ক্ষেত্র PC_INTERVAL:
		ctr = নতুন perf_ctr_interval();
		বিরতি;

	পূর্বনির্ধারিত:
		বিরতি;
	}

	যদি (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = মিথ্যা;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	ফেরত ctr;
}

perf_গণক_t
perf_alloc_once(গণনা perf_গণক_type type, ধ্রুবক অক্ষর *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_গণক_t handle = (perf_গণক_t)sq_peek(&perf_counters);

	যখন (handle != nullptr) {
		যদি (!handle->pending_free && !strcmp(handle->name, name)) {
			যদি (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				ফেরত handle;

			} অন্যথা {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				ফেরত nullptr;
			}
		}

		handle = (perf_গণক_t)sq_পরবর্তী(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	ফেরত perf_alloc(type, name);
}

শূন্য
perf_free(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	যদি (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = সত্য;
		pthread_mutex_unlock(&perf_counters_mutex);
		ফেরত;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

শূন্য
perf_গুনতি(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_গুনতি:
		((কাঠামো perf_ctr_গুনতি *)handle)->event_গুনতি.fetch_add(1);
		বিরতি;

	ক্ষেত্র PC_INTERVAL:
		perf_গুনতি_interval(handle, hrt_absolute_time());
		বিরতি;

	পূর্বনির্ধারিত:
		বিরতি;
	}
}

শূন্য
perf_begin(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_ELAPSED:
		((কাঠামো perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		বিরতি;

	পূর্বনির্ধারিত:
		বিরতি;
	}
}

শূন্য
perf_end(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_ELAPSED: {
			কাঠামো perf_ctr_elapsed *pce = (কাঠামো perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			যদি (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		বিরতি;

	পূর্বনির্ধারিত:
		বিরতি;
	}
}

শূন্য
perf_set_elapsed(perf_গণক_t handle, int64_t elapsed)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_ELAPSED: {
			কাঠামো perf_ctr_elapsed *pce = (কাঠামো perf_ctr_elapsed *)handle;

			যদি (elapsed >= 0) {
				uint64_t ec = pce->event_গুনতি.fetch_add(1) + 1;
				pce->time_যোগ.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				যদি ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.load();

				যদি (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				দশমিক dt = elapsed / 1e6f;
				দশমিক mean_val = pce->mean.load();
				দশমিক delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				দশমিক m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		বিরতি;

	পূর্বনির্ধারিত:
		বিরতি;
	}
}

শূন্য
perf_গুনতি_interval(perf_গণক_t handle, hrt_abstime now)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_INTERVAL: {
			কাঠামো perf_ctr_interval *pci = (কাঠামো perf_ctr_interval *)handle;
			uint64_t ec = pci->event_গুনতি.load();

			নির্বাচন (ec) {
			ক্ষেত্র 0:
				pci->time_first.store(now);
				বিরতি;

			ক্ষেত্র 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					বিরতি;
				}

			পূর্বনির্ধারিত: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					যদি ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					যদি ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					দশমিক dt = interval / 1e6f;
					দশমিক mean_val = pci->mean.load();
					দশমিক delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					দশমিক m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					বিরতি;
				}
			}

			pci->time_last.store(now);
			pci->event_গুনতি.fetch_add(1);
			বিরতি;
		}

	পূর্বনির্ধারিত:
		বিরতি;
	}
}

শূন্য
perf_set_গুনতি(perf_গণক_t handle, uint64_t গুনতি)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_গুনতি: {
			((কাঠামো perf_ctr_গুনতি *)handle)->event_গুনতি.store(গুনতি);
		}
		বিরতি;

	পূর্বনির্ধারিত:
		বিরতি;
	}

}

শূন্য
perf_cancel(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_ELAPSED: {
			কাঠামো perf_ctr_elapsed *pce = (কাঠামো perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		বিরতি;

	পূর্বনির্ধারিত:
		বিরতি;
	}
}

শূন্য
perf_reset(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_গুনতি:
		((কাঠামো perf_ctr_গুনতি *)handle)->event_গুনতি.store((uint64_t)0);
		বিরতি;

	ক্ষেত্র PC_ELAPSED: {
			কাঠামো perf_ctr_elapsed *pce = (কাঠামো perf_ctr_elapsed *)handle;
			pce->event_গুনতি.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_যোগ.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			বিরতি;
		}

	ক্ষেত্র PC_INTERVAL: {
			কাঠামো perf_ctr_interval *pci = (কাঠামো perf_ctr_interval *)handle;
			pci->event_গুনতি.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			বিরতি;
		}
	}
}

শূন্য
perf_print_গণক(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_গুনতি: {
			uint64_t ec = ((কাঠামো perf_ctr_গুনতি *)handle)->event_গুনতি.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			বিরতি;
		}

	ক্ষেত্র PC_ELAPSED: {
			কাঠামো perf_ctr_elapsed *pce = (কাঠামো perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_গুনতি.load();
			uint64_t tt = pce->time_যোগ.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			দশমিক m2 = pce->M2.load();
			দশমিক rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (দ্বিগুণ)tt / (দ্বিগুণ)ec,
				     tl, tm, (দ্বিগুণ)(1e6f * rms));
			বিরতি;
		}

	ক্ষেত্র PC_INTERVAL: {
			কাঠামো perf_ctr_interval *pci = (কাঠামো perf_ctr_interval *)handle;
			uint64_t ec = pci->event_গুনতি.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			দশমিক m2 = pci->M2.load();
			দশমিক rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (দ্বিগুণ)(tla - tf) / (দ্বিগুণ)ec,
				     tl, tm, (দ্বিগুণ)(1e6f * rms));
			বিরতি;
		}

	পূর্বনির্ধারিত:
		বিরতি;
	}
}


পূর্ণসংখ্যা
perf_print_গণক_বাফার(অক্ষর *বাফার_2, পূর্ণসংখ্যা দৈর্ঘ্য, perf_গণক_t handle)
{
	পূর্ণসংখ্যা অঙ্ক_written = 0;

	যদি (handle == nullptr) {
		ফেরত 0;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_গুনতি:
		অঙ্ক_written = snprintf(বাফার_2, দৈর্ঘ্য, "%s: %" PRIu64 " events",
				       handle->name,
				       ((কাঠামো perf_ctr_গুনতি *)handle)->event_গুনতি.load());
		বিরতি;

	ক্ষেত্র PC_ELAPSED: {
			কাঠামো perf_ctr_elapsed *pce = (কাঠামো perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_গুনতি.load();
			uint64_t tt = pce->time_যোগ.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			দশমিক m2 = pce->M2.load();
			দশমিক rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			অঙ্ক_written = snprintf(বাফার_2, দৈর্ঘ্য,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (দ্বিগুণ)tt / (দ্বিগুণ)ec,
					       tl,
					       tm,
					       (দ্বিগুণ)(1e6f * rms));
			বিরতি;
		}

	ক্ষেত্র PC_INTERVAL: {
			কাঠামো perf_ctr_interval *pci = (কাঠামো perf_ctr_interval *)handle;
			uint64_t ec = pci->event_গুনতি.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			দশমিক m2 = pci->M2.load();
			দশমিক rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			অঙ্ক_written = snprintf(বাফার_2, দৈর্ঘ্য,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (দ্বিগুণ)(pci->time_last.load() - pci->time_first.load()) / (দ্বিগুণ)ec,
					       tl,
					       tm,
					       (দ্বিগুণ)(1e6f * rms));
			বিরতি;
		}

	পূর্বনির্ধারিত:
		বিরতি;
	}

	বাফার_2[দৈর্ঘ্য - 1] = 0; // ensure 0-termination
	ফেরত অঙ্ক_written;
}

uint64_t
perf_event_গুনতি(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত 0;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_গুনতি:
		ফেরত ((কাঠামো perf_ctr_গুনতি *)handle)->event_গুনতি.load();

	ক্ষেত্র PC_ELAPSED: {
			কাঠামো perf_ctr_elapsed *pce = (কাঠামো perf_ctr_elapsed *)handle;
			ফেরত pce->event_গুনতি.load();
		}

	ক্ষেত্র PC_INTERVAL: {
			কাঠামো perf_ctr_interval *pci = (কাঠামো perf_ctr_interval *)handle;
			ফেরত pci->event_গুনতি.load();
		}

	পূর্বনির্ধারিত:
		বিরতি;
	}

	ফেরত 0;
}

দশমিক
perf_mean(perf_গণক_t handle)
{
	যদি (handle == nullptr) {
		ফেরত 0;
	}

	নির্বাচন (handle->type) {
	ক্ষেত্র PC_ELAPSED: {
			কাঠামো perf_ctr_elapsed *pce = (কাঠামো perf_ctr_elapsed *)handle;
			ফেরত pce->mean.load();
		}

	ক্ষেত্র PC_INTERVAL: {
			কাঠামো perf_ctr_interval *pci = (কাঠামো perf_ctr_interval *)handle;
			ফেরত pci->mean.load();
		}

	পূর্বনির্ধারিত:
		বিরতি;
	}

	ফেরত 0.0f;
}

শূন্য
perf_iterate_all(perf_callback cb, শূন্য *user)
{
	স্থির constexpr size_t PERF_গণক_সারি_বাফার_আকার = 220;
	অক্ষর বাফার_2[PERF_গণক_সারি_বাফার_আকার];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_গণক_t handle = (perf_গণক_t)sq_peek(&perf_counters);

	যখন (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_গণক_t পরবর্তী = (perf_গণক_t)sq_পরবর্তী(&handle->link);

		যদি (!handle->pending_free) {
			perf_print_গণক_বাফার(বাফার_2, আকার(বাফার_2), handle);

			// the callback may block (the logger waits for MAVLink acks here), so run
			// it without the global mutex: perf_alloc()/perf_free() on other threads
			// keep working, and 'next' stays valid because perf_free() defers deletion
			// while perf_iterate_active > 0
			pthread_mutex_unlock(&perf_counters_mutex);
			cb(বাফার_2, user);
			pthread_mutex_lock(&perf_counters_mutex);
		}

		handle = পরবর্তী;
	}

	যদি (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

শূন্য
perf_print_all(শূন্য)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_গণক_t handle = (perf_গণক_t)sq_peek(&perf_counters);

	যখন (handle != nullptr) {
		যদি (!handle->pending_free) {
			perf_print_গণক(handle);
		}

		handle = (perf_গণক_t)sq_পরবর্তী(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

শূন্য
perf_print_latency(শূন্য)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	জন্য (পূর্ণসংখ্যা i = 0; i < get_latency_bucket_গুনতি(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long পূর্ণসংখ্যা)latency.গণক);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_গুনতি() - 1, get_latency_bucket_গুনতি());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.গণক);
}

শূন্য
perf_reset_all(শূন্য)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_গণক_t handle = (perf_গণক_t)sq_peek(&perf_counters);

	যখন (handle != nullptr) {
		যদি (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_গণক_t)sq_পরবর্তী(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
