/****************************************************************************
 *
 *   Copyright (C) 2012-2016 PX4 Development Team. All rights reserved.
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
 * @file perf_counter.h
 * Performance measuring tools.
 */

#اگر_غیر_تعریف_شدہ SYSTEMLIB_PERF_شمار_H
#تعریف_کرو SYSTEMLIB_PERF_شمار_H قدر

#شامل <stdint.h>
#شامل <px4_platform_common/defines.h>

/**
 * Counter types.
 */
شمار perf_شمار_type {
	PC_گنتی,		/**< count the number of times an event occurs */
	PC_ELAPSED,		/**< measure the time elapsed performing an event */
	PC_INTERVAL		/**< measure the interval between instances of an event */
};

ساخت perf_ctr_سرخی;
قسمتعریف ساخت perf_ctr_سرخی	*perf_شمار_t;

__BEGIN_DECLS

/**
 * Create a new local counter.
 *
 * @param type			The type of the new counter.
 * @param name			The counter name.
 * @return			Handle for the new counter, or NULL if a counter
 *				could not be allocated.
 */
#اگر_غیر_تعریف_شدہ perf_alloc	// perf_alloc might be defined to be NULL in src/modules/px4iofirmware/px4io.h
__EXPORT خارجی perf_شمار_t	perf_alloc(شمار perf_شمار_type type, ثابت حرف *name);
#اگر_ختم

/**
 * Get the reference to an existing counter or create a new one if it does not exist.
 *
 * @param type			The type of the counter.
 * @param name			The counter name.
 * @return			Handle for the counter, or NULL if a counter
 *				could not be allocated.
 */
__EXPORT خارجی perf_شمار_t	perf_alloc_once(شمار perf_شمار_type type, ثابت حرف *name);

/**
 * Free a counter.
 *
 * If one or more perf_iterate_all() calls are in progress, the counter is
 * hidden from subsequent callbacks immediately and deleted after the last
 * iteration ends.
 *
 * @param handle		The performance counter's handle.
 */
__EXPORT خارجی خالی		perf_free(perf_شمار_t handle);

/**
 * Count a performance event.
 *
 * This call only affects counters that take single events; PC_COUNT, PC_INTERVAL etc.
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT خارجی خالی		perf_گنتی(perf_شمار_t handle);

/**
 * Begin a performance event.
 *
 * This call applies to counters that operate over ranges of time; PC_ELAPSED etc.
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT خارجی خالی		perf_begin(perf_شمار_t handle);

/**
 * End a performance event.
 *
 * This call applies to counters that operate over ranges of time; PC_ELAPSED etc.
 * If a call is made without a corresponding perf_begin call, or if perf_cancel
 * has been called subsequently, no change is made to the counter.
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT خارجی خالی		perf_انتہا(perf_شمار_t handle);

/**
 * Register a measurement
 *
 * This call applies to counters that operate over ranges of time; PC_ELAPSED etc.
 * If a call is made without a corresponding perf_begin call. It sets the
 * value provided as argument as a new measurement.
 *
 * @param handle		The handle returned from perf_alloc.
 * @param elapsed		The time elapsed. Negative values lead to incrementing the overrun counter.
 */
__EXPORT خارجی خالی		perf_سیٹ_elapsed(perf_شمار_t handle, int64_t elapsed);

/**
 * Register a measurement
 *
 * This call applies to counters that operate over ranges of time; PC_ELAPSED etc.
 * If a call is made without a corresponding perf_begin call. It sets the
 * value provided as argument as a new measurement.
 *
 * @param handle		The handle returned from perf_alloc.
 * @param time			The time for the interval.
 */
__EXPORT خارجی خالی		perf_گنتی_interval(perf_شمار_t handle, uint64_t time);

/**
 * Set a counter
 *
 * This call applies to counters of type PC_COUNT. It (re-)sets the count.
 *
 * @param handle		The handle returned from perf_alloc.
 * @param count			The counter value to be set.
 */
__EXPORT خارجی خالی		perf_سیٹ_گنتی(perf_شمار_t handle, uint64_t گنتی);

/**
 * Cancel a performance event.
 *
 * This call applies to counters that operate over ranges of time; PC_ELAPSED etc.
 * It reverts the effect of a previous perf_begin.
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT خارجی خالی		perf_cancel(perf_شمار_t handle);

/**
 * Reset a performance counter.
 *
 * This call resets performance counter to initial state
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT خارجی خالی		perf_reset(perf_شمار_t handle);

/**
 * Print one performance counter to stdout
 *
 * @param handle		The counter to print.
 */
__EXPORT خارجی خالی		perf_print_شمار(perf_شمار_t handle);

/**
 * Print one performance counter to a buffer.
 *
 * @param buffer			buffer to write to
 * @param length			buffer length
 * @param handle			The counter to print.
 * @param return			number of bytes written
 */
__EXPORT خارجی صحیح		perf_print_شمار_بفر(حرف *بفر_2, صحیح لمبائی_2, perf_شمار_t handle);

/**
 * Print all of the performance counters.
 */
__EXPORT خارجی خالی		perf_print_all(خالی);


قسمتعریف خالی (*perf_callback)(ثابت حرف *شمار_سطر, خالی *user);

/**
 * Iterate over all performance counters using a callback.
 *
 * The callback gets each counter formatted as with perf_print_counter_buffer();
 * the string is only valid during the call. The internal mutex is released
 * while the callback runs, therefore blocking inside the callback (e.g. on I/O)
 * does not stall other perf callers, and the callback may itself call perf methods.
 *
 * @param cb callback method
 * @param user custom argument for the callback
 */
__EXPORT خارجی خالی	perf_iterate_all(perf_callback cb, خالی *user);

/**
 * Print hrt latency counters.
 */
__EXPORT خارجی خالی		perf_print_latency(خالی);

/**
 * Reset all of the performance counters.
 */
__EXPORT خارجی خالی		perf_reset_all(خالی);

/**
 * Return current event_count
 *
 * @param handle		The counter returned from perf_alloc.
 * @return			event_count
 */
__EXPORT خارجی uint64_t	perf_event_گنتی(perf_شمار_t handle);

/**
 * Return current mean
 *
 * @param handle		The handle returned from perf_alloc.
 * @param return		mean
 */
__EXPORT خارجی اعشاری		perf_اوسط(perf_شمار_t handle);

__END_DECLS

#اگر_ختم
