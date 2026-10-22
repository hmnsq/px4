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

#অন্তর্ভাৱ <inttypes.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <drivers/drv_hrt.h>
#অন্তর্ভাৱ <math.h>
#অন্তর্ভাৱ <pthread.h>
#অন্তর্ভাৱ <systemlib/err.h>

#অন্তর্ভাৱ "perf_counter.h"

#অন্তর্ভাৱ <px4_platform_common/atomic.h>

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
শ্ৰেণী PerfVar
{
সার্ৱজনিক:
	PerfVar() = স্ৱভাৱ;
	T load() ধ্রুৱ { ঘূৰাই মান_2; }
	শূন্য store(T value) { মান_2 = value; }
	T fetch_add(T value) { ধ্রুৱ T previous = মান_2; মান_2 += value; ঘূৰাই previous; }
গুপ্ত:
	T মান_2 {};
};
#endif

/**
 * Header common to all counters.
 */
সংরচনা perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	গণনা perf_counter_type	type;	/**< counter type */
	তার্কিক			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ধ্রুৱ ৱর্ণ		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
সংরচনা perf_ctr_গুনতি : সার্ৱজনিক perf_ctr_header {
	PerfVar<uint64_t>	event_গুনতি;
};

/**
 * PC_ELAPSED counter.
 */
সংরচনা perf_ctr_elapsed : সার্ৱজনিক perf_ctr_header {
	PerfVar<uint64_t>	event_গুনতি;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_total;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<ভিন্ন>	mean;
	PerfVar<ভিন্ন>	M2;
};

/**
 * PC_INTERVAL counter.
 */
সংরচনা perf_ctr_interval : সার্ৱজনিক perf_ctr_header {
	PerfVar<uint64_t>	event_গুনতি;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<ভিন্ন>	mean;
	PerfVar<ভিন্ন>	M2;
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
স্থির পূর্ণাঙ্ক perf_iterate_active = 0;

স্থির শূন্য
perf_delete(perf_counter_t handle)
{
	ৱিকল্প (handle->type) {
	পক্ষ PC_গুনতি:
		delete (সংরচনা perf_ctr_গুনতি *)handle;
		ভঙ্গ;

	পক্ষ PC_ELAPSED:
		delete (সংরচনা perf_ctr_elapsed *)handle;
		ভঙ্গ;

	পক্ষ PC_INTERVAL:
		delete (সংরচনা perf_ctr_interval *)handle;
		ভঙ্গ;

	স্ৱভাৱ:
		ভঙ্গ;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
স্থির শূন্য
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	যেতিয়ালৈ (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		যদি (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(গণনা perf_counter_type type, ধ্রুৱ ৱর্ণ *name)
{
	perf_counter_t ctr = nullptr;

	ৱিকল্প (type) {
	পক্ষ PC_গুনতি:
		ctr = নূতন perf_ctr_গুনতি();
		ভঙ্গ;

	পক্ষ PC_ELAPSED:
		ctr = নূতন perf_ctr_elapsed();
		ভঙ্গ;

	পক্ষ PC_INTERVAL:
		ctr = নূতন perf_ctr_interval();
		ভঙ্গ;

	স্ৱভাৱ:
		ভঙ্গ;
	}

	যদি (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = মিছা;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	ঘূৰাই ctr;
}

perf_counter_t
perf_alloc_once(গণনা perf_counter_type type, ধ্রুৱ ৱর্ণ *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	যেতিয়ালৈ (handle != nullptr) {
		যদি (!handle->pending_free && !strcmp(handle->name, name)) {
			যদি (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				ঘূৰাই handle;

			} অন্যথা {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				ঘূৰাই nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	ঘূৰাই perf_alloc(type, name);
}

শূন্য
perf_free(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	যদি (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = সঁচা;
		pthread_mutex_unlock(&perf_counters_mutex);
		ঘূৰাই;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

শূন্য
perf_গুনতি(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_গুনতি:
		((সংরচনা perf_ctr_গুনতি *)handle)->event_গুনতি.fetch_add(1);
		ভঙ্গ;

	পক্ষ PC_INTERVAL:
		perf_গুনতি_interval(handle, hrt_absolute_time());
		ভঙ্গ;

	স্ৱভাৱ:
		ভঙ্গ;
	}
}

শূন্য
perf_begin(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_ELAPSED:
		((সংরচনা perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		ভঙ্গ;

	স্ৱভাৱ:
		ভঙ্গ;
	}
}

শূন্য
perf_end(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_ELAPSED: {
			সংরচনা perf_ctr_elapsed *pce = (সংরচনা perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			যদি (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		ভঙ্গ;

	স্ৱভাৱ:
		ভঙ্গ;
	}
}

শূন্য
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_ELAPSED: {
			সংরচনা perf_ctr_elapsed *pce = (সংরচনা perf_ctr_elapsed *)handle;

			যদি (elapsed >= 0) {
				uint64_t ec = pce->event_গুনতি.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

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
				ভিন্ন dt = elapsed / 1e6f;
				ভিন্ন mean_val = pce->mean.load();
				ভিন্ন delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				ভিন্ন m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		ভঙ্গ;

	স্ৱভাৱ:
		ভঙ্গ;
	}
}

শূন্য
perf_গুনতি_interval(perf_counter_t handle, hrt_abstime now)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_INTERVAL: {
			সংরচনা perf_ctr_interval *pci = (সংরচনা perf_ctr_interval *)handle;
			uint64_t ec = pci->event_গুনতি.load();

			ৱিকল্প (ec) {
			পক্ষ 0:
				pci->time_first.store(now);
				ভঙ্গ;

			পক্ষ 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					ভঙ্গ;
				}

			স্ৱভাৱ: {
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
					ভিন্ন dt = interval / 1e6f;
					ভিন্ন mean_val = pci->mean.load();
					ভিন্ন delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					ভিন্ন m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					ভঙ্গ;
				}
			}

			pci->time_last.store(now);
			pci->event_গুনতি.fetch_add(1);
			ভঙ্গ;
		}

	স্ৱভাৱ:
		ভঙ্গ;
	}
}

শূন্য
perf_set_গুনতি(perf_counter_t handle, uint64_t গুনতি)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_গুনতি: {
			((সংরচনা perf_ctr_গুনতি *)handle)->event_গুনতি.store(গুনতি);
		}
		ভঙ্গ;

	স্ৱভাৱ:
		ভঙ্গ;
	}

}

শূন্য
perf_cancel(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_ELAPSED: {
			সংরচনা perf_ctr_elapsed *pce = (সংরচনা perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		ভঙ্গ;

	স্ৱভাৱ:
		ভঙ্গ;
	}
}

শূন্য
perf_reset(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_গুনতি:
		((সংরচনা perf_ctr_গুনতি *)handle)->event_গুনতি.store((uint64_t)0);
		ভঙ্গ;

	পক্ষ PC_ELAPSED: {
			সংরচনা perf_ctr_elapsed *pce = (সংরচনা perf_ctr_elapsed *)handle;
			pce->event_গুনতি.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			ভঙ্গ;
		}

	পক্ষ PC_INTERVAL: {
			সংরচনা perf_ctr_interval *pci = (সংরচনা perf_ctr_interval *)handle;
			pci->event_গুনতি.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			ভঙ্গ;
		}
	}
}

শূন্য
perf_print_counter(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_গুনতি: {
			uint64_t ec = ((সংরচনা perf_ctr_গুনতি *)handle)->event_গুনতি.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			ভঙ্গ;
		}

	পক্ষ PC_ELAPSED: {
			সংরচনা perf_ctr_elapsed *pce = (সংরচনা perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_গুনতি.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			ভিন্ন m2 = pce->M2.load();
			ভিন্ন rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (দ্ৱিগুণ)tt / (দ্ৱিগুণ)ec,
				     tl, tm, (দ্ৱিগুণ)(1e6f * rms));
			ভঙ্গ;
		}

	পক্ষ PC_INTERVAL: {
			সংরচনা perf_ctr_interval *pci = (সংরচনা perf_ctr_interval *)handle;
			uint64_t ec = pci->event_গুনতি.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			ভিন্ন m2 = pci->M2.load();
			ভিন্ন rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (দ্ৱিগুণ)(tla - tf) / (দ্ৱিগুণ)ec,
				     tl, tm, (দ্ৱিগুণ)(1e6f * rms));
			ভঙ্গ;
		}

	স্ৱভাৱ:
		ভঙ্গ;
	}
}


পূর্ণাঙ্ক
perf_print_counter_buffer(ৱর্ণ *buffer, পূর্ণাঙ্ক length, perf_counter_t handle)
{
	পূর্ণাঙ্ক অঙ্ক_written = 0;

	যদি (handle == nullptr) {
		ঘূৰাই 0;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_গুনতি:
		অঙ্ক_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((সংরচনা perf_ctr_গুনতি *)handle)->event_গুনতি.load());
		ভঙ্গ;

	পক্ষ PC_ELAPSED: {
			সংরচনা perf_ctr_elapsed *pce = (সংরচনা perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_গুনতি.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			ভিন্ন m2 = pce->M2.load();
			ভিন্ন rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			অঙ্ক_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (দ্ৱিগুণ)tt / (দ্ৱিগুণ)ec,
					       tl,
					       tm,
					       (দ্ৱিগুণ)(1e6f * rms));
			ভঙ্গ;
		}

	পক্ষ PC_INTERVAL: {
			সংরচনা perf_ctr_interval *pci = (সংরচনা perf_ctr_interval *)handle;
			uint64_t ec = pci->event_গুনতি.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			ভিন্ন m2 = pci->M2.load();
			ভিন্ন rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			অঙ্ক_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (দ্ৱিগুণ)(pci->time_last.load() - pci->time_first.load()) / (দ্ৱিগুণ)ec,
					       tl,
					       tm,
					       (দ্ৱিগুণ)(1e6f * rms));
			ভঙ্গ;
		}

	স্ৱভাৱ:
		ভঙ্গ;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	ঘূৰাই অঙ্ক_written;
}

uint64_t
perf_event_গুনতি(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই 0;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_গুনতি:
		ঘূৰাই ((সংরচনা perf_ctr_গুনতি *)handle)->event_গুনতি.load();

	পক্ষ PC_ELAPSED: {
			সংরচনা perf_ctr_elapsed *pce = (সংরচনা perf_ctr_elapsed *)handle;
			ঘূৰাই pce->event_গুনতি.load();
		}

	পক্ষ PC_INTERVAL: {
			সংরচনা perf_ctr_interval *pci = (সংরচনা perf_ctr_interval *)handle;
			ঘূৰাই pci->event_গুনতি.load();
		}

	স্ৱভাৱ:
		ভঙ্গ;
	}

	ঘূৰাই 0;
}

ভিন্ন
perf_mean(perf_counter_t handle)
{
	যদি (handle == nullptr) {
		ঘূৰাই 0;
	}

	ৱিকল্প (handle->type) {
	পক্ষ PC_ELAPSED: {
			সংরচনা perf_ctr_elapsed *pce = (সংরচনা perf_ctr_elapsed *)handle;
			ঘূৰাই pce->mean.load();
		}

	পক্ষ PC_INTERVAL: {
			সংরচনা perf_ctr_interval *pci = (সংরচনা perf_ctr_interval *)handle;
			ঘূৰাই pci->mean.load();
		}

	স্ৱভাৱ:
		ভঙ্গ;
	}

	ঘূৰাই 0.0f;
}

শূন্য
perf_iterate_all(perf_callback cb, শূন্য *user)
{
	স্থির constexpr size_t PERF_COUNTER_সাৰি_BUFFER_SIZE = 220;
	ৱর্ণ buffer[PERF_COUNTER_সাৰি_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	যেতিয়ালৈ (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		যদি (!handle->pending_free) {
			perf_print_counter_buffer(buffer, পরিমাণ(buffer), handle);

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

	যদি (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

শূন্য
perf_print_all(শূন্য)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	যেতিয়ালৈ (handle != nullptr) {
		যদি (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

শূন্য
perf_print_latency(শূন্য)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	বাবে (পূর্ণাঙ্ক i = 0; i < get_latency_bucket_গুনতি(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long পূর্ণাঙ্ক)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_গুনতি() - 1, get_latency_bucket_গুনতি());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.counter);
}

শূন্য
perf_reset_all(শূন্য)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	যেতিয়ালৈ (handle != nullptr) {
		যদি (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
