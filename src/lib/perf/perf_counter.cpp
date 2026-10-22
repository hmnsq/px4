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

#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <pthread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <systemlib/err.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "perf_counter.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/atomic.h>

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
#ਜੇ defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ T> ਵਰਤੋਂ PerfVar = px4::atomic<T>;
#ਨਹੀਂਤਾਂ
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ T>
ਵਰਗ PerfVar
{
ਜਨਤਕ:
	PerfVar() = ਮੂਲ;
	T ਲੱਦੋ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ_2; }
	ਖਾਲੀ store(T value) { ਮੁੱਲ_2 = value; }
	T fetch_ਜੋੜੋ(T value) { ਅਚਲ T ਪਿਛਲਾ = ਮੁੱਲ_2; ਮੁੱਲ_2 += value; ਵਾਪਸ ਪਿਛਲਾ; }
ਨਿਜੀ:
	T ਮੁੱਲ_2 {};
};
#ਜੇ_ਅੰਤ

/**
 * Header common to all counters.
 */
ਬਣਤਰ perf_ctr_ਸਿਰਲੇਖ {
	sq_ਇੰਦਰਾਜ_t		link;	/**< list linkage */
	ਗਿਣਤੀ perf_counter_type	type;	/**< counter type */
	ਤਾਰਕਿਕ			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ਅਚਲ ਅੱਖਰ		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
ਬਣਤਰ perf_ctr_ਗਿਣਤੀ : ਜਨਤਕ perf_ctr_ਸਿਰਲੇਖ {
	PerfVar<uint64_t>	event_ਗਿਣਤੀ;
};

/**
 * PC_ELAPSED counter.
 */
ਬਣਤਰ perf_ctr_elapsed : ਜਨਤਕ perf_ctr_ਸਿਰਲੇਖ {
	PerfVar<uint64_t>	event_ਗਿਣਤੀ;
	PerfVar<uint64_t>	ਸਮਾਂ_ਸ਼ੁਰੂ;
	PerfVar<uint64_t>	ਸਮਾਂ_ਕੁੱਲ;
	PerfVar<uint32_t>	ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ;
	PerfVar<uint32_t>	ਸਮਾਂ_most;
	PerfVar<ਦਸ਼ਮਲਵ>	ਔਸਤ;
	PerfVar<ਦਸ਼ਮਲਵ>	M2;
};

/**
 * PC_INTERVAL counter.
 */
ਬਣਤਰ perf_ctr_interval : ਜਨਤਕ perf_ctr_ਸਿਰਲੇਖ {
	PerfVar<uint64_t>	event_ਗਿਣਤੀ;
	PerfVar<uint64_t>	ਸਮਾਂ_event;
	PerfVar<uint64_t>	ਸਮਾਂ_ਪਹਿਲਾ;
	PerfVar<uint64_t>	ਸਮਾਂ_ਆਖਰੀ;
	PerfVar<uint32_t>	ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ;
	PerfVar<uint32_t>	ਸਮਾਂ_most;
	PerfVar<ਦਸ਼ਮਲਵ>	ਔਸਤ;
	PerfVar<ਦਸ਼ਮਲਵ>	M2;
};

/**
 * List of all known counters.
 */
ਸਥਿਰ sq_ਕਤਾਰ_t	perf_counters = { ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ };

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
ਸਥਿਰ ਪੂਰਨ perf_iterate_ਸਰਗਰਮ = 0;

ਸਥਿਰ ਖਾਲੀ
perf_delete(perf_counter_t handle)
{
	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ਗਿਣਤੀ:
		ਮਿਟਾਓ (ਬਣਤਰ perf_ctr_ਗਿਣਤੀ *)handle;
		ਰੋਕੋ;

	ਹਾਲਤ PC_ELAPSED:
		ਮਿਟਾਓ (ਬਣਤਰ perf_ctr_elapsed *)handle;
		ਰੋਕੋ;

	ਹਾਲਤ PC_INTERVAL:
		ਮਿਟਾਓ (ਬਣਤਰ perf_ctr_interval *)handle;
		ਰੋਕੋ;

	ਮੂਲ:
		ਰੋਕੋ;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
ਸਥਿਰ ਖਾਲੀ
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ਜਦੋਂ (handle != ਖਾਲੀ_ਸੂਚਕ) {
		perf_counter_t ਅਗਲਾ = (perf_counter_t)sq_ਅਗਲਾ(&handle->link);

		ਜੇ (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = ਅਗਲਾ;
	}
}

perf_counter_t
perf_alloc(ਗਿਣਤੀ perf_counter_type type, ਅਚਲ ਅੱਖਰ *name)
{
	perf_counter_t ctr = ਖਾਲੀ_ਸੂਚਕ;

	ਚੋਣ (type) {
	ਹਾਲਤ PC_ਗਿਣਤੀ:
		ctr = ਨਵਾਂ perf_ctr_ਗਿਣਤੀ();
		ਰੋਕੋ;

	ਹਾਲਤ PC_ELAPSED:
		ctr = ਨਵਾਂ perf_ctr_elapsed();
		ਰੋਕੋ;

	ਹਾਲਤ PC_INTERVAL:
		ctr = ਨਵਾਂ perf_ctr_interval();
		ਰੋਕੋ;

	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਜੇ (ctr != ਖਾਲੀ_ਸੂਚਕ) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = ਝੂਠ;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	ਵਾਪਸ ctr;
}

perf_counter_t
perf_alloc_once(ਗਿਣਤੀ perf_counter_type type, ਅਚਲ ਅੱਖਰ *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ਜਦੋਂ (handle != ਖਾਲੀ_ਸੂਚਕ) {
		ਜੇ (!handle->pending_free && !strcmp(handle->name, name)) {
			ਜੇ (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				ਵਾਪਸ handle;

			} ਨਹੀਂਤਾਂ {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
			}
		}

		handle = (perf_counter_t)sq_ਅਗਲਾ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	ਵਾਪਸ perf_alloc(type, name);
}

ਖਾਲੀ
perf_free(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	ਜੇ (perf_iterate_ਸਰਗਰਮ > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = ਸੱਚ;
		pthread_mutex_unlock(&perf_counters_mutex);
		ਵਾਪਸ;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

ਖਾਲੀ
perf_ਗਿਣਤੀ(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ਗਿਣਤੀ:
		((ਬਣਤਰ perf_ctr_ਗਿਣਤੀ *)handle)->event_ਗਿਣਤੀ.fetch_ਜੋੜੋ(1);
		ਰੋਕੋ;

	ਹਾਲਤ PC_INTERVAL:
		perf_ਗਿਣਤੀ_interval(handle, hrt_absolute_ਸਮਾਂ());
		ਰੋਕੋ;

	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਖਾਲੀ
perf_begin(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ELAPSED:
		((ਬਣਤਰ perf_ctr_elapsed *)handle)->ਸਮਾਂ_ਸ਼ੁਰੂ.store(hrt_absolute_ਸਮਾਂ());
		ਰੋਕੋ;

	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਖਾਲੀ
perf_ਸਿਰਾ(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ELAPSED: {
			ਬਣਤਰ perf_ctr_elapsed *pce = (ਬਣਤਰ perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->ਸਮਾਂ_ਸ਼ੁਰੂ.ਲੱਦੋ();

			ਜੇ (ts != 0) {
				perf_ਸੈੱਟ_elapsed(handle, hrt_absolute_ਸਮਾਂ() - ts);
			}
		}
		ਰੋਕੋ;

	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਖਾਲੀ
perf_ਸੈੱਟ_elapsed(perf_counter_t handle, int64_t elapsed)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ELAPSED: {
			ਬਣਤਰ perf_ctr_elapsed *pce = (ਬਣਤਰ perf_ctr_elapsed *)handle;

			ਜੇ (elapsed >= 0) {
				uint64_t ec = pce->event_ਗਿਣਤੀ.fetch_ਜੋੜੋ(1) + 1;
				pce->ਸਮਾਂ_ਕੁੱਲ.fetch_ਜੋੜੋ((uint64_t)elapsed);

				uint32_t tl = pce->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.ਲੱਦੋ();

				ਜੇ ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->ਸਮਾਂ_most.ਲੱਦੋ();

				ਜੇ (tm < (uint32_t)elapsed) {
					pce->ਸਮਾਂ_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				ਦਸ਼ਮਲਵ dt = elapsed / 1e6f;
				ਦਸ਼ਮਲਵ ਔਸਤ_val = pce->ਔਸਤ.ਲੱਦੋ();
				ਦਸ਼ਮਲਵ delta_intvl = dt - ਔਸਤ_val;
				ਔਸਤ_val += delta_intvl / ec;
				pce->ਔਸਤ.store(ਔਸਤ_val);
				ਦਸ਼ਮਲਵ m2_val = pce->M2.ਲੱਦੋ();
				m2_val += delta_intvl * (dt - ਔਸਤ_val);
				pce->M2.store(m2_val);

				pce->ਸਮਾਂ_ਸ਼ੁਰੂ.store((uint64_t)0);
			}
		}
		ਰੋਕੋ;

	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਖਾਲੀ
perf_ਗਿਣਤੀ_interval(perf_counter_t handle, hrt_abstime now)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_INTERVAL: {
			ਬਣਤਰ perf_ctr_interval *pci = (ਬਣਤਰ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ਗਿਣਤੀ.ਲੱਦੋ();

			ਚੋਣ (ec) {
			ਹਾਲਤ 0:
				pci->ਸਮਾਂ_ਪਹਿਲਾ.store(now);
				ਰੋਕੋ;

			ਹਾਲਤ 1: {
					uint64_t tl = pci->ਸਮਾਂ_ਆਖਰੀ.ਲੱਦੋ();
					uint32_t interval = (uint32_t)(now - tl);
					pci->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.store(interval);
					pci->ਸਮਾਂ_most.store(interval);
					pci->ਔਸਤ.store(interval / 1e6f);
					pci->M2.store(0.0f);
					ਰੋਕੋ;
				}

			ਮੂਲ: {
					uint64_t tl = pci->ਸਮਾਂ_ਆਖਰੀ.ਲੱਦੋ();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.ਲੱਦੋ();

					ਜੇ ((uint32_t)interval < least) {
						pci->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.store((uint32_t)interval);
					}

					uint32_t most = pci->ਸਮਾਂ_most.ਲੱਦੋ();

					ਜੇ ((uint32_t)interval > most) {
						pci->ਸਮਾਂ_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					ਦਸ਼ਮਲਵ dt = interval / 1e6f;
					ਦਸ਼ਮਲਵ ਔਸਤ_val = pci->ਔਸਤ.ਲੱਦੋ();
					ਦਸ਼ਮਲਵ delta_intvl = dt - ਔਸਤ_val;
					ਔਸਤ_val += delta_intvl / ec;
					pci->ਔਸਤ.store(ਔਸਤ_val);
					ਦਸ਼ਮਲਵ m2_val = pci->M2.ਲੱਦੋ();
					m2_val += delta_intvl * (dt - ਔਸਤ_val);
					pci->M2.store(m2_val);
					ਰੋਕੋ;
				}
			}

			pci->ਸਮਾਂ_ਆਖਰੀ.store(now);
			pci->event_ਗਿਣਤੀ.fetch_ਜੋੜੋ(1);
			ਰੋਕੋ;
		}

	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਖਾਲੀ
perf_ਸੈੱਟ_ਗਿਣਤੀ(perf_counter_t handle, uint64_t ਗਿਣਤੀ_2)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ਗਿਣਤੀ: {
			((ਬਣਤਰ perf_ctr_ਗਿਣਤੀ *)handle)->event_ਗਿਣਤੀ.store(ਗਿਣਤੀ_2);
		}
		ਰੋਕੋ;

	ਮੂਲ:
		ਰੋਕੋ;
	}

}

ਖਾਲੀ
perf_cancel(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ELAPSED: {
			ਬਣਤਰ perf_ctr_elapsed *pce = (ਬਣਤਰ perf_ctr_elapsed *)handle;
			pce->ਸਮਾਂ_ਸ਼ੁਰੂ.store((uint64_t)0);
		}
		ਰੋਕੋ;

	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਖਾਲੀ
perf_reset(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ਗਿਣਤੀ:
		((ਬਣਤਰ perf_ctr_ਗਿਣਤੀ *)handle)->event_ਗਿਣਤੀ.store((uint64_t)0);
		ਰੋਕੋ;

	ਹਾਲਤ PC_ELAPSED: {
			ਬਣਤਰ perf_ctr_elapsed *pce = (ਬਣਤਰ perf_ctr_elapsed *)handle;
			pce->event_ਗਿਣਤੀ.store((uint64_t)0);
			pce->ਸਮਾਂ_ਸ਼ੁਰੂ.store((uint64_t)0);
			pce->ਸਮਾਂ_ਕੁੱਲ.store((uint64_t)0);
			pce->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.store((uint32_t)0);
			pce->ਸਮਾਂ_most.store((uint32_t)0);
			pce->ਔਸਤ.store(0.0f);
			pce->M2.store(0.0f);
			ਰੋਕੋ;
		}

	ਹਾਲਤ PC_INTERVAL: {
			ਬਣਤਰ perf_ctr_interval *pci = (ਬਣਤਰ perf_ctr_interval *)handle;
			pci->event_ਗਿਣਤੀ.store((uint64_t)0);
			pci->ਸਮਾਂ_event.store((uint64_t)0);
			pci->ਸਮਾਂ_ਪਹਿਲਾ.store((uint64_t)0);
			pci->ਸਮਾਂ_ਆਖਰੀ.store((uint64_t)0);
			pci->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.store((uint32_t)0);
			pci->ਸਮਾਂ_most.store((uint32_t)0);
			pci->ਔਸਤ.store(0.0f);
			pci->M2.store(0.0f);
			ਰੋਕੋ;
		}
	}
}

ਖਾਲੀ
perf_print_counter(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ਗਿਣਤੀ: {
			uint64_t ec = ((ਬਣਤਰ perf_ctr_ਗਿਣਤੀ *)handle)->event_ਗਿਣਤੀ.ਲੱਦੋ();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			ਰੋਕੋ;
		}

	ਹਾਲਤ PC_ELAPSED: {
			ਬਣਤਰ perf_ctr_elapsed *pce = (ਬਣਤਰ perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ਗਿਣਤੀ.ਲੱਦੋ();
			uint64_t tt = pce->ਸਮਾਂ_ਕੁੱਲ.ਲੱਦੋ();
			uint32_t tl = pce->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.ਲੱਦੋ();
			uint32_t tm = pce->ਸਮਾਂ_most.ਲੱਦੋ();
			ਦਸ਼ਮਲਵ m2 = pce->M2.ਲੱਦੋ();
			ਦਸ਼ਮਲਵ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (ਦੁੱਗਣਾ)tt / (ਦੁੱਗਣਾ)ec,
				     tl, tm, (ਦੁੱਗਣਾ)(1e6f * rms));
			ਰੋਕੋ;
		}

	ਹਾਲਤ PC_INTERVAL: {
			ਬਣਤਰ perf_ctr_interval *pci = (ਬਣਤਰ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ਗਿਣਤੀ.ਲੱਦੋ();
			uint64_t tf = pci->ਸਮਾਂ_ਪਹਿਲਾ.ਲੱਦੋ();
			uint64_t tla = pci->ਸਮਾਂ_ਆਖਰੀ.ਲੱਦੋ();
			uint32_t tl = pci->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.ਲੱਦੋ();
			uint32_t tm = pci->ਸਮਾਂ_most.ਲੱਦੋ();
			ਦਸ਼ਮਲਵ m2 = pci->M2.ਲੱਦੋ();
			ਦਸ਼ਮਲਵ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (ਦੁੱਗਣਾ)(tla - tf) / (ਦੁੱਗਣਾ)ec,
				     tl, tm, (ਦੁੱਗਣਾ)(1e6f * rms));
			ਰੋਕੋ;
		}

	ਮੂਲ:
		ਰੋਕੋ;
	}
}


ਪੂਰਨ
perf_print_counter_ਬਫਰ(ਅੱਖਰ *ਬਫਰ_2, ਪੂਰਨ ਲੰਮਾਈ, perf_counter_t handle)
{
	ਪੂਰਨ ਅੰਕ_written = 0;

	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ 0;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ਗਿਣਤੀ:
		ਅੰਕ_written = snprintf(ਬਫਰ_2, ਲੰਮਾਈ, "%s: %" PRIu64 " events",
				       handle->name,
				       ((ਬਣਤਰ perf_ctr_ਗਿਣਤੀ *)handle)->event_ਗਿਣਤੀ.ਲੱਦੋ());
		ਰੋਕੋ;

	ਹਾਲਤ PC_ELAPSED: {
			ਬਣਤਰ perf_ctr_elapsed *pce = (ਬਣਤਰ perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ਗਿਣਤੀ.ਲੱਦੋ();
			uint64_t tt = pce->ਸਮਾਂ_ਕੁੱਲ.ਲੱਦੋ();
			uint32_t tl = pce->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.ਲੱਦੋ();
			uint32_t tm = pce->ਸਮਾਂ_most.ਲੱਦੋ();
			ਦਸ਼ਮਲਵ m2 = pce->M2.ਲੱਦੋ();
			ਦਸ਼ਮਲਵ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			ਅੰਕ_written = snprintf(ਬਫਰ_2, ਲੰਮਾਈ,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (ਦੁੱਗਣਾ)tt / (ਦੁੱਗਣਾ)ec,
					       tl,
					       tm,
					       (ਦੁੱਗਣਾ)(1e6f * rms));
			ਰੋਕੋ;
		}

	ਹਾਲਤ PC_INTERVAL: {
			ਬਣਤਰ perf_ctr_interval *pci = (ਬਣਤਰ perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ਗਿਣਤੀ.ਲੱਦੋ();
			uint32_t tl = pci->ਸਮਾਂ_ਘੱਟ_ਤੋਂ_ਘੱਟ.ਲੱਦੋ();
			uint32_t tm = pci->ਸਮਾਂ_most.ਲੱਦੋ();
			ਦਸ਼ਮਲਵ m2 = pci->M2.ਲੱਦੋ();
			ਦਸ਼ਮਲਵ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			ਅੰਕ_written = snprintf(ਬਫਰ_2, ਲੰਮਾਈ,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (ਦੁੱਗਣਾ)(pci->ਸਮਾਂ_ਆਖਰੀ.ਲੱਦੋ() - pci->ਸਮਾਂ_ਪਹਿਲਾ.ਲੱਦੋ()) / (ਦੁੱਗਣਾ)ec,
					       tl,
					       tm,
					       (ਦੁੱਗਣਾ)(1e6f * rms));
			ਰੋਕੋ;
		}

	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਬਫਰ_2[ਲੰਮਾਈ - 1] = 0; // ensure 0-termination
	ਵਾਪਸ ਅੰਕ_written;
}

uint64_t
perf_event_ਗਿਣਤੀ(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ 0;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ਗਿਣਤੀ:
		ਵਾਪਸ ((ਬਣਤਰ perf_ctr_ਗਿਣਤੀ *)handle)->event_ਗਿਣਤੀ.ਲੱਦੋ();

	ਹਾਲਤ PC_ELAPSED: {
			ਬਣਤਰ perf_ctr_elapsed *pce = (ਬਣਤਰ perf_ctr_elapsed *)handle;
			ਵਾਪਸ pce->event_ਗਿਣਤੀ.ਲੱਦੋ();
		}

	ਹਾਲਤ PC_INTERVAL: {
			ਬਣਤਰ perf_ctr_interval *pci = (ਬਣਤਰ perf_ctr_interval *)handle;
			ਵਾਪਸ pci->event_ਗਿਣਤੀ.ਲੱਦੋ();
		}

	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਵਾਪਸ 0;
}

ਦਸ਼ਮਲਵ
perf_ਔਸਤ(perf_counter_t handle)
{
	ਜੇ (handle == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ 0;
	}

	ਚੋਣ (handle->type) {
	ਹਾਲਤ PC_ELAPSED: {
			ਬਣਤਰ perf_ctr_elapsed *pce = (ਬਣਤਰ perf_ctr_elapsed *)handle;
			ਵਾਪਸ pce->ਔਸਤ.ਲੱਦੋ();
		}

	ਹਾਲਤ PC_INTERVAL: {
			ਬਣਤਰ perf_ctr_interval *pci = (ਬਣਤਰ perf_ctr_interval *)handle;
			ਵਾਪਸ pci->ਔਸਤ.ਲੱਦੋ();
		}

	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਵਾਪਸ 0.0f;
}

ਖਾਲੀ
perf_iterate_all(perf_callback cb, ਖਾਲੀ *user)
{
	ਸਥਿਰ constexpr size_t PERF_COUNTER_ਸਤਰ_ਬਫਰ_ਮਾਪ = 220;
	ਅੱਖਰ ਬਫਰ_2[PERF_COUNTER_ਸਤਰ_ਬਫਰ_ਮਾਪ];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_ਸਰਗਰਮ;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ਜਦੋਂ (handle != ਖਾਲੀ_ਸੂਚਕ) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t ਅਗਲਾ = (perf_counter_t)sq_ਅਗਲਾ(&handle->link);

		ਜੇ (!handle->pending_free) {
			perf_print_counter_ਬਫਰ(ਬਫਰ_2, ਆਕਾਰ(ਬਫਰ_2), handle);

			// the callback may block (the logger waits for MAVLink acks here), so run
			// it without the global mutex: perf_alloc()/perf_free() on other threads
			// keep working, and 'next' stays valid because perf_free() defers deletion
			// while perf_iterate_active > 0
			pthread_mutex_unlock(&perf_counters_mutex);
			cb(ਬਫਰ_2, user);
			pthread_mutex_lock(&perf_counters_mutex);
		}

		handle = ਅਗਲਾ;
	}

	ਜੇ (--perf_iterate_ਸਰਗਰਮ == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ਖਾਲੀ
perf_print_all(ਖਾਲੀ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ਜਦੋਂ (handle != ਖਾਲੀ_ਸੂਚਕ) {
		ਜੇ (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_ਅਗਲਾ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ਖਾਲੀ
perf_print_latency(ਖਾਲੀ)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	ਲਈ (ਪੂਰਨ i = 0; i < get_latency_ਬਾਲਟੀ_ਗਿਣਤੀ(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.ਬਾਲਟੀ, (ਲੰਮਾ ਪੂਰਨ)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_ਬਾਲਟੀ_ਗਿਣਤੀ() - 1, get_latency_ਬਾਲਟੀ_ਗਿਣਤੀ());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.ਬਾਲਟੀ, latency.counter);
}

ਖਾਲੀ
perf_reset_all(ਖਾਲੀ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ਜਦੋਂ (handle != ਖਾਲੀ_ਸੂਚਕ) {
		ਜੇ (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_ਅਗਲਾ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
