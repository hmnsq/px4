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

#ضمن <inttypes.h>
#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <drivers/drv_hrt.h>
#ضمن <math.h>
#ضمن <pthread.h>
#ضمن <systemlib/err.h>

#ضمن "perf_counter.h"

#ضمن <px4_platform_common/atomic.h>

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
#إذا defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
سانچا <قسم_ناں T> ورتوں PerfVar = px4::atomic<T>;
#وإلا
سانچا <قسم_ناں T>
صنف PerfVar
{
عام:
	PerfVar() = افتراضي;
	T لدو() ثابت { أرجع قدر_2; }
	فراغ store(T value) { قدر_2 = value; }
	T fetch_جوڑو(T value) { ثابت T پچھلا = قدر_2; قدر_2 += value; أرجع پچھلا; }
خاص:
	T قدر_2 {};
};
#جے_انت

/**
 * Header common to all counters.
 */
بنية perf_ctr_سرخی {
	sq_اندراج_t		link;	/**< list linkage */
	تعداد perf_counter_type	type;	/**< counter type */
	منطقي			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ثابت حرف		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
بنية perf_ctr_گنتی : عام perf_ctr_سرخی {
	PerfVar<uint64_t>	event_گنتی;
};

/**
 * PC_ELAPSED counter.
 */
بنية perf_ctr_elapsed : عام perf_ctr_سرخی {
	PerfVar<uint64_t>	event_گنتی;
	PerfVar<uint64_t>	ویلا_شروع;
	PerfVar<uint64_t>	ویلا_کل;
	PerfVar<uint32_t>	ویلا_گھٹ_توں_گھٹ;
	PerfVar<uint32_t>	ویلا_most;
	PerfVar<عائم>	اوسط;
	PerfVar<عائم>	M2;
};

/**
 * PC_INTERVAL counter.
 */
بنية perf_ctr_interval : عام perf_ctr_سرخی {
	PerfVar<uint64_t>	event_گنتی;
	PerfVar<uint64_t>	ویلا_event;
	PerfVar<uint64_t>	ویلا_پہلا;
	PerfVar<uint64_t>	ویلا_آخری;
	PerfVar<uint32_t>	ویلا_گھٹ_توں_گھٹ;
	PerfVar<uint32_t>	ویلا_most;
	PerfVar<عائم>	اوسط;
	PerfVar<عائم>	M2;
};

/**
 * List of all known counters.
 */
ساكن sq_قطار_t	perf_counters = { خالی_سوچک, خالی_سوچک };

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
ساكن صحيح perf_iterate_سرگرم = 0;

ساكن فراغ
perf_delete(perf_counter_t handle)
{
	بدل (handle->type) {
	حالة PC_گنتی:
		مٹاؤ (بنية perf_ctr_گنتی *)handle;
		اكسر;

	حالة PC_ELAPSED:
		مٹاؤ (بنية perf_ctr_elapsed *)handle;
		اكسر;

	حالة PC_INTERVAL:
		مٹاؤ (بنية perf_ctr_interval *)handle;
		اكسر;

	افتراضي:
		اكسر;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
ساكن فراغ
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != خالی_سوچک) {
		perf_counter_t اگلا = (perf_counter_t)sq_اگلا(&handle->link);

		إذا (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = اگلا;
	}
}

perf_counter_t
perf_alloc(تعداد perf_counter_type type, ثابت حرف *name)
{
	perf_counter_t ctr = خالی_سوچک;

	بدل (type) {
	حالة PC_گنتی:
		ctr = جديد perf_ctr_گنتی();
		اكسر;

	حالة PC_ELAPSED:
		ctr = جديد perf_ctr_elapsed();
		اكسر;

	حالة PC_INTERVAL:
		ctr = جديد perf_ctr_interval();
		اكسر;

	افتراضي:
		اكسر;
	}

	إذا (ctr != خالی_سوچک) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = خطأ;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	أرجع ctr;
}

perf_counter_t
perf_alloc_once(تعداد perf_counter_type type, ثابت حرف *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != خالی_سوچک) {
		إذا (!handle->pending_free && !strcmp(handle->name, name)) {
			إذا (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				أرجع handle;

			} وإلا {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				أرجع خالی_سوچک;
			}
		}

		handle = (perf_counter_t)sq_اگلا(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	أرجع perf_alloc(type, name);
}

فراغ
perf_free(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	إذا (perf_iterate_سرگرم > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = سچ;
		pthread_mutex_unlock(&perf_counters_mutex);
		أرجع;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

فراغ
perf_گنتی(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_گنتی:
		((بنية perf_ctr_گنتی *)handle)->event_گنتی.fetch_جوڑو(1);
		اكسر;

	حالة PC_INTERVAL:
		perf_گنتی_interval(handle, hrt_absolute_ویلا());
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_begin(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED:
		((بنية perf_ctr_elapsed *)handle)->ویلا_شروع.store(hrt_absolute_ویلا());
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_سرا(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->ویلا_شروع.لدو();

			إذا (ts != 0) {
				perf_سیٹ_elapsed(handle, hrt_absolute_ویلا() - ts);
			}
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_سیٹ_elapsed(perf_counter_t handle, int64_t elapsed)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;

			إذا (elapsed >= 0) {
				uint64_t ec = pce->event_گنتی.fetch_جوڑو(1) + 1;
				pce->ویلا_کل.fetch_جوڑو((uint64_t)elapsed);

				uint32_t tl = pce->ویلا_گھٹ_توں_گھٹ.لدو();

				إذا ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->ویلا_گھٹ_توں_گھٹ.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->ویلا_most.لدو();

				إذا (tm < (uint32_t)elapsed) {
					pce->ویلا_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				عائم dt = elapsed / 1e6f;
				عائم اوسط_val = pce->اوسط.لدو();
				عائم delta_intvl = dt - اوسط_val;
				اوسط_val += delta_intvl / ec;
				pce->اوسط.store(اوسط_val);
				عائم m2_val = pce->M2.لدو();
				m2_val += delta_intvl * (dt - اوسط_val);
				pce->M2.store(m2_val);

				pce->ویلا_شروع.store((uint64_t)0);
			}
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_گنتی_interval(perf_counter_t handle, hrt_abstime now)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			uint64_t ec = pci->event_گنتی.لدو();

			بدل (ec) {
			حالة 0:
				pci->ویلا_پہلا.store(now);
				اكسر;

			حالة 1: {
					uint64_t tl = pci->ویلا_آخری.لدو();
					uint32_t interval = (uint32_t)(now - tl);
					pci->ویلا_گھٹ_توں_گھٹ.store(interval);
					pci->ویلا_most.store(interval);
					pci->اوسط.store(interval / 1e6f);
					pci->M2.store(0.0f);
					اكسر;
				}

			افتراضي: {
					uint64_t tl = pci->ویلا_آخری.لدو();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->ویلا_گھٹ_توں_گھٹ.لدو();

					إذا ((uint32_t)interval < least) {
						pci->ویلا_گھٹ_توں_گھٹ.store((uint32_t)interval);
					}

					uint32_t most = pci->ویلا_most.لدو();

					إذا ((uint32_t)interval > most) {
						pci->ویلا_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					عائم dt = interval / 1e6f;
					عائم اوسط_val = pci->اوسط.لدو();
					عائم delta_intvl = dt - اوسط_val;
					اوسط_val += delta_intvl / ec;
					pci->اوسط.store(اوسط_val);
					عائم m2_val = pci->M2.لدو();
					m2_val += delta_intvl * (dt - اوسط_val);
					pci->M2.store(m2_val);
					اكسر;
				}
			}

			pci->ویلا_آخری.store(now);
			pci->event_گنتی.fetch_جوڑو(1);
			اكسر;
		}

	افتراضي:
		اكسر;
	}
}

فراغ
perf_سیٹ_گنتی(perf_counter_t handle, uint64_t گنتی)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_گنتی: {
			((بنية perf_ctr_گنتی *)handle)->event_گنتی.store(گنتی);
		}
		اكسر;

	افتراضي:
		اكسر;
	}

}

فراغ
perf_cancel(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			pce->ویلا_شروع.store((uint64_t)0);
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_reset(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_گنتی:
		((بنية perf_ctr_گنتی *)handle)->event_گنتی.store((uint64_t)0);
		اكسر;

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			pce->event_گنتی.store((uint64_t)0);
			pce->ویلا_شروع.store((uint64_t)0);
			pce->ویلا_کل.store((uint64_t)0);
			pce->ویلا_گھٹ_توں_گھٹ.store((uint32_t)0);
			pce->ویلا_most.store((uint32_t)0);
			pce->اوسط.store(0.0f);
			pce->M2.store(0.0f);
			اكسر;
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			pci->event_گنتی.store((uint64_t)0);
			pci->ویلا_event.store((uint64_t)0);
			pci->ویلا_پہلا.store((uint64_t)0);
			pci->ویلا_آخری.store((uint64_t)0);
			pci->ویلا_گھٹ_توں_گھٹ.store((uint32_t)0);
			pci->ویلا_most.store((uint32_t)0);
			pci->اوسط.store(0.0f);
			pci->M2.store(0.0f);
			اكسر;
		}
	}
}

فراغ
perf_print_counter(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_گنتی: {
			uint64_t ec = ((بنية perf_ctr_گنتی *)handle)->event_گنتی.لدو();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			اكسر;
		}

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_گنتی.لدو();
			uint64_t tt = pce->ویلا_کل.لدو();
			uint32_t tl = pce->ویلا_گھٹ_توں_گھٹ.لدو();
			uint32_t tm = pce->ویلا_most.لدو();
			عائم m2 = pce->M2.لدو();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (مزدوج)tt / (مزدوج)ec,
				     tl, tm, (مزدوج)(1e6f * rms));
			اكسر;
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			uint64_t ec = pci->event_گنتی.لدو();
			uint64_t tf = pci->ویلا_پہلا.لدو();
			uint64_t tla = pci->ویلا_آخری.لدو();
			uint32_t tl = pci->ویلا_گھٹ_توں_گھٹ.لدو();
			uint32_t tm = pci->ویلا_most.لدو();
			عائم m2 = pci->M2.لدو();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (مزدوج)(tla - tf) / (مزدوج)ec,
				     tl, tm, (مزدوج)(1e6f * rms));
			اكسر;
		}

	افتراضي:
		اكسر;
	}
}


صحيح
perf_print_counter_بفر(حرف *بفر_2, صحيح لمائی, perf_counter_t handle)
{
	صحيح عدد_written = 0;

	إذا (handle == خالی_سوچک) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_گنتی:
		عدد_written = snprintf(بفر_2, لمائی, "%s: %" PRIu64 " events",
				       handle->name,
				       ((بنية perf_ctr_گنتی *)handle)->event_گنتی.لدو());
		اكسر;

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_گنتی.لدو();
			uint64_t tt = pce->ویلا_کل.لدو();
			uint32_t tl = pce->ویلا_گھٹ_توں_گھٹ.لدو();
			uint32_t tm = pce->ویلا_most.لدو();
			عائم m2 = pce->M2.لدو();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			عدد_written = snprintf(بفر_2, لمائی,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (مزدوج)tt / (مزدوج)ec,
					       tl,
					       tm,
					       (مزدوج)(1e6f * rms));
			اكسر;
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			uint64_t ec = pci->event_گنتی.لدو();
			uint32_t tl = pci->ویلا_گھٹ_توں_گھٹ.لدو();
			uint32_t tm = pci->ویلا_most.لدو();
			عائم m2 = pci->M2.لدو();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			عدد_written = snprintf(بفر_2, لمائی,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (مزدوج)(pci->ویلا_آخری.لدو() - pci->ویلا_پہلا.لدو()) / (مزدوج)ec,
					       tl,
					       tm,
					       (مزدوج)(1e6f * rms));
			اكسر;
		}

	افتراضي:
		اكسر;
	}

	بفر_2[لمائی - 1] = 0; // ensure 0-termination
	أرجع عدد_written;
}

uint64_t
perf_event_گنتی(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_گنتی:
		أرجع ((بنية perf_ctr_گنتی *)handle)->event_گنتی.لدو();

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			أرجع pce->event_گنتی.لدو();
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			أرجع pci->event_گنتی.لدو();
		}

	افتراضي:
		اكسر;
	}

	أرجع 0;
}

عائم
perf_اوسط(perf_counter_t handle)
{
	إذا (handle == خالی_سوچک) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			أرجع pce->اوسط.لدو();
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			أرجع pci->اوسط.لدو();
		}

	افتراضي:
		اكسر;
	}

	أرجع 0.0f;
}

فراغ
perf_iterate_all(perf_callback cb, فراغ *user)
{
	ساكن constexpr size_t PERF_COUNTER_سطر_بفر_ناپ = 220;
	حرف بفر_2[PERF_COUNTER_سطر_بفر_ناپ];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_سرگرم;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != خالی_سوچک) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t اگلا = (perf_counter_t)sq_اگلا(&handle->link);

		إذا (!handle->pending_free) {
			perf_print_counter_بفر(بفر_2, حجم(بفر_2), handle);

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

	إذا (--perf_iterate_سرگرم == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

فراغ
perf_print_all(فراغ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != خالی_سوچک) {
		إذا (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_اگلا(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

فراغ
perf_print_latency(فراغ)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	لكل (صحيح i = 0; i < get_latency_بالٹی_گنتی(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.بالٹی, (لما صحيح)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_بالٹی_گنتی() - 1, get_latency_بالٹی_گنتی());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.بالٹی, latency.counter);
}

فراغ
perf_reset_all(فراغ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != خالی_سوچک) {
		إذا (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_اگلا(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
