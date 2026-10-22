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

#ضمن "spartn.h"

نطاق gnss
{

uint32_t spartn_پیام_بررسی_چرخهای(ثابت uint8_t *داده, size_t len, uint8_t بررسی_چرخهای_type)
{
	// Use uint64_t so n==32 (CRC-32) does not shift a 32-bit value by 32.
	ثابت بیعلامت n = 8u * (static_cast<بیعلامت>(بررسی_چرخهای_type) + 1u);
	ثابت uint64_t poly = (بررسی_چرخهای_type == 0) ? 0x07ull :
			      (بررسی_چرخهای_type == 1) ? 0x1021ull :
			      (بررسی_چرخهای_type == 2) ? 0x864CFBull : 0x04C11DB7ull;
	ثابت uint64_t بالاترین = 1ull << n;
	ثابت uint64_t g = بالاترین | poly;
	uint64_t بررسی_چرخهای = (بررسی_چرخهای_type == 3) ? 0xFFFFFFFFull : 0ull;

	لكل (size_t i = 0; i < len; i++) {
		بررسی_چرخهای ^= static_cast<uint64_t>(داده[i]) << (n - 8);

		لكل (صحيح b = 0; b < 8; b++) {
			بررسی_چرخهای <<= 1;

			إذا (بررسی_چرخهای & بالاترین) {
				بررسی_چرخهای ^= g;
			}
		}
	}

	إذا (بررسی_چرخهای_type == 3) {
		بررسی_چرخهای ^= 0xFFFFFFFFull;
	}

	أرجع static_cast<uint32_t>(بررسی_چرخهای & (بالاترین - 1ull));
}

} // namespace gnss
