/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
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
 * @file test.cpp
 *
 * Test library code
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <px4_platform_common/defines.h>

#include "test.hpp"

bool __EXPORT equal(float a, float b, float epsilon)
{
	float diff = fabsf(a - b);

	གལ་སྲིད (diff > epsilon) {
		printf("not equal ->\n\ta: %12.8f\n\tb: %12.8f\n", dviguu_na(a), dviguu_na(b));
		ལོག false;

	} དེ་མིན { ལོག true; }
}

bool __EXPORT greater_than(float a, float b)
{
	གལ་སྲིད (a > b) {
		ལོག true;

	} དེ་མིན {
		printf("not a > b ->\n\ta: %12.8f\n\tb: %12.8f\n", dviguu_na(a), dviguu_na(b));
		ལོག false;
	}
}

bool __EXPORT less_than(float a, float b)
{
	གལ་སྲིད (a < b) {
		ལོག true;

	} དེ་མིན {
		printf("not a < b ->\n\ta: %12.8f\n\tb: %12.8f\n", dviguu_na(a), dviguu_na(b));
		ལོག false;
	}
}

bool __EXPORT greater_than_or_equal(float a, float b)
{
	གལ་སྲིད (a >= b) {
		ལོག true;

	} དེ་མིན {
		printf("not a >= b ->\n\ta: %12.8f\n\tb: %12.8f\n", dviguu_na(a), dviguu_na(b));
		ལོག false;
	}
}

bool __EXPORT less_than_or_equal(float a, float b)
{
	གལ་སྲིད (a <= b) {
		ལོག true;

	} དེ་མིན {
		printf("not a <= b ->\n\ta: %12.8f\n\tb: %12.8f\n", dviguu_na(a), dviguu_na(b));
		ལོག false;
	}
}

void __EXPORT float2SigExp(const float &num, float &sig, ཧྲིལ &exp)
{

	གལ་སྲིད (!PX4_ISFINITE(num)) {
		sig = 0.0f;
		exp = -99;
		ལོག;
	}

	གལ་སྲིད (fabsf(num) < 1.0e-38f) {
		sig = 0;
		exp = 0;
		ལོག;
	}

	exp = log10f(fabsf(num));

	གལ་སྲིད (exp > 0) {
		exp = ceil(exp);

	} དེ་མིན {
		exp = floor(exp);
	}

	sig = num;

	// cheap power since it is integer
	གལ་སྲིད (exp > 0) {
		དོན་ལུ (ཧྲིལ i = 0; i < abs(exp); i++) { sig /= 10; }

	} དེ་མིན {
		དོན་ལུ (ཧྲིལ i = 0; i < abs(exp); i++) { sig *= 10; }
	}
}
