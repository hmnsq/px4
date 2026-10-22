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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>

#ಅನ್ತರ್ಭಾವ <parameters/px4_parameters.hpp>
#ಅನ್ತರ್ಭಾವ "DynamicSparseLayer.h"

#ಅನ್ತರ್ಭಾವ <atomic>
#ಅನ್ತರ್ಭಾವ <thread>
#ಅನ್ತರ್ಭಾವ <vector>

/**
 * Minimal ParamLayer parent that returns a default zero value for all params.
 */
ವರ್ಗ StubParamLayer : ಸಾರ್ವಜನಿಕ ParamLayer
{
ಸಾರ್ವಜನಿಕ:
	StubParamLayer() : ParamLayer(ಶೂನ್ಯಸೂಚಕ) {}
	ವಾಸ್ತವಿಕ ~StubParamLayer() = ಪೂರ್ವನಿಯೋಜಿತ;

	ತಾರ್ಕಿಕ store(param_t, param_ಮೌಲ್ಯ_u) override { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
	ತಾರ್ಕಿಕ ಒಳಗೊಂಡಿದೆ(param_t) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

	px4::AtomicBitset<PARAM_ಎಣಿಕೆ> containedAsBitset() ಸ್ಥಿರಾಂಕ override
	{
		ಹಿಂತಿರುಗಿ px4::AtomicBitset<PARAM_ಎಣಿಕೆ>();
	}

	param_ಮೌಲ್ಯ_u get(param_t) ಸ್ಥಿರಾಂಕ override
	{
		param_ಮೌಲ್ಯ_u v{};
		v.i = 0;
		ಹಿಂತಿರುಗಿ v;
	}

	ಶೂನ್ಯ reset(param_t) override {}
	ಶೂನ್ಯ refresh(param_t) override {}
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ 0; }
	ಪೂರ್ಣಾಂಕ byte_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ 0; }
};

ವರ್ಗ DynamicSparseLayerConcurrentTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಂರಕ್ಷಿತ:
	StubParamLayer stub;
};

// Single writer forces frequent _grow() while readers concurrently access
// stored params. With only one writer there is no concurrent growth, so
// this safely stresses the read-side during buffer reallocation.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentStoreGetRace)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ rep = 0; rep < 50; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);

		constexpr ಪೂರ್ಣಾಂಕ ಅಂಕೆ_PARAMS = 100;
		std::atomic<ತಾರ್ಕಿಕ> stop{ಸುಳ್ಳು};

		std::thread writer([&]() {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_PARAMS; i++) {
				param_ಮೌಲ್ಯ_u v{};
				v.i = i * 10;
				layer.store(static_cast<param_t>(i), v);
			}

			stop.store(ನಿಜ, std::ಸ್ಮರಣೆ_ಕ್ರಮ_release);
		});

		std::ಸದಿಶ_2<std::thread> readers;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < 4; r++) {
			readers.emplace_ಹಿಂಭಾಗ([&]() {
				ಆಗ (!stop.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_acquire)) {
					ಪೂರ್ಣಾಂಕ current_ಗಾತ್ರ = layer.ಗಾತ್ರ_2();

					ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < current_ಗಾತ್ರ && i < ಅಂಕೆ_PARAMS; i++) {
						layer.ಒಳಗೊಂಡಿದೆ(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		writer.join();

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &t : readers) {
			t.join();
		}

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_PARAMS; i++) {
			ASSERT_TRUE(layer.ಒಳಗೊಂಡಿದೆ(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ಮೌಲ್ಯ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}

		ASSERT_EQ(layer.ಗಾತ್ರ_2(), ಅಂಕೆ_PARAMS);
	}
}

// Multiple writers store to disjoint param ranges concurrently.
// Pre-allocated to avoid concurrent _grow() which has a known ABA
// limitation in the CAS retry loop (see _grow() implementation).
// This test validates concurrent store correctness.
TEST_F(DynamicSparseLayerConcurrentTest, Concurrent_ಅಪವರ್ತ್ಯ_Writers)
{
	constexpr ಪೂರ್ಣಾಂಕ PARAMS_ಪ್ರತಿ_WRITER = 50;
	constexpr ಪೂರ್ಣಾಂಕ ಅಂಕೆ_WRITERS = 4;
	constexpr ಪೂರ್ಣಾಂಕ ಒಟ್ಟು = ಅಂಕೆ_WRITERS * PARAMS_ಪ್ರತಿ_WRITER;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/ಒಟ್ಟು, /*n_grow=*/1);

		std::ಸದಿಶ_2<std::thread> writers;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ w = 0; w < ಅಂಕೆ_WRITERS; w++) {
			writers.emplace_ಹಿಂಭಾಗ([&layer, w]() {
				ಪೂರ್ಣಾಂಕ base = w * PARAMS_ಪ್ರತಿ_WRITER;

				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < PARAMS_ಪ್ರತಿ_WRITER; i++) {
					param_ಮೌಲ್ಯ_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &t : writers) {
			t.join();
		}

		ASSERT_EQ(layer.ಗಾತ್ರ_2(), ಒಟ್ಟು) << "rep=" << rep;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಒಟ್ಟು; i++) {
			ASSERT_TRUE(layer.ಒಳಗೊಂಡಿದೆ(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ಮೌಲ್ಯ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}

// Combined stress: writers store while readers access concurrently.
TEST_F(DynamicSparseLayerConcurrentTest, ConcurrentWritersAndReaders)
{
	constexpr ಪೂರ್ಣಾಂಕ PARAMS_ಪ್ರತಿ_WRITER = 50;
	constexpr ಪೂರ್ಣಾಂಕ ಅಂಕೆ_WRITERS = 2;
	constexpr ಪೂರ್ಣಾಂಕ ಒಟ್ಟು = ಅಂಕೆ_WRITERS * PARAMS_ಪ್ರತಿ_WRITER;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ rep = 0; rep < 20; rep++) {
		DynamicSparseLayer layer(&stub, /*n_prealloc=*/2, /*n_grow=*/1);
		std::atomic<ತಾರ್ಕಿಕ> stop{ಸುಳ್ಳು};

		std::ಸದಿಶ_2<std::thread> writers;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ w = 0; w < ಅಂಕೆ_WRITERS; w++) {
			writers.emplace_ಹಿಂಭಾಗ([&layer, w]() {
				ಪೂರ್ಣಾಂಕ base = w * PARAMS_ಪ್ರತಿ_WRITER;

				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < PARAMS_ಪ್ರತಿ_WRITER; i++) {
					param_ಮೌಲ್ಯ_u v{};
					v.i = (base + i) * 10;
					layer.store(static_cast<param_t>(base + i), v);
				}
			});
		}

		std::ಸದಿಶ_2<std::thread> readers;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = 0; r < 4; r++) {
			readers.emplace_ಹಿಂಭಾಗ([&]() {
				ಆಗ (!stop.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_acquire)) {
					ಪೂರ್ಣಾಂಕ current_ಗಾತ್ರ = layer.ಗಾತ್ರ_2();

					ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < current_ಗಾತ್ರ && i < ಒಟ್ಟು; i++) {
						layer.ಒಳಗೊಂಡಿದೆ(static_cast<param_t>(i));
						layer.get(static_cast<param_t>(i));
					}
				}
			});
		}

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &t : writers) {
			t.join();
		}

		stop.store(ನಿಜ, std::ಸ್ಮರಣೆ_ಕ್ರಮ_release);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &t : readers) {
			t.join();
		}

		ASSERT_EQ(layer.ಗಾತ್ರ_2(), ಒಟ್ಟು) << "rep=" << rep;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಒಟ್ಟು; i++) {
			ASSERT_TRUE(layer.ಒಳಗೊಂಡಿದೆ(static_cast<param_t>(i)))
					<< "rep=" << rep << " param=" << i;
			param_ಮೌಲ್ಯ_u v = layer.get(static_cast<param_t>(i));
			ASSERT_EQ(v.i, i * 10)
					<< "rep=" << rep << " param=" << i;
		}
	}
}
