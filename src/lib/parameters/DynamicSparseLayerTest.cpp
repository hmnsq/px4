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

#ଅନ୍ତର୍ଭାଵ <gtest/gtest.h>

#ଅନ୍ତର୍ଭାଵ <parameters/px4_parameters.hpp>
#ଅନ୍ତର୍ଭାଵ "DynamicSparseLayer.h"

#ଅନ୍ତର୍ଭାଵ <atomic>
#ଅନ୍ତର୍ଭାଵ <thread>
#ଅନ୍ତର୍ଭାଵ <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
ଶ୍ରେଣୀ StubParamLayer : ସର୍ବସାଧାରଣ ParamLayer
{
ସର୍ବସାଧାରଣ:
	StubParamLayer() : ParamLayer(nullptr) {}
	ଆଭାସୀ ~StubParamLayer() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ତାର୍କିକ store(param_t, param_ମୂଲ୍ୟ_u) override { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
	ତାର୍କିକ contains(param_t) ଅଚଳ override { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

	px4::AtomicBitset<PARAM_ଗଣନା> containedAsBitset() ଅଚଳ override
	{
		ଫେରନ୍ତୁ px4::AtomicBitset<PARAM_ଗଣନା>();
	}

	param_ମୂଲ୍ୟ_u get(param_t) ଅଚଳ override
	{
		param_ମୂଲ୍ୟ_u v{};
		v.i = 0;
		ଫେରନ୍ତୁ v;
	}

	ଶୂନ୍ୟ reset(param_t) override {}
	ଶୂନ୍ୟ refresh(param_t) override {}
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ override { ଫେରନ୍ତୁ 0; }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା byteSize() ଅଚଳ override { ଫେରନ୍ତୁ 0; }
};

ଶ୍ରେଣୀ DynamicSparseLayerConcurrentTest : ସର୍ବସାଧାରଣ ::testing::Test
{
ସୁରକ୍ଷିତ:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_PARAMS = 100;
		std::atomic<ତାର୍କିକ> stop{ମିଥ୍ୟା};

		std::thread writer([&]() {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_PARAMS; i++) {
				param_ମୂଲ୍ୟ_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(ସତ୍ୟ, std::memory_order_release);
		});

		std::vector<std::thread> readers;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				ଯେତେବେଳେ (!stop.load(std::memory_order_acquire)) {
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_size = layer.size();

					ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < current_size && i < ଅଙ୍କ_PARAMS; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		ପାଇଁ (auto &t : readers) {
			t.join();
		}

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_PARAMS; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ମୂଲ୍ୟ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.size(), ଅଙ୍କ_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentMultipleWriters)
{
	constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା PARAMS_PER_WRITER = 50;
	constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_WRITERS = 4;
	constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା TOTAL = ଅଙ୍କ_WRITERS * PARAMS_PER_WRITER;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/TOTAL, /*n_grow=*/1);

		std::vector<std::thread> writers;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = 0; w < ଅଙ୍କ_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା base = w * PARAMS_PER_WRITER;

				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < PARAMS_PER_WRITER; i++) {
					param_ମୂଲ୍ୟ_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		ପାଇଁ (auto &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ମୂଲ୍ୟ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା PARAMS_PER_WRITER = 50;
	constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_WRITERS = 2;
	constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା TOTAL = ଅଙ୍କ_WRITERS * PARAMS_PER_WRITER;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<ତାର୍କିକ> stop{ମିଥ୍ୟା};

		std::vector<std::thread> writers;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = 0; w < ଅଙ୍କ_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା base = w * PARAMS_PER_WRITER;

				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < PARAMS_PER_WRITER; i++) {
					param_ମୂଲ୍ୟ_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::vector<std::thread> readers;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				ଯେତେବେଳେ (!stop.load(std::memory_order_acquire)) {
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_size = layer.size();

					ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < current_size && i < TOTAL; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		ପାଇଁ (auto &t : writers) {
			t.join();
		}

		stop.store(ସତ୍ୟ, std::memory_order_release);

		ପାଇଁ (auto &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ମୂଲ୍ୟ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
