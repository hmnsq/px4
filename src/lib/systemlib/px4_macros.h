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
#ಅನ್ತರ್ಭಾವ <assert.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _PX4_MACROS_H
#ವ್ಯಾಖ್ಯಾನಿಸು _PX4_MACROS_H


#ಆದರೆ !defined(array_ಗಾತ್ರ)
#ವ್ಯಾಖ್ಯಾನಿಸು array_ಗಾತ್ರ(a) (ಗಾತ್ರ((a))/ಗಾತ್ರ((a[0])))
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(NO_BLOAT)
#ಆದರೆ defined(__BASE_FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು _FILE_NAME_ __BASE_FILE__
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು _FILE_NAME_ __FILE__
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು _FILE_NAME_ ""
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(INVALID_CASE)
#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_CASE(c) ಬರೆ("Invalid Case %d, %s:%d",(c),__BASE_FILE__,__LINE__) /* todo use PANIC */
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(ಬಳಸದ_2)
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಳಸದ_2(var) (ಶೂನ್ಯ)(var)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CAT)
#ಆದರೆ !defined(_CAT)
#ವ್ಯಾಖ್ಯಾನಿಸು _CAT(a, b) a ## b
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು CAT(a, b) _CAT(a, b)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(FREEZE_STR)
#  define FREEZE_STR(s) #s
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(STRINGIFY)
#ವ್ಯಾಖ್ಯಾನಿಸು STRINGIFY(s) FREEZE_STR(s)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CCASSERT)
#ಆದರೆ defined(static_assert)
#		define CCASSERT(predicate) static_assert(predicate, STRINGIFY(predicate))
#	ಇಲ್ಲದಿದ್ದರೆ
#		define CCASSERT(predicate) x_CCASSERT_ಸಾಲು(predicate, __LINE__)
#		ಆದರೆ !defined(x_CCASSERT_ಸಾಲು)
#			define x_CCASSERT_ಸಾಲು(predicate, ಸಾಲು) ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ CAT(constraint_violated_on_ಸಾಲು,ಸಾಲು)[2*((predicate)!=0)-1] __attribute__ ((ಬಳಸದ)) ;
#		endif
#	endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(DO_PRAGMA)
#  define DO_PRAGMA(x) _Pragma (#x)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(TODO)
#  define TODO(x) DO_PRAGMA(ಸಂದೇಶ_3 ("TODO - " #x))
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* _PX4_MACROS_H */
