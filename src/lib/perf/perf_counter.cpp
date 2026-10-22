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
سانچو <قسم_نالو T> استعمال PerfVar = px4::atomic<T>;
#وإلا
سانچو <قسم_نالو T>
صنف PerfVar
{
عام:
	PerfVar() = افتراضي;
	T ڀريو() ثابت { أرجع قدر_2; }
	فراغ store(T value) { قدر_2 = value; }
	T fetch_جوڙيو(T value) { ثابت T اڳيون = قدر_2; قدر_2 += value; أرجع اڳيون; }
خاص:
	T قدر_2 {};
};
#جيڪڏهن_پورو

/**
 * Header common to all counters.
 */
بنية perf_ctr_سرو {
	sq_داخلا_t		link;	/**< list linkage */
	تعداد perf_counter_type	type;	/**< counter type */
	منطقي			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ثابت حرف		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
بنية perf_ctr_ڳڻپ : عام perf_ctr_سرو {
	PerfVar<uint64_t>	event_ڳڻپ;
};

/**
 * PC_ELAPSED counter.
 */
بنية perf_ctr_elapsed : عام perf_ctr_سرو {
	PerfVar<uint64_t>	event_ڳڻپ;
	PerfVar<uint64_t>	وقت_شروعات;
	PerfVar<uint64_t>	وقت_ڪل;
	PerfVar<uint32_t>	وقت_گهٽ_کان_گهٽ;
	PerfVar<uint32_t>	وقت_most;
	PerfVar<عائم>	سراسري;
	PerfVar<عائم>	M2;
};

/**
 * PC_INTERVAL counter.
 */
بنية perf_ctr_interval : عام perf_ctr_سرو {
	PerfVar<uint64_t>	event_ڳڻپ;
	PerfVar<uint64_t>	وقت_event;
	PerfVar<uint64_t>	وقت_پهريون;
	PerfVar<uint64_t>	وقت_آخري;
	PerfVar<uint32_t>	وقت_گهٽ_کان_گهٽ;
	PerfVar<uint32_t>	وقت_most;
	PerfVar<عائم>	سراسري;
	PerfVar<عائم>	M2;
};

/**
 * List of all known counters.
 */
ساكن sq_قطار_t	perf_counters = { خالي_اشارو, خالي_اشارو };

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
	حالة PC_ڳڻپ:
		ميٽيو (بنية perf_ctr_ڳڻپ *)handle;
		اكسر;

	حالة PC_ELAPSED:
		ميٽيو (بنية perf_ctr_elapsed *)handle;
		اكسر;

	حالة PC_INTERVAL:
		ميٽيو (بنية perf_ctr_interval *)handle;
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

	طالما (handle != خالي_اشارو) {
		perf_counter_t ايندڙ = (perf_counter_t)sq_ايندڙ(&handle->link);

		إذا (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = ايندڙ;
	}
}

perf_counter_t
perf_alloc(تعداد perf_counter_type type, ثابت حرف *name)
{
	perf_counter_t ctr = خالي_اشارو;

	بدل (type) {
	حالة PC_ڳڻپ:
		ctr = جديد perf_ctr_ڳڻپ();
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

	إذا (ctr != خالي_اشارو) {
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

	طالما (handle != خالي_اشارو) {
		إذا (!handle->pending_free && !strcmp(handle->name, name)) {
			إذا (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				أرجع handle;

			} وإلا {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				أرجع خالي_اشارو;
			}
		}

		handle = (perf_counter_t)sq_ايندڙ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	أرجع perf_alloc(type, name);
}

فراغ
perf_free(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
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
perf_ڳڻپ(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ڳڻپ:
		((بنية perf_ctr_ڳڻپ *)handle)->event_ڳڻپ.fetch_جوڙيو(1);
		اكسر;

	حالة PC_INTERVAL:
		perf_ڳڻپ_interval(handle, hrt_absolute_وقت());
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_begin(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED:
		((بنية perf_ctr_elapsed *)handle)->وقت_شروعات.store(hrt_absolute_وقت());
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_پڇاڙي(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->وقت_شروعات.ڀريو();

			إذا (ts != 0) {
				perf_سيٽ_elapsed(handle, hrt_absolute_وقت() - ts);
			}
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_سيٽ_elapsed(perf_counter_t handle, int64_t elapsed)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;

			إذا (elapsed >= 0) {
				uint64_t ec = pce->event_ڳڻپ.fetch_جوڙيو(1) + 1;
				pce->وقت_ڪل.fetch_جوڙيو((uint64_t)elapsed);

				uint32_t tl = pce->وقت_گهٽ_کان_گهٽ.ڀريو();

				إذا ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->وقت_گهٽ_کان_گهٽ.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->وقت_most.ڀريو();

				إذا (tm < (uint32_t)elapsed) {
					pce->وقت_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				عائم dt = elapsed / 1e6f;
				عائم سراسري_val = pce->سراسري.ڀريو();
				عائم delta_intvl = dt - سراسري_val;
				سراسري_val += delta_intvl / ec;
				pce->سراسري.store(سراسري_val);
				عائم m2_val = pce->M2.ڀريو();
				m2_val += delta_intvl * (dt - سراسري_val);
				pce->M2.store(m2_val);

				pce->وقت_شروعات.store((uint64_t)0);
			}
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_ڳڻپ_interval(perf_counter_t handle, hrt_abstime now)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			uint64_t ec = pci->event_ڳڻپ.ڀريو();

			بدل (ec) {
			حالة 0:
				pci->وقت_پهريون.store(now);
				اكسر;

			حالة 1: {
					uint64_t tl = pci->وقت_آخري.ڀريو();
					uint32_t interval = (uint32_t)(now - tl);
					pci->وقت_گهٽ_کان_گهٽ.store(interval);
					pci->وقت_most.store(interval);
					pci->سراسري.store(interval / 1e6f);
					pci->M2.store(0.0f);
					اكسر;
				}

			افتراضي: {
					uint64_t tl = pci->وقت_آخري.ڀريو();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->وقت_گهٽ_کان_گهٽ.ڀريو();

					إذا ((uint32_t)interval < least) {
						pci->وقت_گهٽ_کان_گهٽ.store((uint32_t)interval);
					}

					uint32_t most = pci->وقت_most.ڀريو();

					إذا ((uint32_t)interval > most) {
						pci->وقت_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					عائم dt = interval / 1e6f;
					عائم سراسري_val = pci->سراسري.ڀريو();
					عائم delta_intvl = dt - سراسري_val;
					سراسري_val += delta_intvl / ec;
					pci->سراسري.store(سراسري_val);
					عائم m2_val = pci->M2.ڀريو();
					m2_val += delta_intvl * (dt - سراسري_val);
					pci->M2.store(m2_val);
					اكسر;
				}
			}

			pci->وقت_آخري.store(now);
			pci->event_ڳڻپ.fetch_جوڙيو(1);
			اكسر;
		}

	افتراضي:
		اكسر;
	}
}

فراغ
perf_سيٽ_ڳڻپ(perf_counter_t handle, uint64_t ڳڻپ)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ڳڻپ: {
			((بنية perf_ctr_ڳڻپ *)handle)->event_ڳڻپ.store(ڳڻپ);
		}
		اكسر;

	افتراضي:
		اكسر;
	}

}

فراغ
perf_cancel(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			pce->وقت_شروعات.store((uint64_t)0);
		}
		اكسر;

	افتراضي:
		اكسر;
	}
}

فراغ
perf_reset(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ڳڻپ:
		((بنية perf_ctr_ڳڻپ *)handle)->event_ڳڻپ.store((uint64_t)0);
		اكسر;

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			pce->event_ڳڻپ.store((uint64_t)0);
			pce->وقت_شروعات.store((uint64_t)0);
			pce->وقت_ڪل.store((uint64_t)0);
			pce->وقت_گهٽ_کان_گهٽ.store((uint32_t)0);
			pce->وقت_most.store((uint32_t)0);
			pce->سراسري.store(0.0f);
			pce->M2.store(0.0f);
			اكسر;
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			pci->event_ڳڻپ.store((uint64_t)0);
			pci->وقت_event.store((uint64_t)0);
			pci->وقت_پهريون.store((uint64_t)0);
			pci->وقت_آخري.store((uint64_t)0);
			pci->وقت_گهٽ_کان_گهٽ.store((uint32_t)0);
			pci->وقت_most.store((uint32_t)0);
			pci->سراسري.store(0.0f);
			pci->M2.store(0.0f);
			اكسر;
		}
	}
}

فراغ
perf_print_counter(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
		أرجع;
	}

	بدل (handle->type) {
	حالة PC_ڳڻپ: {
			uint64_t ec = ((بنية perf_ctr_ڳڻپ *)handle)->event_ڳڻپ.ڀريو();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			اكسر;
		}

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ڳڻپ.ڀريو();
			uint64_t tt = pce->وقت_ڪل.ڀريو();
			uint32_t tl = pce->وقت_گهٽ_کان_گهٽ.ڀريو();
			uint32_t tm = pce->وقت_most.ڀريو();
			عائم m2 = pce->M2.ڀريو();
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
			uint64_t ec = pci->event_ڳڻپ.ڀريو();
			uint64_t tf = pci->وقت_پهريون.ڀريو();
			uint64_t tla = pci->وقت_آخري.ڀريو();
			uint32_t tl = pci->وقت_گهٽ_کان_گهٽ.ڀريو();
			uint32_t tm = pci->وقت_most.ڀريو();
			عائم m2 = pci->M2.ڀريو();
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
perf_print_counter_بفر(حرف *بفر_2, صحيح ڊيگهائي, perf_counter_t handle)
{
	صحيح انگ_written = 0;

	إذا (handle == خالي_اشارو) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_ڳڻپ:
		انگ_written = snprintf(بفر_2, ڊيگهائي, "%s: %" PRIu64 " events",
				       handle->name,
				       ((بنية perf_ctr_ڳڻپ *)handle)->event_ڳڻپ.ڀريو());
		اكسر;

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_ڳڻپ.ڀريو();
			uint64_t tt = pce->وقت_ڪل.ڀريو();
			uint32_t tl = pce->وقت_گهٽ_کان_گهٽ.ڀريو();
			uint32_t tm = pce->وقت_most.ڀريو();
			عائم m2 = pce->M2.ڀريو();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			انگ_written = snprintf(بفر_2, ڊيگهائي,
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
			uint64_t ec = pci->event_ڳڻپ.ڀريو();
			uint32_t tl = pci->وقت_گهٽ_کان_گهٽ.ڀريو();
			uint32_t tm = pci->وقت_most.ڀريو();
			عائم m2 = pci->M2.ڀريو();
			عائم rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			انگ_written = snprintf(بفر_2, ڊيگهائي,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (مزدوج)(pci->وقت_آخري.ڀريو() - pci->وقت_پهريون.ڀريو()) / (مزدوج)ec,
					       tl,
					       tm,
					       (مزدوج)(1e6f * rms));
			اكسر;
		}

	افتراضي:
		اكسر;
	}

	بفر_2[ڊيگهائي - 1] = 0; // ensure 0-termination
	أرجع انگ_written;
}

uint64_t
perf_event_ڳڻپ(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_ڳڻپ:
		أرجع ((بنية perf_ctr_ڳڻپ *)handle)->event_ڳڻپ.ڀريو();

	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			أرجع pce->event_ڳڻپ.ڀريو();
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			أرجع pci->event_ڳڻپ.ڀريو();
		}

	افتراضي:
		اكسر;
	}

	أرجع 0;
}

عائم
perf_سراسري(perf_counter_t handle)
{
	إذا (handle == خالي_اشارو) {
		أرجع 0;
	}

	بدل (handle->type) {
	حالة PC_ELAPSED: {
			بنية perf_ctr_elapsed *pce = (بنية perf_ctr_elapsed *)handle;
			أرجع pce->سراسري.ڀريو();
		}

	حالة PC_INTERVAL: {
			بنية perf_ctr_interval *pci = (بنية perf_ctr_interval *)handle;
			أرجع pci->سراسري.ڀريو();
		}

	افتراضي:
		اكسر;
	}

	أرجع 0.0f;
}

فراغ
perf_iterate_all(perf_callback cb, فراغ *user)
{
	ساكن constexpr size_t PERF_COUNTER_سٽ_بفر_سائيز = 220;
	حرف بفر_2[PERF_COUNTER_سٽ_بفر_سائيز];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_سرگرم;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != خالي_اشارو) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t ايندڙ = (perf_counter_t)sq_ايندڙ(&handle->link);

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

		handle = ايندڙ;
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

	طالما (handle != خالي_اشارو) {
		إذا (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_ايندڙ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

فراغ
perf_print_latency(فراغ)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	لكل (صحيح i = 0; i < get_latency_ٻالٽي_ڳڻپ(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.ٻالٽي, (ڊگهو صحيح)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_ٻالٽي_ڳڻپ() - 1, get_latency_ٻالٽي_ڳڻپ());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.ٻالٽي, latency.counter);
}

فراغ
perf_reset_all(فراغ)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	طالما (handle != خالي_اشارو) {
		إذا (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_ايندڙ(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
