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

#অন্তর্ভাব <gtest/gtest.h>

#অন্তর্ভাব <parameters/px4_parameters.hpp>
#অন্তর্ভাব "DynamicSparseLayer.h"

#অন্তর্ভাব <atomic>
#অন্তর্ভাব <thread>
#অন্তর্ভাব <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
বর্গ StubParamLayer : সার্বজনিক ParamLayer
{
সার্বজনিক:
	StubParamLayer() : ParamLayer(nullptr) {}
	আভাস ~StubParamLayer() = স্বভাব;

	তার্কিক store(param_t, param_মান_u) override { প্রত্যাগম অসত্য; }
	তার্কিক contains(param_t) ধ্রুব override { প্রত্যাগম অসত্য; }

	px4::AtomicBitset<PARAM_গুনতি> containedAsBitset() ধ্রুব override
	{
		প্রত্যাগম px4::AtomicBitset<PARAM_গুনতি>();
	}

	param_মান_u get(param_t) ধ্রুব override
	{
		param_মান_u v{};
		v.i = 0;
		প্রত্যাগম v;
	}

	শূন্য reset(param_t) override {}
	শূন্য refresh(param_t) override {}
	পূর্ণাঙ্ক size() ধ্রুব override { প্রত্যাগম 0; }
	পূর্ণাঙ্ক byteSize() ধ্রুব override { প্রত্যাগম 0; }
};

বর্গ DynamicSparseLayerConcurrentTest : সার্বজনিক ::testing::Test
{
রক্ষিত:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	প্রতি (পূর্ণাঙ্ক rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr পূর্ণাঙ্ক অঙ্ক_PARAMS = 100;
		std::atomic<তার্কিক> stop{অসত্য};

		std::thread writer([&]() {
			প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_PARAMS; i++) {
				param_মান_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(সত্য, std::memory_order_release);
		});

		std::vector<std::thread> readers;

		প্রতি (পূর্ণাঙ্ক r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				যাবত্ (!stop.load(std::memory_order_acquire)) {
					পূর্ণাঙ্ক current_size = layer.size();

					প্রতি (পূর্ণাঙ্ক i = 0; i < current_size && i < অঙ্ক_PARAMS; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		প্রতি (auto &t : readers) {
			t.join();
		}

		প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_PARAMS; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_মান_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.size(), অঙ্ক_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentMultipleWriters)
{
	constexpr পূর্ণাঙ্ক PARAMS_PER_WRITER = 50;
	constexpr পূর্ণাঙ্ক অঙ্ক_WRITERS = 4;
	constexpr পূর্ণাঙ্ক TOTAL = অঙ্ক_WRITERS * PARAMS_PER_WRITER;

	প্রতি (পূর্ণাঙ্ক rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/TOTAL, /*n_grow=*/1);

		std::vector<std::thread> writers;

		প্রতি (পূর্ণাঙ্ক w = 0; w < অঙ্ক_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				পূর্ণাঙ্ক base = w * PARAMS_PER_WRITER;

				প্রতি (পূর্ণাঙ্ক i = 0; i < PARAMS_PER_WRITER; i++) {
					param_মান_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		প্রতি (auto &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		প্রতি (পূর্ণাঙ্ক i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_মান_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr পূর্ণাঙ্ক PARAMS_PER_WRITER = 50;
	constexpr পূর্ণাঙ্ক অঙ্ক_WRITERS = 2;
	constexpr পূর্ণাঙ্ক TOTAL = অঙ্ক_WRITERS * PARAMS_PER_WRITER;

	প্রতি (পূর্ণাঙ্ক rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<তার্কিক> stop{অসত্য};

		std::vector<std::thread> writers;

		প্রতি (পূর্ণাঙ্ক w = 0; w < অঙ্ক_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				পূর্ণাঙ্ক base = w * PARAMS_PER_WRITER;

				প্রতি (পূর্ণাঙ্ক i = 0; i < PARAMS_PER_WRITER; i++) {
					param_মান_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::vector<std::thread> readers;

		প্রতি (পূর্ণাঙ্ক r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				যাবত্ (!stop.load(std::memory_order_acquire)) {
					পূর্ণাঙ্ক current_size = layer.size();

					প্রতি (পূর্ণাঙ্ক i = 0; i < current_size && i < TOTAL; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		প্রতি (auto &t : writers) {
			t.join();
		}

		stop.store(সত্য, std::memory_order_release);

		প্রতি (auto &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		প্রতি (পূর্ণাঙ্ক i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_মান_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
