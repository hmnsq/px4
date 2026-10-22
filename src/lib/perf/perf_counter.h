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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _SYSTEMLIB_PERF_COUNTER_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _SYSTEMLIB_PERF_COUNTER_H ਮੁੱਲ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

/**
 * Counter types.
 */
ਗਿਣਤੀ perf_counter_type {
	PC_ਗਿਣਤੀ,		/**< count the number of times an event occurs */
	PC_ELAPSED,		/**< measure the time elapsed performing an event */
	PC_INTERVAL		/**< measure the interval between instances of an event */
};

ਬਣਤਰ perf_ctr_ਸਿਰਲੇਖ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ perf_ctr_ਸਿਰਲੇਖ	*perf_counter_t;

__BEGIN_DECLS

/**
 * Create a new local counter.
 *
 * @param type			The type of the new counter.
 * @param name			The counter name.
 * @return			Handle for the new counter, or NULL if a counter
 *				could not be allocated.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ perf_alloc	// perf_alloc might be defined to be NULL in src/modules/px4iofirmware/px4io.h
__EXPORT ਬਾਹਰੀ perf_counter_t	perf_alloc(ਗਿਣਤੀ perf_counter_type type, ਅਚਲ ਅੱਖਰ *name);
#ਜੇ_ਅੰਤ

/**
 * Get the reference to an existing counter or create a new one if it does not exist.
 *
 * @param type			The type of the counter.
 * @param name			The counter name.
 * @return			Handle for the counter, or NULL if a counter
 *				could not be allocated.
 */
__EXPORT ਬਾਹਰੀ perf_counter_t	perf_alloc_once(ਗਿਣਤੀ perf_counter_type type, ਅਚਲ ਅੱਖਰ *name);

/**
 * Free a counter.
 *
 * If one or more perf_iterate_all() calls are in progress, the counter is
 * hidden from subsequent callbacks immediately and deleted after the last
 * iteration ends.
 *
 * @param handle		The performance counter's handle.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_free(perf_counter_t handle);

/**
 * Count a performance event.
 *
 * This call only affects counters that take single events; PC_COUNT, PC_INTERVAL etc.
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_ਗਿਣਤੀ(perf_counter_t handle);

/**
 * Begin a performance event.
 *
 * This call applies to counters that operate over ranges of time; PC_ELAPSED etc.
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_begin(perf_counter_t handle);

/**
 * End a performance event.
 *
 * This call applies to counters that operate over ranges of time; PC_ELAPSED etc.
 * If a call is made without a corresponding perf_begin call, or if perf_cancel
 * has been called subsequently, no change is made to the counter.
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_ਸਿਰਾ(perf_counter_t handle);

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
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_ਸੈੱਟ_elapsed(perf_counter_t handle, int64_t elapsed);

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
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_ਗਿਣਤੀ_interval(perf_counter_t handle, uint64_t time);

/**
 * Set a counter
 *
 * This call applies to counters of type PC_COUNT. It (re-)sets the count.
 *
 * @param handle		The handle returned from perf_alloc.
 * @param count			The counter value to be set.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_ਸੈੱਟ_ਗਿਣਤੀ(perf_counter_t handle, uint64_t ਗਿਣਤੀ_2);

/**
 * Cancel a performance event.
 *
 * This call applies to counters that operate over ranges of time; PC_ELAPSED etc.
 * It reverts the effect of a previous perf_begin.
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_cancel(perf_counter_t handle);

/**
 * Reset a performance counter.
 *
 * This call resets performance counter to initial state
 *
 * @param handle		The handle returned from perf_alloc.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_reset(perf_counter_t handle);

/**
 * Print one performance counter to stdout
 *
 * @param handle		The counter to print.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_print_counter(perf_counter_t handle);

/**
 * Print one performance counter to a buffer.
 *
 * @param buffer			buffer to write to
 * @param length			buffer length
 * @param handle			The counter to print.
 * @param return			number of bytes written
 */
__EXPORT ਬਾਹਰੀ ਪੂਰਨ		perf_print_counter_ਬਫਰ(ਅੱਖਰ *ਬਫਰ_2, ਪੂਰਨ ਲੰਮਾਈ, perf_counter_t handle);

/**
 * Print all of the performance counters.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_print_all(ਖਾਲੀ);


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*perf_callback)(ਅਚਲ ਅੱਖਰ *counter_ਸਤਰ, ਖਾਲੀ *user);

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
__EXPORT ਬਾਹਰੀ ਖਾਲੀ	perf_iterate_all(perf_callback cb, ਖਾਲੀ *user);

/**
 * Print hrt latency counters.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_print_latency(ਖਾਲੀ);

/**
 * Reset all of the performance counters.
 */
__EXPORT ਬਾਹਰੀ ਖਾਲੀ		perf_reset_all(ਖਾਲੀ);

/**
 * Return current event_count
 *
 * @param handle		The counter returned from perf_alloc.
 * @return			event_count
 */
__EXPORT ਬਾਹਰੀ uint64_t	perf_event_ਗਿਣਤੀ(perf_counter_t handle);

/**
 * Return current mean
 *
 * @param handle		The handle returned from perf_alloc.
 * @param return		mean
 */
__EXPORT ਬਾਹਰੀ ਦਸ਼ਮਲਵ		perf_ਔਸਤ(perf_counter_t handle);

__END_DECLS

#ਜੇ_ਅੰਤ
