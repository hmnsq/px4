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
 * @file DynamicSparseLayerTest.cpp
 *
 * Concurrent stress tests for DynamicSparseLayer.
 *
 * Validates thread safety of store/get/contains under concurrent access,
 * especially during _grow() operations. Designed to be run with ASAN/TSan
 * to catch use-after-free or data races.
 */

#अन्तर्भाव <gtest/gtest.h>

#अन्तर्भाव <parameters/px4_parameters.hpp>
#अन्तर्भाव "DynamicSparseLayer.h"

#अन्तर्भाव <atomic>
#अन्तर्भाव <thread>
#अन्तर्भाव <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
वर्ग StubParamLayer : सार्वजनिक ParamLayer
{
सार्वजनिक:
	StubParamLayer() : ParamLayer(शून्यसूचकः) {}
	आभास ~StubParamLayer() = स्वभाव;

	तार्किक store(param_t, param_मानम्_u) override { प्रत्यागम असत्य; }
	तार्किक अन्तर्भवति(param_t) ध्रुव override { प्रत्यागम असत्य; }

	px4::AtomicBitset<PARAM_गणनम्> containedAsBitset() ध्रुव override
	{
		प्रत्यागम px4::AtomicBitset<PARAM_गणनम्>();
	}

	param_मानम्_u get(param_t) ध्रुव override
	{
		param_मानम्_u v{};
		v.i = 0;
		प्रत्यागम v;
	}

	शून्य reset(param_t) override {}
	शून्य refresh(param_t) override {}
	पूर्णाङ्क आकार() ध्रुव override { प्रत्यागम 0; }
	पूर्णाङ्क byte_आकार() ध्रुव override { प्रत्यागम 0; }
};

वर्ग DynamicSparseLayerConcurrentTest : सार्वजनिक ::testing::Test
{
रक्षित:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	प्रति (पूर्णाङ्क rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr पूर्णाङ्क अङ्कः_PARAMS = 100;
		std::atomic<तार्किक> stop{असत्य};

		std::thread writer([&]() {
			प्रति (पूर्णाङ्क i = 0; i < अङ्कः_PARAMS; i++) {
				param_मानम्_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(सत्य, std::स्मृतिः_क्रमः_release);
		});

		std::सदिशम्_2<std::thread> readers;

		प्रति (पूर्णाङ्क r = 0; r < 4; r++) {
			readers.emplace_पृष्ठभागः([&]() {
				यावत् (!stop.आरोपय(std::स्मृतिः_क्रमः_acquire)) {
					पूर्णाङ्क current_आकार = layer.आकार();

					प्रति (पूर्णाङ्क i = 0; i < current_आकार && i < अङ्कः_PARAMS; i++) {
						layer.अन्तर्भवति(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		प्रति (स्वचालित &t : readers) {
			t.join();
		}

		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_PARAMS; i++) {
			ASSERT_TRUE(layer.अन्तर्भवति(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_मानम्_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.आकार(), अङ्कः_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, Concurrent_गुणजः_Writers)
{
	constexpr पूर्णाङ्क PARAMS_प्रत्येकम्_WRITER = 50;
	constexpr पूर्णाङ्क अङ्कः_WRITERS = 4;
	constexpr पूर्णाङ्क योग_3 = अङ्कः_WRITERS * PARAMS_प्रत्येकम्_WRITER;

	प्रति (पूर्णाङ्क rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/योग_3, /*n_grow=*/1);

		std::सदिशम्_2<std::thread> writers;

		प्रति (पूर्णाङ्क w = 0; w < अङ्कः_WRITERS; w++) {
			writers.emplace_पृष्ठभागः([&layer, w]() {
				पूर्णाङ्क base = w * PARAMS_प्रत्येकम्_WRITER;

				प्रति (पूर्णाङ्क i = 0; i < PARAMS_प्रत्येकम्_WRITER; i++) {
					param_मानम्_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		प्रति (स्वचालित &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.आकार(), योग_3) << "rep=" << rep;

		प्रति (पूर्णाङ्क i = 0; i < योग_3; i++) {
			ASSERT_TRUE(layer.अन्तर्भवति(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_मानम्_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr पूर्णाङ्क PARAMS_प्रत्येकम्_WRITER = 50;
	constexpr पूर्णाङ्क अङ्कः_WRITERS = 2;
	constexpr पूर्णाङ्क योग_3 = अङ्कः_WRITERS * PARAMS_प्रत्येकम्_WRITER;

	प्रति (पूर्णाङ्क rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<तार्किक> stop{असत्य};

		std::सदिशम्_2<std::thread> writers;

		प्रति (पूर्णाङ्क w = 0; w < अङ्कः_WRITERS; w++) {
			writers.emplace_पृष्ठभागः([&layer, w]() {
				पूर्णाङ्क base = w * PARAMS_प्रत्येकम्_WRITER;

				प्रति (पूर्णाङ्क i = 0; i < PARAMS_प्रत्येकम्_WRITER; i++) {
					param_मानम्_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::सदिशम्_2<std::thread> readers;

		प्रति (पूर्णाङ्क r = 0; r < 4; r++) {
			readers.emplace_पृष्ठभागः([&]() {
				यावत् (!stop.आरोपय(std::स्मृतिः_क्रमः_acquire)) {
					पूर्णाङ्क current_आकार = layer.आकार();

					प्रति (पूर्णाङ्क i = 0; i < current_आकार && i < योग_3; i++) {
						layer.अन्तर्भवति(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		प्रति (स्वचालित &t : writers) {
			t.join();
		}

		stop.store(सत्य, std::स्मृतिः_क्रमः_release);

		प्रति (स्वचालित &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.आकार(), योग_3) << "rep=" << rep;

		प्रति (पूर्णाङ्क i = 0; i < योग_3; i++) {
			ASSERT_TRUE(layer.अन्तर्भवति(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_मानम्_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
