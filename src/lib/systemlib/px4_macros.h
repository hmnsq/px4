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
#அந்தர்பாவ <assert.h>

#ifndef _PX4_MACROS_H
#define _PX4_MACROS_H


#எனில் !defined(arraySize)
#define arraySize(a) (அளவு((a))/அளவு((a[0])))
#endif

#எனில் !defined(NO_BLOAT)
#எனில் defined(__BASE_FILE__)
#define _FILE_NAME_ __BASE_FILE__
#இல்லையெனில்
#define _FILE_NAME_ __FILE__
#endif
#இல்லையெனில்
#define _FILE_NAME_ ""
#endif

#எனில் !defined(INVALID_CASE)
#define INVALID_CASE(c) printf("Invalid Case %d, %s:%d",(c),__BASE_FILE__,__LINE__) /* todo use PANIC */
#endif

#எனில் !defined(UNUSED)
#define UNUSED(var) (வெறுமை)(var)
#endif

#எனில் !defined(CAT)
#எனில் !defined(_CAT)
#define _CAT(a, b) a ## b
#endif
#define CAT(a, b) _CAT(a, b)
#endif

#எனில் !defined(FREEZE_STR)
#  define FREEZE_STR(s) #s
#endif

#எனில் !defined(STRINGIFY)
#define STRINGIFY(s) FREEZE_STR(s)
#endif

#எனில் !defined(CCASSERT)
#எனில் defined(static_assert)
#		define CCASSERT(predicate) static_assert(predicate, STRINGIFY(predicate))
#	இல்லையெனில்
#		define CCASSERT(predicate) x_CCASSERT_வரி(predicate, __LINE__)
#		எனில் !defined(x_CCASSERT_வரி)
#			define x_CCASSERT_வரி(predicate, வரி) வகைவரை எழுத்து CAT(constraint_violated_on_வரி,வரி)[2*((predicate)!=0)-1] __attribute__ ((unused)) ;
#		endif
#	endif
#endif

#எனில் !defined(DO_PRAGMA)
#  define DO_PRAGMA(x) _Pragma (#x)
#endif

#எனில் !defined(TODO)
#  define TODO(x) DO_PRAGMA(message ("TODO - " #x))
#endif

#endif /* _PX4_MACROS_H */
