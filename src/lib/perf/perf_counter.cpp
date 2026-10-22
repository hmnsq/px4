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
#जर defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
ढाँचा <typename T> using PerfVar = px4::atomic<T>;
#अन्यथा
ढाँचा <typename T>
वर्ग PerfVar
{
सार्वजनिक:
	PerfVar() = पूर्वनिर्धारित;
	T load() स्थिरांक { परत मूल्य_2; }
	शून्य store(T value) { मूल्य_2 = value; }
	T fetch_add(T value) { स्थिरांक T previous = मूल्य_2; मूल्य_2 += value; परत previous; }
खाजगी:
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
	स्थिरांक अक्षर		*name;	/**< counter name */
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
	PerfVar<दशांश>	mean;
	PerfVar<दशांश>	M2;
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
	PerfVar<दशांश>	mean;
	PerfVar<दशांश>	M2;
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
स्थिर पूर्णांक perf_iterate_active = 0;

स्थिर शून्य
perf_delete(perf_counter_t handle)
{
	निवड (handle->type) {
	स्थिती PC_मोजणी:
		delete (संरचना perf_ctr_मोजणी *)handle;
		थांबा;

	स्थिती PC_ELAPSED:
		delete (संरचना perf_ctr_elapsed *)handle;
		थांबा;

	स्थिती PC_INTERVAL:
		delete (संरचना perf_ctr_interval *)handle;
		थांबा;

	पूर्वनिर्धारित:
		थांबा;
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

	पर्यंत (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		जर (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(गणना perf_counter_type type, स्थिरांक अक्षर *name)
{
	perf_counter_t ctr = nullptr;

	निवड (type) {
	स्थिती PC_मोजणी:
		ctr = नवीन perf_ctr_मोजणी();
		थांबा;

	स्थिती PC_ELAPSED:
		ctr = नवीन perf_ctr_elapsed();
		थांबा;

	स्थिती PC_INTERVAL:
		ctr = नवीन perf_ctr_interval();
		थांबा;

	पूर्वनिर्धारित:
		थांबा;
	}

	जर (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = असत्य;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	परत ctr;
}

perf_counter_t
perf_alloc_once(गणना perf_counter_type type, स्थिरांक अक्षर *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	पर्यंत (handle != nullptr) {
		जर (!handle->pending_free && !म_भेद(handle->name, name)) {
			जर (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				परत handle;

			} अन्यथा {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				परत nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	परत perf_alloc(type, name);
}

शून्य
perf_free(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	जर (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = सत्य;
		pthread_mutex_unlock(&perf_counters_mutex);
		परत;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

शून्य
perf_मोजणी(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_मोजणी:
		((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.fetch_add(1);
		थांबा;

	स्थिती PC_INTERVAL:
		perf_मोजणी_interval(handle, hrt_absolute_time());
		थांबा;

	पूर्वनिर्धारित:
		थांबा;
	}
}

शून्य
perf_begin(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_ELAPSED:
		((संरचना perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		थांबा;

	पूर्वनिर्धारित:
		थांबा;
	}
}

शून्य
perf_end(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			जर (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		थांबा;

	पूर्वनिर्धारित:
		थांबा;
	}
}

शून्य
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;

			जर (elapsed >= 0) {
				uint64_t ec = pce->event_मोजणी.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				जर ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t पंचांग = pce->time_most.load();

				जर (पंचांग < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				दशांश dt = elapsed / 1e6f;
				दशांश mean_val = pce->mean.load();
				दशांश delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				दशांश m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		थांबा;

	पूर्वनिर्धारित:
		थांबा;
	}
}

शून्य
perf_मोजणी_interval(perf_counter_t handle, hrt_abstime now)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			uint64_t ec = pci->event_मोजणी.load();

			निवड (ec) {
			स्थिती 0:
				pci->time_first.store(now);
				थांबा;

			स्थिती 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					थांबा;
				}

			पूर्वनिर्धारित: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					जर ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					जर ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					दशांश dt = interval / 1e6f;
					दशांश mean_val = pci->mean.load();
					दशांश delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					दशांश m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					थांबा;
				}
			}

			pci->time_last.store(now);
			pci->event_मोजणी.fetch_add(1);
			थांबा;
		}

	पूर्वनिर्धारित:
		थांबा;
	}
}

शून्य
perf_set_मोजणी(perf_counter_t handle, uint64_t मोजणी)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_मोजणी: {
			((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.store(मोजणी);
		}
		थांबा;

	पूर्वनिर्धारित:
		थांबा;
	}

}

शून्य
perf_cancel(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		थांबा;

	पूर्वनिर्धारित:
		थांबा;
	}
}

शून्य
perf_reset(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_मोजणी:
		((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.store((uint64_t)0);
		थांबा;

	स्थिती PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			pce->event_मोजणी.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			थांबा;
		}

	स्थिती PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			pci->event_मोजणी.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			थांबा;
		}
	}
}

शून्य
perf_print_counter(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत;
	}

	निवड (handle->type) {
	स्थिती PC_मोजणी: {
			uint64_t ec = ((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			थांबा;
		}

	स्थिती PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_मोजणी.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t पंचांग = pce->time_most.load();
			दशांश m2 = pce->M2.load();
			दशांश rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (द्विगुण)tt / (द्विगुण)ec,
				     tl, पंचांग, (द्विगुण)(1e6f * rms));
			थांबा;
		}

	स्थिती PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			uint64_t ec = pci->event_मोजणी.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t पंचांग = pci->time_most.load();
			दशांश m2 = pci->M2.load();
			दशांश rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (द्विगुण)(tla - tf) / (द्विगुण)ec,
				     tl, पंचांग, (द्विगुण)(1e6f * rms));
			थांबा;
		}

	पूर्वनिर्धारित:
		थांबा;
	}
}


पूर्णांक
perf_print_counter_buffer(अक्षर *buffer, पूर्णांक length, perf_counter_t handle)
{
	पूर्णांक अंक_written = 0;

	जर (handle == nullptr) {
		परत 0;
	}

	निवड (handle->type) {
	स्थिती PC_मोजणी:
		अंक_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.load());
		थांबा;

	स्थिती PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_मोजणी.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t पंचांग = pce->time_most.load();
			दशांश m2 = pce->M2.load();
			दशांश rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			अंक_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (द्विगुण)tt / (द्विगुण)ec,
					       tl,
					       पंचांग,
					       (द्विगुण)(1e6f * rms));
			थांबा;
		}

	स्थिती PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			uint64_t ec = pci->event_मोजणी.load();
			uint32_t tl = pci->time_least.load();
			uint32_t पंचांग = pci->time_most.load();
			दशांश m2 = pci->M2.load();
			दशांश rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			अंक_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (द्विगुण)(pci->time_last.load() - pci->time_first.load()) / (द्विगुण)ec,
					       tl,
					       पंचांग,
					       (द्विगुण)(1e6f * rms));
			थांबा;
		}

	पूर्वनिर्धारित:
		थांबा;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	परत अंक_written;
}

uint64_t
perf_event_मोजणी(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत 0;
	}

	निवड (handle->type) {
	स्थिती PC_मोजणी:
		परत ((संरचना perf_ctr_मोजणी *)handle)->event_मोजणी.load();

	स्थिती PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			परत pce->event_मोजणी.load();
		}

	स्थिती PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			परत pci->event_मोजणी.load();
		}

	पूर्वनिर्धारित:
		थांबा;
	}

	परत 0;
}

दशांश
perf_mean(perf_counter_t handle)
{
	जर (handle == nullptr) {
		परत 0;
	}

	निवड (handle->type) {
	स्थिती PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			परत pce->mean.load();
		}

	स्थिती PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			परत pci->mean.load();
		}

	पूर्वनिर्धारित:
		थांबा;
	}

	परत 0.0f;
}

शून्य
perf_iterate_all(perf_callback cb, शून्य *user)
{
	स्थिर constexpr माप_प्रकार PERF_COUNTER_ओळ_BUFFER_SIZE = 220;
	अक्षर buffer[PERF_COUNTER_ओळ_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	पर्यंत (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		जर (!handle->pending_free) {
			perf_print_counter_buffer(buffer, आकार(buffer), handle);

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

	जर (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

शून्य
perf_print_all(शून्य)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	पर्यंत (handle != nullptr) {
		जर (!handle->pending_free) {
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

	साठी (पूर्णांक i = 0; i < get_latency_bucket_मोजणी(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (दीर्घ पूर्णांक)latency.counter);
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

	पर्यंत (handle != nullptr) {
		जर (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
