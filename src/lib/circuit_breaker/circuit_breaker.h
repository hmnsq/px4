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

#اگر_غیر_تعریف_شدہ CIRCUIT_BREAKER_H_
#تعریف_کرو CIRCUIT_BREAKER_H_

/* SAFETY WARNING  --  SAFETY WARNING  --  SAFETY WARNING
 *
 * OBEY THE DOCUMENTATION FOR ALL CIRCUIT BREAKERS HERE,
 * ENSURE TO READ CAREFULLY ALL SAFETY WARNINGS.
 * http://pixhawk.org/dev/circuit_breakers
 *
 * CIRCUIT BREAKERS ARE NOT PART OF THE STANDARD OPERATION PROCEDURE
 * AND MAY DISABLE CHECKS THAT ARE VITAL FOR SAFE FLIGHT.
 */

#تعریف_کرو CBRK_BUZZER_کلید         782097
#تعریف_کرو CBRK_SUPPLY_CHK_کلید	894281
#تعریف_کرو CBRK_FLIGHTTERM_کلید	121212
#تعریف_کرو CBRK_USB_CHK_کلید	197848
#تعریف_کرو CBRK_VTOLARMING_کلید	159753
#تعریف_کرو CBRK_UAVCAN_FW_کلید	5318008

#شامل <stdint.h>

__BEGIN_DECLS

خارجی "C" __EXPORT bool circuit_breaker_enabled(ثابت حرف *breaker, int32_t magic);

ساکن اندرونی bool circuit_breaker_enabled_by_val(int32_t breaker_val, int32_t magic)
{
	واپس breaker_val == magic;
}

__END_DECLS

#اگر_ختم /* CIRCUIT_BREAKER_H_ */
