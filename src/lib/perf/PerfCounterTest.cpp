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

#అన్తర్భావ <gtest/gtest.h>

#అన్తర్భావ <atomic>
#అన్తర్భావ <chrono>
#అన్తర్భావ <cstring>
#అన్తర్భావ <string>
#అన్తర్భావ <thread>

#అన్తర్భావ "perf_counter.h"

నామాకాశ
{

తార్కిక wait_until(స్థిరరాశి std::atomic<తార్కిక> &condition)
{
	స్థిరరాశి స్వయంచాలక deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	అయితే (!condition.ఎక్కించు() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	తిరిగి condition.ఎక్కించు();
}

} // namespace

TEST(PerfCounter, Iterate_పంక్తి_Content)
{
	perf_counter_t counter = perf_alloc(PC_లెక్క, "test_perf_line_content");
	ASSERT_NE(counter, శూన్యసూచిక);

	perf_లెక్క(counter);
	perf_లెక్క(counter);
	perf_లెక్క(counter);

	నిర్మాణం Context {
		std::string పంక్తి;
	} context;

	perf_iterate_all([](స్థిరరాశి అక్షరం *counter_పంక్తి, శూన్య *user) {
		ఒకవేళ (అక్షరశ్రేణి_శోధన(counter_పంక్తి, "test_perf_line_content") != శూన్యసూచిక) {
			static_cast<Context *>(user)->పంక్తి = counter_పంక్తి;
		}
	}, &context);

	EXPECT_EQ(context.పంక్తి, "test_perf_line_content: 3 events");

	perf_free(counter);
}

TEST(PerfCounter, Concurrent_Alloc_Does_Not_ఖండం_On_Slow_Callback)
{
	perf_counter_t counter = perf_alloc(PC_లెక్క, "test_perf_slow_callback");
	ASSERT_NE(counter, శూన్యసూచిక);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	నిర్మాణం Context {
		std::atomic<తార్కిక> first{నిజం};
		std::atomic<తార్కిక> alloc_పూర్తి{అబద్ధం};
		std::atomic<తార్కిక> alloc_succeeded{అబద్ధం};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](స్థిరరాశి అక్షరం *, శూన్య *user) {
		Context *ctx = static_cast<Context *>(user);

		ఒకవేళ (ctx->first.exchange(అబద్ధం)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_counter_t other = perf_alloc(PC_లెక్క, "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != శూన్యసూచిక);
				perf_free(other);
				ctx->alloc_పూర్తి.store(నిజం);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_పూర్తి));
		}
	}, &context);

	ఒకవేళ (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_పూర్తి.ఎక్కించు());
	EXPECT_TRUE(context.alloc_succeeded.ఎక్కించు());

	perf_free(counter);
}

TEST(PerfCounter, AllocFromCallbackDoesNotDeadlock)
{
	perf_counter_t counter = perf_alloc(PC_లెక్క, "test_perf_callback_alloc");
	ASSERT_NE(counter, శూన్యసూచిక);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	నిర్మాణం Context {
		std::atomic<తార్కిక> first{నిజం};
		std::atomic<తార్కిక> పూర్తి{అబద్ధం};
		perf_counter_t allocated{శూన్యసూచిక};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](స్థిరరాశి అక్షరం *, శూన్య *user) {
			Context *ctx = static_cast<Context *>(user);

			ఒకవేళ (ctx->first.exchange(అబద్ధం)) {
				ctx->allocated = perf_alloc(PC_లెక్క, "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.పూర్తి.store(నిజం);
	});

	ఒకవేళ (!wait_until(context.పూర్తి)) {
		iterate_thread.detach();
		FAIL() << "perf_alloc() from the callback deadlocked";
	}

	iterate_thread.join();
	EXPECT_NE(context.allocated, శూన్యసూచిక);

	perf_free(context.allocated);
	perf_free(counter);
}

TEST(PerfCounter, FreeDuringIterationIsDeferred)
{
	// allocation order puts counter_b at the list head, so it is visited first
	perf_counter_t counter_a = perf_alloc(PC_లెక్క, "test_perf_free_defer_a");
	perf_counter_t counter_b = perf_alloc(PC_లెక్క, "test_perf_free_defer_b");
	ASSERT_NE(counter_a, శూన్యసూచిక);
	ASSERT_NE(counter_b, శూన్యసూచిక);

	నిర్మాణం Context {
		perf_counter_t counter_a;
		std::atomic<తార్కిక> free_పూర్తి{అబద్ధం};
		std::thread free_thread;
		పూర్ణాంకం పంక్తులు_a{0};
	} context{counter_a};

	perf_iterate_all([](స్థిరరాశి అక్షరం *counter_పంక్తి, శూన్య *user) {
		Context *ctx = static_cast<Context *>(user);

		ఒకవేళ (!ctx->free_thread.joinable() && అక్షరశ్రేణి_శోధన(counter_పంక్తి, "test_perf_free_defer_b") != శూన్యసూచిక) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->counter_a);
				ctx->free_పూర్తి.store(నిజం);
			});

			EXPECT_TRUE(wait_until(ctx->free_పూర్తి));
		}

		ఒకవేళ (అక్షరశ్రేణి_శోధన(counter_పంక్తి, "test_perf_free_defer_a") != శూన్యసూచిక) {
			ctx->పంక్తులు_a++;
		}
	}, &context);

	ఒకవేళ (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_పూర్తి.ఎక్కించు());
	EXPECT_EQ(context.పంక్తులు_a, 0);

	// the freed entry must not reappear after deferred cleanup
	నిర్మాణం లెక్క_Context {
		పూర్ణాంకం పంక్తులు_a{0};
	} లెక్క_context;

	perf_iterate_all([](స్థిరరాశి అక్షరం *counter_పంక్తి, శూన్య *user) {
		ఒకవేళ (అక్షరశ్రేణి_శోధన(counter_పంక్తి, "test_perf_free_defer_a") != శూన్యసూచిక) {
			static_cast<లెక్క_Context *>(user)->పంక్తులు_a++;
		}
	}, &లెక్క_context);

	EXPECT_EQ(లెక్క_context.పంక్తులు_a, 0);

	perf_free(counter_b);
}

TEST(PerfCounter, Free_Is_Deferred_Until_చివరి_Overlapping_Iteration_చివర్లు)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_counter_t counter_a = perf_alloc(PC_లెక్క, "test_perf_overlap_a");
	perf_counter_t counter_b = perf_alloc(PC_లెక్క, "test_perf_overlap_b");
	ASSERT_NE(counter_a, శూన్యసూచిక);
	ASSERT_NE(counter_b, శూన్యసూచిక);

	నిర్మాణం IterateContext {
		std::atomic<తార్కిక> entered{అబద్ధం};
		std::atomic<తార్కిక> release{అబద్ధం};
		std::atomic<తార్కిక> timed_బహిర్గతం{అబద్ధం};
		పూర్ణాంకం పంక్తులు_a{0};
		పూర్ణాంకం replacement_పంక్తులు{0};
	} మొదటి_context, రెండవ_context;

	perf_callback callback = [](స్థిరరాశి అక్షరం *counter_పంక్తి, శూన్య *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		ఒకవేళ (అక్షరశ్రేణి_శోధన(counter_పంక్తి, "test_perf_overlap_b") != శూన్యసూచిక) {
			ctx->entered.store(నిజం);

			ఒకవేళ (!wait_until(ctx->release)) {
				ctx->timed_బహిర్గతం.store(నిజం);
			}
		}

		ఒకవేళ (అక్షరశ్రేణి_శోధన(counter_పంక్తి, "test_perf_overlap_a") != శూన్యసూచిక) {
			ctx->పంక్తులు_a++;
		}

		ఒకవేళ (అక్షరశ్రేణి_శోధన(counter_పంక్తి, "test_perf_overlap_replacement") != శూన్యసూచిక) {
			ctx->replacement_పంక్తులు++;
		}
	};

	std::thread మొదటి_thread([&]() { perf_iterate_all(callback, &మొదటి_context); });

	ఒకవేళ (!wait_until(మొదటి_context.entered)) {
		మొదటి_context.release.store(నిజం);
		మొదటి_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread రెండవ_thread([&]() { perf_iterate_all(callback, &రెండవ_context); });

	ఒకవేళ (!wait_until(రెండవ_context.entered)) {
		మొదటి_context.release.store(నిజం);
		రెండవ_context.release.store(నిజం);
		మొదటి_thread.join();
		రెండవ_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<తార్కిక> free_పూర్తి{అబద్ధం};
	std::thread free_thread([&]() {
		perf_free(counter_a);
		free_పూర్తి.store(నిజం);
	});

	ఒకవేళ (!wait_until(free_పూర్తి)) {
		మొదటి_context.release.store(నిజం);
		రెండవ_context.release.store(నిజం);
		మొదటి_thread.join();
		రెండవ_thread.join();
		free_thread.join();
		perf_free(counter_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	మొదటి_context.release.store(నిజం);
	మొదటి_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_counter_t replacement = perf_alloc(PC_లెక్క, "test_perf_overlap_replacement");

	ఒకవేళ (replacement == శూన్యసూచిక) {
		రెండవ_context.release.store(నిజం);
		రెండవ_thread.join();
		perf_free(counter_b);
		FAIL() << "replacement counter allocation failed";
	}

	రెండవ_context.release.store(నిజం);
	రెండవ_thread.join();

	EXPECT_FALSE(మొదటి_context.timed_బహిర్గతం.ఎక్కించు());
	EXPECT_FALSE(రెండవ_context.timed_బహిర్గతం.ఎక్కించు());
	EXPECT_EQ(మొదటి_context.పంక్తులు_a, 0);
	EXPECT_EQ(రెండవ_context.పంక్తులు_a, 0);
	EXPECT_EQ(రెండవ_context.replacement_పంక్తులు, 0);

	perf_free(replacement);
	perf_free(counter_b);
}
