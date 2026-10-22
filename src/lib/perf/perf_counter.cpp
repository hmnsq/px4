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
کينډۍ <ډول_نوم T> کارول PerfVar = px4::atomic<T>;
#وإلا
کينډۍ <ډول_نوم T>
صنف PerfVar
{
عام:
	PerfVar() = افتراضي;
	T بارول() ثابت { أرجع ارزښت_2; }
	فراغ store(T value) { ارزښت_2 = value; }
	T fetch_جمع_کړه(T value) { ثابت T پخوانی = ارزښت_2; ارزښت_2 += value; أرجع پخوانی; }
خاص:
	T ارزښت_2 {};
};
#د_که_پای

/**
 * Header common to all counters.
 */
بنية perf_ctr_سرلیک {
	sq_ثبت_t		link;	/**< list linkage */
	تعداد perf_counter_type	type;	/**< counter type */
	منطقي			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ثابت حرف		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
بنية perf_ctr_شمېر : عام perf_ctr_سرلیک {
	PerfVar<uint64_t>	event_شمېر;
};

/**
 * PC_ELAPSED counter.
 */
بنية perf_ctr_elapsed : عام perf_ctr_سرلیک {
	PerfVar<uint64_t>	event_شمېر;
	PerfVar<uint64_t>	وخت_پیل;
	PerfVar<uint64_t>	وخت_ټول;
	PerfVar<uint32_t>	وخت_لږترلږه;
	PerfVar<uint32_t>	وخت_most;
	PerfVar<عائم>	منځنۍ;
	PerfVar<عائم>	M2;
};

/**
 * PC_INTERVAL counter.
 */
بنية perf_ctr_interval : عام perf_ctr_سرلیک {
	PerfVar<uint64_t>	event_شمېر;
	PerfVar<uint64_t>	وخت_event;
	PerfVar<uint64_t>	وخت_لومړی;
	PerfVar<uint64_t>	وخت_وروستی;
	PerfVar<uint32_t>	وخت_لږترلږه;
	PerfVar<uint32_t>	وخت_most;
	PerfVar<عائم>	منځنۍ;
	PerfVar<عائم>	M2;
};

/**
 * List of all known counters.
 */
ساكن sq_کتار_t	perf_counters = { تش_اشاره, تش_اشاره };

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
ساكن صحيح perf_iterate_فعال = 0;

ساكن فراغ
perf_delete(perf_counter_t handle)
{
	بدل (handle->type) {
	حالة PC_شمېر:
		ړنګول (بنية perf_ctr_شمېر *)handle;
		اكسر;

	حالة PC_ELAPSED:
		ړنګول (بنية perf_ctr_elapsed *)handle;
		اكسر;

	حالة PC_INTERVAL:
		ړنګول (بنية perf_ctr_interval *)handle;
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

	طالما (handle != تش_اشاره) {
		perf_counter_t بل = (perf_counter_t)sq_بل(&handle->link);

		إذا (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = بل;
	}
}

perf_counter_t
perf_alloc(تعداد perf_counter_type type, ثابت حرف *name)
{
	perf_counter_t ctr = تش_اشاره;

	بدل (type) {
	حالة PC_شمېر:
		ctr = جديد perf_ctr_شمېر();
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

	إذا (ctr != تش_اشاره) {
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

	طالما (handle != تش_اشاره) {
		إذا (!handle->pending_free && !strcmp(handle->name, name)) {
			إذا (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				أرجع handle;

			} وإلا {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				أرجع تش_اشاره;
			}
		}

		handle = (perf_counter_t)sq_بل(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	أرجع perf_alloc(type, name);
}

فراغ
perf_free(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	إذا (perf_iterate_فعال > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = ریښتیا;
		pthread_mutex_unlock(&perf_counters_mutex);
		أرجع;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

فراغ
perf_شمېر(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_شمېر:
		((بنية perf_ctr_شمېر *)handle)->event_شمېر.fetch_جمع_کړه(1);
		اكسر;

	حالة PC_INTERVAL:
		perf_شمېر_interval(handle, hrt_absolute_وخت());
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_begin(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED:
		((بنية perf_ctr_elapsed *)handle)->وخت_پیل.store(hrt_absolute_وخت());
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_پای_ټکی(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->وخت_پیل.بارول();

			إذا (ts != 0) {
				perf_سېټ_elapsed(handle, hrt_absolute_وخت() - ts);
			}
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_سېټ_elapsed(perf_counter_t handle, int64_t elapsed)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;

			إذا (elapsed >= 0) {
				uint64_t ec = pce->event_شمېر.fetch_جمع_کړه(1) + 1;
				pce->وخت_ټول.fetch_جمع_کړه((uint64_t)elapsed);

				uint32_t tl = pce->وخت_لږترلږه.بارول();

				إذا ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->وخت_لږترلږه.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->وخت_most.بارول();

				إذا (tm < (uint32_t)elapsed) {
					pce->وخت_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				عائم dt = elapsed / 1e6f;
				عائم منځنۍ_val = pce->منځنۍ.بارول();
				عائم delta_intvl = dt - منځنۍ_val;
				منځنۍ_val += delta_intvl / ec;
				pce->منځنۍ.store(منځنۍ_val);
				عائم m2_val = pce->M2.بارول();
				m2_val += delta_intvl * (dt - منځنۍ_val);
				pce->M2.store(m2_val);

				pce->وخت_پیل.store((uint64_t)0);
			}
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_شمېر_interval(perf_counter_t handle, hrt_abstime now)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			uint64_t ec = pci->event_شمېر.بارول();

			بدل (ec) {
			حالة 0:
				pci->وخت_لومړی.store(now);
				اكسر;

			حالة 1: {
					uint64_t tl = pci->وخت_وروستی.بارول();
					uint32_t interval = (uint32_t)(now - tl);
					pci->وخت_لږترلږه.store(interval);
					pci->وخت_most.store(interval);
					pci->منځنۍ.store(interval / 1e6f);
					pci->M2.store(0.0f);
					اكسر;
				}

			افتراضي: {
					uint64_t tl = pci->وخت_وروستی.بارول();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->وخت_لږترلږه.بارول();

					إذا ((uint32_t)interval < least) {
						pci->وخت_لږترلږه.store((uint32_t)interval);
					}

					uint32_t most = pci->وخت_most.بارول();

					إذا ((uint32_t)interval > most) {
						pci->وخت_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					عائم dt = interval / 1e6f;
					عائم منځنۍ_val = pci->منځنۍ.بارول();
					عائم delta_intvl = dt - منځنۍ_val;
					منځنۍ_val += delta_intvl / ec;
					pci->منځنۍ.store(منځنۍ_val);
					عائم m2_val = pci->M2.بارول();
					m2_val += delta_intvl * (dt - منځنۍ_val);
					pci->M2.store(m2_val);
					اكسر;
				}
			}

			pci->وخت_وروستی.store(now);
			pci->event_شمېر.fetch_جمع_کړه(1);
			اكسر;
		}

	افتراضي:
		اكسر;
	}
}

فراغ
perf_سېټ_شمېر(perf_counter_t handle, uint64_t شمېر)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_شمېر: {
			((بنية perf_ctr_شمېر *)handle)->event_شمېر.store(شمېر);
		}
		اكسر;

	افتراضي:
		اكسر;
	}

}

فراغ
perf_cancel(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			pce->وخت_پیل.store((uint64_t)0);
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_reset(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_شمېر:
		((بنية perf_ctr_شمېر *)handle)->event_شمېر.store((uint64_t)0);
		اكسر;

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			pce->event_شمېر.store((uint64_t)0);
			pce->وخت_پیل.store((uint64_t)0);
			pce->وخت_ټول.store((uint64_t)0);
			pce->وخت_لږترلږه.store((uint32_t)0);
			pce->وخت_most.store((uint32_t)0);
			pce->منځنۍ.store(0.0f);
			pce->M2.store(0.0f);
			اكسر;
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			pci->event_شمېر.store((uint64_t)0);
			pci->وخت_event.store((uint64_t)0);
			pci->وخت_لومړی.store((uint64_t)0);
			pci->وخت_وروستی.store((uint64_t)0);
			pci->وخت_لږترلږه.store((uint32_t)0);
			pci->وخت_most.store((uint32_t)0);
			pci->منځنۍ.store(0.0f);
			pci->M2.store(0.0f);
			اكسر;
		}
	}
}

فراغ
perf_print_counter(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_شمېر: {
			uint64_t ec = ((بنية perf_ctr_شمېر *)handle)->event_شمېر.بارول();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			اكسر;
		}

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_شمېر.بارول();
			uint64_t tt = pce->وخت_ټول.بارول();
			uint32_t tl = pce->وخت_لږترلږه.بارول();
			uint32_t tm = pce->وخت_most.بارول();
			عائم m2 = pce->M2.بارول();
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
			uint64_t ec = pci->event_شمېر.بارول();
			uint64_t tf = pci->وخت_لومړی.بارول();
			uint64_t tla = pci->وخت_وروستی.بارول();
			uint32_t tl = pci->وخت_لږترلږه.بارول();
			uint32_t tm = pci->وخت_most.بارول();
			عائم m2 = pci->M2.بارول();
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
perf_print_counter_بفر(حرف *بفر_2, صحيح اوږدتیا, perf_counter_t handle)
{
	صحيح عدد_written = 0;

	إذا (handle == تش_اشاره) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_شمېر:
		عدد_written = snprintf(بفر_2, اوږدتیا, "%s: %" PRIu64 " events",
				       handle->name,
				       ((بنية perf_ctr_شمېر *)handle)->event_شمېر.بارول());
		اكسر;

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_شمېر.بارول();
			uint64_t tt = pce->وخت_ټول.بارول();
			uint32_t tl = pce->وخت_لږترلږه.بارول();
			uint32_t tm = pce->وخت_most.بارول();
			عائم m2 = pce->M2.بارول();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			عدد_written = snprintf(بفر_2, اوږدتیا,
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
			uint64_t ec = pci->event_شمېر.بارول();
			uint32_t tl = pci->وخت_لږترلږه.بارول();
			uint32_t tm = pci->وخت_most.بارول();
			عائم m2 = pci->M2.بارول();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			عدد_written = snprintf(بفر_2, اوږدتیا,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (مزدوج)(pci->وخت_وروستی.بارول() - pci->وخت_لومړی.بارول()) / (مزدوج)ec,
					       tl,
					       tm,
					       (مزدوج)(1e6f * rms));
			اكسر;
		}

	افتراضي:
		اكسر;
	}

	بفر_2[اوږدتیا - 1] = 0; // ensure 0-termination
	أرجع عدد_written;
}

uint64_t
perf_event_شمېر(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_شمېر:
		أرجع ((بنية perf_ctr_شمېر *)handle)->event_شمېر.بارول();

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			أرجع pce->event_شمېر.بارول();
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			أرجع pci->event_شمېر.بارول();
		}

	افتراضي:
		اكسر;
	}

	أرجع 0;
}

عائم
perf_منځنۍ(perf_counter_t handle)
{
	إذا (handle == تش_اشاره) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			أرجع pce->منځنۍ.بارول();
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			أرجع pci->منځنۍ.بارول();
		}

	افتراضي:
		اكسر;
	}

	أرجع 0.0f;
}

فراغ
perf_iterate_all(perf_callback cb, فراغ *user)
{
	ساكن constexpr size_t PERF_COUNTER_کرښه_بفر_اندازه = 220;
	حرف بفر_2[PERF_COUNTER_کرښه_بفر_اندازه];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_فعال;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != تش_اشاره) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t بل = (perf_counter_t)sq_بل(&handle->link);

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

		handle = بل;
	}

	إذا (--perf_iterate_فعال == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

فراغ
perf_print_all(فراغ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != تش_اشاره) {
		إذا (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_بل(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

فراغ
perf_print_latency(فراغ)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	لكل (صحيح i = 0; i < get_latency_لوښی_شمېر(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.لوښی, (اوږد صحيح)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_لوښی_شمېر() - 1, get_latency_لوښی_شمېر());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.لوښی, latency.counter);
}

فراغ
perf_reset_all(فراغ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != تش_اشاره) {
		إذا (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_بل(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
