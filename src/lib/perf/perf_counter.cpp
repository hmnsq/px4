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

#अन्तर्भाव <inttypes.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <pthread.h>
#अन्तर्भाव <systemlib/err.h>

#अन्तर्भाव "perf_counter.h"

#अन्तर्भाव <px4_platform_common/atomic.h>

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
#यदि defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
template <typename T> using PerfVar = px4::atomic<T>;
#अन्यथा
template <typename T>
वर्ग PerfVar
{
सार्वजनिक:
	PerfVar() = स्वभाव;
	T load() ध्रुव { प्रत्यागम मूल्य_2; }
	शून्य store(T value) { मूल्य_2 = value; }
	T fetch_add(T value) { ध्रुव T previous = मूल्य_2; मूल्य_2 += value; प्रत्यागम previous; }
गुप्त:
	T मूल्य_2 {};
};
#endif

/**
 * Header common to all counters.
 */
संरचना perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	गणना perf_counter_type	type;	/**< counter type */
	तार्किक			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ध्रुव वर्ण		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
संरचना perf_ctr_मोजणी : सार्वजनिक perf_ctr_header {
	PerfVar<uint64_t>	event_मोजणी;
};

/**
 * PC_ELAPSED counter.
 */
संरचना perf_ctr_elapsed : सार्वजनिक perf_ctr_header {
	PerfVar<uint64_t>	event_मोजणी;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_total;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<भिन्न>	mean;
	PerfVar<भिन्न>	M2;
};

/**
 * PC_INTERVAL counter.
 */
संरचना perf_ctr_interval : सार्वजनिक perf_ctr_header {
	PerfVar<uint64_t>	event_मोजणी;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<भिन्न>	mean;
	PerfVar<भिन्न>	M2;
};

/**
 * List of all known counters.
 */
स्थिर sq_queue_t	perf_counters = { nullptr, nullptr };

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
स्थिर पूर्णाङ्क perf_iterate_active = 0;

स्थिर शून्य
perf_delete(perf_counter_t handle)
{
	विकल्प (handle->type) {
	पक्ष PC_मोजणी:
		delete (संरचना perf_ctr_मोजणी *)handle;
		भङ्ग;

	पक्ष PC_ELAPSED:
		delete (संरचना perf_ctr_elapsed *)handle;
		भङ्ग;

	पक्ष PC_INTERVAL:
		delete (संरचना perf_ctr_interval *)handle;
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
स्थिर शून्य
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	यावत् (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		यदि (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(गणना perf_counter_type type, ध्रुव वर्ण *name)
{
	perf_counter_t ctr = nullptr;

	विकल्प (type) {
	पक्ष PC_मोजणी:
		ctr = नूतन perf_ctr_मोजणी();
		भङ्ग;

	पक्ष PC_ELAPSED:
		ctr = नूतन perf_ctr_elapsed();
		भङ्ग;

	पक्ष PC_INTERVAL:
		ctr = नूतन perf_ctr_interval();
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}

	यदि (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = असत्य;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	प्रत्यागम ctr;
}

perf_counter_t
perf_alloc_once(गणना perf_counter_type type, ध्रुव वर्ण *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	यावत् (handle != nullptr) {
		यदि (!handle->pending_free && !strcmp(handle->name, name)) {
			यदि (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				प्रत्यागम handle;

			} अन्यथा {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				प्रत्यागम nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	प्रत्यागम perf_alloc(type, name);
}

शून्य
perf_free(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	यदि (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = सत्य;
		pthread_mutex_unlock(&perf_counters_mutex);
		प्रत्यागम;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

शून्य
perf_मोजणी(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_मोजणी:
		((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.fetch_add(1);
		भङ्ग;

	पक्ष PC_INTERVAL:
		perf_मोजणी_interval(handle, hrt_absolute_time());
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_begin(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_ELAPSED:
		((संरचना perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_end(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			यदि (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;

			यदि (elapsed >= 0) {
				uint64_t ec = pce->event_मोजणी.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				यदि ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.load();

				यदि (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				भिन्न dt = elapsed / 1e6f;
				भिन्न mean_val = pce->mean.load();
				भिन्न delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				भिन्न m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_मोजणी_interval(perf_counter_t handle, hrt_abstime now)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			uint64_t ec = pci->event_मोजणी.load();

			विकल्प (ec) {
			पक्ष 0:
				pci->time_first.store(now);
				भङ्ग;

			पक्ष 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					भङ्ग;
				}

			स्वभाव: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					यदि ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					यदि ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					भिन्न dt = interval / 1e6f;
					भिन्न mean_val = pci->mean.load();
					भिन्न delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					भिन्न m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					भङ्ग;
				}
			}

			pci->time_last.store(now);
			pci->event_मोजणी.fetch_add(1);
			भङ्ग;
		}

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_set_मोजणी(perf_counter_t handle, uint64_t मोजणी)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_मोजणी: {
			((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.store(मोजणी);
		}
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}

}

शून्य
perf_cancel(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_reset(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_मोजणी:
		((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.store((uint64_t)0);
		भङ्ग;

	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			pce->event_मोजणी.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			भङ्ग;
		}

	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			pci->event_मोजणी.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			भङ्ग;
		}
	}
}

शून्य
perf_print_counter(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_मोजणी: {
			uint64_t ec = ((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			भङ्ग;
		}

	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_मोजणी.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			भिन्न m2 = pce->M2.load();
			भिन्न rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (द्विगुण)tt / (द्विगुण)ec,
				     tl, tm, (द्विगुण)(1e6f * rms));
			भङ्ग;
		}

	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			uint64_t ec = pci->event_मोजणी.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			भिन्न m2 = pci->M2.load();
			भिन्न rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (द्विगुण)(tla - tf) / (द्विगुण)ec,
				     tl, tm, (द्विगुण)(1e6f * rms));
			भङ्ग;
		}

	स्वभाव:
		भङ्ग;
	}
}


पूर्णाङ्क
perf_print_counter_buffer(वर्ण *buffer, पूर्णाङ्क length, perf_counter_t handle)
{
	पूर्णाङ्क अंक_written = 0;

	यदि (handle == nullptr) {
		प्रत्यागम 0;
	}

	विकल्प (handle->type) {
	पक्ष PC_मोजणी:
		अंक_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.load());
		भङ्ग;

	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_मोजणी.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			भिन्न m2 = pce->M2.load();
			भिन्न rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			अंक_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (द्विगुण)tt / (द्विगुण)ec,
					       tl,
					       tm,
					       (द्विगुण)(1e6f * rms));
			भङ्ग;
		}

	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			uint64_t ec = pci->event_मोजणी.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			भिन्न m2 = pci->M2.load();
			भिन्न rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			अंक_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (द्विगुण)(pci->time_last.load() - pci->time_first.load()) / (द्विगुण)ec,
					       tl,
					       tm,
					       (द्विगुण)(1e6f * rms));
			भङ्ग;
		}

	स्वभाव:
		भङ्ग;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	प्रत्यागम अंक_written;
}

uint64_t
perf_event_मोजणी(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम 0;
	}

	विकल्प (handle->type) {
	पक्ष PC_मोजणी:
		प्रत्यागम ((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.load();

	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			प्रत्यागम pce->event_मोजणी.load();
		}

	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			प्रत्यागम pci->event_मोजणी.load();
		}

	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम 0;
}

भिन्न
perf_mean(perf_counter_t handle)
{
	यदि (handle == nullptr) {
		प्रत्यागम 0;
	}

	विकल्प (handle->type) {
	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			प्रत्यागम pce->mean.load();
		}

	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			प्रत्यागम pci->mean.load();
		}

	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम 0.0f;
}

शून्य
perf_iterate_all(perf_callback cb, शून्य *user)
{
	स्थिर constexpr size_t PERF_COUNTER_ओळ_BUFFER_SIZE = 220;
	वर्ण buffer[PERF_COUNTER_ओळ_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	यावत् (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		यदि (!handle->pending_free) {
			perf_print_counter_buffer(buffer, परिमाण(buffer), handle);

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

	यदि (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

शून्य
perf_print_all(शून्य)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	यावत् (handle != nullptr) {
		यदि (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

शून्य
perf_print_latency(शून्य)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	प्रति (पूर्णाङ्क i = 0; i < get_latency_bucket_मोजणी(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long पूर्णाङ्क)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_मोजणी() - 1, get_latency_bucket_मोजणी());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.counter);
}

शून्य
perf_reset_all(शून्य)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	यावत् (handle != nullptr) {
		यदि (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
