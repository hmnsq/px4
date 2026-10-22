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
प्रतिरूपम् <प्रकारनाम T> उपयुज्य PerfVar = px4::atomic<T>;
#अन्यथा
प्रतिरूपम् <प्रकारनाम T>
वर्ग PerfVar
{
सार्वजनिक:
	PerfVar() = स्वभाव;
	T आरोपय() ध्रुव { प्रत्यागम मानम्_2; }
	शून्य store(T value) { मानम्_2 = value; }
	T fetch_सङ्कलय(T value) { ध्रुव T previous = मानम्_2; मानम्_2 += value; प्रत्यागम previous; }
गुप्त:
	T मानम्_2 {};
};
#यदि_अन्तः

/**
 * Header common to all counters.
 */
संरचना perf_ctr_शीर्षकम् {
	sq_प्रविष्टिः_t		link;	/**< list linkage */
	गणना perf_गणक_type	type;	/**< counter type */
	तार्किक			pending_free;	/**< freed during iteration, deleted after the last iteration ends */
	ध्रुव वर्ण		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
संरचना perf_ctr_गणनम् : सार्वजनिक perf_ctr_शीर्षकम् {
	PerfVar<uint64_t>	event_गणनम्;
};

/**
 * PC_ELAPSED counter.
 */
संरचना perf_ctr_elapsed : सार्वजनिक perf_ctr_शीर्षकम् {
	PerfVar<uint64_t>	event_गणनम्;
	PerfVar<uint64_t>	time_start;
	PerfVar<uint64_t>	time_योग;
	PerfVar<uint32_t>	time_न्यूनतमम्;
	PerfVar<uint32_t>	time_most;
	PerfVar<भिन्न>	माध्यम्;
	PerfVar<भिन्न>	M2;
};

/**
 * PC_INTERVAL counter.
 */
संरचना perf_ctr_interval : सार्वजनिक perf_ctr_शीर्षकम् {
	PerfVar<uint64_t>	event_गणनम्;
	PerfVar<uint64_t>	time_event;
	PerfVar<uint64_t>	time_first;
	PerfVar<uint64_t>	time_चरमः;
	PerfVar<uint32_t>	time_न्यूनतमम्;
	PerfVar<uint32_t>	time_most;
	PerfVar<भिन्न>	माध्यम्;
	PerfVar<भिन्न>	M2;
};

/**
 * List of all known counters.
 */
स्थिर sq_queue_t	perf_counters = { शून्यसूचकः, शून्यसूचकः };

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
स्थिर पूर्णाङ्क perf_iterate_सक्रिय = 0;

स्थिर शून्य
perf_delete(perf_गणक_t handle)
{
	विकल्प (handle->type) {
	पक्ष PC_गणनम्:
		विलोपय (संरचना perf_ctr_गणनम् *)handle;
		भङ्ग;

	पक्ष PC_ELAPSED:
		विलोपय (संरचना perf_ctr_elapsed *)handle;
		भङ्ग;

	पक्ष PC_INTERVAL:
		विलोपय (संरचना perf_ctr_interval *)handle;
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
	perf_गणक_t handle = (perf_गणक_t)sq_peek(&perf_counters);

	यावत् (handle != शून्यसूचकः) {
		perf_गणक_t अग्र = (perf_गणक_t)sq_अग्र(&handle->link);

		यदि (handle->pending_free) {
			sq_rem(&handle->link, &perf_counters);
			perf_delete(handle);
		}

		handle = अग्र;
	}
}

perf_गणक_t
perf_alloc(गणना perf_गणक_type type, ध्रुव वर्ण *name)
{
	perf_गणक_t ctr = शून्यसूचकः;

	विकल्प (type) {
	पक्ष PC_गणनम्:
		ctr = नूतन perf_ctr_गणनम्();
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

	यदि (ctr != शून्यसूचकः) {
		ctr->type = type;
		ctr->name = name;
		ctr->pending_free = असत्य;
		pthread_mutex_lock(&perf_counters_mutex);
		sq_addfirst(&ctr->link, &perf_counters);
		pthread_mutex_unlock(&perf_counters_mutex);
	}

	प्रत्यागम ctr;
}

perf_गणक_t
perf_alloc_once(गणना perf_गणक_type type, ध्रुव वर्ण *name)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_गणक_t handle = (perf_गणक_t)sq_peek(&perf_counters);

	यावत् (handle != शून्यसूचकः) {
		यदि (!handle->pending_free && !वाक्यं_तुलय(handle->name, name)) {
			यदि (type == handle->type) {
				/* they are the same counter */
				pthread_mutex_unlock(&perf_counters_mutex);
				प्रत्यागम handle;

			} अन्यथा {
				/* same name but different type, assuming this is an error and not intended */
				pthread_mutex_unlock(&perf_counters_mutex);
				प्रत्यागम शून्यसूचकः;
			}
		}

		handle = (perf_गणक_t)sq_अग्र(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	/* if the execution reaches here, no existing counter of that name was found */
	प्रत्यागम perf_alloc(type, name);
}

शून्य
perf_free(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम;
	}

	pthread_mutex_lock(&perf_counters_mutex);

	यदि (perf_iterate_सक्रिय > 0) {
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
perf_गणनम्(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_गणनम्:
		((संरचना perf_ctr_गणनम् *)handle)->event_गणनम्.fetch_सङ्कलय(1);
		भङ्ग;

	पक्ष PC_INTERVAL:
		perf_गणनम्_interval(handle, hrt_absolute_time());
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_begin(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
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
perf_अन्तः(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			hrt_abstime ts = pce->time_start.आरोपय();

			यदि (ts != 0) {
				perf_समुच्चयः_elapsed(handle, hrt_absolute_time() - ts);
			}
		}
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_समुच्चयः_elapsed(perf_गणक_t handle, int64_t elapsed)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;

			यदि (elapsed >= 0) {
				uint64_t ec = pce->event_गणनम्.fetch_सङ्कलय(1) + 1;
				pce->time_योग.fetch_सङ्कलय((uint64_t)elapsed);

				uint32_t tl = pce->time_न्यूनतमम्.आरोपय();

				यदि ((tl > (uint32_t)elapsed) || (tl == 0)) {
					pce->time_न्यूनतमम्.store((uint32_t)elapsed);
				}

				uint32_t tm = pce->time_most.आरोपय();

				यदि (tm < (uint32_t)elapsed) {
					pce->time_most.store((uint32_t)elapsed);
				}

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				भिन्न dt = elapsed / 1e6f;
				भिन्न माध्यम्_val = pce->माध्यम्.आरोपय();
				भिन्न delta_intvl = dt - माध्यम्_val;
				माध्यम्_val += delta_intvl / ec;
				pce->माध्यम्.store(माध्यम्_val);
				भिन्न m2_val = pce->M2.आरोपय();
				m2_val += delta_intvl * (dt - माध्यम्_val);
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
perf_गणनम्_interval(perf_गणक_t handle, hrt_abstime now)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			uint64_t ec = pci->event_गणनम्.आरोपय();

			विकल्प (ec) {
			पक्ष 0:
				pci->time_first.store(now);
				भङ्ग;

			पक्ष 1: {
					uint64_t tl = pci->time_चरमः.आरोपय();
					uint32_t interval = (uint32_t)(now - tl);
					pci->time_न्यूनतमम्.store(interval);
					pci->time_most.store(interval);
					pci->माध्यम्.store(interval / 1e6f);
					pci->M2.store(0.0f);
					भङ्ग;
				}

			स्वभाव: {
					uint64_t tl = pci->time_चरमः.आरोपय();
					hrt_abstime interval = now - tl;

					uint32_t least = pci->time_न्यूनतमम्.आरोपय();

					यदि ((uint32_t)interval < least) {
						pci->time_न्यूनतमम्.store((uint32_t)interval);
					}

					uint32_t most = pci->time_most.आरोपय();

					यदि ((uint32_t)interval > most) {
						pci->time_most.store((uint32_t)interval);
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					भिन्न dt = interval / 1e6f;
					भिन्न माध्यम्_val = pci->माध्यम्.आरोपय();
					भिन्न delta_intvl = dt - माध्यम्_val;
					माध्यम्_val += delta_intvl / ec;
					pci->माध्यम्.store(माध्यम्_val);
					भिन्न m2_val = pci->M2.आरोपय();
					m2_val += delta_intvl * (dt - माध्यम्_val);
					pci->M2.store(m2_val);
					भङ्ग;
				}
			}

			pci->time_चरमः.store(now);
			pci->event_गणनम्.fetch_सङ्कलय(1);
			भङ्ग;
		}

	स्वभाव:
		भङ्ग;
	}
}

शून्य
perf_समुच्चयः_गणनम्(perf_गणक_t handle, uint64_t गणनम्)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_गणनम्: {
			((संरचना perf_ctr_गणनम् *)handle)->event_गणनम्.store(गणनम्);
		}
		भङ्ग;

	स्वभाव:
		भङ्ग;
	}

}

शून्य
perf_cancel(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
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
perf_reset(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_गणनम्:
		((संरचना perf_ctr_गणनम् *)handle)->event_गणनम्.store((uint64_t)0);
		भङ्ग;

	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			pce->event_गणनम्.store((uint64_t)0);
			pce->time_start.store((uint64_t)0);
			pce->time_योग.store((uint64_t)0);
			pce->time_न्यूनतमम्.store((uint32_t)0);
			pce->time_most.store((uint32_t)0);
			pce->माध्यम्.store(0.0f);
			pce->M2.store(0.0f);
			भङ्ग;
		}

	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			pci->event_गणनम्.store((uint64_t)0);
			pci->time_event.store((uint64_t)0);
			pci->time_first.store((uint64_t)0);
			pci->time_चरमः.store((uint64_t)0);
			pci->time_न्यूनतमम्.store((uint32_t)0);
			pci->time_most.store((uint32_t)0);
			pci->माध्यम्.store(0.0f);
			pci->M2.store(0.0f);
			भङ्ग;
		}
	}
}

शून्य
perf_print_गणक(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम;
	}

	विकल्प (handle->type) {
	पक्ष PC_गणनम्: {
			uint64_t ec = ((संरचना perf_ctr_गणनम् *)handle)->event_गणनम्.आरोपय();
			PX4_INFO_RAW("%s: %" PRIu64 " events\n",
				     handle->name, ec);
			भङ्ग;
		}

	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_गणनम्.आरोपय();
			uint64_t tt = pce->time_योग.आरोपय();
			uint32_t tl = pce->time_न्यूनतमम्.आरोपय();
			uint32_t tm = pce->time_most.आरोपय();
			भिन्न m2 = pce->M2.आरोपय();
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
			uint64_t ec = pci->event_गणनम्.आरोपय();
			uint64_t tf = pci->time_first.आरोपय();
			uint64_t tla = pci->time_चरमः.आरोपय();
			uint32_t tl = pci->time_न्यूनतमम्.आरोपय();
			uint32_t tm = pci->time_most.आरोपय();
			भिन्न m2 = pci->M2.आरोपय();
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
perf_print_गणक_बफर(वर्ण *बफर_2, पूर्णाङ्क दीर्घता_2, perf_गणक_t handle)
{
	पूर्णाङ्क अङ्कः_written = 0;

	यदि (handle == शून्यसूचकः) {
		प्रत्यागम 0;
	}

	विकल्प (handle->type) {
	पक्ष PC_गणनम्:
		अङ्कः_written = snprintf(बफर_2, दीर्घता_2, "%s: %" PRIu64 " events",
				       handle->name,
				       ((संरचना perf_ctr_गणनम् *)handle)->event_गणनम्.आरोपय());
		भङ्ग;

	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			uint64_t ec = pce->event_गणनम्.आरोपय();
			uint64_t tt = pce->time_योग.आरोपय();
			uint32_t tl = pce->time_न्यूनतमम्.आरोपय();
			uint32_t tm = pce->time_most.आरोपय();
			भिन्न m2 = pce->M2.आरोपय();
			भिन्न rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;
			अङ्कः_written = snprintf(बफर_2, दीर्घता_2,
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
			uint64_t ec = pci->event_गणनम्.आरोपय();
			uint32_t tl = pci->time_न्यूनतमम्.आरोपय();
			uint32_t tm = pci->time_most.आरोपय();
			भिन्न m2 = pci->M2.आरोपय();
			भिन्न rms = (ec > 1) ? sqrtf(m2 / (ec - 1)) : 0.f;

			अङ्कः_written = snprintf(बफर_2, दीर्घता_2,
					       "%s: %" PRIu64 " events, %.2f avg, min %" PRIu32 "us max %" PRIu32 "us %5.3fus rms",
					       handle->name,
					       ec,
					       (ec == 0) ? 0 : (द्विगुण)(pci->time_चरमः.आरोपय() - pci->time_first.आरोपय()) / (द्विगुण)ec,
					       tl,
					       tm,
					       (द्विगुण)(1e6f * rms));
			भङ्ग;
		}

	स्वभाव:
		भङ्ग;
	}

	बफर_2[दीर्घता_2 - 1] = 0; // ensure 0-termination
	प्रत्यागम अङ्कः_written;
}

uint64_t
perf_event_गणनम्(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम 0;
	}

	विकल्प (handle->type) {
	पक्ष PC_गणनम्:
		प्रत्यागम ((संरचना perf_ctr_गणनम् *)handle)->event_गणनम्.आरोपय();

	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			प्रत्यागम pce->event_गणनम्.आरोपय();
		}

	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			प्रत्यागम pci->event_गणनम्.आरोपय();
		}

	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम 0;
}

भिन्न
perf_माध्यम्(perf_गणक_t handle)
{
	यदि (handle == शून्यसूचकः) {
		प्रत्यागम 0;
	}

	विकल्प (handle->type) {
	पक्ष PC_ELAPSED: {
			संरचना perf_ctr_elapsed *pce = (संरचना perf_ctr_elapsed *)handle;
			प्रत्यागम pce->माध्यम्.आरोपय();
		}

	पक्ष PC_INTERVAL: {
			संरचना perf_ctr_interval *pci = (संरचना perf_ctr_interval *)handle;
			प्रत्यागम pci->माध्यम्.आरोपय();
		}

	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम 0.0f;
}

शून्य
perf_iterate_all(perf_callback cb, शून्य *user)
{
	स्थिर constexpr size_t PERF_गणक_पङ्क्तिः_बफर_आकार = 220;
	वर्ण बफर_2[PERF_गणक_पङ्क्तिः_बफर_आकार];

	pthread_mutex_lock(&perf_counters_mutex);
	++perf_iterate_सक्रिय;

	perf_गणक_t handle = (perf_गणक_t)sq_peek(&perf_counters);

	यावत् (handle != शून्यसूचकः) {
		// advance while locked; entries stay linked until all active iterations end
		perf_गणक_t अग्र = (perf_गणक_t)sq_अग्र(&handle->link);

		यदि (!handle->pending_free) {
			perf_print_गणक_बफर(बफर_2, परिमाण(बफर_2), handle);

			// the callback may block (the logger waits for MAVLink acks here), so run
			// it without the global mutex: perf_alloc()/perf_free() on other threads
			// keep working, and 'next' stays valid because perf_free() defers deletion
			// while perf_iterate_active > 0
			pthread_mutex_unlock(&perf_counters_mutex);
			cb(बफर_2, user);
			pthread_mutex_lock(&perf_counters_mutex);
		}

		handle = अग्र;
	}

	यदि (--perf_iterate_सक्रिय == 0) {
		perf_free_pending_locked();
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

शून्य
perf_print_all(शून्य)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_गणक_t handle = (perf_गणक_t)sq_peek(&perf_counters);

	यावत् (handle != शून्यसूचकः) {
		यदि (!handle->pending_free) {
			perf_print_गणक(handle);
		}

		handle = (perf_गणक_t)sq_अग्र(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);
}

शून्य
perf_print_latency(शून्य)
{
	latency_info_t latency;
	PX4_INFO_RAW("bucket [us] : events\n");

	प्रति (पूर्णाङ्क i = 0; i < get_latency_bucket_गणनम्(); i++) {
		latency = get_latency(i, i);
		PX4_INFO_RAW("       %4i : %li\n", latency.bucket, (दीर्घ पूर्णाङ्क)latency.गणक);
	}

	// print the overflow bucket value
	latency = get_latency(get_latency_bucket_गणनम्() - 1, get_latency_bucket_गणनम्());
	PX4_INFO_RAW(" >%4" PRIu16 " : %" PRIu32 "\n", latency.bucket, latency.गणक);
}

शून्य
perf_reset_all(शून्य)
{
	pthread_mutex_lock(&perf_counters_mutex);
	perf_गणक_t handle = (perf_गणक_t)sq_peek(&perf_counters);

	यावत् (handle != शून्यसूचकः) {
		यदि (!handle->pending_free) {
			perf_reset(handle);
		}

		handle = (perf_गणक_t)sq_अग्र(&handle->link);
	}

	pthread_mutex_unlock(&perf_counters_mutex);

	reset_latency_counters();
}
