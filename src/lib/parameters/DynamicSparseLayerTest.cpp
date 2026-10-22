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

#અન્તર્ભાવ <gtest/gtest.h>

#અન્તર્ભાવ <parameters/px4_parameters.hpp>
#અન્તર્ભાવ "DynamicSparseLayer.h"

#અન્તર્ભાવ <atomic>
#અન્તર્ભાવ <thread>
#અન્તર્ભાવ <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
વર્ગ StubParamLayer : જાહેર ParamLayer
{
જાહેર:
	StubParamLayer() : ParamLayer(nullptr) {}
	આભાસી ~StubParamLayer() = મૂળભૂત;

	તાર્કિક store(param_t, param_કિંમત_u) override { પાછા ખોટું; }
	તાર્કિક contains(param_t) અચલ override { પાછા ખોટું; }

	px4::AtomicBitset<PARAM_ગણતરી> containedAsBitset() અચલ override
	{
		પાછા px4::AtomicBitset<PARAM_ગણતરી>();
	}

	param_કિંમત_u get(param_t) અચલ override
	{
		param_કિંમત_u v{};
		v.i = 0;
		પાછા v;
	}

	શૂન્ય reset(param_t) override {}
	શૂન્ય refresh(param_t) override {}
	પૂર્ણાંક size() અચલ override { પાછા 0; }
	પૂર્ણાંક byteSize() અચલ override { પાછા 0; }
};

વર્ગ DynamicSparseLayerConcurrentTest : જાહેર ::testing::Test
{
સુરક્ષિત:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	માટે (પૂર્ણાંક rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr પૂર્ણાંક અંક_PARAMS = 100;
		std::atomic<તાર્કિક> stop{ખોટું};

		std::thread writer([&]() {
			માટે (પૂર્ણાંક i = 0; i < અંક_PARAMS; i++) {
				param_કિંમત_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(સાચું, std::memory_order_release);
		});

		std::vector<std::thread> readers;

		માટે (પૂર્ણાંક r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				જ્યારે (!stop.load(std::memory_order_acquire)) {
					પૂર્ણાંક current_size = layer.size();

					માટે (પૂર્ણાંક i = 0; i < current_size && i < અંક_PARAMS; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		માટે (auto &t : readers) {
			t.join();
		}

		માટે (પૂર્ણાંક i = 0; i < અંક_PARAMS; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_કિંમત_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.size(), અંક_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentMultipleWriters)
{
	constexpr પૂર્ણાંક PARAMS_PER_WRITER = 50;
	constexpr પૂર્ણાંક અંક_WRITERS = 4;
	constexpr પૂર્ણાંક TOTAL = અંક_WRITERS * PARAMS_PER_WRITER;

	માટે (પૂર્ણાંક rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/TOTAL, /*n_grow=*/1);

		std::vector<std::thread> writers;

		માટે (પૂર્ણાંક w = 0; w < અંક_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				પૂર્ણાંક base = w * PARAMS_PER_WRITER;

				માટે (પૂર્ણાંક i = 0; i < PARAMS_PER_WRITER; i++) {
					param_કિંમત_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		માટે (auto &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		માટે (પૂર્ણાંક i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_કિંમત_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr પૂર્ણાંક PARAMS_PER_WRITER = 50;
	constexpr પૂર્ણાંક અંક_WRITERS = 2;
	constexpr પૂર્ણાંક TOTAL = અંક_WRITERS * PARAMS_PER_WRITER;

	માટે (પૂર્ણાંક rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<તાર્કિક> stop{ખોટું};

		std::vector<std::thread> writers;

		માટે (પૂર્ણાંક w = 0; w < અંક_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				પૂર્ણાંક base = w * PARAMS_PER_WRITER;

				માટે (પૂર્ણાંક i = 0; i < PARAMS_PER_WRITER; i++) {
					param_કિંમત_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::vector<std::thread> readers;

		માટે (પૂર્ણાંક r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				જ્યારે (!stop.load(std::memory_order_acquire)) {
					પૂર્ણાંક current_size = layer.size();

					માટે (પૂર્ણાંક i = 0; i < current_size && i < TOTAL; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		માટે (auto &t : writers) {
			t.join();
		}

		stop.store(સાચું, std::memory_order_release);

		માટે (auto &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		માટે (પૂર્ણાંક i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_કિંમત_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
