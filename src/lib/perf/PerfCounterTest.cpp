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

#ضمن <gtest/gtest.h>

#ضمن <atomic>
#ضمن <chrono>
#ضمن <cstring>
#ضمن <string>
#ضمن <thread>

#ضمن "perf_counter.h"

نطاق
{

منطقي wait_until(ثابت std::atomic<منطقي> &condition)
{
	ثابت خودکار deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	طالما (!condition.لدو() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	أرجع condition.لدو();
}

} // namespace

TEST(PerfCounter, Iterate_سطر_Content)
{
	perf_counter_t counter = perf_alloc(PC_گنتی, "test_perf_line_content");
	ASSERT_NE(counter, خالی_سوچک);

	perf_گنتی(counter);
	perf_گنتی(counter);
	perf_گنتی(counter);

	بنية Context {
		std::string سطر;
	} context;

	perf_iterate_all([](ثابت حرف *counter_سطر, فراغ *user) {
		إذا (لڑی_کھوج(counter_سطر, "test_perf_line_content") != خالی_سوچک) {
			static_cast<Context *>(user)->سطر = counter_سطر;
		}
	}, &context);

	EXPECT_EQ(context.سطر, "test_perf_line_content: 3 events");

	perf_free(counter);
}

TEST(PerfCounter, Concurrent_Alloc_Does_Not_بلاک_On_Slow_Callback)
{
	perf_counter_t counter = perf_alloc(PC_گنتی, "test_perf_slow_callback");
	ASSERT_NE(counter, خالی_سوچک);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	بنية Context {
		std::atomic<منطقي> first{سچ};
		std::atomic<منطقي> alloc_پورا{خطأ};
		std::atomic<منطقي> alloc_succeeded{خطأ};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](ثابت حرف *, فراغ *user) {
		Context *ctx = static_cast<Context *>(user);

		إذا (ctx->first.exchange(خطأ)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_counter_t other = perf_alloc(PC_گنتی, "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != خالی_سوچک);
				perf_free(other);
				ctx->alloc_پورا.store(سچ);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_پورا));
		}
	}, &context);

	إذا (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_پورا.لدو());
	EXPECT_TRUE(context.alloc_succeeded.لدو());

	perf_free(counter);
}

TEST(PerfCounter, AllocFromCallbackDoesNotDeadlock)
{
	perf_counter_t counter = perf_alloc(PC_گنتی, "test_perf_callback_alloc");
	ASSERT_NE(counter, خالی_سوچک);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	بنية Context {
		std::atomic<منطقي> first{سچ};
		std::atomic<منطقي> پورا{خطأ};
		perf_counter_t allocated{خالی_سوچک};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](ثابت حرف *, فراغ *user) {
			Context *ctx = static_cast<Context *>(user);

			إذا (ctx->first.exchange(خطأ)) {
				ctx->allocated = perf_alloc(PC_گنتی, "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.پورا.store(سچ);
	});

	إذا (!wait_until(context.پورا)) {
		iterate_thread.detach();
		FAIL() << "perf_alloc() from the callback deadlocked";
	}

	iterate_thread.join();
	EXPECT_NE(context.allocated, خالی_سوچک);

	perf_free(context.allocated);
	perf_free(counter);
}

TEST(PerfCounter, FreeDuringIterationIsDeferred)
{
	// allocation order puts counter_b at the list head, so it is visited first
	perf_counter_t counter_a = perf_alloc(PC_گنتی, "test_perf_free_defer_a");
	perf_counter_t counter_b = perf_alloc(PC_گنتی, "test_perf_free_defer_b");
	ASSERT_NE(counter_a, خالی_سوچک);
	ASSERT_NE(counter_b, خالی_سوچک);

	بنية Context {
		perf_counter_t counter_a;
		std::atomic<منطقي> free_پورا{خطأ};
		std::thread free_thread;
		صحيح سطریں_a{0};
	} context{counter_a};

	perf_iterate_all([](ثابت حرف *counter_سطر, فراغ *user) {
		Context *ctx = static_cast<Context *>(user);

		إذا (!ctx->free_thread.joinable() && لڑی_کھوج(counter_سطر, "test_perf_free_defer_b") != خالی_سوچک) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->counter_a);
				ctx->free_پورا.store(سچ);
			});

			EXPECT_TRUE(wait_until(ctx->free_پورا));
		}

		إذا (لڑی_کھوج(counter_سطر, "test_perf_free_defer_a") != خالی_سوچک) {
			ctx->سطریں_a++;
		}
	}, &context);

	إذا (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_پورا.لدو());
	EXPECT_EQ(context.سطریں_a, 0);

	// the freed entry must not reappear after deferred cleanup
	بنية گنتی_Context {
		صحيح سطریں_a{0};
	} گنتی_context;

	perf_iterate_all([](ثابت حرف *counter_سطر, فراغ *user) {
		إذا (لڑی_کھوج(counter_سطر, "test_perf_free_defer_a") != خالی_سوچک) {
			static_cast<گنتی_Context *>(user)->سطریں_a++;
		}
	}, &گنتی_context);

	EXPECT_EQ(گنتی_context.سطریں_a, 0);

	perf_free(counter_b);
}

TEST(PerfCounter, Free_Is_Deferred_Until_آخری_Overlapping_Iteration_سرے)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_counter_t counter_a = perf_alloc(PC_گنتی, "test_perf_overlap_a");
	perf_counter_t counter_b = perf_alloc(PC_گنتی, "test_perf_overlap_b");
	ASSERT_NE(counter_a, خالی_سوچک);
	ASSERT_NE(counter_b, خالی_سوچک);

	بنية IterateContext {
		std::atomic<منطقي> entered{خطأ};
		std::atomic<منطقي> release{خطأ};
		std::atomic<منطقي> timed_باہرلا{خطأ};
		صحيح سطریں_a{0};
		صحيح replacement_سطریں{0};
	} پہلا_context, دوجا_context;

	perf_callback callback = [](ثابت حرف *counter_سطر, فراغ *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		إذا (لڑی_کھوج(counter_سطر, "test_perf_overlap_b") != خالی_سوچک) {
			ctx->entered.store(سچ);

			إذا (!wait_until(ctx->release)) {
				ctx->timed_باہرلا.store(سچ);
			}
		}

		إذا (لڑی_کھوج(counter_سطر, "test_perf_overlap_a") != خالی_سوچک) {
			ctx->سطریں_a++;
		}

		إذا (لڑی_کھوج(counter_سطر, "test_perf_overlap_replacement") != خالی_سوچک) {
			ctx->replacement_سطریں++;
		}
	};

	std::thread پہلا_thread([&]() { perf_iterate_all(callback, &پہلا_context); });

	إذا (!wait_until(پہلا_context.entered)) {
		پہلا_context.release.store(سچ);
		پہلا_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread دوجا_thread([&]() { perf_iterate_all(callback, &دوجا_context); });

	إذا (!wait_until(دوجا_context.entered)) {
		پہلا_context.release.store(سچ);
		دوجا_context.release.store(سچ);
		پہلا_thread.join();
		دوجا_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<منطقي> free_پورا{خطأ};
	std::thread free_thread([&]() {
		perf_free(counter_a);
		free_پورا.store(سچ);
	});

	إذا (!wait_until(free_پورا)) {
		پہلا_context.release.store(سچ);
		دوجا_context.release.store(سچ);
		پہلا_thread.join();
		دوجا_thread.join();
		free_thread.join();
		perf_free(counter_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	پہلا_context.release.store(سچ);
	پہلا_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_counter_t replacement = perf_alloc(PC_گنتی, "test_perf_overlap_replacement");

	إذا (replacement == خالی_سوچک) {
		دوجا_context.release.store(سچ);
		دوجا_thread.join();
		perf_free(counter_b);
		FAIL() << "replacement counter allocation failed";
	}

	دوجا_context.release.store(سچ);
	دوجا_thread.join();

	EXPECT_FALSE(پہلا_context.timed_باہرلا.لدو());
	EXPECT_FALSE(دوجا_context.timed_باہرلا.لدو());
	EXPECT_EQ(پہلا_context.سطریں_a, 0);
	EXPECT_EQ(دوجا_context.سطریں_a, 0);
	EXPECT_EQ(دوجا_context.replacement_سطریں, 0);

	perf_free(replacement);
	perf_free(counter_b);
}
