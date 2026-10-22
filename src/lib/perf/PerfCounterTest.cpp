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
	ثابت خودڪار deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	طالما (!condition.ڀريو() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	أرجع condition.ڀريو();
}

} // namespace

TEST(PerfCounter, Iterate_سٽ_Content)
{
	perf_counter_t counter = perf_alloc(PC_ڳڻپ, "test_perf_line_content");
	ASSERT_NE(counter, خالي_اشارو);

	perf_ڳڻپ(counter);
	perf_ڳڻپ(counter);
	perf_ڳڻپ(counter);

	بنية Context {
		std::string سٽ;
	} context;

	perf_iterate_all([](ثابت حرف *counter_سٽ, فراغ *user) {
		إذا (لڙي_ڳولا(counter_سٽ, "test_perf_line_content") != خالي_اشارو) {
			static_cast<Context *>(user)->سٽ = counter_سٽ;
		}
	}, &context);

	EXPECT_EQ(context.سٽ, "test_perf_line_content: 3 events");

	perf_free(counter);
}

TEST(PerfCounter, Concurrent_Alloc_Does_Not_بلاڪ_On_Slow_Callback)
{
	perf_counter_t counter = perf_alloc(PC_ڳڻپ, "test_perf_slow_callback");
	ASSERT_NE(counter, خالي_اشارو);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	بنية Context {
		std::atomic<منطقي> first{سچ};
		std::atomic<منطقي> alloc_پورو{خطأ};
		std::atomic<منطقي> alloc_succeeded{خطأ};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](ثابت حرف *, فراغ *user) {
		Context *ctx = static_cast<Context *>(user);

		إذا (ctx->first.exchange(خطأ)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_counter_t other = perf_alloc(PC_ڳڻپ, "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != خالي_اشارو);
				perf_free(other);
				ctx->alloc_پورو.store(سچ);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_پورو));
		}
	}, &context);

	إذا (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_پورو.ڀريو());
	EXPECT_TRUE(context.alloc_succeeded.ڀريو());

	perf_free(counter);
}

TEST(PerfCounter, AllocFromCallbackDoesNotDeadlock)
{
	perf_counter_t counter = perf_alloc(PC_ڳڻپ, "test_perf_callback_alloc");
	ASSERT_NE(counter, خالي_اشارو);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	بنية Context {
		std::atomic<منطقي> first{سچ};
		std::atomic<منطقي> پورو{خطأ};
		perf_counter_t allocated{خالي_اشارو};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](ثابت حرف *, فراغ *user) {
			Context *ctx = static_cast<Context *>(user);

			إذا (ctx->first.exchange(خطأ)) {
				ctx->allocated = perf_alloc(PC_ڳڻپ, "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.پورو.store(سچ);
	});

	إذا (!wait_until(context.پورو)) {
		iterate_thread.detach();
		FAIL() << "perf_alloc() from the callback deadlocked";
	}

	iterate_thread.join();
	EXPECT_NE(context.allocated, خالي_اشارو);

	perf_free(context.allocated);
	perf_free(counter);
}

TEST(PerfCounter, FreeDuringIterationIsDeferred)
{
	// allocation order puts counter_b at the list head, so it is visited first
	perf_counter_t counter_a = perf_alloc(PC_ڳڻپ, "test_perf_free_defer_a");
	perf_counter_t counter_b = perf_alloc(PC_ڳڻپ, "test_perf_free_defer_b");
	ASSERT_NE(counter_a, خالي_اشارو);
	ASSERT_NE(counter_b, خالي_اشارو);

	بنية Context {
		perf_counter_t counter_a;
		std::atomic<منطقي> free_پورو{خطأ};
		std::thread free_thread;
		صحيح سٽون_a{0};
	} context{counter_a};

	perf_iterate_all([](ثابت حرف *counter_سٽ, فراغ *user) {
		Context *ctx = static_cast<Context *>(user);

		إذا (!ctx->free_thread.joinable() && لڙي_ڳولا(counter_سٽ, "test_perf_free_defer_b") != خالي_اشارو) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->counter_a);
				ctx->free_پورو.store(سچ);
			});

			EXPECT_TRUE(wait_until(ctx->free_پورو));
		}

		إذا (لڙي_ڳولا(counter_سٽ, "test_perf_free_defer_a") != خالي_اشارو) {
			ctx->سٽون_a++;
		}
	}, &context);

	إذا (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_پورو.ڀريو());
	EXPECT_EQ(context.سٽون_a, 0);

	// the freed entry must not reappear after deferred cleanup
	بنية ڳڻپ_Context {
		صحيح سٽون_a{0};
	} ڳڻپ_context;

	perf_iterate_all([](ثابت حرف *counter_سٽ, فراغ *user) {
		إذا (لڙي_ڳولا(counter_سٽ, "test_perf_free_defer_a") != خالي_اشارو) {
			static_cast<ڳڻپ_Context *>(user)->سٽون_a++;
		}
	}, &ڳڻپ_context);

	EXPECT_EQ(ڳڻپ_context.سٽون_a, 0);

	perf_free(counter_b);
}

TEST(PerfCounter, Free_Is_Deferred_Until_آخري_Overlapping_Iteration_پڇاڙيون)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_counter_t counter_a = perf_alloc(PC_ڳڻپ, "test_perf_overlap_a");
	perf_counter_t counter_b = perf_alloc(PC_ڳڻپ, "test_perf_overlap_b");
	ASSERT_NE(counter_a, خالي_اشارو);
	ASSERT_NE(counter_b, خالي_اشارو);

	بنية IterateContext {
		std::atomic<منطقي> entered{خطأ};
		std::atomic<منطقي> release{خطأ};
		std::atomic<منطقي> timed_ٻاهريون{خطأ};
		صحيح سٽون_a{0};
		صحيح replacement_سٽون{0};
	} پهريون_context, ٻيو_context;

	perf_callback callback = [](ثابت حرف *counter_سٽ, فراغ *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		إذا (لڙي_ڳولا(counter_سٽ, "test_perf_overlap_b") != خالي_اشارو) {
			ctx->entered.store(سچ);

			إذا (!wait_until(ctx->release)) {
				ctx->timed_ٻاهريون.store(سچ);
			}
		}

		إذا (لڙي_ڳولا(counter_سٽ, "test_perf_overlap_a") != خالي_اشارو) {
			ctx->سٽون_a++;
		}

		إذا (لڙي_ڳولا(counter_سٽ, "test_perf_overlap_replacement") != خالي_اشارو) {
			ctx->replacement_سٽون++;
		}
	};

	std::thread پهريون_thread([&]() { perf_iterate_all(callback, &پهريون_context); });

	إذا (!wait_until(پهريون_context.entered)) {
		پهريون_context.release.store(سچ);
		پهريون_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread ٻيو_thread([&]() { perf_iterate_all(callback, &ٻيو_context); });

	إذا (!wait_until(ٻيو_context.entered)) {
		پهريون_context.release.store(سچ);
		ٻيو_context.release.store(سچ);
		پهريون_thread.join();
		ٻيو_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<منطقي> free_پورو{خطأ};
	std::thread free_thread([&]() {
		perf_free(counter_a);
		free_پورو.store(سچ);
	});

	إذا (!wait_until(free_پورو)) {
		پهريون_context.release.store(سچ);
		ٻيو_context.release.store(سچ);
		پهريون_thread.join();
		ٻيو_thread.join();
		free_thread.join();
		perf_free(counter_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	پهريون_context.release.store(سچ);
	پهريون_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_counter_t replacement = perf_alloc(PC_ڳڻپ, "test_perf_overlap_replacement");

	إذا (replacement == خالي_اشارو) {
		ٻيو_context.release.store(سچ);
		ٻيو_thread.join();
		perf_free(counter_b);
		FAIL() << "replacement counter allocation failed";
	}

	ٻيو_context.release.store(سچ);
	ٻيو_thread.join();

	EXPECT_FALSE(پهريون_context.timed_ٻاهريون.ڀريو());
	EXPECT_FALSE(ٻيو_context.timed_ٻاهريون.ڀريو());
	EXPECT_EQ(پهريون_context.سٽون_a, 0);
	EXPECT_EQ(ٻيو_context.سٽون_a, 0);
	EXPECT_EQ(ٻيو_context.replacement_سٽون, 0);

	perf_free(replacement);
	perf_free(counter_b);
}
