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

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>

#ଅନ୍ତର୍ଭାଵ "test.hpp"

ତାର୍କିକ __EXPORT equal(ଦଶମିକ a, ଦଶମିକ b, ଦଶମିକ epsilon)
{
	ଦଶମିକ diff = fabsf(a - b);

	ଯଦି (diff > epsilon) {
		printf("not equal ->\n\ta: %12.8f\n\tb: %12.8f\n", ଦ୍ୱିଗୁଣ(a), ଦ୍ୱିଗୁଣ(b));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	} ଅନ୍ୟଥା { ଫେରନ୍ତୁ ସତ୍ୟ; }
}

ତାର୍କିକ __EXPORT greater_than(ଦଶମିକ a, ଦଶମିକ b)
{
	ଯଦି (a > b) {
		ଫେରନ୍ତୁ ସତ୍ୟ;

	} ଅନ୍ୟଥା {
		printf("not a > b ->\n\ta: %12.8f\n\tb: %12.8f\n", ଦ୍ୱିଗୁଣ(a), ଦ୍ୱିଗୁଣ(b));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
}

ତାର୍କିକ __EXPORT less_than(ଦଶମିକ a, ଦଶମିକ b)
{
	ଯଦି (a < b) {
		ଫେରନ୍ତୁ ସତ୍ୟ;

	} ଅନ୍ୟଥା {
		printf("not a < b ->\n\ta: %12.8f\n\tb: %12.8f\n", ଦ୍ୱିଗୁଣ(a), ଦ୍ୱିଗୁଣ(b));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
}

ତାର୍କିକ __EXPORT greater_than_or_equal(ଦଶମିକ a, ଦଶମିକ b)
{
	ଯଦି (a >= b) {
		ଫେରନ୍ତୁ ସତ୍ୟ;

	} ଅନ୍ୟଥା {
		printf("not a >= b ->\n\ta: %12.8f\n\tb: %12.8f\n", ଦ୍ୱିଗୁଣ(a), ଦ୍ୱିଗୁଣ(b));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
}

ତାର୍କିକ __EXPORT less_than_or_equal(ଦଶମିକ a, ଦଶମିକ b)
{
	ଯଦି (a <= b) {
		ଫେରନ୍ତୁ ସତ୍ୟ;

	} ଅନ୍ୟଥା {
		printf("not a <= b ->\n\ta: %12.8f\n\tb: %12.8f\n", ଦ୍ୱିଗୁଣ(a), ଦ୍ୱିଗୁଣ(b));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
}

ଶୂନ୍ୟ __EXPORT float2SigExp(ଅଚଳ ଦଶମିକ &ଅଙ୍କ, ଦଶମିକ &sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &exp)
{

	ଯଦି (!PX4_ISFINITE(ଅଙ୍କ)) {
		sig = 0.0f;
		exp = -99;
		ଫେରନ୍ତୁ;
	}

	ଯଦି (fabsf(ଅଙ୍କ) < 1.0e-38f) {
		sig = 0;
		exp = 0;
		ଫେରନ୍ତୁ;
	}

	exp = log10f(fabsf(ଅଙ୍କ));

	ଯଦି (exp > 0) {
		exp = ceil(exp);

	} ଅନ୍ୟଥା {
		exp = floor(exp);
	}

	sig = ଅଙ୍କ;

	// cheap power since it is integer
	ଯଦି (exp > 0) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < abs(exp); i++) { sig /= 10; }

	} ଅନ୍ୟଥା {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < abs(exp); i++) { sig *= 10; }
	}
}
