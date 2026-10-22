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

#جيڪڏهن_وصف_نه_ٿيل _PX4_MACROS_H
#وصف_ڪريو _PX4_MACROS_H


#إذا !defined(array_سائيز)
#وصف_ڪريو array_سائيز(a) (حجم((a))/حجم((a[0])))
#جيڪڏهن_پورو

#إذا !defined(NO_BLOAT)
#إذا defined(__BASE_FILE__)
#وصف_ڪريو _FILE_NAME_ __BASE_FILE__
#وإلا
#وصف_ڪريو _FILE_NAME_ __FILE__
#جيڪڏهن_پورو
#وإلا
#وصف_ڪريو _FILE_NAME_ ""
#جيڪڏهن_پورو

#إذا !defined(INVALID_CASE)
#وصف_ڪريو INVALID_CASE(c) لکو("Invalid Case %d, %s:%d",(c),__BASE_FILE__,__LINE__) /* todo use PANIC */
#جيڪڏهن_پورو

#إذا !defined(اڻ_استعمال_ٿيل_2)
#وصف_ڪريو اڻ_استعمال_ٿيل_2(var) (فراغ)(var)
#جيڪڏهن_پورو

#إذا !defined(CAT)
#إذا !defined(_CAT)
#وصف_ڪريو _CAT(a, b) a ## b
#جيڪڏهن_پورو
#وصف_ڪريو CAT(a, b) _CAT(a, b)
#جيڪڏهن_پورو

#إذا !defined(FREEZE_STR)
#  define FREEZE_STR(s) #s
#جيڪڏهن_پورو

#إذا !defined(STRINGIFY)
#وصف_ڪريو STRINGIFY(s) FREEZE_STR(s)
#جيڪڏهن_پورو

#إذا !defined(CCASSERT)
#إذا defined(static_assert)
#		define CCASSERT(predicate) static_assert(predicate, STRINGIFY(predicate))
#	وإلا
#		define CCASSERT(predicate) x_CCASSERT_سٽ(predicate, __LINE__)
#		إذا !defined(x_CCASSERT_سٽ)
#			define x_CCASSERT_سٽ(predicate, سٽ) تعريفنوع حرف CAT(constraint_violated_on_سٽ,سٽ)[2*((predicate)!=0)-1] __attribute__ ((اڻ_استعمال_ٿيل)) ;
#		endif
#	endif
#جيڪڏهن_پورو

#إذا !defined(DO_PRAGMA)
#  define DO_PRAGMA(x) _Pragma (#x)
#جيڪڏهن_پورو

#إذا !defined(TODO)
#  define TODO(x) DO_PRAGMA(پيغام_2 ("TODO - " #x))
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* _PX4_MACROS_H */
