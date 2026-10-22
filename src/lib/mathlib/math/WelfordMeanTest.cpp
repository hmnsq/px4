/****************************************************************************
 *
 *   Copyright (C) 2021-2022 PX4 Development Team. All rights reserved.
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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <random>
#ಅನ್ತರ್ಭಾವ <lib/matrix/matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ "WelfordMean.hpp"

ಬಳಸಿ ನಾಮಾಕಾಶ math;
ಬಳಸಿ ಮಾತೃಕೆ::Vector3f;

TEST(Welford_ಸರಾಸರಿ_Test, NoisySignal)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ std_dev = 3.f;
	std::ಸಾಮಾನ್ಯ_distribution<ದಶಮಾಂಶ> standard_ಸಾಮಾನ್ಯ_distribution{0.f, std_dev};
	std::default_random_engine random_generator{}; // Pseudo-random generator with constant seed
	random_generator.ಬೀಜ(42);
	Welford_ಸರಾಸರಿ<ದಶಮಾಂಶ> welford{};

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 1000; i++) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ noisy_ಮೌಲ್ಯ = standard_ಸಾಮಾನ್ಯ_distribution(random_generator);
		welford.update(noisy_ಮೌಲ್ಯ);
	}

	EXPECT_TRUE(welford.valid());
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸರಾಸರಿ = welford.ಸರಾಸರಿ();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ var = welford.variance();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ var_real = std_dev * std_dev;

	EXPECT_NEAR(ಸರಾಸರಿ, 0.f, 0.7f);

	EXPECT_NEAR(var, var_real, 0.1f);

}
