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

#അന്തര്ഭാവ <inttypes.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <drivers/drv_hrt.h>
#അന്തര്ഭാവ <math.h>
#അന്തര്ഭാവ <pthread.h>
#അന്തര്ഭാവ <systemlib/err.h>

#അന്തര്ഭാവ "perf_counter.h"

#അന്തര്ഭാവ <px4_platform_common/atomic.h>

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
#എങ്കിൽ defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
template <typename T> using PerfVar = px4::atomic<T>;
#അല്ലെങ്കിൽ
template <typename T>
വർഗ്ഗം PerfVar
{
പൊതു:
	PerfVar() = സ്ഥിരസ്ഥിതി;
	T load() സ്ഥിരം { തിരികെ മൂല്യം_2; }
	ശൂന്യം store(T value) { മൂല്യം_2 = value; }
	T fetch_add(T value) { സ്ഥിരം T previous = മൂല്യം_2; മൂല്യം_2 += value; തിരികെ previous; }
സ്വകാര്യ:
	T മൂല്യം_2 {};
};
#endif

/**
 * Header common to all counters.
 */
ഘടന perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	എണ്ണൽ perf_counter_type	type;	/**< counter type */
	തർക്കം			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	സ്ഥിരം അക്ഷരം		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
ഘടന perf_ctr_എണ്ണം : പൊതു perf_ctr_header {
	PerfVar<uint64_t>	event_എണ്ണം;
};

/**
 * PC_ELAPSED counter.
 */
ഘടന perf_ctr_elapsed : പൊതു perf_ctr_header {
	PerfVar<uint64_t>	event_എണ്ണം;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_total;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<ദശാംശം>	mean;
	PerfVar<ദശാംശം>	M2;
};

/**
 * PC_INTERVAL counter.
 */
ഘടന perf_ctr_interval : പൊതു perf_ctr_header {
	PerfVar<uint64_t>	event_എണ്ണം;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_last;
	PerfVar<uint32_t>	time_least;
	PerfVar<uint32_t>	time_most;
	PerfVar<ദശാംശം>	mean;
	PerfVar<ദശാംശം>	M2;
};

/**
 * List of all known counters.
 */
സ്ഥിതം sq_queue_t	perf_counters = { nullptr, nullptr };

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
സ്ഥിതം പൂർണ്ണസംഖ്യ perf_iterate_active = 0;

സ്ഥിതം ശൂന്യം
perf_delete(perf_counter_t handle)
{
	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_എണ്ണം:
		delete (ഘടന perf_ctr_എണ്ണം *)handle;
		നിർത്തുക;

	അവസ്ഥ PC_ELAPSED:
		delete (ഘടന perf_ctr_elapsed *)handle;
		നിർത്തുക;

	അവസ്ഥ PC_INTERVAL:
		delete (ഘടന perf_ctr_interval *)handle;
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
സ്ഥിതം ശൂന്യം
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ആകുമ്പോൾ (handle != nullptr) {
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		എങ്കിൽ (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = next;
	}
}

perf_counter_t
perf_alloc(എണ്ണൽ perf_counter_type type, സ്ഥിരം അക്ഷരം *name)
{
	perf_counter_t ctr = nullptr;

	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ PC_എണ്ണം:
		ctr = പുതിയ perf_ctr_എണ്ണം();
		നിർത്തുക;

	അവസ്ഥ PC_ELAPSED:
		ctr = പുതിയ perf_ctr_elapsed();
		നിർത്തുക;

	അവസ്ഥ PC_INTERVAL:
		ctr = പുതിയ perf_ctr_interval();
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	എങ്കിൽ (ctr != nullptr) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = അസത്യം;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	തിരികെ ctr;
}

perf_counter_t
perf_alloc_once(എണ്ണൽ perf_counter_type type, സ്ഥിരം അക്ഷരം *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ആകുമ്പോൾ (handle != nullptr) {
		എങ്കിൽ (!handle->pending_free && !strcmp(handle->name, name)) {
			എങ്കിൽ (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				തിരികെ handle;

			} അല്ലെങ്കിൽ {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				തിരികെ nullptr;
			}
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	തിരികെ perf_alloc(type, name);
}

ശൂന്യം
perf_free(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	എങ്കിൽ (perf_iterate_active > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = സത്യം;
		pthread_mutex_unlock(&perf_counters_mutex);
		തിരികെ;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

ശൂന്യം
perf_എണ്ണം(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_എണ്ണം:
		((ഘടന perf_ctr_എണ്ണം *)handle)->event_എണ്ണം.fetch_add(1);
		നിർത്തുക;

	അവസ്ഥ PC_INTERVAL:
		perf_എണ്ണം_interval(handle, hrt_absolute_time());
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
}

ശൂന്യം
perf_begin(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_ELAPSED:
		((ഘടന perf_ctr_elapsed *)handle)->time_start.store(hrt_absolute_time());
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
}

ശൂന്യം
perf_end(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_ELAPSED: {
			ഘടന perf_ctr_elapsed *pce = (ഘടന perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.load();

			എങ്കിൽ (ts != 0) {
				perf_set_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
}

ശൂന്യം
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_ELAPSED: {
			ഘടന perf_ctr_elapsed *pce = (ഘടന perf_ctr_elapsed *)handle;

			എങ്കിൽ (elapsed >= 0) {
				uint64_t ec = pce->event_എണ്ണം.fetch_add(1) + 1;
				pce->time_total.fetch_add((uint64_t)elapsed);

				uint32_t tl = pce->time_least.load();

				എങ്കിൽ ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_least.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.load();

				എങ്കിൽ (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				ദശാംശം dt = elapsed / 1e6f;
				ദശാംശം mean_val = pce->mean.load();
				ദശാംശം delta_intvl = dt - mean_val;
				mean_val += delta_intvl / ec;
				pce->mean.store(mean_val);
				ദശാംശം m2_val = pce->M2.load();
				m2_val += delta_intvl * (dt - mean_val);
				pce->M2.store(m2_val);

				pce->time_start.store((uint64_t)0);
			}
		}
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
}

ശൂന്യം
perf_എണ്ണം_interval(perf_counter_t handle, hrt_abstime now)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_INTERVAL: {
			ഘടന perf_ctr_interval *pci = (ഘടന perf_ctr_interval *)handle;
			uint64_t ec = pci->event_എണ്ണം.load();

			തിരഞ്ഞെടുപ്പ് (ec) {
			അവസ്ഥ 0:
				pci->time_first.store(now);
				നിർത്തുക;

			അവസ്ഥ 1: {
					uint64_t tl = pci->time_last.load();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_least.store(interval);
					pci->time_most.store(interval);
					pci->mean.store(interval / 1e6f);
					pci->M2.store(0.0f);
					നിർത്തുക;
				}

			സ്ഥിരസ്ഥിതി: {
					uint64_t tl = pci->time_last.load();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_least.load();

					എങ്കിൽ ((uint32_t)interval < least) {
						pci->time_least.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.load();

					എങ്കിൽ ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					ദശാംശം dt = interval / 1e6f;
					ദശാംശം mean_val = pci->mean.load();
					ദശാംശം delta_intvl = dt - mean_val;
					mean_val += delta_intvl / ec;
					pci->mean.store(mean_val);
					ദശാംശം m2_val = pci->M2.load();
					m2_val += delta_intvl * (dt - mean_val);
					pci->M2.store(m2_val);
					നിർത്തുക;
				}
			}

			pci->time_last.store(now);
			pci->event_എണ്ണം.fetch_add(1);
			നിർത്തുക;
		}

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
}

ശൂന്യം
perf_set_എണ്ണം(perf_counter_t handle, uint64_t എണ്ണം)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_എണ്ണം: {
			((ഘടന perf_ctr_എണ്ണം *)handle)->event_എണ്ണം.store(എണ്ണം);
		}
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

}

ശൂന്യം
perf_cancel(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_ELAPSED: {
			ഘടന perf_ctr_elapsed *pce = (ഘടന perf_ctr_elapsed *)handle;
			pce->time_start.store((uint64_t)0);
		}
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
}

ശൂന്യം
perf_reset(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_എണ്ണം:
		((ഘടന perf_ctr_എണ്ണം *)handle)->event_എണ്ണം.store((uint64_t)0);
		നിർത്തുക;

	അവസ്ഥ PC_ELAPSED: {
			ഘടന perf_ctr_elapsed *pce = (ഘടന perf_ctr_elapsed *)handle;
			pce->event_എണ്ണം.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_total.store((uint64_t)0);
			pce->time_least.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->mean.store(0.0f);
			pce->M2.store(0.0f);
			നിർത്തുക;
		}

	അവസ്ഥ PC_INTERVAL: {
			ഘടന perf_ctr_interval *pci = (ഘടന perf_ctr_interval *)handle;
			pci->event_എണ്ണം.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_last.store((uint64_t)0);
			pci->time_least.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->mean.store(0.0f);
			pci->M2.store(0.0f);
			നിർത്തുക;
		}
	}
}

ശൂന്യം
perf_print_counter(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_എണ്ണം: {
			uint64_t ec = ((ഘടന perf_ctr_എണ്ണം *)handle)->event_എണ്ണം.load();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			നിർത്തുക;
		}

	അവസ്ഥ PC_ELAPSED: {
			ഘടന perf_ctr_elapsed *pce = (ഘടന perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_എണ്ണം.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			ദശാംശം m2 = pce->M2.load();
			ദശാംശം rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (ഇരട്ട)tt / (ഇരട്ട)ec,
				     tl, tm, (ഇരട്ട)(1e6f * rms));
			നിർത്തുക;
		}

	അവസ്ഥ PC_INTERVAL: {
			ഘടന perf_ctr_interval *pci = (ഘടന perf_ctr_interval *)handle;
			uint64_t ec = pci->event_എണ്ണം.load();
			uint64_t tf = pci->time_first.load();
			uint64_t tla = pci->time_last.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			ദശാംശം m2 = pci->M2.load();
			ദശാംശം rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (ഇരട്ട)(tla - tf) / (ഇരട്ട)ec,
				     tl, tm, (ഇരട്ട)(1e6f * rms));
			നിർത്തുക;
		}

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
}


പൂർണ്ണസംഖ്യ
perf_print_counter_buffer(അക്ഷരം *buffer, പൂർണ്ണസംഖ്യ length, perf_counter_t handle)
{
	പൂർണ്ണസംഖ്യ അക്കം_written = 0;

	എങ്കിൽ (handle == nullptr) {
		തിരികെ 0;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_എണ്ണം:
		അക്കം_written = snprintf(buffer, length, "%s: %" PRIu64 " events",
				       handle->name,
				       ((ഘടന perf_ctr_എണ്ണം *)handle)->event_എണ്ണം.load());
		നിർത്തുക;

	അവസ്ഥ PC_ELAPSED: {
			ഘടന perf_ctr_elapsed *pce = (ഘടന perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_എണ്ണം.load();
			uint64_t tt = pce->time_total.load();
			uint32_t tl = pce->time_least.load();
			uint32_t tm = pce->time_most.load();
			ദശാംശം m2 = pce->M2.load();
			ദശാംശം rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			അക്കം_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (ഇരട്ട)tt / (ഇരട്ട)ec,
					       tl,
					       tm,
					       (ഇരട്ട)(1e6f * rms));
			നിർത്തുക;
		}

	അവസ്ഥ PC_INTERVAL: {
			ഘടന perf_ctr_interval *pci = (ഘടന perf_ctr_interval *)handle;
			uint64_t ec = pci->event_എണ്ണം.load();
			uint32_t tl = pci->time_least.load();
			uint32_t tm = pci->time_most.load();
			ദശാംശം m2 = pci->M2.load();
			ദശാംശം rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			അക്കം_written = snprintf(buffer, length,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (ഇരട്ട)(pci->time_last.load() - pci->time_first.load()) / (ഇരട്ട)ec,
					       tl,
					       tm,
					       (ഇരട്ട)(1e6f * rms));
			നിർത്തുക;
		}

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	buffer[length - 1] = 0; // ensure 0-termination
	തിരികെ അക്കം_written;
}

uint64_t
perf_event_എണ്ണം(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ 0;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_എണ്ണം:
		തിരികെ ((ഘടന perf_ctr_എണ്ണം *)handle)->event_എണ്ണം.load();

	അവസ്ഥ PC_ELAPSED: {
			ഘടന perf_ctr_elapsed *pce = (ഘടന perf_ctr_elapsed *)handle;
			തിരികെ pce->event_എണ്ണം.load();
		}

	അവസ്ഥ PC_INTERVAL: {
			ഘടന perf_ctr_interval *pci = (ഘടന perf_ctr_interval *)handle;
			തിരികെ pci->event_എണ്ണം.load();
		}

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	തിരികെ 0;
}

ദശാംശം
perf_mean(perf_counter_t handle)
{
	എങ്കിൽ (handle == nullptr) {
		തിരികെ 0;
	}

	തിരഞ്ഞെടുപ്പ് (handle->type) {
	അവസ്ഥ PC_ELAPSED: {
			ഘടന perf_ctr_elapsed *pce = (ഘടന perf_ctr_elapsed *)handle;
			തിരികെ pce->mean.load();
		}

	അവസ്ഥ PC_INTERVAL: {
			ഘടന perf_ctr_interval *pci = (ഘടന perf_ctr_interval *)handle;
			തിരികെ pci->mean.load();
		}

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	തിരികെ 0.0f;
}

ശൂന്യം
perf_iterate_all(perf_callback cb, ശൂന്യം *user)
{
	സ്ഥിതം constexpr size_t PERF_COUNTER_വരി_BUFFER_SIZE = 220;
	അക്ഷരം buffer[PERF_COUNTER_വരി_BUFFER_SIZE];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_active;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ആകുമ്പോൾ (handle != nullptr) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t next = (perf_counter_t)sq_next(&handle->link);

		എങ്കിൽ (!handle->pending_free) {
			perf_print_counter_buffer(buffer, വലുപ്പം(buffer), handle);

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

	എങ്കിൽ (--perf_iterate_active == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ശൂന്യം
perf_print_all(ശൂന്യം)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ആകുമ്പോൾ (handle != nullptr) {
		എങ്കിൽ (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

ശൂന്യം
perf_print_latency(ശൂന്യം)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < get_latency_bucket_എണ്ണം(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (long പൂർണ്ണസംഖ്യ)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_എണ്ണം() - 1, get_latency_bucket_എണ്ണം());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.counter);
}

ശൂന്യം
perf_reset_all(ശൂന്യം)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	ആകുമ്പോൾ (handle != nullptr) {
		എങ്കിൽ (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_next(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
