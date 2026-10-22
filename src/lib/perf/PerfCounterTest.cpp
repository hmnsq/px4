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

#شامل <gtest/gtest.h>

#شامل <atomic>
#شامل <chrono>
#شامل <cstring>
#شامل <string>
#شامل <thread>

#شامل "perf_counter.h"

نامخلا
{

منطقی wait_until(ثابت std::atomic<منطقی> &condition)
{
	ثابت خودکار deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	جب (!condition.بھرو() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	واپس condition.بھرو();
}

} // namespace

TEST(Perf_شمار, Iterate_سطر_Content)
{
	perf_شمار_t شمار_2 = perf_alloc(PC_گنتی, "test_perf_line_content");
	ASSERT_NE(شمار_2, خالی_اشاریہ);

	perf_گنتی(شمار_2);
	perf_گنتی(شمار_2);
	perf_گنتی(شمار_2);

	ساخت Context {
		std::string سطر;
	} context;

	perf_iterate_all([](ثابت حرف *شمار_سطر, خالی *user) {
		اگر (لڑی_تلاش(شمار_سطر, "test_perf_line_content") != خالی_اشاریہ) {
			static_cast<Context *>(user)->سطر = شمار_سطر;
		}
	}, &context);

	EXPECT_EQ(context.سطر, "test_perf_line_content: 3 events");

	perf_free(شمار_2);
}

TEST(Perf_شمار, Concurrent_Alloc_Does_Not_بلاک_On_Slow_Callback)
{
	perf_شمار_t شمار_2 = perf_alloc(PC_گنتی, "test_perf_slow_callback");
	ASSERT_NE(شمار_2, خالی_اشاریہ);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	ساخت Context {
		std::atomic<منطقی> first{سچ};
		std::atomic<منطقی> alloc_مکمل{جھوٹ};
		std::atomic<منطقی> alloc_succeeded{جھوٹ};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](ثابت حرف *, خالی *user) {
		Context *ctx = static_cast<Context *>(user);

		اگر (ctx->first.exchange(جھوٹ)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_شمار_t other = perf_alloc(PC_گنتی, "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != خالی_اشاریہ);
				perf_free(other);
				ctx->alloc_مکمل.store(سچ);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_مکمل));
		}
	}, &context);

	اگر (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_مکمل.بھرو());
	EXPECT_TRUE(context.alloc_succeeded.بھرو());

	perf_free(شمار_2);
}

TEST(Perf_شمار, AllocFromCallbackDoesNotDeadlock)
{
	perf_شمار_t شمار_2 = perf_alloc(PC_گنتی, "test_perf_callback_alloc");
	ASSERT_NE(شمار_2, خالی_اشاریہ);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	ساخت Context {
		std::atomic<منطقی> first{سچ};
		std::atomic<منطقی> مکمل{جھوٹ};
		perf_شمار_t allocated{خالی_اشاریہ};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](ثابت حرف *, خالی *user) {
			Context *ctx = static_cast<Context *>(user);

			اگر (ctx->first.exchange(جھوٹ)) {
				ctx->allocated = perf_alloc(PC_گنتی, "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.مکمل.store(سچ);
	});

	اگر (!wait_until(context.مکمل)) {
		iterate_thread.detach();
		FAIL() << "perf_alloc() from the callback deadlocked";
	}

	iterate_thread.join();
	EXPECT_NE(context.allocated, خالی_اشاریہ);

	perf_free(context.allocated);
	perf_free(شمار_2);
}

TEST(Perf_شمار, FreeDuringIterationIsDeferred)
{
	// allocation order puts counter_b at the list head, so it is visited first
	perf_شمار_t شمار_a = perf_alloc(PC_گنتی, "test_perf_free_defer_a");
	perf_شمار_t شمار_b = perf_alloc(PC_گنتی, "test_perf_free_defer_b");
	ASSERT_NE(شمار_a, خالی_اشاریہ);
	ASSERT_NE(شمار_b, خالی_اشاریہ);

	ساخت Context {
		perf_شمار_t شمار_a;
		std::atomic<منطقی> free_مکمل{جھوٹ};
		std::thread free_thread;
		صحیح سطریں_a{0};
	} context{شمار_a};

	perf_iterate_all([](ثابت حرف *شمار_سطر, خالی *user) {
		Context *ctx = static_cast<Context *>(user);

		اگر (!ctx->free_thread.joinable() && لڑی_تلاش(شمار_سطر, "test_perf_free_defer_b") != خالی_اشاریہ) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->شمار_a);
				ctx->free_مکمل.store(سچ);
			});

			EXPECT_TRUE(wait_until(ctx->free_مکمل));
		}

		اگر (لڑی_تلاش(شمار_سطر, "test_perf_free_defer_a") != خالی_اشاریہ) {
			ctx->سطریں_a++;
		}
	}, &context);

	اگر (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_مکمل.بھرو());
	EXPECT_EQ(context.سطریں_a, 0);

	// the freed entry must not reappear after deferred cleanup
	ساخت گنتی_Context {
		صحیح سطریں_a{0};
	} گنتی_context;

	perf_iterate_all([](ثابت حرف *شمار_سطر, خالی *user) {
		اگر (لڑی_تلاش(شمار_سطر, "test_perf_free_defer_a") != خالی_اشاریہ) {
			static_cast<گنتی_Context *>(user)->سطریں_a++;
		}
	}, &گنتی_context);

	EXPECT_EQ(گنتی_context.سطریں_a, 0);

	perf_free(شمار_b);
}

TEST(Perf_شمار, Free_Is_Deferred_Until_آخری_Overlapping_Iteration_سرے)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_شمار_t شمار_a = perf_alloc(PC_گنتی, "test_perf_overlap_a");
	perf_شمار_t شمار_b = perf_alloc(PC_گنتی, "test_perf_overlap_b");
	ASSERT_NE(شمار_a, خالی_اشاریہ);
	ASSERT_NE(شمار_b, خالی_اشاریہ);

	ساخت IterateContext {
		std::atomic<منطقی> entered{جھوٹ};
		std::atomic<منطقی> release{جھوٹ};
		std::atomic<منطقی> timed_برآمد{جھوٹ};
		صحیح سطریں_a{0};
		صحیح replacement_سطریں{0};
	} پہلا_context, دوسرا_context;

	perf_callback callback = [](ثابت حرف *شمار_سطر, خالی *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		اگر (لڑی_تلاش(شمار_سطر, "test_perf_overlap_b") != خالی_اشاریہ) {
			ctx->entered.store(سچ);

			اگر (!wait_until(ctx->release)) {
				ctx->timed_برآمد.store(سچ);
			}
		}

		اگر (لڑی_تلاش(شمار_سطر, "test_perf_overlap_a") != خالی_اشاریہ) {
			ctx->سطریں_a++;
		}

		اگر (لڑی_تلاش(شمار_سطر, "test_perf_overlap_replacement") != خالی_اشاریہ) {
			ctx->replacement_سطریں++;
		}
	};

	std::thread پہلا_thread([&]() { perf_iterate_all(callback, &پہلا_context); });

	اگر (!wait_until(پہلا_context.entered)) {
		پہلا_context.release.store(سچ);
		پہلا_thread.join();
		perf_free(شمار_a);
		perf_free(شمار_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread دوسرا_thread([&]() { perf_iterate_all(callback, &دوسرا_context); });

	اگر (!wait_until(دوسرا_context.entered)) {
		پہلا_context.release.store(سچ);
		دوسرا_context.release.store(سچ);
		پہلا_thread.join();
		دوسرا_thread.join();
		perf_free(شمار_a);
		perf_free(شمار_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<منطقی> free_مکمل{جھوٹ};
	std::thread free_thread([&]() {
		perf_free(شمار_a);
		free_مکمل.store(سچ);
	});

	اگر (!wait_until(free_مکمل)) {
		پہلا_context.release.store(سچ);
		دوسرا_context.release.store(سچ);
		پہلا_thread.join();
		دوسرا_thread.join();
		free_thread.join();
		perf_free(شمار_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	پہلا_context.release.store(سچ);
	پہلا_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_شمار_t replacement = perf_alloc(PC_گنتی, "test_perf_overlap_replacement");

	اگر (replacement == خالی_اشاریہ) {
		دوسرا_context.release.store(سچ);
		دوسرا_thread.join();
		perf_free(شمار_b);
		FAIL() << "replacement counter allocation failed";
	}

	دوسرا_context.release.store(سچ);
	دوسرا_thread.join();

	EXPECT_FALSE(پہلا_context.timed_برآمد.بھرو());
	EXPECT_FALSE(دوسرا_context.timed_برآمد.بھرو());
	EXPECT_EQ(پہلا_context.سطریں_a, 0);
	EXPECT_EQ(دوسرا_context.سطریں_a, 0);
	EXPECT_EQ(دوسرا_context.replacement_سطریں, 0);

	perf_free(replacement);
	perf_free(شمار_b);
}
