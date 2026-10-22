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

#अन्तर्भाव <gtest/gtest.h>

#अन्तर्भाव <atomic>
#अन्तर्भाव <chrono>
#अन्तर्भाव <cstring>
#अन्तर्भाव <string>
#अन्तर्भाव <thread>

#अन्तर्भाव "perf_counter.h"

नामाकाश
{

तार्किक wait_until(ध्रुव std::atomic<तार्किक> &condition)
{
	ध्रुव auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	जाव (!condition.load() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	प्रत्यागम condition.load();
}

} // namespace

TEST(PerfCounter, Iterate_पङ्क्तिः_Content)
{
	perf_counter_t counter = perf_alloc(PC_गणनम्, "test_perf_line_content");
	ASSERT_NE(counter, nullptr);

	perf_गणनम्(counter);
	perf_गणनम्(counter);
	perf_गणनम्(counter);

	संरचना Context {
		std::string पङ्क्तिः;
	} context;

	perf_iterate_all([](ध्रुव वर्ण *counter_पङ्क्तिः, शून्य *user) {
		जइ (strstr(counter_पङ्क्तिः, "test_perf_line_content") != nullptr) {
			static_cast<Context *>(user)->पङ्क्तिः = counter_पङ्क्तिः;
		}
	}, &context);

	EXPECT_EQ(context.पङ्क्तिः, "test_perf_line_content: 3 events");

	perf_free(counter);
}

TEST(PerfCounter, ConcurrentAllocDoesNotBlockOnSlowCallback)
{
	perf_counter_t counter = perf_alloc(PC_गणनम्, "test_perf_slow_callback");
	ASSERT_NE(counter, nullptr);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	संरचना Context {
		std::atomic<तार्किक> first{सच्च};
		std::atomic<तार्किक> alloc_done{मिच्छ};
		std::atomic<तार्किक> alloc_succeeded{मिच्छ};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](ध्रुव वर्ण *, शून्य *user) {
		Context *ctx = static_cast<Context *>(user);

		जइ (ctx->first.exchange(मिच्छ)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_counter_t other = perf_alloc(PC_गणनम्, "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != nullptr);
				perf_free(other);
				ctx->alloc_done.store(सच्च);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_done));
		}
	}, &context);

	जइ (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_done.load());
	EXPECT_TRUE(context.alloc_succeeded.load());

	perf_free(counter);
}

TEST(PerfCounter, AllocFromCallbackDoesNotDeadlock)
{
	perf_counter_t counter = perf_alloc(PC_गणनम्, "test_perf_callback_alloc");
	ASSERT_NE(counter, nullptr);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	संरचना Context {
		std::atomic<तार्किक> first{सच्च};
		std::atomic<तार्किक> done{मिच्छ};
		perf_counter_t allocated{nullptr};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](ध्रुव वर्ण *, शून्य *user) {
			Context *ctx = static_cast<Context *>(user);

			जइ (ctx->first.exchange(मिच्छ)) {
				ctx->allocated = perf_alloc(PC_गणनम्, "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.done.store(सच्च);
	});

	जइ (!wait_until(context.done)) {
		iterate_thread.detach();
		FAIL() << "perf_alloc() from the callback deadlocked";
	}

	iterate_thread.join();
	EXPECT_NE(context.allocated, nullptr);

	perf_free(context.allocated);
	perf_free(counter);
}

TEST(PerfCounter, FreeDuringIterationIsDeferred)
{
	// allocation order puts counter_b at the list head, so it is visited first
	perf_counter_t counter_a = perf_alloc(PC_गणनम्, "test_perf_free_defer_a");
	perf_counter_t counter_b = perf_alloc(PC_गणनम्, "test_perf_free_defer_b");
	ASSERT_NE(counter_a, nullptr);
	ASSERT_NE(counter_b, nullptr);

	संरचना Context {
		perf_counter_t counter_a;
		std::atomic<तार्किक> free_done{मिच्छ};
		std::thread free_thread;
		पूर्णाङ्क पङ्क्तयः_a{0};
	} context{counter_a};

	perf_iterate_all([](ध्रुव वर्ण *counter_पङ्क्तिः, शून्य *user) {
		Context *ctx = static_cast<Context *>(user);

		जइ (!ctx->free_thread.joinable() && strstr(counter_पङ्क्तिः, "test_perf_free_defer_b") != nullptr) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->counter_a);
				ctx->free_done.store(सच्च);
			});

			EXPECT_TRUE(wait_until(ctx->free_done));
		}

		जइ (strstr(counter_पङ्क्तिः, "test_perf_free_defer_a") != nullptr) {
			ctx->पङ्क्तयः_a++;
		}
	}, &context);

	जइ (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_done.load());
	EXPECT_EQ(context.पङ्क्तयः_a, 0);

	// the freed entry must not reappear after deferred cleanup
	संरचना गणनम्_Context {
		पूर्णाङ्क पङ्क्तयः_a{0};
	} गणनम्_context;

	perf_iterate_all([](ध्रुव वर्ण *counter_पङ्क्तिः, शून्य *user) {
		जइ (strstr(counter_पङ्क्तिः, "test_perf_free_defer_a") != nullptr) {
			static_cast<गणनम्_Context *>(user)->पङ्क्तयः_a++;
		}
	}, &गणनम्_context);

	EXPECT_EQ(गणनम्_context.पङ्क्तयः_a, 0);

	perf_free(counter_b);
}

TEST(PerfCounter, FreeIsDeferredUntilLastOverlappingIterationEnds)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_counter_t counter_a = perf_alloc(PC_गणनम्, "test_perf_overlap_a");
	perf_counter_t counter_b = perf_alloc(PC_गणनम्, "test_perf_overlap_b");
	ASSERT_NE(counter_a, nullptr);
	ASSERT_NE(counter_b, nullptr);

	संरचना IterateContext {
		std::atomic<तार्किक> entered{मिच्छ};
		std::atomic<तार्किक> release{मिच्छ};
		std::atomic<तार्किक> timed_out{मिच्छ};
		पूर्णाङ्क पङ्क्तयः_a{0};
		पूर्णाङ्क replacement_पङ्क्तयः{0};
	} first_context, second_context;

	perf_callback callback = [](ध्रुव वर्ण *counter_पङ्क्तिः, शून्य *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		जइ (strstr(counter_पङ्क्तिः, "test_perf_overlap_b") != nullptr) {
			ctx->entered.store(सच्च);

			जइ (!wait_until(ctx->release)) {
				ctx->timed_out.store(सच्च);
			}
		}

		जइ (strstr(counter_पङ्क्तिः, "test_perf_overlap_a") != nullptr) {
			ctx->पङ्क्तयः_a++;
		}

		जइ (strstr(counter_पङ्क्तिः, "test_perf_overlap_replacement") != nullptr) {
			ctx->replacement_पङ्क्तयः++;
		}
	};

	std::thread first_thread([&]() { perf_iterate_all(callback, &first_context); });

	जइ (!wait_until(first_context.entered)) {
		first_context.release.store(सच्च);
		first_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread second_thread([&]() { perf_iterate_all(callback, &second_context); });

	जइ (!wait_until(second_context.entered)) {
		first_context.release.store(सच्च);
		second_context.release.store(सच्च);
		first_thread.join();
		second_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<तार्किक> free_done{मिच्छ};
	std::thread free_thread([&]() {
		perf_free(counter_a);
		free_done.store(सच्च);
	});

	जइ (!wait_until(free_done)) {
		first_context.release.store(सच्च);
		second_context.release.store(सच्च);
		first_thread.join();
		second_thread.join();
		free_thread.join();
		perf_free(counter_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	first_context.release.store(सच्च);
	first_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_counter_t replacement = perf_alloc(PC_गणनम्, "test_perf_overlap_replacement");

	जइ (replacement == nullptr) {
		second_context.release.store(सच्च);
		second_thread.join();
		perf_free(counter_b);
		FAIL() << "replacement counter allocation failed";
	}

	second_context.release.store(सच्च);
	second_thread.join();

	EXPECT_FALSE(first_context.timed_out.load());
	EXPECT_FALSE(second_context.timed_out.load());
	EXPECT_EQ(first_context.पङ्क्तयः_a, 0);
	EXPECT_EQ(second_context.पङ्क्तयः_a, 0);
	EXPECT_EQ(second_context.replacement_पङ्क्तयः, 0);

	perf_free(replacement);
	perf_free(counter_b);
}
