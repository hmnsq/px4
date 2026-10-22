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

#ଅନ୍ତର୍ଭାଵ <inttypes.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <drivers/drv_hrt.h>
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <pthread.h>
#ଅନ୍ତର୍ଭାଵ <systemlib/err.h>

#ଅନ୍ତର୍ଭାଵ "perf_counter.h"

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/atomic.h>

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
#ଯଦି defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
template <typename T> using PerfVar = px4::atomic<T>;
#ଅନ୍ୟଥା
template <typename T>
ଶ୍ରେଣୀ PerfVar
{
ସର୍ବସାଧାରଣ:
	PerfVar() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	T load() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ_2; }
	ଶୂନ୍ୟ store(T value) { ମୂଲ୍ୟ_2 = value; }
	T fetch_add(T value) { ଅଚଳ T previous = ମୂଲ୍ୟ_2; ମୂଲ୍ୟ_2 += value; ଫେରନ୍ତୁ previous; }
ବ୍ୟକ୍ତିଗତ:
	T ମୂଲ୍ୟ_2 {};
};
#endif

/**
 * Header common to all counters.
 */
ସଂରଚନା perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	ଗଣନା perf_counter_type	type;	/**< counter type */
	ତାର୍କିକ			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ଅଚଳ ଅକ୍ଷର		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
ସଂରଚନା perf_ctr_ଗଣନା : ସର୍ବସାଧାରଣ perf_ctr_header {
	PerfVar<uint64_t>	event_ଗଣନା;
};

/**
 * PC_ELAPSED counter.
 */
ସଂରଚନା perf_ctr_elapsed : ସର୍ବସାଧାରଣ perf_ctr_header {
	PerfVar<uint64_t>	event_ଗଣନା;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_total;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<ଦଶମିକ>	mean;
	PerfVar<ଦଶମିକ>	M2;
};

/**
 * PC_INTERVAL counter.
 */
ସଂରଚନା perf_ctr_interval : ସର୍ବସାଧାରଣ perf_ctr_header {
	PerfVar<uint64_t>	event_ଗଣନା;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<ଦଶମିକ>	mean;
	PerfVar<ଦଶମିକ>	M2;
};

/**
 * List of all known counters.
 */
ସ୍ଥିର sq_queue_t	perf_counters = { nullptr, nullptr };

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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା perf_iterate_active = 0;

ସ୍ଥିର ଶୂନ୍ୟ
perf_delete(perf_counter_t handle)
{
	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ଗଣନା:
		delete (ସଂରଚନା perf_ctr_ଗଣନା *)handle;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PC_ELAPSED:
		delete (ସଂରଚନା perf_ctr_elapsed *)handle;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PC_INTERVAL:
		delete (ସଂରଚନା perf_ctr_interval *)handle;
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
ସ୍ଥିର ଶୂନ୍ୟ
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ଯେତେବେଳେ (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		ଯଦି (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(ଗଣନା perf_counter_type type, ଅଚଳ ଅକ୍ଷର *name)
{
	perf_counter_t ctr = nullptr;

	ବାଛ (type) {
	ସ୍ଥିତି PC_ଗଣନା:
		ctr = ନୂତନ perf_ctr_ଗଣନା();
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PC_ELAPSED:
		ctr = ନୂତନ perf_ctr_elapsed();
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PC_INTERVAL:
		ctr = ନୂତନ perf_ctr_interval();
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	ଯଦି (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = ମିଥ୍ୟା;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	ଫେରନ୍ତୁ ctr;
}

perf_counter_t
perf_alloc_once(ଗଣନା perf_counter_type type, ଅଚଳ ଅକ୍ଷର *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ଯେତେବେଳେ (handle != nullptr) {
		ଯଦି (!handle->pending_free && !strcmp(handle->name, name)) {
			ଯଦି (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				ଫେରନ୍ତୁ handle;

			} ଅନ୍ୟଥା {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				ଫେରନ୍ତୁ nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	ଫେରନ୍ତୁ perf_alloc(type, name);
}

ଶୂନ୍ୟ
perf_free(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	ଯଦି (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = ସତ୍ୟ;
		pthread_mutex_unlock(&perf_counters_mutex);
		ଫେରନ୍ତୁ;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

ଶୂନ୍ୟ
perf_ଗଣନା(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ଗଣନା:
		((ସଂରଚନା perf_ctr_ଗଣନା *)handle)->event_ଗଣନା.fetch_add(1);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PC_INTERVAL:
		perf_ଗଣନା_interval(handle, hrt_absolute_time());
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
}

ଶୂନ୍ୟ
perf_begin(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ELAPSED:
		((ସଂରଚନା perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
}

ଶୂନ୍ୟ
perf_end(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ELAPSED: {
			ସଂରଚନା perf_ctr_elapsed *pce = (ସଂରଚନା perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			ଯଦି (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
}

ଶୂନ୍ୟ
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ELAPSED: {
			ସଂରଚନା perf_ctr_elapsed *pce = (ସଂରଚନା perf_ctr_elapsed *)handle;

			ଯଦି (elapsed >= 0) {
				uint64_t ec = pce->event_ଗଣନା.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				ଯଦି ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.load();

				ଯଦି (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				ଦଶମିକ dt = elapsed / 1e6f;
				ଦଶମିକ mean_val = pce->mean.load();
				ଦଶମିକ delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				ଦଶମିକ m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
}

ଶୂନ୍ୟ
perf_ଗଣନା_interval(perf_counter_t handle, hrt_abstime now)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_INTERVAL: {
			ସଂରଚନା perf_ctr_interval *pci = (ସଂରଚନା perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ଗଣନା.load();

			ବାଛ (ec) {
			ସ୍ଥିତି 0:
				pci->time_first.store(now);
				ଭାଙ୍ଗ;

			ସ୍ଥିତି 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					ଭାଙ୍ଗ;
				}

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					ଯଦି ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					ଯଦି ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					ଦଶମିକ dt = interval / 1e6f;
					ଦଶମିକ mean_val = pci->mean.load();
					ଦଶମିକ delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					ଦଶମିକ m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					ଭାଙ୍ଗ;
				}
			}

			pci->time_last.store(now);
			pci->event_ଗଣନା.fetch_add(1);
			ଭାଙ୍ଗ;
		}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
}

ଶୂନ୍ୟ
perf_set_ଗଣନା(perf_counter_t handle, uint64_t ଗଣନା_2)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ଗଣନା: {
			((ସଂରଚନା perf_ctr_ଗଣନା *)handle)->event_ଗଣନା.store(ଗଣନା_2);
		}
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

}

ଶୂନ୍ୟ
perf_cancel(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ELAPSED: {
			ସଂରଚନା perf_ctr_elapsed *pce = (ସଂରଚନା perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
}

ଶୂନ୍ୟ
perf_reset(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ଗଣନା:
		((ସଂରଚନା perf_ctr_ଗଣନା *)handle)->event_ଗଣନା.store((uint64_t)0);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PC_ELAPSED: {
			ସଂରଚନା perf_ctr_elapsed *pce = (ସଂରଚନା perf_ctr_elapsed *)handle;
			pce->event_ଗଣନା.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି PC_INTERVAL: {
			ସଂରଚନା perf_ctr_interval *pci = (ସଂରଚନା perf_ctr_interval *)handle;
			pci->event_ଗଣନା.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			ଭାଙ୍ଗ;
		}
	}
}

ଶୂନ୍ୟ
perf_print_counter(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ଗଣନା: {
			uint64_t ec = ((ସଂରଚନା perf_ctr_ଗଣନା *)handle)->event_ଗଣନା.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି PC_ELAPSED: {
			ସଂରଚନା perf_ctr_elapsed *pce = (ସଂରଚନା perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ଗଣନା.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			ଦଶମିକ m2 = pce->M2.load();
			ଦଶମିକ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (ଦ୍ୱିଗୁଣ)tt / (ଦ୍ୱିଗୁଣ)ec,
				     tl, tm, (ଦ୍ୱିଗୁଣ)(1e6f * rms));
			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି PC_INTERVAL: {
			ସଂରଚନା perf_ctr_interval *pci = (ସଂରଚନା perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ଗଣନା.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			ଦଶମିକ m2 = pci->M2.load();
			ଦଶମିକ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (ଦ୍ୱିଗୁଣ)(tla - tf) / (ଦ୍ୱିଗୁଣ)ec,
				     tl, tm, (ଦ୍ୱିଗୁଣ)(1e6f * rms));
			ଭାଙ୍ଗ;
		}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା
perf_print_counter_buffer(ଅକ୍ଷର *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length, perf_counter_t handle)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_written = 0;

	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ 0;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ଗଣନା:
		ଅଙ୍କ_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((ସଂରଚନା perf_ctr_ଗଣନା *)handle)->event_ଗଣନା.load());
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PC_ELAPSED: {
			ସଂରଚନା perf_ctr_elapsed *pce = (ସଂରଚନା perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ଗଣନା.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			ଦଶମିକ m2 = pce->M2.load();
			ଦଶମିକ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			ଅଙ୍କ_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (ଦ୍ୱିଗୁଣ)tt / (ଦ୍ୱିଗୁଣ)ec,
					       tl,
					       tm,
					       (ଦ୍ୱିଗୁଣ)(1e6f * rms));
			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି PC_INTERVAL: {
			ସଂରଚନା perf_ctr_interval *pci = (ସଂରଚନା perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ଗଣନା.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			ଦଶମିକ m2 = pci->M2.load();
			ଦଶମିକ rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			ଅଙ୍କ_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (ଦ୍ୱିଗୁଣ)(pci->time_last.load() - pci->time_first.load()) / (ଦ୍ୱିଗୁଣ)ec,
					       tl,
					       tm,
					       (ଦ୍ୱିଗୁଣ)(1e6f * rms));
			ଭାଙ୍ଗ;
		}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	ଫେରନ୍ତୁ ଅଙ୍କ_written;
}

uint64_t
perf_event_ଗଣନା(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ 0;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ଗଣନା:
		ଫେରନ୍ତୁ ((ସଂରଚନା perf_ctr_ଗଣନା *)handle)->event_ଗଣନା.load();

	ସ୍ଥିତି PC_ELAPSED: {
			ସଂରଚନା perf_ctr_elapsed *pce = (ସଂରଚନା perf_ctr_elapsed *)handle;
			ଫେରନ୍ତୁ pce->event_ଗଣନା.load();
		}

	ସ୍ଥିତି PC_INTERVAL: {
			ସଂରଚନା perf_ctr_interval *pci = (ସଂରଚନା perf_ctr_interval *)handle;
			ଫେରନ୍ତୁ pci->event_ଗଣନା.load();
		}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ 0;
}

ଦଶମିକ
perf_mean(perf_counter_t handle)
{
	ଯଦି (handle == nullptr) {
		ଫେରନ୍ତୁ 0;
	}

	ବାଛ (handle->type) {
	ସ୍ଥିତି PC_ELAPSED: {
			ସଂରଚନା perf_ctr_elapsed *pce = (ସଂରଚନା perf_ctr_elapsed *)handle;
			ଫେରନ୍ତୁ pce->mean.load();
		}

	ସ୍ଥିତି PC_INTERVAL: {
			ସଂରଚନା perf_ctr_interval *pci = (ସଂରଚନା perf_ctr_interval *)handle;
			ଫେରନ୍ତୁ pci->mean.load();
		}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ 0.0f;
}

ଶୂନ୍ୟ
perf_iterate_all(perf_callback cb, ଶୂନ୍ୟ *user)
{
	ସ୍ଥିର constexpr size_t PERF_COUNTER_ଧାଡ଼ି_BUFFER_SIZE = 220;
	ଅକ୍ଷର buffer[PERF_COUNTER_ଧାଡ଼ି_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ଯେତେବେଳେ (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		ଯଦି (!handle->pending_free) {
			perf_print_counter_buffer(buffer, ଆକାର(buffer), handle);

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

	ଯଦି (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ଶୂନ୍ୟ
perf_print_all(ଶୂନ୍ୟ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ଯେତେବେଳେ (handle != nullptr) {
		ଯଦି (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ଶୂନ୍ୟ
perf_print_latency(ଶୂନ୍ୟ)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < get_latency_bucket_ଗଣନା(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long ପୂର୍ଣ୍ଣସଂଖ୍ୟା)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_ଗଣନା() - 1, get_latency_bucket_ଗଣନା());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.counter);
}

ଶୂନ୍ୟ
perf_reset_all(ଶୂନ୍ୟ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ଯେତେବେଳେ (handle != nullptr) {
		ଯଦି (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
