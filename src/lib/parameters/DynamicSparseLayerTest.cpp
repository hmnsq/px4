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

#അന്തര്ഭാവ <gtest/gtest.h>

#അന്തര്ഭാവ <parameters/px4_parameters.hpp>
#അന്തര്ഭാവ "DynamicSparseLayer.h"

#അന്തര്ഭാവ <atomic>
#അന്തര്ഭാവ <thread>
#അന്തര്ഭാവ <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
വർഗ്ഗം StubParamLayer : പൊതു ParamLayer
{
പൊതു:
	StubParamLayer() : ParamLayer(nullptr) {}
	മായാ ~StubParamLayer() = സ്ഥിരസ്ഥിതി;

	തർക്കം store(param_t, param_മൂല്യം_u) override { തിരികെ അസത്യം; }
	തർക്കം contains(param_t) സ്ഥിരം override { തിരികെ അസത്യം; }

	px4::AtomicBitset<PARAM_എണ്ണം> containedAsBitset() സ്ഥിരം override
	{
		തിരികെ px4::AtomicBitset<PARAM_എണ്ണം>();
	}

	param_മൂല്യം_u get(param_t) സ്ഥിരം override
	{
		param_മൂല്യം_u v{};
		v.i = 0;
		തിരികെ v;
	}

	ശൂന്യം reset(param_t) override {}
	ശൂന്യം refresh(param_t) override {}
	പൂർണ്ണസംഖ്യ size() സ്ഥിരം override { തിരികെ 0; }
	പൂർണ്ണസംഖ്യ byteSize() സ്ഥിരം override { തിരികെ 0; }
};

വർഗ്ഗം DynamicSparseLayerConcurrentTest : പൊതു ::testing::Test
{
സംരക്ഷിത:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	വേണ്ടി (പൂർണ്ണസംഖ്യ rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr പൂർണ്ണസംഖ്യ അക്കം_PARAMS = 100;
		std::atomic<തർക്കം> stop{അസത്യം};

		std::thread writer([&]() {
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_PARAMS; i++) {
				param_മൂല്യം_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(സത്യം, std::memory_order_release);
		});

		std::vector<std::thread> readers;

		വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				ആകുമ്പോൾ (!stop.load(std::memory_order_acquire)) {
					പൂർണ്ണസംഖ്യ current_size = layer.size();

					വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < current_size && i < അക്കം_PARAMS; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		വേണ്ടി (auto &t : readers) {
			t.join();
		}

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_PARAMS; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_മൂല്യം_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.size(), അക്കം_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentMultipleWriters)
{
	constexpr പൂർണ്ണസംഖ്യ PARAMS_PER_WRITER = 50;
	constexpr പൂർണ്ണസംഖ്യ അക്കം_WRITERS = 4;
	constexpr പൂർണ്ണസംഖ്യ TOTAL = അക്കം_WRITERS * PARAMS_PER_WRITER;

	വേണ്ടി (പൂർണ്ണസംഖ്യ rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/TOTAL, /*n_grow=*/1);

		std::vector<std::thread> writers;

		വേണ്ടി (പൂർണ്ണസംഖ്യ w = 0; w < അക്കം_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				പൂർണ്ണസംഖ്യ base = w * PARAMS_PER_WRITER;

				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < PARAMS_PER_WRITER; i++) {
					param_മൂല്യം_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		വേണ്ടി (auto &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_മൂല്യം_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr പൂർണ്ണസംഖ്യ PARAMS_PER_WRITER = 50;
	constexpr പൂർണ്ണസംഖ്യ അക്കം_WRITERS = 2;
	constexpr പൂർണ്ണസംഖ്യ TOTAL = അക്കം_WRITERS * PARAMS_PER_WRITER;

	വേണ്ടി (പൂർണ്ണസംഖ്യ rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<തർക്കം> stop{അസത്യം};

		std::vector<std::thread> writers;

		വേണ്ടി (പൂർണ്ണസംഖ്യ w = 0; w < അക്കം_WRITERS; w++) {
			writers.emplace_back([&layer, w]() {
				പൂർണ്ണസംഖ്യ base = w * PARAMS_PER_WRITER;

				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < PARAMS_PER_WRITER; i++) {
					param_മൂല്യം_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::vector<std::thread> readers;

		വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < 4; r++) {
			readers.emplace_back([&]() {
				ആകുമ്പോൾ (!stop.load(std::memory_order_acquire)) {
					പൂർണ്ണസംഖ്യ current_size = layer.size();

					വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < current_size && i < TOTAL; i++) {
						layer.contains(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		വേണ്ടി (auto &t : writers) {
			t.join();
		}

		stop.store(സത്യം, std::memory_order_release);

		വേണ്ടി (auto &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.size(), TOTAL) << "rep=" << rep;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < TOTAL; i++) {
			ASSERT_TRUE(layer.contains(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_മൂല്യം_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
