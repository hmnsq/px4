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
#अन्तर्भाव <assert.h>

#यदि_अपरिभाषितम् _PX4_MACROS_H
#परिभाषय _PX4_MACROS_H


#यदि !defined(array_आकार)
#परिभाषय array_आकार(a) (परिमाण((a))/परिमाण((a[0])))
#यदि_अन्तः

#यदि !defined(NO_BLOAT)
#यदि defined(__BASE_FILE__)
#परिभाषय _FILE_NAME_ __BASE_FILE__
#अन्यथा
#परिभाषय _FILE_NAME_ __FILE__
#यदि_अन्तः
#अन्यथा
#परिभाषय _FILE_NAME_ ""
#यदि_अन्तः

#यदि !defined(INVALID_CASE)
#परिभाषय INVALID_CASE(c) प्रारूप_लिख("Invalid Case %d, %s:%d",(c),__BASE_FILE__,__LINE__) /* todo use PANIC */
#यदि_अन्तः

#यदि !defined(अप्रयुक्तम्_2)
#परिभाषय अप्रयुक्तम्_2(var) (शून्य)(var)
#यदि_अन्तः

#यदि !defined(CAT)
#यदि !defined(_CAT)
#परिभाषय _CAT(a, b) a ## b
#यदि_अन्तः
#परिभाषय CAT(a, b) _CAT(a, b)
#यदि_अन्तः

#यदि !defined(FREEZE_STR)
#  define FREEZE_STR(s) #s
#यदि_अन्तः

#यदि !defined(STRINGIFY)
#परिभाषय STRINGIFY(s) FREEZE_STR(s)
#यदि_अन्तः

#यदि !defined(CCASSERT)
#यदि defined(static_assert)
#		define CCASSERT(predicate) static_assert(predicate, STRINGIFY(predicate))
#	अन्यथा
#		define CCASSERT(predicate) x_CCASSERT_पङ्क्तिः(predicate, __LINE__)
#		यदि !defined(x_CCASSERT_पङ्क्तिः)
#			define x_CCASSERT_पङ्क्तिः(predicate, पङ्क्तिः) प्रकारनिर्देश वर्ण CAT(constraint_violated_on_पङ्क्तिः,पङ्क्तिः)[2*((predicate)!=0)-1] __attribute__ ((अप्रयुक्तम्)) ;
#		endif
#	endif
#यदि_अन्तः

#यदि !defined(DO_PRAGMA)
#  define DO_PRAGMA(x) _Pragma (#x)
#यदि_अन्तः

#यदि !defined(TODO)
#  define TODO(x) DO_PRAGMA(सन्देशः_3 ("TODO - " #x))
#यदि_अन्तः

#यदि_अन्तः /* _PX4_MACROS_H */
