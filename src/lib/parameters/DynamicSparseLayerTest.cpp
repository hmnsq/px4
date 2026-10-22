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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/px4_parameters.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ "DynamicSparseLayer.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <atomic>
#ਅਨ੍ਤਰ੍ਭਾਵ <thread>
#ਅਨ੍ਤਰ੍ਭਾਵ <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
ਵਰਗ StubParamLayer : ਜਨਤਕ ParamLayer
{
ਜਨਤਕ:
	StubParamLayer() : ParamLayer(ਖਾਲੀ_ਸੂਚਕ) {}
	ਆਭਾਸੀ ~StubParamLayer() = ਮੂਲ;

	ਤਾਰਕਿਕ store(param_t, param_ਮੁੱਲ_u) override { ਵਾਪਸ ਝੂਠ; }
	ਤਾਰਕਿਕ ਸ਼ਾਮਲ_ਹੈ(param_t) ਅਚਲ override { ਵਾਪਸ ਝੂਠ; }

	px4::AtomicBitset<PARAM_ਗਿਣਤੀ> containedAsBitset() ਅਚਲ override
	{
		ਵਾਪਸ px4::AtomicBitset<PARAM_ਗਿਣਤੀ>();
	}

	param_ਮੁੱਲ_u get(param_t) ਅਚਲ override
	{
		param_ਮੁੱਲ_u v{};
		v.i = 0;
		ਵਾਪਸ v;
	}

	ਖਾਲੀ reset(param_t) override {}
	ਖਾਲੀ refresh(param_t) override {}
	ਪੂਰਨ ਮਾਪ() ਅਚਲ override { ਵਾਪਸ 0; }
	ਪੂਰਨ byte_ਮਾਪ() ਅਚਲ override { ਵਾਪਸ 0; }
};

ਵਰਗ DynamicSparseLayerConcurrentTest : ਜਨਤਕ ::testing::Test
{
ਸੁਰੱਖਿਅਤ:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	ਲਈ (ਪੂਰਨ rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr ਪੂਰਨ ਅੰਕ_PARAMS = 100;
		std::atomic<ਤਾਰਕਿਕ> stop{ਝੂਠ};

		std::thread writer([&]() {
			ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_PARAMS; i++) {
				param_ਮੁੱਲ_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(ਸੱਚ, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_release);
		});

		std::ਸਦਿਸ਼_2<std::thread> readers;

		ਲਈ (ਪੂਰਨ r = 0; r < 4; r++) {
			readers.emplace_ਪਿੱਠ([&]() {
				ਜਦੋਂ (!stop.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_acquire)) {
					ਪੂਰਨ current_ਮਾਪ = layer.ਮਾਪ();

					ਲਈ (ਪੂਰਨ i = 0; i < current_ਮਾਪ && i < ਅੰਕ_PARAMS; i++) {
						layer.ਸ਼ਾਮਲ_ਹੈ(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		ਲਈ (ਸਵੈਚਾਲਿਤ &t : readers) {
			t.join();
		}

		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_PARAMS; i++) {
			ASSERT_TRUE(layer.ਸ਼ਾਮਲ_ਹੈ(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ਮੁੱਲ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.ਮਾਪ(), ਅੰਕ_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, Concurrent_ਗੁਣਜ_Writers)
{
	constexpr ਪੂਰਨ PARAMS_ਪ੍ਰਤੀ_WRITER = 50;
	constexpr ਪੂਰਨ ਅੰਕ_WRITERS = 4;
	constexpr ਪੂਰਨ ਕੁੱਲ = ਅੰਕ_WRITERS * PARAMS_ਪ੍ਰਤੀ_WRITER;

	ਲਈ (ਪੂਰਨ rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/ਕੁੱਲ, /*n_grow=*/1);

		std::ਸਦਿਸ਼_2<std::thread> writers;

		ਲਈ (ਪੂਰਨ w = 0; w < ਅੰਕ_WRITERS; w++) {
			writers.emplace_ਪਿੱਠ([&layer, w]() {
				ਪੂਰਨ base = w * PARAMS_ਪ੍ਰਤੀ_WRITER;

				ਲਈ (ਪੂਰਨ i = 0; i < PARAMS_ਪ੍ਰਤੀ_WRITER; i++) {
					param_ਮੁੱਲ_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		ਲਈ (ਸਵੈਚਾਲਿਤ &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.ਮਾਪ(), ਕੁੱਲ) << "rep=" << rep;

		ਲਈ (ਪੂਰਨ i = 0; i < ਕੁੱਲ; i++) {
			ASSERT_TRUE(layer.ਸ਼ਾਮਲ_ਹੈ(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ਮੁੱਲ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr ਪੂਰਨ PARAMS_ਪ੍ਰਤੀ_WRITER = 50;
	constexpr ਪੂਰਨ ਅੰਕ_WRITERS = 2;
	constexpr ਪੂਰਨ ਕੁੱਲ = ਅੰਕ_WRITERS * PARAMS_ਪ੍ਰਤੀ_WRITER;

	ਲਈ (ਪੂਰਨ rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<ਤਾਰਕਿਕ> stop{ਝੂਠ};

		std::ਸਦਿਸ਼_2<std::thread> writers;

		ਲਈ (ਪੂਰਨ w = 0; w < ਅੰਕ_WRITERS; w++) {
			writers.emplace_ਪਿੱਠ([&layer, w]() {
				ਪੂਰਨ base = w * PARAMS_ਪ੍ਰਤੀ_WRITER;

				ਲਈ (ਪੂਰਨ i = 0; i < PARAMS_ਪ੍ਰਤੀ_WRITER; i++) {
					param_ਮੁੱਲ_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::ਸਦਿਸ਼_2<std::thread> readers;

		ਲਈ (ਪੂਰਨ r = 0; r < 4; r++) {
			readers.emplace_ਪਿੱਠ([&]() {
				ਜਦੋਂ (!stop.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_acquire)) {
					ਪੂਰਨ current_ਮਾਪ = layer.ਮਾਪ();

					ਲਈ (ਪੂਰਨ i = 0; i < current_ਮਾਪ && i < ਕੁੱਲ; i++) {
						layer.ਸ਼ਾਮਲ_ਹੈ(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		ਲਈ (ਸਵੈਚਾਲਿਤ &t : writers) {
			t.join();
		}

		stop.store(ਸੱਚ, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_release);

		ਲਈ (ਸਵੈਚਾਲਿਤ &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.ਮਾਪ(), ਕੁੱਲ) << "rep=" << rep;

		ਲਈ (ਪੂਰਨ i = 0; i < ਕੁੱਲ; i++) {
			ASSERT_TRUE(layer.ਸ਼ਾਮਲ_ਹੈ(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ਮੁੱਲ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
