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

#ضمن <gtest/gtest.h>

#ضمن <parameters/px4_parameters.hpp>
#ضمن "DynamicSparseLayer.h"

#ضمن <atomic>
#ضمن <thread>
#ضمن <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
صنف StubParamLayer : عام ParamLayer
{
عام:
	StubParamLayer() : ParamLayer(nullptr) {}
	virtual ~StubParamLayer() = افتراضي;

	منطقي store(param_t, param_قىممەت_u) override { أرجع خطأ; }
	منطقي contains(param_t) ثابت override { أرجع خطأ; }

	px4::AtomicBitset<PARAM_سانى> containedAsBitset() ثابت override
	{
		أرجع px4::AtomicBitset<PARAM_سانى>();
	}

	param_قىممەت_u get(param_t) ثابت override
	{
		param_قىممەت_u v{};
		v.i = 0;
		أرجع v;
	}

	فراغ reset(param_t) override {}
	فراغ refresh(param_t) override {}
	صحيح size() ثابت override { أرجع 0; }
	صحيح byteSize() ثابت override { أرجع 0; }
};

صنف DynamicSparseLayerConcurrentTest : عام ::testing::Test
{
محمي:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	لكل (صحيح rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr صحيح رەقەم_PARAMS = 100;
		std::atomic<منطقي> stop{خطأ};

		std::thread writer([&]() {
			لكل (صحيح i = 0; i < رەقەم_PARAMS; i++) {
				param_قىممەت_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(true, std::memory_order_release);
		});

		std::vector<std::thread> readers;

		لكل (صحيح r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				طالما (!stop.load(std::memory_order_acquire)) {
					صحيح current_size = layer.size();

					لكل (صحيح i = 0; i < current_size && i < رەقەم_PARAMS; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		لكل (auto &t : readers) {
			t.join();
		}

		لكل (صحيح i = 0; i < رەقەم_PARAMS; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_قىممەت_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.size(), رەقەم_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentMultipleWriters)
{
	constexpr صحيح PARAMS_PER_WRITER = 50;
	constexpr صحيح رەقەم_WRITERS = 4;
	constexpr صحيح TOTAL = رەقەم_WRITERS * PARAMS_PER_WRITER;

	لكل (صحيح rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/TOTAL, /*n_grow=*/1);

		std::vector<std::thread> writers;

		لكل (صحيح w = 0; w < رەقەم_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				صحيح base = w * PARAMS_PER_WRITER;

				لكل (صحيح i = 0; i < PARAMS_PER_WRITER; i++) {
					param_قىممەت_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		لكل (auto &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		لكل (صحيح i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_قىممەت_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr صحيح PARAMS_PER_WRITER = 50;
	constexpr صحيح رەقەم_WRITERS = 2;
	constexpr صحيح TOTAL = رەقەم_WRITERS * PARAMS_PER_WRITER;

	لكل (صحيح rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<منطقي> stop{خطأ};

		std::vector<std::thread> writers;

		لكل (صحيح w = 0; w < رەقەم_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				صحيح base = w * PARAMS_PER_WRITER;

				لكل (صحيح i = 0; i < PARAMS_PER_WRITER; i++) {
					param_قىممەت_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::vector<std::thread> readers;

		لكل (صحيح r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				طالما (!stop.load(std::memory_order_acquire)) {
					صحيح current_size = layer.size();

					لكل (صحيح i = 0; i < current_size && i < TOTAL; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		لكل (auto &t : writers) {
			t.join();
		}

		stop.store(true, std::memory_order_release);

		لكل (auto &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		لكل (صحيح i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_قىممەت_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
