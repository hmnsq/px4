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

#అన్తర్భావ <inttypes.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <math.h>
#అన్తర్భావ <pthread.h>
#అన్తర్భావ <systemlib/err.h>

#అన్తర్భావ "perf_counter.h"

#అన్తర్భావ <px4_platform_common/atomic.h>

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
#ఒకవేళ defined(__PX4_POSIX) || defined(__SANITIZE_THREAD__)
మూస <రకం_పేరు T> ఉపయోగించి PerfVar = px4::atomic<T>;
#లేకపోతే
మూస <రకం_పేరు T>
తరగతి PerfVar
{
బహిరంగ:
	PerfVar() = అప్రమేయ;
	T ఎక్కించు() స్థిరరాశి { తిరిగి విలువ_2; }
	శూన్య store(T value) { విలువ_2 = value; }
	T fetch_కూడు(T value) { స్థిరరాశి T మునుపటి = విలువ_2; విలువ_2 += value; తిరిగి మునుపటి; }
అంతరంగ:
	T విలువ_2 {};
};
#అయితే_ముగింపు

/**
 * Header common to all counters.
 */
నిర్మాణం perf_ctr_శీర్షిక {
	sq_నమోదు_t		link;	/**< list linkage */
	గణన perf_counter_type	type;	/**< counter type */
	తార్కిక			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	స్థిరరాశి అక్షరం		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
నిర్మాణం perf_ctr_లెక్క : బహిరంగ perf_ctr_శీర్షిక {
	PerfVar<uint64_t>	event_లెక్క;
};

/**
 * PC_ELAPSED counter.
 */
నిర్మాణం perf_ctr_elapsed : బహిరంగ perf_ctr_శీర్షిక {
	PerfVar<uint64_t>	event_లెక్క;
	PerfVar<uint64_t>	సమయం_ప్రారంభం;
	PerfVar<uint64_t>	సమయం_మొత్తం;
	PerfVar<uint32_t>	సమయం_కనిష్ఠ;
	PerfVar<uint32_t>	సమయం_most;
	PerfVar<దశాంశం>	సగటు;
	PerfVar<దశాంశం>	M2;
};

/**
 * PC_INTERVAL counter.
 */
నిర్మాణం perf_ctr_interval : బహిరంగ perf_ctr_శీర్షిక {
	PerfVar<uint64_t>	event_లెక్క;
	PerfVar<uint64_t>	సమయం_event;
	PerfVar<uint64_t>	సమయం_మొదటి;
	PerfVar<uint64_t>	సమయం_చివరి;
	PerfVar<uint32_t>	సమయం_కనిష్ఠ;
	PerfVar<uint32_t>	సమయం_most;
	PerfVar<దశాంశం>	సగటు;
	PerfVar<దశాంశం>	M2;
};

/**
 * List of all known counters.
 */
స్థిర sq_వరుస_t	perf_counters = { శూన్యసూచిక, శూన్యసూచిక };

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
స్థిర పూర్ణాంకం perf_iterate_చురుకైన = 0;

స్థిర శూన్య
perf_delete(perf_counter_t handle)
{
	ఎంపిక (handle->type) {
	స్థితి PC_లెక్క:
		తొలగించు (నిర్మాణం perf_ctr_లెక్క *)handle;
		ఆపు;

	స్థితి PC_ELAPSED:
		తొలగించు (నిర్మాణం perf_ctr_elapsed *)handle;
		ఆపు;

	స్థితి PC_INTERVAL:
		తొలగించు (నిర్మాణం perf_ctr_interval *)handle;
		ఆపు;

	అప్రమేయ:
		ఆపు;
	}
}

/**
 * Delete all entries marked pending_free. Call with perf_counters_mutex held
 * and no iteration active.
 */
స్థిర శూన్య
perf_free_pending_locked()
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	అయితే (handle != శూన్యసూచిక) {
		perf_counter_t తదుపరి = (perf_counter_t)sq_తదుపరి(&handle->link);

		ఒకవేళ (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = తదుపరి;
	}
}

perf_counter_t
perf_alloc(గణన perf_counter_type type, స్థిరరాశి అక్షరం *name)
{
	perf_counter_t ctr = శూన్యసూచిక;

	ఎంపిక (type) {
	స్థితి PC_లెక్క:
		ctr = కొత్త perf_ctr_లెక్క();
		ఆపు;

	స్థితి PC_ELAPSED:
		ctr = కొత్త perf_ctr_elapsed();
		ఆపు;

	స్థితి PC_INTERVAL:
		ctr = కొత్త perf_ctr_interval();
		ఆపు;

	అప్రమేయ:
		ఆపు;
	}

	ఒకవేళ (ctr != శూన్యసూచిక) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = అబద్ధం;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	తిరిగి ctr;
}

perf_counter_t
perf_alloc_once(గణన perf_counter_type type, స్థిరరాశి అక్షరం *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	అయితే (handle != శూన్యసూచిక) {
		ఒకవేళ (!handle->pending_free && !strcmp(handle->name, name)) {
			ఒకవేళ (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				తిరిగి handle;

			} లేకపోతే {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				తిరిగి శూన్యసూచిక;
			}
		}

		handle = (perf_counter_t)sq_తదుపరి(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	తిరిగి perf_alloc(type, name);
}

శూన్య
perf_free(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	ఒకవేళ (perf_iterate_చురుకైన > 0) {
		// an iteration may be positioned on this entry: keep it linked
		// and let the last active iteration delete it
		handle->pending_free = నిజం;
		pthread_mutex_unlock(&perf_counters_mutex);
		తిరిగి;
	}

	sq_rem(&handle->link, &perf_counters);
	pthread_mutex_unlock(&perf_counters_mutex);

	perf_delete(handle);
}

శూన్య
perf_లెక్క(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_లెక్క:
		((నిర్మాణం perf_ctr_లెక్క *)handle)->event_లెక్క.fetch_కూడు(1);
		ఆపు;

	స్థితి PC_INTERVAL:
		perf_లెక్క_interval(handle, hrt_absolute_సమయం());
		ఆపు;

	అప్రమేయ:
		ఆపు;
	}
}

శూన్య
perf_begin(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_ELAPSED:
		((నిర్మాణం perf_ctr_elapsed *)handle)->సమయం_ప్రారంభం.store(hrt_absolute_సమయం());
		ఆపు;

	అప్రమేయ:
		ఆపు;
	}
}

శూన్య
perf_చివర(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_ELAPSED: {
			నిర్మాణం perf_ctr_elapsed *pce = (నిర్మాణం perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->సమయం_ప్రారంభం.ఎక్కించు();

			ఒకవేళ (ts != 0) {
				perf_సమితి_elapsed(handle, hrt_absolute_సమయం() - ts);
			}
		}
		ఆపు;

	అప్రమేయ:
		ఆపు;
	}
}

శూన్య
perf_సమితి_elapsed(perf_counter_t handle, int64_t elapsed)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_ELAPSED: {
			నిర్మాణం perf_ctr_elapsed *pce = (నిర్మాణం perf_ctr_elapsed *)handle;

			ఒకవేళ (elapsed >= 0) {
				uint64_t ec = pce->event_లెక్క.fetch_కూడు(1) + 1;
				pce->సమయం_మొత్తం.fetch_కూడు((uint64_t)elapsed);

				uint32_t tl = pce->సమయం_కనిష్ఠ.ఎక్కించు();

				ఒకవేళ ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->సమయం_కనిష్ఠ.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->సమయం_most.ఎక్కించు();

				ఒకవేళ (tm < (uint32_t)elapsed) {
					pce->సమయం_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				దశాంశం dt = elapsed / 1e6f;
				దశాంశం సగటు_val = pce->సగటు.ఎక్కించు();
				దశాంశం delta_intvl = dt - సగటు_val;
				సగటు_val += delta_intvl / ec;
				pce->సగటు.store(సగటు_val);
				దశాంశం m2_val = pce->M2.ఎక్కించు();
				m2_val += delta_intvl * (dt - సగటు_val);
				pce->M2.store(m2_val);

				pce->సమయం_ప్రారంభం.store((uint64_t)0);
			}
		}
		ఆపు;

	అప్రమేయ:
		ఆపు;
	}
}

శూన్య
perf_లెక్క_interval(perf_counter_t handle, hrt_abstime now)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_INTERVAL: {
			నిర్మాణం perf_ctr_interval *pci = (నిర్మాణం perf_ctr_interval *)handle;
			uint64_t ec = pci->event_లెక్క.ఎక్కించు();

			ఎంపిక (ec) {
			స్థితి 0:
				pci->సమయం_మొదటి.store(now);
				ఆపు;

			స్థితి 1: {
					uint64_t tl = pci->సమయం_చివరి.ఎక్కించు();
					uint32_t interval = (uint32_t)(now - tl);
					pci->సమయం_కనిష్ఠ.store(interval);
					pci->సమయం_most.store(interval);
					pci->సగటు.store(interval / 1e6f);
					pci->M2.store(0.0f);
					ఆపు;
				}

			అప్రమేయ: {
					uint64_t tl = pci->సమయం_చివరి.ఎక్కించు();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->సమయం_కనిష్ఠ.ఎక్కించు();

					ఒకవేళ ((uint32_t)interval < least) {
						pci->సమయం_కనిష్ఠ.store((uint32_t)interval);
					}

					uint32_t most = pci->సమయం_most.ఎక్కించు();

					ఒకవేళ ((uint32_t)interval > most) {
						pci->సమయం_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					దశాంశం dt = interval / 1e6f;
					దశాంశం సగటు_val = pci->సగటు.ఎక్కించు();
					దశాంశం delta_intvl = dt - సగటు_val;
					సగటు_val += delta_intvl / ec;
					pci->సగటు.store(సగటు_val);
					దశాంశం m2_val = pci->M2.ఎక్కించు();
					m2_val += delta_intvl * (dt - సగటు_val);
					pci->M2.store(m2_val);
					ఆపు;
				}
			}

			pci->సమయం_చివరి.store(now);
			pci->event_లెక్క.fetch_కూడు(1);
			ఆపు;
		}

	అప్రమేయ:
		ఆపు;
	}
}

శూన్య
perf_సమితి_లెక్క(perf_counter_t handle, uint64_t లెక్క)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_లెక్క: {
			((నిర్మాణం perf_ctr_లెక్క *)handle)->event_లెక్క.store(లెక్క);
		}
		ఆపు;

	అప్రమేయ:
		ఆపు;
	}

}

శూన్య
perf_cancel(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_ELAPSED: {
			నిర్మాణం perf_ctr_elapsed *pce = (నిర్మాణం perf_ctr_elapsed *)handle;
			pce->సమయం_ప్రారంభం.store((uint64_t)0);
		}
		ఆపు;

	అప్రమేయ:
		ఆపు;
	}
}

శూన్య
perf_reset(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_లెక్క:
		((నిర్మాణం perf_ctr_లెక్క *)handle)->event_లెక్క.store((uint64_t)0);
		ఆపు;

	స్థితి PC_ELAPSED: {
			నిర్మాణం perf_ctr_elapsed *pce = (నిర్మాణం perf_ctr_elapsed *)handle;
			pce->event_లెక్క.store((uint64_t)0);
			pce->సమయం_ప్రారంభం.store((uint64_t)0);
			pce->సమయం_మొత్తం.store((uint64_t)0);
			pce->సమయం_కనిష్ఠ.store((uint32_t)0);
			pce->సమయం_most.store((uint32_t)0);
			pce->సగటు.store(0.0f);
			pce->M2.store(0.0f);
			ఆపు;
		}

	స్థితి PC_INTERVAL: {
			నిర్మాణం perf_ctr_interval *pci = (నిర్మాణం perf_ctr_interval *)handle;
			pci->event_లెక్క.store((uint64_t)0);
			pci->సమయం_event.store((uint64_t)0);
			pci->సమయం_మొదటి.store((uint64_t)0);
			pci->సమయం_చివరి.store((uint64_t)0);
			pci->సమయం_కనిష్ఠ.store((uint32_t)0);
			pci->సమయం_most.store((uint32_t)0);
			pci->సగటు.store(0.0f);
			pci->M2.store(0.0f);
			ఆపు;
		}
	}
}

శూన్య
perf_print_counter(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_లెక్క: {
			uint64_t ec = ((నిర్మాణం perf_ctr_లెక్క *)handle)->event_లెక్క.ఎక్కించు();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			ఆపు;
		}

	స్థితి PC_ELAPSED: {
			నిర్మాణం perf_ctr_elapsed *pce = (నిర్మాణం perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_లెక్క.ఎక్కించు();
			uint64_t tt = pce->సమయం_మొత్తం.ఎక్కించు();
			uint32_t tl = pce->సమయం_కనిష్ఠ.ఎక్కించు();
			uint32_t tm = pce->సమయం_most.ఎక్కించు();
			దశాంశం m2 = pce->M2.ఎక్కించు();
			దశాంశం rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			PX4_INFO_RAW("%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32
				     "us %5.3fus rms\n",
				     handle->name, ec, tt,
				     (ec == 0) ? 0 : (ద్విగుణ)tt / (ద్విగుణ)ec,
				     tl, tm, (ద్విగుణ)(1e6f * rms));
			ఆపు;
		}

	స్థితి PC_INTERVAL: {
			నిర్మాణం perf_ctr_interval *pci = (నిర్మాణం perf_ctr_interval *)handle;
			uint64_t ec = pci->event_లెక్క.ఎక్కించు();
			uint64_t tf = pci->సమయం_మొదటి.ఎక్కించు();
			uint64_t tla = pci->సమయం_చివరి.ఎక్కించు();
			uint32_t tl = pci->సమయం_కనిష్ఠ.ఎక్కించు();
			uint32_t tm = pci->సమయం_most.ఎక్కించు();
			దశాంశం m2 = pci->M2.ఎక్కించు();
			దశాంశం rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			PX4_INFO_RAW("%s: %" PRIu64 " events, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms\n",
				     handle->name, ec,
				     (ec == 0) ? 0 : (ద్విగుణ)(tla - tf) / (ద్విగుణ)ec,
				     tl, tm, (ద్విగుణ)(1e6f * rms));
			ఆపు;
		}

	అప్రమేయ:
		ఆపు;
	}
}


పూర్ణాంకం
perf_print_counter_బఫరు(అక్షరం *బఫరు_2, పూర్ణాంకం నిడివి, perf_counter_t handle)
{
	పూర్ణాంకం అంకె_written = 0;

	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి 0;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_లెక్క:
		అంకె_written = snprintf(బఫరు_2, నిడివి, "%s: %" PRIu64 " events",
				       handle->name,
				       ((నిర్మాణం perf_ctr_లెక్క *)handle)->event_లెక్క.ఎక్కించు());
		ఆపు;

	స్థితి PC_ELAPSED: {
			నిర్మాణం perf_ctr_elapsed *pce = (నిర్మాణం perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_లెక్క.ఎక్కించు();
			uint64_t tt = pce->సమయం_మొత్తం.ఎక్కించు();
			uint32_t tl = pce->సమయం_కనిష్ఠ.ఎక్కించు();
			uint32_t tm = pce->సమయం_most.ఎక్కించు();
			దశాంశం m2 = pce->M2.ఎక్కించు();
			దశాంశం rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			అంకె_written = snprintf(బఫరు_2, నిడివి,
					       "%s: %" PRIu64 " events, %" PRIu64 "us elapsed, %.2fus avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       tt,
					       (ec == 0) ? 0 : (ద్విగుణ)tt / (ద్విగుణ)ec,
					       tl,
					       tm,
					       (ద్విగుణ)(1e6f * rms));
			ఆపు;
		}

	స్థితి PC_INTERVAL: {
			నిర్మాణం perf_ctr_interval *pci = (నిర్మాణం perf_ctr_interval *)handle;
			uint64_t ec = pci->event_లెక్క.ఎక్కించు();
			uint32_t tl = pci->సమయం_కనిష్ఠ.ఎక్కించు();
			uint32_t tm = pci->సమయం_most.ఎక్కించు();
			దశాంశం m2 = pci->M2.ఎక్కించు();
			దశాంశం rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			అంకె_written = snprintf(బఫరు_2, నిడివి,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (ద్విగుణ)(pci->సమయం_చివరి.ఎక్కించు() - pci->సమయం_మొదటి.ఎక్కించు()) / (ద్విగుణ)ec,
					       tl,
					       tm,
					       (ద్విగుణ)(1e6f * rms));
			ఆపు;
		}

	అప్రమేయ:
		ఆపు;
	}

	బఫరు_2[నిడివి - 1] = 0; // ensure 0-termination
	తిరిగి అంకె_written;
}

uint64_t
perf_event_లెక్క(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి 0;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_లెక్క:
		తిరిగి ((నిర్మాణం perf_ctr_లెక్క *)handle)->event_లెక్క.ఎక్కించు();

	స్థితి PC_ELAPSED: {
			నిర్మాణం perf_ctr_elapsed *pce = (నిర్మాణం perf_ctr_elapsed *)handle;
			తిరిగి pce->event_లెక్క.ఎక్కించు();
		}

	స్థితి PC_INTERVAL: {
			నిర్మాణం perf_ctr_interval *pci = (నిర్మాణం perf_ctr_interval *)handle;
			తిరిగి pci->event_లెక్క.ఎక్కించు();
		}

	అప్రమేయ:
		ఆపు;
	}

	తిరిగి 0;
}

దశాంశం
perf_సగటు(perf_counter_t handle)
{
	ఒకవేళ (handle == శూన్యసూచిక) {
		తిరిగి 0;
	}

	ఎంపిక (handle->type) {
	స్థితి PC_ELAPSED: {
			నిర్మాణం perf_ctr_elapsed *pce = (నిర్మాణం perf_ctr_elapsed *)handle;
			తిరిగి pce->సగటు.ఎక్కించు();
		}

	స్థితి PC_INTERVAL: {
			నిర్మాణం perf_ctr_interval *pci = (నిర్మాణం perf_ctr_interval *)handle;
			తిరిగి pci->సగటు.ఎక్కించు();
		}

	అప్రమేయ:
		ఆపు;
	}

	తిరిగి 0.0f;
}

శూన్య
perf_iterate_all(perf_callback cb, శూన్య *user)
{
	స్థిర constexpr size_t PERF_COUNTER_పంక్తి_బఫరు_కొలత = 220;
	అక్షరం బఫరు_2[PERF_COUNTER_పంక్తి_బఫరు_కొలత];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_చురుకైన;

	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	అయితే (handle != శూన్యసూచిక) {
		// advance while locked; entries stay linked until all active iterations end
		perf_counter_t తదుపరి = (perf_counter_t)sq_తదుపరి(&handle->link);

		ఒకవేళ (!handle->pending_free) {
			perf_print_counter_బఫరు(బఫరు_2, పరిమాణం(బఫరు_2), handle);

			// the callback may block (the logger waits for MAVLink acks here), so run
			// it without the global mutex: perf_alloc()/perf_free() on other threads
			// keep working, and 'next' stays valid because perf_free() defers deletion
			// while perf_iterate_active > 0
			pthread_mutex_unlock(&perf_counters_mutex);
			cb(బఫరు_2, user);
			pthread_mutex_lock(&perf_counters_mutex);
		}

		handle = తదుపరి;
	}

	ఒకవేళ (--perf_iterate_చురుకైన == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

శూన్య
perf_print_all(శూన్య)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	అయితే (handle != శూన్యసూచిక) {
		ఒకవేళ (!handle->pending_free) {
			perf_print_counter(handle);
		}

		handle = (perf_counter_t)sq_తదుపరి(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

శూన్య
perf_print_latency(శూన్య)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	కొరకు (పూర్ణాంకం i = 0; i < get_latency_పాత్ర_లెక్క(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.పాత్ర, (దీర్ఘ పూర్ణాంకం)latency.counter);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_పాత్ర_లెక్క() - 1, get_latency_పాత్ర_లెక్క());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.పాత్ర, latency.counter);
}

శూన్య
perf_reset_all(శూన్య)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	అయితే (handle != శూన్యసూచిక) {
		ఒకవేళ (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_counter_t)sq_తదుపరి(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
