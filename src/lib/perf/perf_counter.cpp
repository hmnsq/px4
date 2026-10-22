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

#અન્તર્ભાવ <inttypes.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <drivers/drv_hrt.h>
#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <pthread.h>
#અન્તર્ભાવ <systemlib/err.h>

#અન્તર્ભાવ "perf_counter.h"

#અન્તર્ભાવ <px4_platform_common/atomic.h>

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
#જો defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
template <typename T> using PerfVar = px4::atomic<T>;
#નહીંતર
template <typename T>
વર્ગ PerfVar
{
જાહેર:
	PerfVar() = મૂળભૂત;
	T load() અચલ { પાછા કિંમત_2; }
	શૂન્ય store(T value) { કિંમત_2 = value; }
	T fetch_add(T value) { અચલ T previous = કિંમત_2; કિંમત_2 += value; પાછા previous; }
ખાનગી:
	T કિંમત_2 {};
};
#endif

/**
 * Header common to all counters.
 */
સંરચના perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	ગણના perf_counter_type	type;	/**< counter type */
	તાર્કિક			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	અચલ અક્ષર		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
સંરચના perf_ctr_ગણતરી : જાહેર perf_ctr_header {
	PerfVar<uint64_t>	event_ગણતરી;
};

/**
 * PC_ELAPSED counter.
 */
સંરચના perf_ctr_elapsed : જાહેર perf_ctr_header {
	PerfVar<uint64_t>	event_ગણતરી;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_total;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<દશાંશ>	mean;
	PerfVar<દશાંશ>	M2;
};

/**
 * PC_INTERVAL counter.
 */
સંરચના perf_ctr_interval : જાહેર perf_ctr_header {
	PerfVar<uint64_t>	event_ગણતરી;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<દશાંશ>	mean;
	PerfVar<દશાંશ>	M2;
};

/**
 * List of all known counters.
 */
સ્થિર sq_queue_t	perf_counters = { nullptr, nullptr };

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
સ્થિર પૂર્ણાંક perf_iterate_active = 0;

સ્થિર શૂન્ય
perf_delete(perf_counter_t handle)
{
	પસંદગી (handle->type) {
	સ્થિતિ PC_ગણતરી:
		delete (સંરચના perf_ctr_ગણતરી *)handle;
		તોડો;

	સ્થિતિ PC_ELAPSED:
		delete (સંરચના perf_ctr_elapsed *)handle;
		તોડો;

	સ્થિતિ PC_INTERVAL:
		delete (સંરચના perf_ctr_interval *)handle;
		તોડો;

	મૂળભૂત:
		તોડો;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
સ્થિર શૂન્ય
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	જ્યારે (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		જો (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(ગણના perf_counter_type type, અચલ અક્ષર *name)
{
	perf_counter_t ctr = nullptr;

	પસંદગી (type) {
	સ્થિતિ PC_ગણતરી:
		ctr = નવું perf_ctr_ગણતરી();
		તોડો;

	સ્થિતિ PC_ELAPSED:
		ctr = નવું perf_ctr_elapsed();
		તોડો;

	સ્થિતિ PC_INTERVAL:
		ctr = નવું perf_ctr_interval();
		તોડો;

	મૂળભૂત:
		તોડો;
	}

	જો (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = ખોટું;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	પાછા ctr;
}

perf_counter_t
perf_alloc_once(ગણના perf_counter_type type, અચલ અક્ષર *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	જ્યારે (handle != nullptr) {
		જો (!handle->pending_free && !strcmp(handle->name, name)) {
			જો (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				પાછા handle;

			} નહીંતર {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				પાછા nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	પાછા perf_alloc(type, name);
}

શૂન્ય
perf_free(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	જો (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = સાચું;
		pthread_mutex_unlock(&perf_counters_mutex);
		પાછા;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

શૂન્ય
perf_ગણતરી(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ગણતરી:
		((સંરચના perf_ctr_ગણતરી *)handle)->event_ગણતરી.fetch_add(1);
		તોડો;

	સ્થિતિ PC_INTERVAL:
		perf_ગણતરી_interval(handle, hrt_absolute_time());
		તોડો;

	મૂળભૂત:
		તોડો;
	}
}

શૂન્ય
perf_begin(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ELAPSED:
		((સંરચના perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		તોડો;

	મૂળભૂત:
		તોડો;
	}
}

શૂન્ય
perf_end(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ELAPSED: {
			સંરચના perf_ctr_elapsed *pce = (સંરચના perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			જો (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		તોડો;

	મૂળભૂત:
		તોડો;
	}
}

શૂન્ય
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ELAPSED: {
			સંરચના perf_ctr_elapsed *pce = (સંરચના perf_ctr_elapsed *)handle;

			જો (elapsed >= 0) {
				uint64_t ec = pce->event_ગણતરી.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				જો ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.load();

				જો (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				દશાંશ dt = elapsed / 1e6f;
				દશાંશ mean_val = pce->mean.load();
				દશાંશ delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				દશાંશ m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		તોડો;

	મૂળભૂત:
		તોડો;
	}
}

શૂન્ય
perf_ગણતરી_interval(perf_counter_t handle, hrt_abstime now)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_INTERVAL: {
			સંરચના perf_ctr_interval *pci = (સંરચના perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ગણતરી.load();

			પસંદગી (ec) {
			સ્થિતિ 0:
				pci->time_first.store(now);
				તોડો;

			સ્થિતિ 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					તોડો;
				}

			મૂળભૂત: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					જો ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					જો ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					દશાંશ dt = interval / 1e6f;
					દશાંશ mean_val = pci->mean.load();
					દશાંશ delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					દશાંશ m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					તોડો;
				}
			}

			pci->time_last.store(now);
			pci->event_ગણતરી.fetch_add(1);
			તોડો;
		}

	મૂળભૂત:
		તોડો;
	}
}

શૂન્ય
perf_set_ગણતરી(perf_counter_t handle, uint64_t ગણતરી)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ગણતરી: {
			((સંરચના perf_ctr_ગણતરી *)handle)->event_ગણતરી.store(ગણતરી);
		}
		તોડો;

	મૂળભૂત:
		તોડો;
	}

}

શૂન્ય
perf_cancel(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ELAPSED: {
			સંરચના perf_ctr_elapsed *pce = (સંરચના perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		તોડો;

	મૂળભૂત:
		તોડો;
	}
}

શૂન્ય
perf_reset(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ગણતરી:
		((સંરચના perf_ctr_ગણતરી *)handle)->event_ગણતરી.store((uint64_t)0);
		તોડો;

	સ્થિતિ PC_ELAPSED: {
			સંરચના perf_ctr_elapsed *pce = (સંરચના perf_ctr_elapsed *)handle;
			pce->event_ગણતરી.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			તોડો;
		}

	સ્થિતિ PC_INTERVAL: {
			સંરચના perf_ctr_interval *pci = (સંરચના perf_ctr_interval *)handle;
			pci->event_ગણતરી.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			તોડો;
		}
	}
}

શૂન્ય
perf_print_counter(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ગણતરી: {
			uint64_t ec = ((સંરચના perf_ctr_ગણતરી *)handle)->event_ગણતરી.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			તોડો;
		}

	સ્થિતિ PC_ELAPSED: {
			સંરચના perf_ctr_elapsed *pce = (સંરચના perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ગણતરી.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			દશાંશ m2 = pce->M2.load();
			દશાંશ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (દ્વિગુણ)tt / (દ્વિગુણ)ec,
				     tl, tm, (દ્વિગુણ)(1e6f * rms));
			તોડો;
		}

	સ્થિતિ PC_INTERVAL: {
			સંરચના perf_ctr_interval *pci = (સંરચના perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ગણતરી.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			દશાંશ m2 = pci->M2.load();
			દશાંશ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (દ્વિગુણ)(tla - tf) / (દ્વિગુણ)ec,
				     tl, tm, (દ્વિગુણ)(1e6f * rms));
			તોડો;
		}

	મૂળભૂત:
		તોડો;
	}
}


પૂર્ણાંક
perf_print_counter_buffer(અક્ષર *buffer, પૂર્ણાંક length, perf_counter_t handle)
{
	પૂર્ણાંક અંક_written = 0;

	જો (handle == nullptr) {
		પાછા 0;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ગણતરી:
		અંક_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((સંરચના perf_ctr_ગણતરી *)handle)->event_ગણતરી.load());
		તોડો;

	સ્થિતિ PC_ELAPSED: {
			સંરચના perf_ctr_elapsed *pce = (સંરચના perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ગણતરી.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			દશાંશ m2 = pce->M2.load();
			દશાંશ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			અંક_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (દ્વિગુણ)tt / (દ્વિગુણ)ec,
					       tl,
					       tm,
					       (દ્વિગુણ)(1e6f * rms));
			તોડો;
		}

	સ્થિતિ PC_INTERVAL: {
			સંરચના perf_ctr_interval *pci = (સંરચના perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ગણતરી.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			દશાંશ m2 = pci->M2.load();
			દશાંશ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			અંક_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (દ્વિગુણ)(pci->time_last.load() - pci->time_first.load()) / (દ્વિગુણ)ec,
					       tl,
					       tm,
					       (દ્વિગુણ)(1e6f * rms));
			તોડો;
		}

	મૂળભૂત:
		તોડો;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	પાછા અંક_written;
}

uint64_t
perf_event_ગણતરી(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા 0;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ગણતરી:
		પાછા ((સંરચના perf_ctr_ગણતરી *)handle)->event_ગણતરી.load();

	સ્થિતિ PC_ELAPSED: {
			સંરચના perf_ctr_elapsed *pce = (સંરચના perf_ctr_elapsed *)handle;
			પાછા pce->event_ગણતરી.load();
		}

	સ્થિતિ PC_INTERVAL: {
			સંરચના perf_ctr_interval *pci = (સંરચના perf_ctr_interval *)handle;
			પાછા pci->event_ગણતરી.load();
		}

	મૂળભૂત:
		તોડો;
	}

	પાછા 0;
}

દશાંશ
perf_mean(perf_counter_t handle)
{
	જો (handle == nullptr) {
		પાછા 0;
	}

	પસંદગી (handle->type) {
	સ્થિતિ PC_ELAPSED: {
			સંરચના perf_ctr_elapsed *pce = (સંરચના perf_ctr_elapsed *)handle;
			પાછા pce->mean.load();
		}

	સ્થિતિ PC_INTERVAL: {
			સંરચના perf_ctr_interval *pci = (સંરચના perf_ctr_interval *)handle;
			પાછા pci->mean.load();
		}

	મૂળભૂત:
		તોડો;
	}

	પાછા 0.0f;
}

શૂન્ય
perf_iterate_all(perf_callback cb, શૂન્ય *user)
{
	સ્થિર constexpr size_t PERF_COUNTER_લીટી_BUFFER_SIZE = 220;
	અક્ષર buffer[PERF_COUNTER_લીટી_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	જ્યારે (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		જો (!handle->pending_free) {
			perf_print_counter_buffer(buffer, કદ(buffer), handle);

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

	જો (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

શૂન્ય
perf_print_all(શૂન્ય)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	જ્યારે (handle != nullptr) {
		જો (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

શૂન્ય
perf_print_latency(શૂન્ય)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	માટે (પૂર્ણાંક i = 0; i < get_latency_bucket_ગણતરી(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long પૂર્ણાંક)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_ગણતરી() - 1, get_latency_bucket_ગણતરી());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.counter);
}

શૂન્ય
perf_reset_all(શૂન્ય)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	જ્યારે (handle != nullptr) {
		જો (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
