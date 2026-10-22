/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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
 * @file spartn.cpp
 *
 * SPARTN transport-layer implementation.
 */

#ಅನ್ತರ್ಭಾವ "spartn.h"

ನಾಮಾಕಾಶ gnss
{

uint32_t spartn_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸ್ಥಿರಾಂಕ uint8_t *ದತ್ತಾಂಶ, size_t len, uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type)
{
	// Use uint64_t so n==32 (CRC-32) does not shift a 32-bit value by 32.
	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ n = 8u * (static_cast<ಚಿಹ್ನೆರಹಿತ>(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type) + 1u);
	ಸ್ಥಿರಾಂಕ uint64_t poly = (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type == 0) ? 0x07ull :
			      (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type == 1) ? 0x1021ull :
			      (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type == 2) ? 0x864CFBull : 0x04C11DB7ull;
	ಸ್ಥಿರಾಂಕ uint64_t ತುದಿ = 1ull << n;
	ಸ್ಥಿರಾಂಕ uint64_t g = ತುದಿ | poly;
	uint64_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type == 3) ? 0xFFFFFFFFull : 0ull;

	ಗಾಗಿ (size_t i = 0; i < len; i++) {
		ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ ^= static_cast<uint64_t>(ದತ್ತಾಂಶ[i]) << (n - 8);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ b = 0; b < 8; b++) {
			ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ <<= 1;

			ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & ತುದಿ) {
				ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ ^= g;
			}
		}
	}

	ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type == 3) {
		ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ ^= 0xFFFFFFFFull;
	}

	ಹಿಂತಿರುಗಿ static_cast<uint32_t>(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & (ತುದಿ - 1ull));
}

} // namespace gnss
