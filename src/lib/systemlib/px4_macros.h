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
#అన్తర్భావ <assert.h>

#నిర్వచితం_కాకపోతే _PX4_MACROS_H
#నిర్వచించు _PX4_MACROS_H


#ఒకవేళ !defined(array_కొలత)
#నిర్వచించు array_కొలత(a) (పరిమాణం((a))/పరిమాణం((a[0])))
#అయితే_ముగింపు

#ఒకవేళ !defined(NO_BLOAT)
#ఒకవేళ defined(__BASE_FILE__)
#నిర్వచించు _FILE_NAME_ __BASE_FILE__
#లేకపోతే
#నిర్వచించు _FILE_NAME_ __FILE__
#అయితే_ముగింపు
#లేకపోతే
#నిర్వచించు _FILE_NAME_ ""
#అయితే_ముగింపు

#ఒకవేళ !defined(INVALID_CASE)
#నిర్వచించు INVALID_CASE(c) రాయి("Invalid Case %d, %s:%d",(c),__BASE_FILE__,__LINE__) /* todo use PANIC */
#అయితే_ముగింపు

#ఒకవేళ !defined(వాడని_2)
#నిర్వచించు వాడని_2(var) (శూన్య)(var)
#అయితే_ముగింపు

#ఒకవేళ !defined(CAT)
#ఒకవేళ !defined(_CAT)
#నిర్వచించు _CAT(a, b) a ## b
#అయితే_ముగింపు
#నిర్వచించు CAT(a, b) _CAT(a, b)
#అయితే_ముగింపు

#ఒకవేళ !defined(FREEZE_STR)
#  define FREEZE_STR(s) #s
#అయితే_ముగింపు

#ఒకవేళ !defined(STRINGIFY)
#నిర్వచించు STRINGIFY(s) FREEZE_STR(s)
#అయితే_ముగింపు

#ఒకవేళ !defined(CCASSERT)
#ఒకవేళ defined(static_assert)
#		define CCASSERT(predicate) static_assert(predicate, STRINGIFY(predicate))
#	లేకపోతే
#		define CCASSERT(predicate) x_CCASSERT_పంక్తి(predicate, __LINE__)
#		ఒకవేళ !defined(x_CCASSERT_పంక్తి)
#			define x_CCASSERT_పంక్తి(predicate, పంక్తి) రకనిర్వచనం అక్షరం CAT(constraint_violated_on_పంక్తి,పంక్తి)[2*((predicate)!=0)-1] __attribute__ ((వాడని)) ;
#		endif
#	endif
#అయితే_ముగింపు

#ఒకవేళ !defined(DO_PRAGMA)
#  define DO_PRAGMA(x) _Pragma (#x)
#అయితే_ముగింపు

#ఒకవేళ !defined(TODO)
#  define TODO(x) DO_PRAGMA(సందేశం_3 ("TODO - " #x))
#అయితే_ముగింపు

#అయితే_ముగింపు /* _PX4_MACROS_H */
