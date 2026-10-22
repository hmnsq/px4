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

#అన్తర్భావ <gtest/gtest.h>

#అన్తర్భావ <parameters/px4_parameters.hpp>
#అన్తర్భావ "DynamicSparseLayer.h"

#అన్తర్భావ <atomic>
#అన్తర్భావ <thread>
#అన్తర్భావ <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
తరగతి StubParamLayer : బహిరంగ ParamLayer
{
బహిరంగ:
	StubParamLayer() : ParamLayer(శూన్యసూచిక) {}
	మాయా ~StubParamLayer() = అప్రమేయ;

	తార్కిక store(param_t, param_విలువ_u) override { తిరిగి అబద్ధం; }
	తార్కిక కలిగియుంది(param_t) స్థిరరాశి override { తిరిగి అబద్ధం; }

	px4::AtomicBitset<PARAM_లెక్క> containedAsBitset() స్థిరరాశి override
	{
		తిరిగి px4::AtomicBitset<PARAM_లెక్క>();
	}

	param_విలువ_u get(param_t) స్థిరరాశి override
	{
		param_విలువ_u v{};
		v.i = 0;
		తిరిగి v;
	}

	శూన్య reset(param_t) override {}
	శూన్య refresh(param_t) override {}
	పూర్ణాంకం కొలత() స్థిరరాశి override { తిరిగి 0; }
	పూర్ణాంకం byte_కొలత() స్థిరరాశి override { తిరిగి 0; }
};

తరగతి DynamicSparseLayerConcurrentTest : బహిరంగ ::testing::Test
{
రక్షిత:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	కొరకు (పూర్ణాంకం rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr పూర్ణాంకం అంకె_PARAMS = 100;
		std::atomic<తార్కిక> stop{అబద్ధం};

		std::thread writer([&]() {
			కొరకు (పూర్ణాంకం i = 0; i < అంకె_PARAMS; i++) {
				param_విలువ_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(నిజం, std::జ్ఞాపకం_క్రమం_release);
		});

		std::సదిశ_2<std::thread> readers;

		కొరకు (పూర్ణాంకం r = 0; r < 4; r++) {
			readers.emplace_వెనుక([&]() {
				అయితే (!stop.ఎక్కించు(std::జ్ఞాపకం_క్రమం_acquire)) {
					పూర్ణాంకం current_కొలత = layer.కొలత();

					కొరకు (పూర్ణాంకం i = 0; i < current_కొలత && i < అంకె_PARAMS; i++) {
						layer.కలిగియుంది(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		కొరకు (స్వయంచాలక &t : readers) {
			t.join();
		}

		కొరకు (పూర్ణాంకం i = 0; i < అంకె_PARAMS; i++) {
			ASSERT_TRUE(layer.కలిగియుంది(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_విలువ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.కొలత(), అంకె_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, Concurrent_గుణిజం_Writers)
{
	constexpr పూర్ణాంకం PARAMS_చొప్పున_WRITER = 50;
	constexpr పూర్ణాంకం అంకె_WRITERS = 4;
	constexpr పూర్ణాంకం మొత్తం = అంకె_WRITERS * PARAMS_చొప్పున_WRITER;

	కొరకు (పూర్ణాంకం rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/మొత్తం, /*n_grow=*/1);

		std::సదిశ_2<std::thread> writers;

		కొరకు (పూర్ణాంకం w = 0; w < అంకె_WRITERS; w++) {
			writers.emplace_వెనుక([&layer, w]() {
				పూర్ణాంకం base = w * PARAMS_చొప్పున_WRITER;

				కొరకు (పూర్ణాంకం i = 0; i < PARAMS_చొప్పున_WRITER; i++) {
					param_విలువ_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		కొరకు (స్వయంచాలక &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.కొలత(), మొత్తం) << "rep=" << rep;

		కొరకు (పూర్ణాంకం i = 0; i < మొత్తం; i++) {
			ASSERT_TRUE(layer.కలిగియుంది(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_విలువ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr పూర్ణాంకం PARAMS_చొప్పున_WRITER = 50;
	constexpr పూర్ణాంకం అంకె_WRITERS = 2;
	constexpr పూర్ణాంకం మొత్తం = అంకె_WRITERS * PARAMS_చొప్పున_WRITER;

	కొరకు (పూర్ణాంకం rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<తార్కిక> stop{అబద్ధం};

		std::సదిశ_2<std::thread> writers;

		కొరకు (పూర్ణాంకం w = 0; w < అంకె_WRITERS; w++) {
			writers.emplace_వెనుక([&layer, w]() {
				పూర్ణాంకం base = w * PARAMS_చొప్పున_WRITER;

				కొరకు (పూర్ణాంకం i = 0; i < PARAMS_చొప్పున_WRITER; i++) {
					param_విలువ_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::సదిశ_2<std::thread> readers;

		కొరకు (పూర్ణాంకం r = 0; r < 4; r++) {
			readers.emplace_వెనుక([&]() {
				అయితే (!stop.ఎక్కించు(std::జ్ఞాపకం_క్రమం_acquire)) {
					పూర్ణాంకం current_కొలత = layer.కొలత();

					కొరకు (పూర్ణాంకం i = 0; i < current_కొలత && i < మొత్తం; i++) {
						layer.కలిగియుంది(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		కొరకు (స్వయంచాలక &t : writers) {
			t.join();
		}

		stop.store(నిజం, std::జ్ఞాపకం_క్రమం_release);

		కొరకు (స్వయంచాలక &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.కొలత(), మొత్తం) << "rep=" << rep;

		కొరకు (పూర్ణాంకం i = 0; i < మొత్తం; i++) {
			ASSERT_TRUE(layer.కలిగియుంది(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_విలువ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
