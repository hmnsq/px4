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
#ಆದರೆ defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು T> ಬಳಸಿ PerfVar = px4::atomic<T>;
#ಇಲ್ಲದಿದ್ದರೆ
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು T>
ವರ್ಗ PerfVar
{
ಸಾರ್ವಜನಿಕ:
	PerfVar() = ಪೂರ್ವನಿಯೋಜಿತ;
	T ತುಂಬು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_2; }
	ಶೂನ್ಯ store(T value) { ಮೌಲ್ಯ_2 = value; }
	T fetch_ಕೂಡಿಸು(T value) { ಸ್ಥಿರಾಂಕ T ಹಿಂದಿನ = ಮೌಲ್ಯ_2; ಮೌಲ್ಯ_2 += value; ಹಿಂತಿರುಗಿ ಹಿಂದಿನ; }
ಖಾಸಗಿ:
	T ಮೌಲ್ಯ_2 {};
};
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Header common to all counters.
 */
ರಚನೆ perf_ctr_ಶೀರ್ಷಿಕೆ {
	sq_ನಮೂದು_t		link;	/**< list linkage */
	ಎಣಿಕೆ perf_counter_type	type;	/**< counter type */
	ತಾರ್ಕಿಕ			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
ರಚನೆ perf_ctr_ಎಣಿಕೆ : ಸಾರ್ವಜನಿಕ perf_ctr_ಶೀರ್ಷಿಕೆ {
	PerfVar<uint64_t>	event_ಎಣಿಕೆ;
};

/**
 * PC_ELAPSED counter.
 */
ರಚನೆ perf_ctr_elapsed : ಸಾರ್ವಜನಿಕ perf_ctr_ಶೀರ್ಷಿಕೆ {
	PerfVar<uint64_t>	event_ಎಣಿಕೆ;
	PerfVar<uint64_t>	ಸಮಯ_ಆರಂಭ;
	PerfVar<uint64_t>	ಸಮಯ_ಒಟ್ಟು;
	PerfVar<uint32_t>	ಸಮಯ_ಕನಿಷ್ಠ;
	PerfVar<uint32_t>	ಸಮಯ_most;
	PerfVar<ದಶಮಾಂಶ>	ಸರಾಸರಿ;
	PerfVar<ದಶಮಾಂಶ>	M2;
};

/**
 * PC_INTERVAL counter.
 */
ರಚನೆ perf_ctr_interval : ಸಾರ್ವಜನಿಕ perf_ctr_ಶೀರ್ಷಿಕೆ {
	PerfVar<uint64_t>	event_ಎಣಿಕೆ;
	PerfVar<uint64_t>	ಸಮಯ_event;
	PerfVar<uint64_t>	ಸಮಯ_ಮೊದಲ;
	PerfVar<uint64_t>	ಸಮಯ_ಕೊನೆಯ;
	PerfVar<uint32_t>	ಸಮಯ_ಕನಿಷ್ಠ;
	PerfVar<uint32_t>	ಸಮಯ_most;
	PerfVar<ದಶಮಾಂಶ>	ಸರಾಸರಿ;
	PerfVar<ದಶಮಾಂಶ>	M2;
};

/**
 * List of all known counters.
 */
ಸ್ಥಿರ sq_ಸರತಿ_t	perf_counters = { ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ };

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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ perf_iterate_ಸಕ್ರಿಯ = 0;

ಸ್ಥಿರ ಶೂನ್ಯ
perf_delete(perf_counter_t handle)
{
	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ಎಣಿಕೆ:
		ಅಳಿಸು (ರಚನೆ perf_ctr_ಎಣಿಕೆ *)handle;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PC_ELAPSED:
		ಅಳಿಸು (ರಚನೆ perf_ctr_elapsed *)handle;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PC_INTERVAL:
		ಅಳಿಸು (ರಚನೆ perf_ctr_interval *)handle;
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
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

	ಆಗ (handle != ಶೂನ್ಯಸೂಚಕ) {
		perf_counter_t ಮುಂದಿನ = (perf_counter_t)sq_ಮುಂದಿನ(&handle->link);

		ಆದರೆ (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = ಮುಂದಿನ;
	}
}

perf_counter_t
perf_alloc(ಎಣಿಕೆ perf_counter_type type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	perf_counter_t ctr = ಶೂನ್ಯಸೂಚಕ;

	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ PC_ಎಣಿಕೆ:
		ctr = ಹೊಸ perf_ctr_ಎಣಿಕೆ();
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PC_ELAPSED:
		ctr = ಹೊಸ perf_ctr_elapsed();
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PC_INTERVAL:
		ctr = ಹೊಸ perf_ctr_interval();
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (ctr != ಶೂನ್ಯಸೂಚಕ) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = ಸುಳ್ಳು;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	ಹಿಂತಿರುಗಿ ctr;
}

perf_counter_t
perf_alloc_once(ಎಣಿಕೆ perf_counter_type type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಆಗ (handle != ಶೂನ್ಯಸೂಚಕ) {
		ಆದರೆ (!handle->pending_free && !strcmp(handle->name, name)) {
			ಆದರೆ (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				ಹಿಂತಿರುಗಿ handle;

			} ಇಲ್ಲದಿದ್ದರೆ {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
			}
		}

		handle = (perf_counter_t)sq_ಮುಂದಿನ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	ಹಿಂತಿರುಗಿ perf_alloc(type, name);
}

ಶೂನ್ಯ
perf_free(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	ಆದರೆ (perf_iterate_ಸಕ್ರಿಯ > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = ನಿಜ;
		pthread_mutex_unlock(&perf_counters_mutex);
		ಹಿಂತಿರುಗಿ;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

ಶೂನ್ಯ
perf_ಎಣಿಕೆ(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ಎಣಿಕೆ:
		((ರಚನೆ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.fetch_ಕೂಡಿಸು(1);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PC_INTERVAL:
		perf_ಎಣಿಕೆ_interval(handle, hrt_absolute_ಸಮಯ());
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಶೂನ್ಯ
perf_begin(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ELAPSED:
		((ರಚನೆ perf_ctr_elapsed *)handle)->ಸಮಯ_ಆರಂಭ.store(hrt_absolute_ಸಮಯ());
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಶೂನ್ಯ
perf_ಕೊನೆ(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ELAPSED: {
			ರಚನೆ perf_ctr_elapsed *pce = (ರಚನೆ perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->ಸಮಯ_ಆರಂಭ.ತುಂಬು();

			ಆದರೆ (ts != 0) {
				perf_ಗಣ_elapsed(handle, hrt_absolute_ಸಮಯ() - ts);
			}
		}
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಶೂನ್ಯ
perf_ಗಣ_elapsed(perf_counter_t handle, int64_t elapsed)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ELAPSED: {
			ರಚನೆ perf_ctr_elapsed *pce = (ರಚನೆ perf_ctr_elapsed *)handle;

			ಆದರೆ (elapsed >= 0) {
				uint64_t ec = pce->event_ಎಣಿಕೆ.fetch_ಕೂಡಿಸು(1) + 1;
				pce->ಸಮಯ_ಒಟ್ಟು.fetch_ಕೂಡಿಸು((uint64_t)elapsed);

				uint32_t tl = pce->ಸಮಯ_ಕನಿಷ್ಠ.ತುಂಬು();

				ಆದರೆ ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->ಸಮಯ_ಕನಿಷ್ಠ.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->ಸಮಯ_most.ತುಂಬು();

				ಆದರೆ (tm < (uint32_t)elapsed) {
					pce->ಸಮಯ_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				ದಶಮಾಂಶ dt = elapsed / 1e6f;
				ದಶಮಾಂಶ ಸರಾಸರಿ_val = pce->ಸರಾಸರಿ.ತುಂಬು();
				ದಶಮಾಂಶ delta_intvl = dt - ಸರಾಸರಿ_val;
				ಸರಾಸರಿ_val += delta_intvl / ec;
				pce->ಸರಾಸರಿ.store(ಸರಾಸರಿ_val);
				ದಶಮಾಂಶ m2_val = pce->M2.ತುಂಬು();
				m2_val += delta_intvl * (dt - ಸರಾಸರಿ_val);
				pce->M2.store(m2_val);

				pce->ಸಮಯ_ಆರಂಭ.store((uint64_t)0);
			}
		}
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಶೂನ್ಯ
perf_ಎಣಿಕೆ_interval(perf_counter_t handle, hrt_abstime now)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_INTERVAL: {
			ರಚನೆ perf_ctr_interval *pci = (ರಚನೆ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ಎಣಿಕೆ.ತುಂಬು();

			ಆಯ್ಕೆ (ec) {
			ಸ್ಥಿತಿ 0:
				pci->ಸಮಯ_ಮೊದಲ.store(now);
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ 1: {
					uint64_t tl = pci->ಸಮಯ_ಕೊನೆಯ.ತುಂಬು();
					uint32_t interval = (uint32_t)(now - tl);
					pci->ಸಮಯ_ಕನಿಷ್ಠ.store(interval);
					pci->ಸಮಯ_most.store(interval);
					pci->ಸರಾಸರಿ.store(interval / 1e6f);
					pci->M2.store(0.0f);
					ನಿಲ್ಲಿಸು;
				}

			ಪೂರ್ವನಿಯೋಜಿತ: {
					uint64_t tl = pci->ಸಮಯ_ಕೊನೆಯ.ತುಂಬು();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->ಸಮಯ_ಕನಿಷ್ಠ.ತುಂಬು();

					ಆದರೆ ((uint32_t)interval < least) {
						pci->ಸಮಯ_ಕನಿಷ್ಠ.store((uint32_t)interval);
					}

					uint32_t most = pci->ಸಮಯ_most.ತುಂಬು();

					ಆದರೆ ((uint32_t)interval > most) {
						pci->ಸಮಯ_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					ದಶಮಾಂಶ dt = interval / 1e6f;
					ದಶಮಾಂಶ ಸರಾಸರಿ_val = pci->ಸರಾಸರಿ.ತುಂಬು();
					ದಶಮಾಂಶ delta_intvl = dt - ಸರಾಸರಿ_val;
					ಸರಾಸರಿ_val += delta_intvl / ec;
					pci->ಸರಾಸರಿ.store(ಸರಾಸರಿ_val);
					ದಶಮಾಂಶ m2_val = pci->M2.ತುಂಬು();
					m2_val += delta_intvl * (dt - ಸರಾಸರಿ_val);
					pci->M2.store(m2_val);
					ನಿಲ್ಲಿಸು;
				}
			}

			pci->ಸಮಯ_ಕೊನೆಯ.store(now);
			pci->event_ಎಣಿಕೆ.fetch_ಕೂಡಿಸು(1);
			ನಿಲ್ಲಿಸು;
		}

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಶೂನ್ಯ
perf_ಗಣ_ಎಣಿಕೆ(perf_counter_t handle, uint64_t ಎಣಿಕೆ_2)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ಎಣಿಕೆ: {
			((ರಚನೆ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.store(ಎಣಿಕೆ_2);
		}
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

}

ಶೂನ್ಯ
perf_cancel(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ELAPSED: {
			ರಚನೆ perf_ctr_elapsed *pce = (ರಚನೆ perf_ctr_elapsed *)handle;
			pce->ಸಮಯ_ಆರಂಭ.store((uint64_t)0);
		}
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಶೂನ್ಯ
perf_reset(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ಎಣಿಕೆ:
		((ರಚನೆ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.store((uint64_t)0);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PC_ELAPSED: {
			ರಚನೆ perf_ctr_elapsed *pce = (ರಚನೆ perf_ctr_elapsed *)handle;
			pce->event_ಎಣಿಕೆ.store((uint64_t)0);
			pce->ಸಮಯ_ಆರಂಭ.store((uint64_t)0);
			pce->ಸಮಯ_ಒಟ್ಟು.store((uint64_t)0);
			pce->ಸಮಯ_ಕನಿಷ್ಠ.store((uint32_t)0);
			pce->ಸಮಯ_most.store((uint32_t)0);
			pce->ಸರಾಸರಿ.store(0.0f);
			pce->M2.store(0.0f);
			ನಿಲ್ಲಿಸು;
		}

	ಸ್ಥಿತಿ PC_INTERVAL: {
			ರಚನೆ perf_ctr_interval *pci = (ರಚನೆ perf_ctr_interval *)handle;
			pci->event_ಎಣಿಕೆ.store((uint64_t)0);
			pci->ಸಮಯ_event.store((uint64_t)0);
			pci->ಸಮಯ_ಮೊದಲ.store((uint64_t)0);
			pci->ಸಮಯ_ಕೊನೆಯ.store((uint64_t)0);
			pci->ಸಮಯ_ಕನಿಷ್ಠ.store((uint32_t)0);
			pci->ಸಮಯ_most.store((uint32_t)0);
			pci->ಸರಾಸರಿ.store(0.0f);
			pci->M2.store(0.0f);
			ನಿಲ್ಲಿಸು;
		}
	}
}

ಶೂನ್ಯ
perf_print_counter(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ಎಣಿಕೆ: {
			uint64_t ec = ((ರಚನೆ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.ತುಂಬು();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			ನಿಲ್ಲಿಸು;
		}

	ಸ್ಥಿತಿ PC_ELAPSED: {
			ರಚನೆ perf_ctr_elapsed *pce = (ರಚನೆ perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ಎಣಿಕೆ.ತುಂಬು();
			uint64_t tt = pce->ಸಮಯ_ಒಟ್ಟು.ತುಂಬು();
			uint32_t tl = pce->ಸಮಯ_ಕನಿಷ್ಠ.ತುಂಬು();
			uint32_t tm = pce->ಸಮಯ_most.ತುಂಬು();
			ದಶಮಾಂಶ m2 = pce->M2.ತುಂಬು();
			ದಶಮಾಂಶ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (ದ್ವಿಗುಣ)tt / (ದ್ವಿಗುಣ)ec,
				     tl, tm, (ದ್ವಿಗುಣ)(1e6f * rms));
			ನಿಲ್ಲಿಸು;
		}

	ಸ್ಥಿತಿ PC_INTERVAL: {
			ರಚನೆ perf_ctr_interval *pci = (ರಚನೆ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ಎಣಿಕೆ.ತುಂಬು();
			uint64_t tf = pci->ಸಮಯ_ಮೊದಲ.ತುಂಬು();
			uint64_t tla = pci->ಸಮಯ_ಕೊನೆಯ.ತುಂಬು();
			uint32_t tl = pci->ಸಮಯ_ಕನಿಷ್ಠ.ತುಂಬು();
			uint32_t tm = pci->ಸಮಯ_most.ತುಂಬು();
			ದಶಮಾಂಶ m2 = pci->M2.ತುಂಬು();
			ದಶಮಾಂಶ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (ದ್ವಿಗುಣ)(tla - tf) / (ದ್ವಿಗುಣ)ec,
				     tl, tm, (ದ್ವಿಗುಣ)(1e6f * rms));
			ನಿಲ್ಲಿಸು;
		}

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}


ಪೂರ್ಣಾಂಕ
perf_print_counter_ಬಫರ್(ಅಕ್ಷರ *ಬಫರ್_2, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ, perf_counter_t handle)
{
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_written = 0;

	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ಎಣಿಕೆ:
		ಅಂಕೆ_written = snprintf(ಬಫರ್_2, ಉದ್ದಳತೆ, "%s: %" PRIu64 " events",
				       handle->name,
				       ((ರಚನೆ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.ತುಂಬು());
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PC_ELAPSED: {
			ರಚನೆ perf_ctr_elapsed *pce = (ರಚನೆ perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ಎಣಿಕೆ.ತುಂಬು();
			uint64_t tt = pce->ಸಮಯ_ಒಟ್ಟು.ತುಂಬು();
			uint32_t tl = pce->ಸಮಯ_ಕನಿಷ್ಠ.ತುಂಬು();
			uint32_t tm = pce->ಸಮಯ_most.ತುಂಬು();
			ದಶಮಾಂಶ m2 = pce->M2.ತುಂಬು();
			ದಶಮಾಂಶ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			ಅಂಕೆ_written = snprintf(ಬಫರ್_2, ಉದ್ದಳತೆ,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (ದ್ವಿಗುಣ)tt / (ದ್ವಿಗುಣ)ec,
					       tl,
					       tm,
					       (ದ್ವಿಗುಣ)(1e6f * rms));
			ನಿಲ್ಲಿಸು;
		}

	ಸ್ಥಿತಿ PC_INTERVAL: {
			ರಚನೆ perf_ctr_interval *pci = (ರಚನೆ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ಎಣಿಕೆ.ತುಂಬು();
			uint32_t tl = pci->ಸಮಯ_ಕನಿಷ್ಠ.ತುಂಬು();
			uint32_t tm = pci->ಸಮಯ_most.ತುಂಬು();
			ದಶಮಾಂಶ m2 = pci->M2.ತುಂಬು();
			ದಶಮಾಂಶ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			ಅಂಕೆ_written = snprintf(ಬಫರ್_2, ಉದ್ದಳತೆ,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (ದ್ವಿಗುಣ)(pci->ಸಮಯ_ಕೊನೆಯ.ತುಂಬು() - pci->ಸಮಯ_ಮೊದಲ.ತುಂಬು()) / (ದ್ವಿಗುಣ)ec,
					       tl,
					       tm,
					       (ದ್ವಿಗುಣ)(1e6f * rms));
			ನಿಲ್ಲಿಸು;
		}

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಬಫರ್_2[ಉದ್ದಳತೆ - 1] = 0; // ensure 0-termination
	ಹಿಂತಿರುಗಿ ಅಂಕೆ_written;
}

uint64_t
perf_event_ಎಣಿಕೆ(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ಎಣಿಕೆ:
		ಹಿಂತಿರುಗಿ ((ರಚನೆ perf_ctr_ಎಣಿಕೆ *)handle)->event_ಎಣಿಕೆ.ತುಂಬು();

	ಸ್ಥಿತಿ PC_ELAPSED: {
			ರಚನೆ perf_ctr_elapsed *pce = (ರಚನೆ perf_ctr_elapsed *)handle;
			ಹಿಂತಿರುಗಿ pce->event_ಎಣಿಕೆ.ತುಂಬು();
		}

	ಸ್ಥಿತಿ PC_INTERVAL: {
			ರಚನೆ perf_ctr_interval *pci = (ರಚನೆ perf_ctr_interval *)handle;
			ಹಿಂತಿರುಗಿ pci->event_ಎಣಿಕೆ.ತುಂಬು();
		}

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ದಶಮಾಂಶ
perf_ಸರಾಸರಿ(perf_counter_t handle)
{
	ಆದರೆ (handle == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ಆಯ್ಕೆ (handle->type) {
	ಸ್ಥಿತಿ PC_ELAPSED: {
			ರಚನೆ perf_ctr_elapsed *pce = (ರಚನೆ perf_ctr_elapsed *)handle;
			ಹಿಂತಿರುಗಿ pce->ಸರಾಸರಿ.ತುಂಬು();
		}

	ಸ್ಥಿತಿ PC_INTERVAL: {
			ರಚನೆ perf_ctr_interval *pci = (ರಚನೆ perf_ctr_interval *)handle;
			ಹಿಂತಿರುಗಿ pci->ಸರಾಸರಿ.ತುಂಬು();
		}

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ 0.0f;
}

ಶೂನ್ಯ
perf_iterate_all(perf_callback cb, ಶೂನ್ಯ *user)
{
	ಸ್ಥಿರ constexpr size_t PERF_COUNTER_ಸಾಲು_ಬಫರ್_ಗಾತ್ರ = 220;
	ಅಕ್ಷರ ಬಫರ್_2[PERF_COUNTER_ಸಾಲು_ಬಫರ್_ಗಾತ್ರ];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_ಸಕ್ರಿಯ;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಆಗ (handle != ಶೂನ್ಯಸೂಚಕ) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t ಮುಂದಿನ = (perf_counter_t)sq_ಮುಂದಿನ(&handle->link);

		ಆದರೆ (!handle->pending_free) {
			perf_print_counter_ಬಫರ್(ಬಫರ್_2, ಗಾತ್ರ(ಬಫರ್_2), handle);

			// the callback may block (the logger waits for MAVLink acks here), so run
			// it without the global mutex: perf_alloc()/perf_free() on other threads
			// keep working, and 'next' stays valid because perf_free() defers deletion
			// while perf_iterate_active > 0
			pthread_mutex_unlock(&perf_counters_mutex);
			cb(ಬಫರ್_2, user);
			pthread_mutex_lock(&perf_counters_mutex);
		}

		handle = ಮುಂದಿನ;
	}

	ಆದರೆ (--perf_iterate_ಸಕ್ರಿಯ == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ಶೂನ್ಯ
perf_print_all(ಶೂನ್ಯ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಆಗ (handle != ಶೂನ್ಯಸೂಚಕ) {
		ಆದರೆ (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_ಮುಂದಿನ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ಶೂನ್ಯ
perf_print_latency(ಶೂನ್ಯ)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < get_latency_ಪಾತ್ರೆ_ಎಣಿಕೆ(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.ಪಾತ್ರೆ, (ದೀರ್ಘ ಪೂರ್ಣಾಂಕ)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_ಪಾತ್ರೆ_ಎಣಿಕೆ() - 1, get_latency_ಪಾತ್ರೆ_ಎಣಿಕೆ());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.ಪಾತ್ರೆ, latency.counter);
}

ಶೂನ್ಯ
perf_reset_all(ಶೂನ್ಯ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ಆಗ (handle != ಶೂನ್ಯಸೂಚಕ) {
		ಆದರೆ (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_ಮುಂದಿನ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
