/****************************************************************************
 *
 *   Copyright (C) 2014 PX4 Development Team. All rights reserved.
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
 * @file px4_macros.h
 *
 * A set of useful macros for enhanced runtime and compile time
 * error detection and warning suppression.
 *
 * Define NO_BLOAT to reduce bloat from file name inclusion.
 *
 * The arraySize() will compute the size of an array regardless
 * it's type
 *
 * INVALID_CASE(c) should be used is case statements to ferret out
 * unintended behavior
 *
 * UNUSED(var) will suppress compile time warnings of unused
 * variables
 *
 * CCASSERT(predicate) Will generate a compile time error it the
 * predicate is false
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _PX4_MACROS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _PX4_MACROS_H


#ਜੇ !defined(array_ਮਾਪ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ array_ਮਾਪ(a) (ਆਕਾਰ((a))/ਆਕਾਰ((a[0])))
#ਜੇ_ਅੰਤ

#ਜੇ !defined(NO_BLOAT)
#ਜੇ defined(__BASE_FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _FILE_NAME_ __BASE_FILE__
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _FILE_NAME_ __FILE__
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _FILE_NAME_ ""
#ਜੇ_ਅੰਤ

#ਜੇ !defined(INVALID_CASE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_CASE(c) ਲਿਖੋ("Invalid Case %d, %s:%d",(c),__BASE_FILE__,__LINE__) /* todo use PANIC */
#ਜੇ_ਅੰਤ

#ਜੇ !defined(ਅਣਵਰਤਿਆ_2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਣਵਰਤਿਆ_2(var) (ਖਾਲੀ)(var)
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CAT)
#ਜੇ !defined(_CAT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _CAT(a, b) a ## b
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAT(a, b) _CAT(a, b)
#ਜੇ_ਅੰਤ

#ਜੇ !defined(FREEZE_STR)
#  define FREEZE_STR(s) #s
#ਜੇ_ਅੰਤ

#ਜੇ !defined(STRINGIFY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRINGIFY(s) FREEZE_STR(s)
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CCASSERT)
#ਜੇ defined(static_assert)
#		define CCASSERT(predicate) static_assert(predicate, STRINGIFY(predicate))
#	ਨਹੀਂਤਾਂ
#		define CCASSERT(predicate) x_CCASSERT_ਸਤਰ(predicate, __LINE__)
#		ਜੇ !defined(x_CCASSERT_ਸਤਰ)
#			define x_CCASSERT_ਸਤਰ(predicate, ਸਤਰ) ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ CAT(constraint_violated_on_ਸਤਰ,ਸਤਰ)[2*((predicate)!=0)-1] __attribute__ ((ਅਣਵਰਤਿਆ)) ;
#		endif
#	endif
#ਜੇ_ਅੰਤ

#ਜੇ !defined(DO_PRAGMA)
#  define DO_PRAGMA(x) _Pragma (#x)
#ਜੇ_ਅੰਤ

#ਜੇ !defined(TODO)
#  define TODO(x) DO_PRAGMA(ਸੁਨੇਹਾ_3 ("TODO - " #x))
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* _PX4_MACROS_H */
