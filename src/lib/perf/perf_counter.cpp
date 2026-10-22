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

#شامل <inttypes.h>
#شامل <stdlib.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <drivers/drv_hrt.h>
#شامل <math.h>
#شامل <pthread.h>
#شامل <systemlib/err.h>

#شامل "perf_counter.h"

#شامل <px4_platform_common/atomic.h>

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
#اگر defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
سانچہ <قسم_نام T> استعمال PerfVar = px4::atomic<T>;
#ورنہ
سانچہ <قسم_نام T>
جماعت PerfVar
{
عوامی:
	PerfVar() = طےشدہ;
	T بھرو() ثابت { واپس قدر_2; }
	خالی store(T value) { قدر_2 = value; }
	T fetch_جوڑو(T value) { ثابت T پچھلا = قدر_2; قدر_2 += value; واپس پچھلا; }
نجی:
	T قدر_2 {};
};
#اگر_ختم

/**
 * Header common to all counters.
 */
ساخت perf_ctr_سرخی {
	sq_اندراج_t		link;	/**< list linkage */
	شمار perf_شمار_type	type;	/**< counter type */
	منطقی			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ثابت حرف		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
ساخت perf_ctr_گنتی : عوامی perf_ctr_سرخی {
	PerfVar<uint64_t>	event_گنتی;
};

/**
 * PC_ELAPSED counter.
 */
ساخت perf_ctr_elapsed : عوامی perf_ctr_سرخی {
	PerfVar<uint64_t>	event_گنتی;
	PerfVar<uint64_t>	وقت_آغاز;
	PerfVar<uint64_t>	وقت_کل;
	PerfVar<uint32_t>	وقت_کم_سے_کم;
	PerfVar<uint32_t>	وقت_most;
	PerfVar<اعشاری>	اوسط;
	PerfVar<اعشاری>	M2;
};

/**
 * PC_INTERVAL counter.
 */
ساخت perf_ctr_interval : عوامی perf_ctr_سرخی {
	PerfVar<uint64_t>	event_گنتی;
	PerfVar<uint64_t>	وقت_event;
	PerfVar<uint64_t>	وقت_پہلا;
	PerfVar<uint64_t>	وقت_آخری;
	PerfVar<uint32_t>	وقت_کم_سے_کم;
	PerfVar<uint32_t>	وقت_most;
	PerfVar<اعشاری>	اوسط;
	PerfVar<اعشاری>	M2;
};

/**
 * List of all known counters.
 */
ساکن sq_قطار_t	perf_counters = { خالی_اشاریہ, خالی_اشاریہ };

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
ساکن صحیح perf_iterate_فعال = 0;

ساکن خالی
perf_delete(perf_شمار_t handle)
{
	انتخاب (handle->type) {
	حالت PC_گنتی:
		حذف (ساخت perf_ctr_گنتی *)handle;
		توڑو;

	حالت PC_ELAPSED:
		حذف (ساخت perf_ctr_elapsed *)handle;
		توڑو;

	حالت PC_INTERVAL:
		حذف (ساخت perf_ctr_interval *)handle;
		توڑو;

	طےشدہ:
		توڑو;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
ساکن خالی
perf_free_pending_locked()
{
	perf_شمار_t handle = (perf_شمار_t)sq_peek(&perf_counters);

	جب (handle != خالی_اشاریہ) {
		perf_شمار_t اگلا = (perf_شمار_t)sq_اگلا(&handle->link);

		اگر (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = اگلا;
	}
}

perf_شمار_t
perf_alloc(شمار perf_شمار_type type, ثابت حرف *name)
{
	perf_شمار_t ctr = خالی_اشاریہ;

	انتخاب (type) {
	حالت PC_گنتی:
		ctr = نیا perf_ctr_گنتی();
		توڑو;

	حالت PC_ELAPSED:
		ctr = نیا perf_ctr_elapsed();
		توڑو;

	حالت PC_INTERVAL:
		ctr = نیا perf_ctr_interval();
		توڑو;

	طےشدہ:
		توڑو;
	}

	اگر (ctr != خالی_اشاریہ) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = جھوٹ;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	واپس ctr;
}

perf_شمار_t
perf_alloc_once(شمار perf_شمار_type type, ثابت حرف *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_شمار_t handle = (perf_شمار_t)sq_peek(&perf_counters);

	جب (handle != خالی_اشاریہ) {
		اگر (!handle->pending_free && !strcmp(handle->name, name)) {
			اگر (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				واپس handle;

			} ورنہ {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				واپس خالی_اشاریہ;
			}
		}

		handle = (perf_شمار_t)sq_اگلا(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	واپس perf_alloc(type, name);
}

خالی
perf_free(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	اگر (perf_iterate_فعال > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = سچ;
		pthread_mutex_unlock(&perf_counters_mutex);
		واپس;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

خالی
perf_گنتی(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_گنتی:
		((ساخت perf_ctr_گنتی *)handle)->event_گنتی.fetch_جوڑو(1);
		توڑو;

	حالت PC_INTERVAL:
		perf_گنتی_interval(handle, hrt_absolute_وقت());
		توڑو;

	طےشدہ:
		توڑو;
	}
}

خالی
perf_begin(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_ELAPSED:
		((ساخت perf_ctr_elapsed *)handle)->وقت_آغاز.store(hrt_absolute_وقت());
		توڑو;

	طےشدہ:
		توڑو;
	}
}

خالی
perf_انتہا(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_ELAPSED: {
			ساخت perf_ctr_elapsed *pce = (ساخت perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->وقت_آغاز.بھرو();

			اگر (ts != 0) {
				perf_سیٹ_elapsed(handle, hrt_absolute_وقت() - ts);
			}
		}
		توڑو;

	طےشدہ:
		توڑو;
	}
}

خالی
perf_سیٹ_elapsed(perf_شمار_t handle, int64_t elapsed)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_ELAPSED: {
			ساخت perf_ctr_elapsed *pce = (ساخت perf_ctr_elapsed *)handle;

			اگر (elapsed >= 0) {
				uint64_t ec = pce->event_گنتی.fetch_جوڑو(1) + 1;
				pce->وقت_کل.fetch_جوڑو((uint64_t)elapsed);

				uint32_t tl = pce->وقت_کم_سے_کم.بھرو();

				اگر ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->وقت_کم_سے_کم.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->وقت_most.بھرو();

				اگر (tm < (uint32_t)elapsed) {
					pce->وقت_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				اعشاری dt = elapsed / 1e6f;
				اعشاری اوسط_val = pce->اوسط.بھرو();
				اعشاری delta_intvl = dt - اوسط_val;
				اوسط_val += delta_intvl / ec;
				pce->اوسط.store(اوسط_val);
				اعشاری m2_val = pce->M2.بھرو();
				m2_val += delta_intvl * (dt - اوسط_val);
				pce->M2.store(m2_val);

				pce->وقت_آغاز.store((uint64_t)0);
			}
		}
		توڑو;

	طےشدہ:
		توڑو;
	}
}

خالی
perf_گنتی_interval(perf_شمار_t handle, hrt_abstime now)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_INTERVAL: {
			ساخت perf_ctr_interval *pci = (ساخت perf_ctr_interval *)handle;
			uint64_t ec = pci->event_گنتی.بھرو();

			انتخاب (ec) {
			حالت 0:
				pci->وقت_پہلا.store(now);
				توڑو;

			حالت 1: {
					uint64_t tl = pci->وقت_آخری.بھرو();
					uint32_t interval = (uint32_t)(now - tl);
					pci->وقت_کم_سے_کم.store(interval);
					pci->وقت_most.store(interval);
					pci->اوسط.store(interval / 1e6f);
					pci->M2.store(0.0f);
					توڑو;
				}

			طےشدہ: {
					uint64_t tl = pci->وقت_آخری.بھرو();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->وقت_کم_سے_کم.بھرو();

					اگر ((uint32_t)interval < least) {
						pci->وقت_کم_سے_کم.store((uint32_t)interval);
					}

					uint32_t most = pci->وقت_most.بھرو();

					اگر ((uint32_t)interval > most) {
						pci->وقت_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					اعشاری dt = interval / 1e6f;
					اعشاری اوسط_val = pci->اوسط.بھرو();
					اعشاری delta_intvl = dt - اوسط_val;
					اوسط_val += delta_intvl / ec;
					pci->اوسط.store(اوسط_val);
					اعشاری m2_val = pci->M2.بھرو();
					m2_val += delta_intvl * (dt - اوسط_val);
					pci->M2.store(m2_val);
					توڑو;
				}
			}

			pci->وقت_آخری.store(now);
			pci->event_گنتی.fetch_جوڑو(1);
			توڑو;
		}

	طےشدہ:
		توڑو;
	}
}

خالی
perf_سیٹ_گنتی(perf_شمار_t handle, uint64_t گنتی)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_گنتی: {
			((ساخت perf_ctr_گنتی *)handle)->event_گنتی.store(گنتی);
		}
		توڑو;

	طےشدہ:
		توڑو;
	}

}

خالی
perf_cancel(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_ELAPSED: {
			ساخت perf_ctr_elapsed *pce = (ساخت perf_ctr_elapsed *)handle;
			pce->وقت_آغاز.store((uint64_t)0);
		}
		توڑو;

	طےشدہ:
		توڑو;
	}
}

خالی
perf_reset(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_گنتی:
		((ساخت perf_ctr_گنتی *)handle)->event_گنتی.store((uint64_t)0);
		توڑو;

	حالت PC_ELAPSED: {
			ساخت perf_ctr_elapsed *pce = (ساخت perf_ctr_elapsed *)handle;
			pce->event_گنتی.store((uint64_t)0);
			pce->وقت_آغاز.store((uint64_t)0);
			pce->وقت_کل.store((uint64_t)0);
			pce->وقت_کم_سے_کم.store((uint32_t)0);
			pce->وقت_most.store((uint32_t)0);
			pce->اوسط.store(0.0f);
			pce->M2.store(0.0f);
			توڑو;
		}

	حالت PC_INTERVAL: {
			ساخت perf_ctr_interval *pci = (ساخت perf_ctr_interval *)handle;
			pci->event_گنتی.store((uint64_t)0);
			pci->وقت_event.store((uint64_t)0);
			pci->وقت_پہلا.store((uint64_t)0);
			pci->وقت_آخری.store((uint64_t)0);
			pci->وقت_کم_سے_کم.store((uint32_t)0);
			pci->وقت_most.store((uint32_t)0);
			pci->اوسط.store(0.0f);
			pci->M2.store(0.0f);
			توڑو;
		}
	}
}

خالی
perf_print_شمار(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس;
	}

	انتخاب (handle->type) {
	حالت PC_گنتی: {
			uint64_t ec = ((ساخت perf_ctr_گنتی *)handle)->event_گنتی.بھرو();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			توڑو;
		}

	حالت PC_ELAPSED: {
			ساخت perf_ctr_elapsed *pce = (ساخت perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_گنتی.بھرو();
			uint64_t tt = pce->وقت_کل.بھرو();
			uint32_t tl = pce->وقت_کم_سے_کم.بھرو();
			uint32_t tm = pce->وقت_most.بھرو();
			اعشاری m2 = pce->M2.بھرو();
			اعشاری rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (دوہرا)tt / (دوہرا)ec,
				     tl, tm, (دوہرا)(1e6f * rms));
			توڑو;
		}

	حالت PC_INTERVAL: {
			ساخت perf_ctr_interval *pci = (ساخت perf_ctr_interval *)handle;
			uint64_t ec = pci->event_گنتی.بھرو();
			uint64_t tf = pci->وقت_پہلا.بھرو();
			uint64_t tla = pci->وقت_آخری.بھرو();
			uint32_t tl = pci->وقت_کم_سے_کم.بھرو();
			uint32_t tm = pci->وقت_most.بھرو();
			اعشاری m2 = pci->M2.بھرو();
			اعشاری rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (دوہرا)(tla - tf) / (دوہرا)ec,
				     tl, tm, (دوہرا)(1e6f * rms));
			توڑو;
		}

	طےشدہ:
		توڑو;
	}
}


صحیح
perf_print_شمار_بفر(حرف *بفر_2, صحیح لمبائی_2, perf_شمار_t handle)
{
	صحیح عدد_written = 0;

	اگر (handle == خالی_اشاریہ) {
		واپس 0;
	}

	انتخاب (handle->type) {
	حالت PC_گنتی:
		عدد_written = snprintf(بفر_2, لمبائی_2, "%s: %" PRIu64 " events",
				       handle->name,
				       ((ساخت perf_ctr_گنتی *)handle)->event_گنتی.بھرو());
		توڑو;

	حالت PC_ELAPSED: {
			ساخت perf_ctr_elapsed *pce = (ساخت perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_گنتی.بھرو();
			uint64_t tt = pce->وقت_کل.بھرو();
			uint32_t tl = pce->وقت_کم_سے_کم.بھرو();
			uint32_t tm = pce->وقت_most.بھرو();
			اعشاری m2 = pce->M2.بھرو();
			اعشاری rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			عدد_written = snprintf(بفر_2, لمبائی_2,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (دوہرا)tt / (دوہرا)ec,
					       tl,
					       tm,
					       (دوہرا)(1e6f * rms));
			توڑو;
		}

	حالت PC_INTERVAL: {
			ساخت perf_ctr_interval *pci = (ساخت perf_ctr_interval *)handle;
			uint64_t ec = pci->event_گنتی.بھرو();
			uint32_t tl = pci->وقت_کم_سے_کم.بھرو();
			uint32_t tm = pci->وقت_most.بھرو();
			اعشاری m2 = pci->M2.بھرو();
			اعشاری rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			عدد_written = snprintf(بفر_2, لمبائی_2,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (دوہرا)(pci->وقت_آخری.بھرو() - pci->وقت_پہلا.بھرو()) / (دوہرا)ec,
					       tl,
					       tm,
					       (دوہرا)(1e6f * rms));
			توڑو;
		}

	طےشدہ:
		توڑو;
	}

	بفر_2[لمبائی_2 - 1] = 0; // ensure 0-termination
	واپس عدد_written;
}

uint64_t
perf_event_گنتی(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس 0;
	}

	انتخاب (handle->type) {
	حالت PC_گنتی:
		واپس ((ساخت perf_ctr_گنتی *)handle)->event_گنتی.بھرو();

	حالت PC_ELAPSED: {
			ساخت perf_ctr_elapsed *pce = (ساخت perf_ctr_elapsed *)handle;
			واپس pce->event_گنتی.بھرو();
		}

	حالت PC_INTERVAL: {
			ساخت perf_ctr_interval *pci = (ساخت perf_ctr_interval *)handle;
			واپس pci->event_گنتی.بھرو();
		}

	طےشدہ:
		توڑو;
	}

	واپس 0;
}

اعشاری
perf_اوسط(perf_شمار_t handle)
{
	اگر (handle == خالی_اشاریہ) {
		واپس 0;
	}

	انتخاب (handle->type) {
	حالت PC_ELAPSED: {
			ساخت perf_ctr_elapsed *pce = (ساخت perf_ctr_elapsed *)handle;
			واپس pce->اوسط.بھرو();
		}

	حالت PC_INTERVAL: {
			ساخت perf_ctr_interval *pci = (ساخت perf_ctr_interval *)handle;
			واپس pci->اوسط.بھرو();
		}

	طےشدہ:
		توڑو;
	}

	واپس 0.0f;
}

خالی
perf_iterate_all(perf_callback cb, خالی *user)
{
	ساکن constexpr size_t PERF_شمار_سطر_بفر_سائز = 220;
	حرف بفر_2[PERF_شمار_سطر_بفر_سائز];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_فعال;

	perf_شمار_t handle = (perf_شمار_t)sq_peek(&perf_counters);

	جب (handle != خالی_اشاریہ) {
		// advance while locked; entries stay linked until all active iterations end
		perf_شمار_t اگلا = (perf_شمار_t)sq_اگلا(&handle->link);

		اگر (!handle->pending_free) {
			perf_print_شمار_بفر(بفر_2, حجم(بفر_2), handle);

			// the callback may block (the logger waits for MAVLink acks here), so run
			// it without the global mutex: perf_alloc()/perf_free() on other threads
			// keep working, and 'next' stays valid because perf_free() defers deletion
			// while perf_iterate_active > 0
			pthread_mutex_unlock(&perf_counters_mutex);
			cb(بفر_2, user);
			pthread_mutex_lock(&perf_counters_mutex);
		}

		handle = اگلا;
	}

	اگر (--perf_iterate_فعال == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

خالی
perf_print_all(خالی)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_شمار_t handle = (perf_شمار_t)sq_peek(&perf_counters);

	جب (handle != خالی_اشاریہ) {
		اگر (!handle->pending_free) {
			perf_print_شمار(handle);
		}

		handle = (perf_شمار_t)sq_اگلا(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

خالی
perf_print_latency(خالی)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	برائے (صحیح i = 0; i < get_latency_بالٹی_گنتی(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.بالٹی, (طویل صحیح)latency.شمار_2);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_بالٹی_گنتی() - 1, get_latency_بالٹی_گنتی());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.بالٹی, latency.شمار_2);
}

خالی
perf_reset_all(خالی)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_شمار_t handle = (perf_شمار_t)sq_peek(&perf_counters);

	جب (handle != خالی_اشاریہ) {
		اگر (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_شمار_t)sq_اگلا(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
