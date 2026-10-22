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
#ضمن <assert.h>

#که_نه_تعریف_شوی _PX4_MACROS_H
#تعریف_کړه _PX4_MACROS_H


#إذا !defined(array_اندازه)
#تعریف_کړه array_اندازه(a) (حجم((a))/حجم((a[0])))
#د_که_پای

#إذا !defined(NO_BLOAT)
#إذا defined(__BASE_FILE__)
#تعریف_کړه _FILE_NAME_ __BASE_FILE__
#وإلا
#تعریف_کړه _FILE_NAME_ __FILE__
#د_که_پای
#وإلا
#تعریف_کړه _FILE_NAME_ ""
#د_که_پای

#إذا !defined(INVALID_CASE)
#تعریف_کړه INVALID_CASE(c) چاپ_کړه("Invalid Case %d, %s:%d",(c),__BASE_FILE__,__LINE__) /* todo use PANIC */
#د_که_پای

#إذا !defined(ناکارول_شوی_2)
#تعریف_کړه ناکارول_شوی_2(var) (فراغ)(var)
#د_که_پای

#إذا !defined(CAT)
#إذا !defined(_CAT)
#تعریف_کړه _CAT(a, b) a ## b
#د_که_پای
#تعریف_کړه CAT(a, b) _CAT(a, b)
#د_که_پای

#إذا !defined(FREEZE_STR)
#  define FREEZE_STR(s) #s
#د_که_پای

#إذا !defined(STRINGIFY)
#تعریف_کړه STRINGIFY(s) FREEZE_STR(s)
#د_که_پای

#إذا !defined(CCASSERT)
#إذا defined(static_assert)
#		define CCASSERT(predicate) static_assert(predicate, STRINGIFY(predicate))
#	وإلا
#		define CCASSERT(predicate) x_CCASSERT_کرښه(predicate, __LINE__)
#		إذا !defined(x_CCASSERT_کرښه)
#			define x_CCASSERT_کرښه(predicate, کرښه) تعريفنوع حرف CAT(constraint_violated_on_کرښه,کرښه)[2*((predicate)!=0)-1] __attribute__ ((ناکارول_شوی)) ;
#		endif
#	endif
#د_که_پای

#إذا !defined(DO_PRAGMA)
#  define DO_PRAGMA(x) _Pragma (#x)
#د_که_پای

#إذا !defined(TODO)
#  define TODO(x) DO_PRAGMA(پیغام_3 ("TODO - " #x))
#د_که_پای

#د_که_پای /* _PX4_MACROS_H */
