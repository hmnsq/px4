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
	ثابت خپلکاری deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	طالما (!condition.بارول() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	أرجع condition.بارول();
}

} // namespace

TEST(PerfCounter, Iterate_کرښه_Content)
{
	perf_counter_t counter = perf_alloc(PC_شمېر, "test_perf_line_content");
	ASSERT_NE(counter, تش_اشاره);

	perf_شمېر(counter);
	perf_شمېر(counter);
	perf_شمېر(counter);

	بنية Context {
		std::string کرښه;
	} context;

	perf_iterate_all([](ثابت حرف *counter_کرښه, فراغ *user) {
		إذا (مزي_پلټنه(counter_کرښه, "test_perf_line_content") != تش_اشاره) {
			static_cast<Context *>(user)->کرښه = counter_کرښه;
		}
	}, &context);

	EXPECT_EQ(context.کرښه, "test_perf_line_content: 3 events");

	perf_free(counter);
}

TEST(PerfCounter, Concurrent_Alloc_Does_Not_بلاک_On_Slow_Callback)
{
	perf_counter_t counter = perf_alloc(PC_شمېر, "test_perf_slow_callback");
	ASSERT_NE(counter, تش_اشاره);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	بنية Context {
		std::atomic<منطقي> first{ریښتیا};
		std::atomic<منطقي> alloc_بشپړ{خطأ};
		std::atomic<منطقي> alloc_succeeded{خطأ};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](ثابت حرف *, فراغ *user) {
		Context *ctx = static_cast<Context *>(user);

		إذا (ctx->first.exchange(خطأ)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_counter_t other = perf_alloc(PC_شمېر, "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != تش_اشاره);
				perf_free(other);
				ctx->alloc_بشپړ.store(ریښتیا);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_بشپړ));
		}
	}, &context);

	إذا (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_بشپړ.بارول());
	EXPECT_TRUE(context.alloc_succeeded.بارول());

	perf_free(counter);
}

TEST(PerfCounter, AllocFromCallbackDoesNotDeadlock)
{
	perf_counter_t counter = perf_alloc(PC_شمېر, "test_perf_callback_alloc");
	ASSERT_NE(counter, تش_اشاره);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	بنية Context {
		std::atomic<منطقي> first{ریښتیا};
		std::atomic<منطقي> بشپړ{خطأ};
		perf_counter_t allocated{تش_اشاره};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](ثابت حرف *, فراغ *user) {
			Context *ctx = static_cast<Context *>(user);

			إذا (ctx->first.exchange(خطأ)) {
				ctx->allocated = perf_alloc(PC_شمېر, "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.بشپړ.store(ریښتیا);
	});

	إذا (!wait_until(context.بشپړ)) {
		iterate_thread.detach();
		FAIL() << "perf_alloc() from the callback deadlocked";
	}

	iterate_thread.join();
	EXPECT_NE(context.allocated, تش_اشاره);

	perf_free(context.allocated);
	perf_free(counter);
}

TEST(PerfCounter, FreeDuringIterationIsDeferred)
{
	// allocation order puts counter_b at the list head, so it is visited first
	perf_counter_t counter_a = perf_alloc(PC_شمېر, "test_perf_free_defer_a");
	perf_counter_t counter_b = perf_alloc(PC_شمېر, "test_perf_free_defer_b");
	ASSERT_NE(counter_a, تش_اشاره);
	ASSERT_NE(counter_b, تش_اشاره);

	بنية Context {
		perf_counter_t counter_a;
		std::atomic<منطقي> free_بشپړ{خطأ};
		std::thread free_thread;
		صحيح کرښې_a{0};
	} context{counter_a};

	perf_iterate_all([](ثابت حرف *counter_کرښه, فراغ *user) {
		Context *ctx = static_cast<Context *>(user);

		إذا (!ctx->free_thread.joinable() && مزي_پلټنه(counter_کرښه, "test_perf_free_defer_b") != تش_اشاره) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->counter_a);
				ctx->free_بشپړ.store(ریښتیا);
			});

			EXPECT_TRUE(wait_until(ctx->free_بشپړ));
		}

		إذا (مزي_پلټنه(counter_کرښه, "test_perf_free_defer_a") != تش_اشاره) {
			ctx->کرښې_a++;
		}
	}, &context);

	إذا (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_بشپړ.بارول());
	EXPECT_EQ(context.کرښې_a, 0);

	// the freed entry must not reappear after deferred cleanup
	بنية شمېر_Context {
		صحيح کرښې_a{0};
	} شمېر_context;

	perf_iterate_all([](ثابت حرف *counter_کرښه, فراغ *user) {
		إذا (مزي_پلټنه(counter_کرښه, "test_perf_free_defer_a") != تش_اشاره) {
			static_cast<شمېر_Context *>(user)->کرښې_a++;
		}
	}, &شمېر_context);

	EXPECT_EQ(شمېر_context.کرښې_a, 0);

	perf_free(counter_b);
}

TEST(PerfCounter, Free_Is_Deferred_Until_وروستی_Overlapping_Iteration_پایونه)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_counter_t counter_a = perf_alloc(PC_شمېر, "test_perf_overlap_a");
	perf_counter_t counter_b = perf_alloc(PC_شمېر, "test_perf_overlap_b");
	ASSERT_NE(counter_a, تش_اشاره);
	ASSERT_NE(counter_b, تش_اشاره);

	بنية IterateContext {
		std::atomic<منطقي> entered{خطأ};
		std::atomic<منطقي> release{خطأ};
		std::atomic<منطقي> timed_بهر{خطأ};
		صحيح کرښې_a{0};
		صحيح replacement_کرښې{0};
	} لومړی_context, دوهم_context;

	perf_callback callback = [](ثابت حرف *counter_کرښه, فراغ *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		إذا (مزي_پلټنه(counter_کرښه, "test_perf_overlap_b") != تش_اشاره) {
			ctx->entered.store(ریښتیا);

			إذا (!wait_until(ctx->release)) {
				ctx->timed_بهر.store(ریښتیا);
			}
		}

		إذا (مزي_پلټنه(counter_کرښه, "test_perf_overlap_a") != تش_اشاره) {
			ctx->کرښې_a++;
		}

		إذا (مزي_پلټنه(counter_کرښه, "test_perf_overlap_replacement") != تش_اشاره) {
			ctx->replacement_کرښې++;
		}
	};

	std::thread لومړی_thread([&]() { perf_iterate_all(callback, &لومړی_context); });

	إذا (!wait_until(لومړی_context.entered)) {
		لومړی_context.release.store(ریښتیا);
		لومړی_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread دوهم_thread([&]() { perf_iterate_all(callback, &دوهم_context); });

	إذا (!wait_until(دوهم_context.entered)) {
		لومړی_context.release.store(ریښتیا);
		دوهم_context.release.store(ریښتیا);
		لومړی_thread.join();
		دوهم_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<منطقي> free_بشپړ{خطأ};
	std::thread free_thread([&]() {
		perf_free(counter_a);
		free_بشپړ.store(ریښتیا);
	});

	إذا (!wait_until(free_بشپړ)) {
		لومړی_context.release.store(ریښتیا);
		دوهم_context.release.store(ریښتیا);
		لومړی_thread.join();
		دوهم_thread.join();
		free_thread.join();
		perf_free(counter_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	لومړی_context.release.store(ریښتیا);
	لومړی_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_counter_t replacement = perf_alloc(PC_شمېر, "test_perf_overlap_replacement");

	إذا (replacement == تش_اشاره) {
		دوهم_context.release.store(ریښتیا);
		دوهم_thread.join();
		perf_free(counter_b);
		FAIL() << "replacement counter allocation failed";
	}

	دوهم_context.release.store(ریښتیا);
	دوهم_thread.join();

	EXPECT_FALSE(لومړی_context.timed_بهر.بارول());
	EXPECT_FALSE(دوهم_context.timed_بهر.بارول());
	EXPECT_EQ(لومړی_context.کرښې_a, 0);
	EXPECT_EQ(دوهم_context.کرښې_a, 0);
	EXPECT_EQ(دوهم_context.replacement_کرښې, 0);

	perf_free(replacement);
	perf_free(counter_b);
}
