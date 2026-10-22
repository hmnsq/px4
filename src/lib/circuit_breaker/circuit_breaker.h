/****************************************************************************
 *
 *   Copyright (c) 2014 PX4 Development Team. All rights reserved.
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

/*
 * @file circuit_breaker.h
 *
 * Circuit breaker functionality.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CIRCUIT_BREAKER_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CIRCUIT_BREAKER_H_

/* SAFETY WARNING  --  SAFETY WARNING  --  SAFETY WARNING
 *
 * OBEY THE DOCUMENTATION FOR ALL CIRCUIT BREAKERS HERE,
 * ENSURE TO READ CAREFULLY ALL SAFETY WARNINGS.
 * http://pixhawk.org/dev/circuit_breakers
 *
 * CIRCUIT BREAKERS ARE NOT PART OF THE STANDARD OPERATION PROCEDURE
 * AND MAY DISABLE CHECKS THAT ARE VITAL FOR SAFE FLIGHT.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CBRK_BUZZER_ਕੁੰਜੀ         782097
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CBRK_SUPPLY_CHK_ਕੁੰਜੀ	894281
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CBRK_FLIGHTTERM_ਕੁੰਜੀ	121212
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CBRK_USB_CHK_ਕੁੰਜੀ	197848
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CBRK_VTOLARMING_ਕੁੰਜੀ	159753
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CBRK_UAVCAN_FW_ਕੁੰਜੀ	5318008

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

__BEGIN_DECLS

ਬਾਹਰੀ "C" __EXPORT bool circuit_breaker_enabled(ਅਚਲ ਅੱਖਰ *breaker, int32_t magic);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool circuit_breaker_enabled_by_val(int32_t breaker_val, int32_t magic)
{
	ਵਾਪਸ breaker_val == magic;
}

__END_DECLS

#ਜੇ_ਅੰਤ /* CIRCUIT_BREAKER_H_ */
