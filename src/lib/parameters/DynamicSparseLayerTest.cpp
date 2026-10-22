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

#அந்தர்பாவ <gtest/gtest.h>

#அந்தர்பாவ <parameters/px4_parameters.hpp>
#அந்தர்பாவ "DynamicSparseLayer.h"

#அந்தர்பாவ <atomic>
#அந்தர்பாவ <thread>
#அந்தர்பாவ <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
வகுப்பு StubParamLayer : பொது ParamLayer
{
பொது:
	StubParamLayer() : ParamLayer(nullptr) {}
	மெய்நிகர் ~StubParamLayer() = இயல்பு;

	தருக்க store(param_t, param_மதிப்பு_u) override { திரும்பு பொய்; }
	தருக்க contains(param_t) மாறிலி override { திரும்பு பொய்; }

	px4::AtomicBitset<PARAM_மொத்தம்> containedAsBitset() மாறிலி override
	{
		திரும்பு px4::AtomicBitset<PARAM_மொத்தம்>();
	}

	param_மதிப்பு_u get(param_t) மாறிலி override
	{
		param_மதிப்பு_u v{};
		v.i = 0;
		திரும்பு v;
	}

	வெறுமை reset(param_t) override {}
	வெறுமை refresh(param_t) override {}
	முழு size() மாறிலி override { திரும்பு 0; }
	முழு byteSize() மாறிலி override { திரும்பு 0; }
};

வகுப்பு DynamicSparseLayerConcurrentTest : பொது ::testing::Test
{
பாதுகாக்கப்பட்ட:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	க்கு (முழு rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr முழு இலக்கம்_PARAMS = 100;
		std::atomic<தருக்க> stop{பொய்};

		std::thread writer([&]() {
			க்கு (முழு i = 0; i < இலக்கம்_PARAMS; i++) {
				param_மதிப்பு_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(உண்மை, std::memory_order_release);
		});

		std::vector<std::thread> readers;

		க்கு (முழு r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				போது (!stop.load(std::memory_order_acquire)) {
					முழு current_size = layer.size();

					க்கு (முழு i = 0; i < current_size && i < இலக்கம்_PARAMS; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		க்கு (auto &t : readers) {
			t.join();
		}

		க்கு (முழு i = 0; i < இலக்கம்_PARAMS; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_மதிப்பு_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.size(), இலக்கம்_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentMultipleWriters)
{
	constexpr முழு PARAMS_PER_WRITER = 50;
	constexpr முழு இலக்கம்_WRITERS = 4;
	constexpr முழு TOTAL = இலக்கம்_WRITERS * PARAMS_PER_WRITER;

	க்கு (முழு rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/TOTAL, /*n_grow=*/1);

		std::vector<std::thread> writers;

		க்கு (முழு w = 0; w < இலக்கம்_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				முழு base = w * PARAMS_PER_WRITER;

				க்கு (முழு i = 0; i < PARAMS_PER_WRITER; i++) {
					param_மதிப்பு_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		க்கு (auto &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		க்கு (முழு i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_மதிப்பு_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr முழு PARAMS_PER_WRITER = 50;
	constexpr முழு இலக்கம்_WRITERS = 2;
	constexpr முழு TOTAL = இலக்கம்_WRITERS * PARAMS_PER_WRITER;

	க்கு (முழு rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<தருக்க> stop{பொய்};

		std::vector<std::thread> writers;

		க்கு (முழு w = 0; w < இலக்கம்_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				முழு base = w * PARAMS_PER_WRITER;

				க்கு (முழு i = 0; i < PARAMS_PER_WRITER; i++) {
					param_மதிப்பு_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::vector<std::thread> readers;

		க்கு (முழு r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				போது (!stop.load(std::memory_order_acquire)) {
					முழு current_size = layer.size();

					க்கு (முழு i = 0; i < current_size && i < TOTAL; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		க்கு (auto &t : writers) {
			t.join();
		}

		stop.store(உண்மை, std::memory_order_release);

		க்கு (auto &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		க்கு (முழு i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_மதிப்பு_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
