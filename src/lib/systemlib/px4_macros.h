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
#شامل <assert.h>

#اگر_غیر_تعریف_شدہ _PX4_MACROS_H
#تعریف_کرو _PX4_MACROS_H


#اگر !defined(array_سائز)
#تعریف_کرو array_سائز(a) (حجم((a))/حجم((a[0])))
#اگر_ختم

#اگر !defined(NO_BLOAT)
#اگر defined(__BASE_FILE__)
#تعریف_کرو _FILE_NAME_ __BASE_FILE__
#ورنہ
#تعریف_کرو _FILE_NAME_ __FILE__
#اگر_ختم
#ورنہ
#تعریف_کرو _FILE_NAME_ ""
#اگر_ختم

#اگر !defined(INVALID_CASE)
#تعریف_کرو INVALID_CASE(c) لکھو("Invalid Case %d, %s:%d",(c),__BASE_FILE__,__LINE__) /* todo use PANIC */
#اگر_ختم

#اگر !defined(غیر_مستعمل_2)
#تعریف_کرو غیر_مستعمل_2(var) (خالی)(var)
#اگر_ختم

#اگر !defined(CAT)
#اگر !defined(_CAT)
#تعریف_کرو _CAT(a, b) a ## b
#اگر_ختم
#تعریف_کرو CAT(a, b) _CAT(a, b)
#اگر_ختم

#اگر !defined(FREEZE_STR)
#  define FREEZE_STR(s) #s
#اگر_ختم

#اگر !defined(STRINGIFY)
#تعریف_کرو STRINGIFY(s) FREEZE_STR(s)
#اگر_ختم

#اگر !defined(CCASSERT)
#اگر defined(static_assert)
#		define CCASSERT(predicate) static_assert(predicate, STRINGIFY(predicate))
#	ورنہ
#		define CCASSERT(predicate) x_CCASSERT_سطر(predicate, __LINE__)
#		اگر !defined(x_CCASSERT_سطر)
#			define x_CCASSERT_سطر(predicate, سطر) قسمتعریف حرف CAT(constraint_violated_on_سطر,سطر)[2*((predicate)!=0)-1] __attribute__ ((غیر_مستعمل)) ;
#		endif
#	endif
#اگر_ختم

#اگر !defined(DO_PRAGMA)
#  define DO_PRAGMA(x) _Pragma (#x)
#اگر_ختم

#اگر !defined(TODO)
#  define TODO(x) DO_PRAGMA(پیغام_3 ("TODO - " #x))
#اگر_ختم

#اگر_ختم /* _PX4_MACROS_H */
