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

#ಅನ್ತರ್ಭಾವ <inttypes.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <pthread.h>
#ಅನ್ತರ್ಭಾವ <systemlib/err.h>

#ಅನ್ತರ್ಭಾವ "perf_counter.h"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/atomic.h>

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
#ಯದಿ defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
template <typename T> using PerfVar = px4::atomic<T>;
#ಅನ್ಯಥಾ
template <typename T>
ವರ್ಗ PerfVar
{
ಸಾರ್ವಜನಿಕ:
	PerfVar() = ಸ್ವಭಾವ;
	T load() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ_2; }
	ಶೂನ್ಯ store(T value) { ಮೌಲ್ಯ_2 = value; }
	T fetch_add(T value) { ಧ್ರುವ T previous = ಮೌಲ್ಯ_2; ಮೌಲ್ಯ_2 += value; ಪ್ರತ್ಯಾಗಮ previous; }
ಗುಪ್ತ:
	T ಮೌಲ್ಯ_2 {};
};
#endif

/**
 * Header common to all counters.
 */
ಸಂರಚನಾ perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	ಗಣನಾ perf_counter_type	type;	/**< counter type */
	ತಾರ್ಕಿಕ			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ಧ್ರುವ ವರ್ಣ		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
ಸಂರಚನಾ perf_ctr_ಎಣಿಕೆ : ಸಾರ್ವಜನಿಕ perf_ctr_header {
	PerfVar<uint64_t>	event_ಎಣಿಕೆ;
};

/**
 * PC_ELAPSED counter.
 */
ಸಂರಚನಾ perf_ctr_elapsed : ಸಾರ್ವಜನಿಕ perf_ctr_header {
	PerfVar<uint64_t>	event_ಎಣಿಕೆ;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_total;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<ಭಿನ್ನ>	mean;
	PerfVar<ಭಿನ್ನ>	M2;
};

/**
 * PC_INTERVAL counter.
 */
ಸಂರಚನಾ perf_ctr_interval : ಸಾರ್ವಜನಿಕ perf_ctr_header {
	PerfVar<uint64_t>	event_ಎಣಿಕೆ;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<ಭಿನ್ನ>	mean;
	PerfVar<ಭಿನ್ನ>	M2;
};

/**
 * List of all known counters.
 */
ಸ್ಥಿರ sq_queue_t	perf_counters = { nullptr, nullptr };

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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ perf_iterate_active = 0;

ಸ್ಥಿರ ಶೂನ್ಯ
perf_delete(perf_counter_t handle)
{
	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ಎಣಿಕೆ:
		delete (ಸಂರಚನಾ perf_ctr_ಎಣಿಕೆ *)handle;
		ಭಙ್ಗ;

	ಪಕ್ಷ PC_ELAPSED:
		delete (ಸಂರಚನಾ perf_ctr_elapsed *)handle;
		ಭಙ್ಗ;

	ಪಕ್ಷ PC_INTERVAL:
		delete (ಸಂರಚನಾ perf_ctr_interval *)handle;
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಯಾವತ್ (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		ಯದಿ (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(ಗಣನಾ perf_counter_type type, ಧ್ರುವ ವರ್ಣ *name)
{
	perf_counter_t ctr = nullptr;

	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ PC_ಎಣಿಕೆ:
		ctr = ನೂತನ perf_ctr_ಎಣಿಕೆ();
		ಭಙ್ಗ;

	ಪಕ್ಷ PC_ELAPSED:
		ctr = ನೂತನ perf_ctr_elapsed();
		ಭಙ್ಗ;

	ಪಕ್ಷ PC_INTERVAL:
		ctr = ನೂತನ perf_ctr_interval();
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಯದಿ (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = ಅಸತ್ಯ;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	ಪ್ರತ್ಯಾಗಮ ctr;
}

perf_counter_t
perf_alloc_once(ಗಣನಾ perf_counter_type type, ಧ್ರುವ ವರ್ಣ *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಯಾವತ್ (handle != nullptr) {
		ಯದಿ (!handle->pending_free && !strcmp(handle->name, name)) {
			ಯದಿ (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				ಪ್ರತ್ಯಾಗಮ handle;

			} ಅನ್ಯಥಾ {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				ಪ್ರತ್ಯಾಗಮ nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	ಪ್ರತ್ಯಾಗಮ perf_alloc(type, name);
}

ಶೂನ್ಯ
perf_free(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	ಯದಿ (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = ಸತ್ಯ;
		pthread_mutex_unlock(&perf_counters_mutex);
		ಪ್ರತ್ಯಾಗಮ;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

ಶೂನ್ಯ
perf_ಎಣಿಕೆ(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ಎಣಿಕೆ:
		((ಸಂರಚನಾ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.fetch_add(1);
		ಭಙ್ಗ;

	ಪಕ್ಷ PC_INTERVAL:
		perf_ಎಣಿಕೆ_interval(handle, hrt_absolute_time());
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

ಶೂನ್ಯ
perf_begin(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ELAPSED:
		((ಸಂರಚನಾ perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

ಶೂನ್ಯ
perf_end(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ELAPSED: {
			ಸಂರಚನಾ perf_ctr_elapsed *pce = (ಸಂರಚನಾ perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			ಯದಿ (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

ಶೂನ್ಯ
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ELAPSED: {
			ಸಂರಚನಾ perf_ctr_elapsed *pce = (ಸಂರಚನಾ perf_ctr_elapsed *)handle;

			ಯದಿ (elapsed >= 0) {
				uint64_t ec = pce->event_ಎಣಿಕೆ.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				ಯದಿ ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.load();

				ಯದಿ (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				ಭಿನ್ನ dt = elapsed / 1e6f;
				ಭಿನ್ನ mean_val = pce->mean.load();
				ಭಿನ್ನ delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				ಭಿನ್ನ m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

ಶೂನ್ಯ
perf_ಎಣಿಕೆ_interval(perf_counter_t handle, hrt_abstime now)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_INTERVAL: {
			ಸಂರಚನಾ perf_ctr_interval *pci = (ಸಂರಚನಾ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ಎಣಿಕೆ.load();

			ವಿಕಲ್ಪ (ec) {
			ಪಕ್ಷ 0:
				pci->time_first.store(now);
				ಭಙ್ಗ;

			ಪಕ್ಷ 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					ಭಙ್ಗ;
				}

			ಸ್ವಭಾವ: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					ಯದಿ ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					ಯದಿ ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					ಭಿನ್ನ dt = interval / 1e6f;
					ಭಿನ್ನ mean_val = pci->mean.load();
					ಭಿನ್ನ delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					ಭಿನ್ನ m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					ಭಙ್ಗ;
				}
			}

			pci->time_last.store(now);
			pci->event_ಎಣಿಕೆ.fetch_add(1);
			ಭಙ್ಗ;
		}

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

ಶೂನ್ಯ
perf_set_ಎಣಿಕೆ(perf_counter_t handle, uint64_t ಎಣಿಕೆ)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ಎಣಿಕೆ: {
			((ಸಂರಚನಾ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.store(ಎಣಿಕೆ);
		}
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

}

ಶೂನ್ಯ
perf_cancel(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ELAPSED: {
			ಸಂರಚನಾ perf_ctr_elapsed *pce = (ಸಂರಚನಾ perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

ಶೂನ್ಯ
perf_reset(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ಎಣಿಕೆ:
		((ಸಂರಚನಾ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.store((uint64_t)0);
		ಭಙ್ಗ;

	ಪಕ್ಷ PC_ELAPSED: {
			ಸಂರಚನಾ perf_ctr_elapsed *pce = (ಸಂರಚನಾ perf_ctr_elapsed *)handle;
			pce->event_ಎಣಿಕೆ.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			ಭಙ್ಗ;
		}

	ಪಕ್ಷ PC_INTERVAL: {
			ಸಂರಚನಾ perf_ctr_interval *pci = (ಸಂರಚನಾ perf_ctr_interval *)handle;
			pci->event_ಎಣಿಕೆ.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			ಭಙ್ಗ;
		}
	}
}

ಶೂನ್ಯ
perf_print_counter(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ಎಣಿಕೆ: {
			uint64_t ec = ((ಸಂರಚನಾ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			ಭಙ್ಗ;
		}

	ಪಕ್ಷ PC_ELAPSED: {
			ಸಂರಚನಾ perf_ctr_elapsed *pce = (ಸಂರಚನಾ perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ಎಣಿಕೆ.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			ಭಿನ್ನ m2 = pce->M2.load();
			ಭಿನ್ನ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (ದ್ವಿಗುಣ)tt / (ದ್ವಿಗುಣ)ec,
				     tl, tm, (ದ್ವಿಗುಣ)(1e6f * rms));
			ಭಙ್ಗ;
		}

	ಪಕ್ಷ PC_INTERVAL: {
			ಸಂರಚನಾ perf_ctr_interval *pci = (ಸಂರಚನಾ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ಎಣಿಕೆ.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			ಭಿನ್ನ m2 = pci->M2.load();
			ಭಿನ್ನ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (ದ್ವಿಗುಣ)(tla - tf) / (ದ್ವಿಗುಣ)ec,
				     tl, tm, (ದ್ವಿಗುಣ)(1e6f * rms));
			ಭಙ್ಗ;
		}

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}


ಪೂರ್ಣಾಙ್ಕ
perf_print_counter_buffer(ವರ್ಣ *buffer, ಪೂರ್ಣಾಙ್ಕ length, perf_counter_t handle)
{
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_written = 0;

	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ಎಣಿಕೆ:
		ಅಂಕೆ_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((ಸಂರಚನಾ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.load());
		ಭಙ್ಗ;

	ಪಕ್ಷ PC_ELAPSED: {
			ಸಂರಚನಾ perf_ctr_elapsed *pce = (ಸಂರಚನಾ perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ಎಣಿಕೆ.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			ಭಿನ್ನ m2 = pce->M2.load();
			ಭಿನ್ನ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			ಅಂಕೆ_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (ದ್ವಿಗುಣ)tt / (ದ್ವಿಗುಣ)ec,
					       tl,
					       tm,
					       (ದ್ವಿಗುಣ)(1e6f * rms));
			ಭಙ್ಗ;
		}

	ಪಕ್ಷ PC_INTERVAL: {
			ಸಂರಚನಾ perf_ctr_interval *pci = (ಸಂರಚನಾ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ಎಣಿಕೆ.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			ಭಿನ್ನ m2 = pci->M2.load();
			ಭಿನ್ನ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			ಅಂಕೆ_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (ದ್ವಿಗುಣ)(pci->time_last.load() - pci->time_first.load()) / (ದ್ವಿಗುಣ)ec,
					       tl,
					       tm,
					       (ದ್ವಿಗುಣ)(1e6f * rms));
			ಭಙ್ಗ;
		}

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_written;
}

uint64_t
perf_event_ಎಣಿಕೆ(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ಎಣಿಕೆ:
		ಪ್ರತ್ಯಾಗಮ ((ಸಂರಚನಾ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.load();

	ಪಕ್ಷ PC_ELAPSED: {
			ಸಂರಚನಾ perf_ctr_elapsed *pce = (ಸಂರಚನಾ perf_ctr_elapsed *)handle;
			ಪ್ರತ್ಯಾಗಮ pce->event_ಎಣಿಕೆ.load();
		}

	ಪಕ್ಷ PC_INTERVAL: {
			ಸಂರಚನಾ perf_ctr_interval *pci = (ಸಂರಚನಾ perf_ctr_interval *)handle;
			ಪ್ರತ್ಯಾಗಮ pci->event_ಎಣಿಕೆ.load();
		}

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಭಿನ್ನ
perf_mean(perf_counter_t handle)
{
	ಯದಿ (handle == nullptr) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ವಿಕಲ್ಪ (handle->type) {
	ಪಕ್ಷ PC_ELAPSED: {
			ಸಂರಚನಾ perf_ctr_elapsed *pce = (ಸಂರಚನಾ perf_ctr_elapsed *)handle;
			ಪ್ರತ್ಯಾಗಮ pce->mean.load();
		}

	ಪಕ್ಷ PC_INTERVAL: {
			ಸಂರಚನಾ perf_ctr_interval *pci = (ಸಂರಚನಾ perf_ctr_interval *)handle;
			ಪ್ರತ್ಯಾಗಮ pci->mean.load();
		}

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ 0.0f;
}

ಶೂನ್ಯ
perf_iterate_all(perf_callback cb, ಶೂನ್ಯ *user)
{
	ಸ್ಥಿರ constexpr size_t PERF_COUNTER_ಸಾಲು_BUFFER_SIZE = 220;
	ವರ್ಣ buffer[PERF_COUNTER_ಸಾಲು_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಯಾವತ್ (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		ಯದಿ (!handle->pending_free) {
			perf_print_counter_buffer(buffer, ಪರಿಮಾಣ(buffer), handle);

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

	ಯದಿ (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ಶೂನ್ಯ
perf_print_all(ಶೂನ್ಯ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಯಾವತ್ (handle != nullptr) {
		ಯದಿ (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ಶೂನ್ಯ
perf_print_latency(ಶೂನ್ಯ)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < get_latency_bucket_ಎಣಿಕೆ(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long ಪೂರ್ಣಾಙ್ಕ)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_ಎಣಿಕೆ() - 1, get_latency_bucket_ಎಣಿಕೆ());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.counter);
}

ಶೂನ್ಯ
perf_reset_all(ಶೂನ್ಯ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಯಾವತ್ (handle != nullptr) {
		ಯದಿ (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
