/****************************************************************************
 *
 *   Copyright (C) 2022 PX4 Development Team. All rights reserved.
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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <matrix/math.hpp>

ఉపయోగించి నామాకాశ మాతృక;

TEST(మాతృక_సమితి_తటస్థం_Test, సమితి_తటస్థం_2)
{
	Matrix3f A;
	A.సమితి_తటస్థం();

	కొరకు (size_t i = 0; i < 3; i++) {
		కొరకు (size_t j = 0; j < 3; j++) {
			ఒకవేళ (i == j) {
				EXPECT_FLOAT_EQ(A(i, j), 1);

			} లేకపోతే {
				EXPECT_FLOAT_EQ(A(i, j), 0);
			}
		}
	}

	Matrix3f B;
	B.తటస్థం();

	కొరకు (size_t i = 0; i < 3; i++) {
		కొరకు (size_t j = 0; j < 3; j++) {
			ఒకవేళ (i == j) {
				EXPECT_FLOAT_EQ(B(i, j), 1);

			} లేకపోతే {
				EXPECT_FLOAT_EQ(B(i, j), 0);
			}
		}
	}
}
