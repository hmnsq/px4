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

#شامل <gtest/gtest.h>

#شامل <parameters/px4_parameters.hpp>
#شامل "DynamicSparseLayer.h"

#شامل <atomic>
#شامل <thread>
#شامل <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
جماعت StubParamLayer : عوامی ParamLayer
{
عوامی:
	StubParamLayer() : ParamLayer(خالی_اشاریہ) {}
	مجازی ~StubParamLayer() = طےشدہ;

	منطقی store(param_t, param_قدر_u) override { واپس جھوٹ; }
	منطقی شامل_ہے(param_t) ثابت override { واپس جھوٹ; }

	px4::AtomicBitset<PARAM_گنتی> containedAsBitset() ثابت override
	{
		واپس px4::AtomicBitset<PARAM_گنتی>();
	}

	param_قدر_u get(param_t) ثابت override
	{
		param_قدر_u v{};
		v.i = 0;
		واپس v;
	}

	خالی reset(param_t) override {}
	خالی refresh(param_t) override {}
	صحیح سائز() ثابت override { واپس 0; }
	صحیح byte_سائز() ثابت override { واپس 0; }
};

جماعت DynamicSparseLayerConcurrentTest : عوامی ::testing::Test
{
محفوظ:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	برائے (صحیح rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr صحیح عدد_PARAMS = 100;
		std::atomic<منطقی> stop{جھوٹ};

		std::thread writer([&]() {
			برائے (صحیح i = 0; i < عدد_PARAMS; i++) {
				param_قدر_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(سچ, std::حافظہ_ترتیب_release);
		});

		std::سمتیہ_2<std::thread> readers;

		برائے (صحیح r = 0; r < 4; r++) {
			readers.emplace_عقب([&]() {
				جب (!stop.بھرو(std::حافظہ_ترتیب_acquire)) {
					صحیح current_سائز = layer.سائز();

					برائے (صحیح i = 0; i < current_سائز && i < عدد_PARAMS; i++) {
						layer.شامل_ہے(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		برائے (خودکار &t : readers) {
			t.join();
		}

		برائے (صحیح i = 0; i < عدد_PARAMS; i++) {
			ASSERT_TRUE(layer.شامل_ہے(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_قدر_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.سائز(), عدد_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, Concurrent_مضرب_Writers)
{
	constexpr صحیح PARAMS_فی_WRITER = 50;
	constexpr صحیح عدد_WRITERS = 4;
	constexpr صحیح کل = عدد_WRITERS * PARAMS_فی_WRITER;

	برائے (صحیح rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/کل, /*n_grow=*/1);

		std::سمتیہ_2<std::thread> writers;

		برائے (صحیح w = 0; w < عدد_WRITERS; w++) {
			writers.emplace_عقب([&layer, w]() {
				صحیح base = w * PARAMS_فی_WRITER;

				برائے (صحیح i = 0; i < PARAMS_فی_WRITER; i++) {
					param_قدر_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		برائے (خودکار &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.سائز(), کل) << "rep=" << rep;

		برائے (صحیح i = 0; i < کل; i++) {
			ASSERT_TRUE(layer.شامل_ہے(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_قدر_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr صحیح PARAMS_فی_WRITER = 50;
	constexpr صحیح عدد_WRITERS = 2;
	constexpr صحیح کل = عدد_WRITERS * PARAMS_فی_WRITER;

	برائے (صحیح rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<منطقی> stop{جھوٹ};

		std::سمتیہ_2<std::thread> writers;

		برائے (صحیح w = 0; w < عدد_WRITERS; w++) {
			writers.emplace_عقب([&layer, w]() {
				صحیح base = w * PARAMS_فی_WRITER;

				برائے (صحیح i = 0; i < PARAMS_فی_WRITER; i++) {
					param_قدر_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::سمتیہ_2<std::thread> readers;

		برائے (صحیح r = 0; r < 4; r++) {
			readers.emplace_عقب([&]() {
				جب (!stop.بھرو(std::حافظہ_ترتیب_acquire)) {
					صحیح current_سائز = layer.سائز();

					برائے (صحیح i = 0; i < current_سائز && i < کل; i++) {
						layer.شامل_ہے(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		برائے (خودکار &t : writers) {
			t.join();
		}

		stop.store(سچ, std::حافظہ_ترتیب_release);

		برائے (خودکار &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.سائز(), کل) << "rep=" << rep;

		برائے (صحیح i = 0; i < کل; i++) {
			ASSERT_TRUE(layer.شامل_ہے(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_قدر_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
