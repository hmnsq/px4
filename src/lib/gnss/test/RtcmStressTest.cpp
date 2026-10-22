/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#شامل "RtcmTestCommon.hpp"

TEST_F(RtcmTest, Stress_Many_Small_Valid_چوکھٹے)
{
	// Simulate a good sender sending many small messages
	// Process as we go to avoid buffer overflow
	ثابت صحیح عدد_چوکھٹے = 1000;
	صحیح گنتی = 0;

	برائے (صحیح i = 0; i < عدد_چوکھٹے; i++) {
		خودکار چوکھٹا = build_Valid_چوکھٹا(1005 + (i % 100), {static_cast<uint8_t>(i & 0xFF)});
		parser.جوڑو_ڈیٹا(چوکھٹا.ڈیٹا(), چوکھٹا.سائز());

		// Consume available messages after each add
		size_t len;

		جب (parser.get_اگلا_پیغام(&len) != خالی_اشاریہ) {
			parser.consume_پیغام(len);
			گنتی++;
		}
	}

	EXPECT_EQ(گنتی, عدد_چوکھٹے);
	خودکار stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, static_cast<uint32_t>(عدد_چوکھٹے));
	EXPECT_EQ(stats.بائٹیں_discarded_2, 0u);
	EXPECT_EQ(stats.چکری_جانچ_errors_2, 0u);
}

TEST_F(RtcmTest, Stress_Long_Garbage_دھارا_Then_Valid)
{
	// This tests the pathological case Alex mentioned: lots of invalid data
	// causing many memmove operations during preamble search
	ثابت size_t garbage_سائز = 10000;
	std::سمتیہ_2<uint8_t> garbage(garbage_سائز);
	std::mt19937 rng(99);

	برائے (خودکار &byte : garbage) {
		// Avoid every protocol preamble to ensure maximum discards: 0xD3 (RTCM3)
		// and 0x73 (SPARTN), which the framer also treats as a frame start when
		// CONFIG_GPS_SPARTN is enabled.
		byte = (rng() % 0x72) + 1;  // 0x01 to 0x72, never 0x73 or 0xD3
	}

	// Add in chunks to simulate streaming
	size_t انحراف = 0;

	جب (انحراف < garbage.سائز()) {
		size_t chunk = std::min<size_t>(256, garbage.سائز() - انحراف);
		parser.جوڑو_ڈیٹا(&garbage[انحراف], chunk);

		// Try to parse (forces discarding)
		size_t len;
		parser.get_اگلا_پیغام(&len);
		انحراف += chunk;
	}

	// Now add a valid frame
	خودکار valid = build_Valid_چوکھٹا(1005, {0x01, 0x02, 0x03});
	parser.جوڑو_ڈیٹا(valid.ڈیٹا(), valid.سائز());

	size_t len;
	ثابت uint8_t *پیغام_2 = parser.get_اگلا_پیغام(&len);
	ASSERT_NE(پیغام_2, خالی_اشاریہ);

	parser.consume_پیغام(len);
	خودکار stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.بائٹیں_discarded_2, garbage_سائز);
}

TEST_F(RtcmTest, Stress_InterleavedGarbageAndValid)
{
	// Simulate a flaky sender that mixes garbage with valid frames
	ثابت صحیح عدد_valid = 100;
	صحیح parsed = 0;

	برائے (صحیح i = 0; i < عدد_valid; i++) {
		// Add some garbage (variable amount)
		std::سمتیہ_2<uint8_t> garbage(10 + (i % 50), 0xAA);
		parser.جوڑو_ڈیٹا(garbage.ڈیٹا(), garbage.سائز());

		// Add valid frame
		خودکار چوکھٹا = build_Valid_چوکھٹا(1005, {static_cast<uint8_t>(i)});
		parser.جوڑو_ڈیٹا(چوکھٹا.ڈیٹا(), چوکھٹا.سائز());

		// Parse what we can
		size_t len;

		جب (parser.get_اگلا_پیغام(&len) != خالی_اشاریہ) {
			parser.consume_پیغام(len);
			parsed++;
		}
	}

	EXPECT_EQ(parsed, عدد_valid);
}
