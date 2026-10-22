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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <atomic>
#ਅਨ੍ਤਰ੍ਭਾਵ <chrono>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>
#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <thread>

#ਅਨ੍ਤਰ੍ਭਾਵ "perf_counter.h"

ਨਾਮਾਕਾਸ਼
{

ਤਾਰਕਿਕ wait_until(ਅਚਲ std::atomic<ਤਾਰਕਿਕ> &condition)
{
	ਅਚਲ ਸਵੈਚਾਲਿਤ deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	ਜਦੋਂ (!condition.ਲੱਦੋ() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	ਵਾਪਸ condition.ਲੱਦੋ();
}

} // namespace

TEST(PerfCounter, Iterate_ਸਤਰ_Content)
{
	perf_counter_t counter = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_line_content");
	ASSERT_NE(counter, ਖਾਲੀ_ਸੂਚਕ);

	perf_ਗਿਣਤੀ(counter);
	perf_ਗਿਣਤੀ(counter);
	perf_ਗਿਣਤੀ(counter);

	ਬਣਤਰ Context {
		std::string ਸਤਰ;
	} context;

	perf_iterate_all([](ਅਚਲ ਅੱਖਰ *counter_ਸਤਰ, ਖਾਲੀ *user) {
		ਜੇ (ਲੜੀ_ਖੋਜ(counter_ਸਤਰ, "test_perf_line_content") != ਖਾਲੀ_ਸੂਚਕ) {
			static_cast<Context *>(user)->ਸਤਰ = counter_ਸਤਰ;
		}
	}, &context);

	EXPECT_EQ(context.ਸਤਰ, "test_perf_line_content: 3 events");

	perf_free(counter);
}

TEST(PerfCounter, Concurrent_Alloc_Does_Not_ਖੰਡ_On_Slow_Callback)
{
	perf_counter_t counter = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_slow_callback");
	ASSERT_NE(counter, ਖਾਲੀ_ਸੂਚਕ);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	ਬਣਤਰ Context {
		std::atomic<ਤਾਰਕਿਕ> first{ਸੱਚ};
		std::atomic<ਤਾਰਕਿਕ> alloc_ਪੂਰਾ{ਝੂਠ};
		std::atomic<ਤਾਰਕਿਕ> alloc_succeeded{ਝੂਠ};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](ਅਚਲ ਅੱਖਰ *, ਖਾਲੀ *user) {
		Context *ctx = static_cast<Context *>(user);

		ਜੇ (ctx->first.exchange(ਝੂਠ)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_counter_t other = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != ਖਾਲੀ_ਸੂਚਕ);
				perf_free(other);
				ctx->alloc_ਪੂਰਾ.store(ਸੱਚ);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_ਪੂਰਾ));
		}
	}, &context);

	ਜੇ (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_ਪੂਰਾ.ਲੱਦੋ());
	EXPECT_TRUE(context.alloc_succeeded.ਲੱਦੋ());

	perf_free(counter);
}

TEST(PerfCounter, AllocFromCallbackDoesNotDeadlock)
{
	perf_counter_t counter = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_callback_alloc");
	ASSERT_NE(counter, ਖਾਲੀ_ਸੂਚਕ);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	ਬਣਤਰ Context {
		std::atomic<ਤਾਰਕਿਕ> first{ਸੱਚ};
		std::atomic<ਤਾਰਕਿਕ> ਪੂਰਾ{ਝੂਠ};
		perf_counter_t allocated{ਖਾਲੀ_ਸੂਚਕ};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](ਅਚਲ ਅੱਖਰ *, ਖਾਲੀ *user) {
			Context *ctx = static_cast<Context *>(user);

			ਜੇ (ctx->first.exchange(ਝੂਠ)) {
				ctx->allocated = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.ਪੂਰਾ.store(ਸੱਚ);
	});

	ਜੇ (!wait_until(context.ਪੂਰਾ)) {
		iterate_thread.detach();
		FAIL() << "perf_alloc() from the callback deadlocked";
	}

	iterate_thread.join();
	EXPECT_NE(context.allocated, ਖਾਲੀ_ਸੂਚਕ);

	perf_free(context.allocated);
	perf_free(counter);
}

TEST(PerfCounter, FreeDuringIterationIsDeferred)
{
	// allocation order puts counter_b at the list head, so it is visited first
	perf_counter_t counter_a = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_free_defer_a");
	perf_counter_t counter_b = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_free_defer_b");
	ASSERT_NE(counter_a, ਖਾਲੀ_ਸੂਚਕ);
	ASSERT_NE(counter_b, ਖਾਲੀ_ਸੂਚਕ);

	ਬਣਤਰ Context {
		perf_counter_t counter_a;
		std::atomic<ਤਾਰਕਿਕ> free_ਪੂਰਾ{ਝੂਠ};
		std::thread free_thread;
		ਪੂਰਨ ਸਤਰਾਂ_a{0};
	} context{counter_a};

	perf_iterate_all([](ਅਚਲ ਅੱਖਰ *counter_ਸਤਰ, ਖਾਲੀ *user) {
		Context *ctx = static_cast<Context *>(user);

		ਜੇ (!ctx->free_thread.joinable() && ਲੜੀ_ਖੋਜ(counter_ਸਤਰ, "test_perf_free_defer_b") != ਖਾਲੀ_ਸੂਚਕ) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->counter_a);
				ctx->free_ਪੂਰਾ.store(ਸੱਚ);
			});

			EXPECT_TRUE(wait_until(ctx->free_ਪੂਰਾ));
		}

		ਜੇ (ਲੜੀ_ਖੋਜ(counter_ਸਤਰ, "test_perf_free_defer_a") != ਖਾਲੀ_ਸੂਚਕ) {
			ctx->ਸਤਰਾਂ_a++;
		}
	}, &context);

	ਜੇ (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_ਪੂਰਾ.ਲੱਦੋ());
	EXPECT_EQ(context.ਸਤਰਾਂ_a, 0);

	// the freed entry must not reappear after deferred cleanup
	ਬਣਤਰ ਗਿਣਤੀ_Context {
		ਪੂਰਨ ਸਤਰਾਂ_a{0};
	} ਗਿਣਤੀ_context;

	perf_iterate_all([](ਅਚਲ ਅੱਖਰ *counter_ਸਤਰ, ਖਾਲੀ *user) {
		ਜੇ (ਲੜੀ_ਖੋਜ(counter_ਸਤਰ, "test_perf_free_defer_a") != ਖਾਲੀ_ਸੂਚਕ) {
			static_cast<ਗਿਣਤੀ_Context *>(user)->ਸਤਰਾਂ_a++;
		}
	}, &ਗਿਣਤੀ_context);

	EXPECT_EQ(ਗਿਣਤੀ_context.ਸਤਰਾਂ_a, 0);

	perf_free(counter_b);
}

TEST(PerfCounter, Free_Is_Deferred_Until_ਆਖਰੀ_Overlapping_Iteration_ਸਿਰੇ)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_counter_t counter_a = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_overlap_a");
	perf_counter_t counter_b = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_overlap_b");
	ASSERT_NE(counter_a, ਖਾਲੀ_ਸੂਚਕ);
	ASSERT_NE(counter_b, ਖਾਲੀ_ਸੂਚਕ);

	ਬਣਤਰ IterateContext {
		std::atomic<ਤਾਰਕਿਕ> entered{ਝੂਠ};
		std::atomic<ਤਾਰਕਿਕ> release{ਝੂਠ};
		std::atomic<ਤਾਰਕਿਕ> timed_ਬਾਹਰਲਾ{ਝੂਠ};
		ਪੂਰਨ ਸਤਰਾਂ_a{0};
		ਪੂਰਨ replacement_ਸਤਰਾਂ{0};
	} ਪਹਿਲਾ_context, ਦੂਜਾ_context;

	perf_callback callback = [](ਅਚਲ ਅੱਖਰ *counter_ਸਤਰ, ਖਾਲੀ *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		ਜੇ (ਲੜੀ_ਖੋਜ(counter_ਸਤਰ, "test_perf_overlap_b") != ਖਾਲੀ_ਸੂਚਕ) {
			ctx->entered.store(ਸੱਚ);

			ਜੇ (!wait_until(ctx->release)) {
				ctx->timed_ਬਾਹਰਲਾ.store(ਸੱਚ);
			}
		}

		ਜੇ (ਲੜੀ_ਖੋਜ(counter_ਸਤਰ, "test_perf_overlap_a") != ਖਾਲੀ_ਸੂਚਕ) {
			ctx->ਸਤਰਾਂ_a++;
		}

		ਜੇ (ਲੜੀ_ਖੋਜ(counter_ਸਤਰ, "test_perf_overlap_replacement") != ਖਾਲੀ_ਸੂਚਕ) {
			ctx->replacement_ਸਤਰਾਂ++;
		}
	};

	std::thread ਪਹਿਲਾ_thread([&]() { perf_iterate_all(callback, &ਪਹਿਲਾ_context); });

	ਜੇ (!wait_until(ਪਹਿਲਾ_context.entered)) {
		ਪਹਿਲਾ_context.release.store(ਸੱਚ);
		ਪਹਿਲਾ_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread ਦੂਜਾ_thread([&]() { perf_iterate_all(callback, &ਦੂਜਾ_context); });

	ਜੇ (!wait_until(ਦੂਜਾ_context.entered)) {
		ਪਹਿਲਾ_context.release.store(ਸੱਚ);
		ਦੂਜਾ_context.release.store(ਸੱਚ);
		ਪਹਿਲਾ_thread.join();
		ਦੂਜਾ_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<ਤਾਰਕਿਕ> free_ਪੂਰਾ{ਝੂਠ};
	std::thread free_thread([&]() {
		perf_free(counter_a);
		free_ਪੂਰਾ.store(ਸੱਚ);
	});

	ਜੇ (!wait_until(free_ਪੂਰਾ)) {
		ਪਹਿਲਾ_context.release.store(ਸੱਚ);
		ਦੂਜਾ_context.release.store(ਸੱਚ);
		ਪਹਿਲਾ_thread.join();
		ਦੂਜਾ_thread.join();
		free_thread.join();
		perf_free(counter_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	ਪਹਿਲਾ_context.release.store(ਸੱਚ);
	ਪਹਿਲਾ_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_counter_t replacement = perf_alloc(PC_ਗਿਣਤੀ, "test_perf_overlap_replacement");

	ਜੇ (replacement == ਖਾਲੀ_ਸੂਚਕ) {
		ਦੂਜਾ_context.release.store(ਸੱਚ);
		ਦੂਜਾ_thread.join();
		perf_free(counter_b);
		FAIL() << "replacement counter allocation failed";
	}

	ਦੂਜਾ_context.release.store(ਸੱਚ);
	ਦੂਜਾ_thread.join();

	EXPECT_FALSE(ਪਹਿਲਾ_context.timed_ਬਾਹਰਲਾ.ਲੱਦੋ());
	EXPECT_FALSE(ਦੂਜਾ_context.timed_ਬਾਹਰਲਾ.ਲੱਦੋ());
	EXPECT_EQ(ਪਹਿਲਾ_context.ਸਤਰਾਂ_a, 0);
	EXPECT_EQ(ਦੂਜਾ_context.ਸਤਰਾਂ_a, 0);
	EXPECT_EQ(ਦੂਜਾ_context.replacement_ਸਤਰਾਂ, 0);

	perf_free(replacement);
	perf_free(counter_b);
}
