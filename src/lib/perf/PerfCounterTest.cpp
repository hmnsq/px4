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

#அந்தர்பாவ <gtest/gtest.h>

#அந்தர்பாவ <atomic>
#அந்தர்பாவ <chrono>
#அந்தர்பாவ <cstring>
#அந்தர்பாவ <string>
#அந்தர்பாவ <thread>

#அந்தர்பாவ "perf_counter.h"

நாமாகாஶ
{

தருக்க wait_until(மாறிலி std::atomic<தருக்க> &condition)
{
	மாறிலி auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	போது (!condition.load() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	திரும்பு condition.load();
}

} // namespace

TEST(PerfCounter, Iterate_வரி_Content)
{
	perf_counter_t counter = perf_alloc(PC_மொத்தம், "test_perf_line_content");
	ASSERT_NE(counter, nullptr);

	perf_மொத்தம்(counter);
	perf_மொத்தம்(counter);
	perf_மொத்தம்(counter);

	கட்டமைப்பு Context {
		std::string வரி;
	} context;

	perf_iterate_all([](மாறிலி எழுத்து *counter_வரி, வெறுமை *user) {
		எனில் (strstr(counter_வரி, "test_perf_line_content") != nullptr) {
			static_cast<Context *>(user)->வரி = counter_வரி;
		}
	}, &context);

	EXPECT_EQ(context.வரி, "test_perf_line_content: 3 events");

	perf_free(counter);
}

TEST(PerfCounter, ConcurrentAllocDoesNotBlockOnSlowCallback)
{
	perf_counter_t counter = perf_alloc(PC_மொத்தம், "test_perf_slow_callback");
	ASSERT_NE(counter, nullptr);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	கட்டமைப்பு Context {
		std::atomic<தருக்க> first{உண்மை};
		std::atomic<தருக்க> alloc_done{பொய்};
		std::atomic<தருக்க> alloc_succeeded{பொய்};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](மாறிலி எழுத்து *, வெறுமை *user) {
		Context *ctx = static_cast<Context *>(user);

		எனில் (ctx->first.exchange(பொய்)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_counter_t other = perf_alloc(PC_மொத்தம், "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != nullptr);
				perf_free(other);
				ctx->alloc_done.store(உண்மை);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_done));
		}
	}, &context);

	எனில் (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_done.load());
	EXPECT_TRUE(context.alloc_succeeded.load());

	perf_free(counter);
}

TEST(PerfCounter, AllocFromCallbackDoesNotDeadlock)
{
	perf_counter_t counter = perf_alloc(PC_மொத்தம், "test_perf_callback_alloc");
	ASSERT_NE(counter, nullptr);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	கட்டமைப்பு Context {
		std::atomic<தருக்க> first{உண்மை};
		std::atomic<தருக்க> done{பொய்};
		perf_counter_t allocated{nullptr};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](மாறிலி எழுத்து *, வெறுமை *user) {
			Context *ctx = static_cast<Context *>(user);

			எனில் (ctx->first.exchange(பொய்)) {
				ctx->allocated = perf_alloc(PC_மொத்தம், "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.done.store(உண்மை);
	});

	எனில் (!wait_until(context.done)) {
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
	perf_counter_t counter_a = perf_alloc(PC_மொத்தம், "test_perf_free_defer_a");
	perf_counter_t counter_b = perf_alloc(PC_மொத்தம், "test_perf_free_defer_b");
	ASSERT_NE(counter_a, nullptr);
	ASSERT_NE(counter_b, nullptr);

	கட்டமைப்பு Context {
		perf_counter_t counter_a;
		std::atomic<தருக்க> free_done{பொய்};
		std::thread free_thread;
		முழு வரிகள்_a{0};
	} context{counter_a};

	perf_iterate_all([](மாறிலி எழுத்து *counter_வரி, வெறுமை *user) {
		Context *ctx = static_cast<Context *>(user);

		எனில் (!ctx->free_thread.joinable() && strstr(counter_வரி, "test_perf_free_defer_b") != nullptr) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->counter_a);
				ctx->free_done.store(உண்மை);
			});

			EXPECT_TRUE(wait_until(ctx->free_done));
		}

		எனில் (strstr(counter_வரி, "test_perf_free_defer_a") != nullptr) {
			ctx->வரிகள்_a++;
		}
	}, &context);

	எனில் (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_done.load());
	EXPECT_EQ(context.வரிகள்_a, 0);

	// the freed entry must not reappear after deferred cleanup
	கட்டமைப்பு மொத்தம்_Context {
		முழு வரிகள்_a{0};
	} மொத்தம்_context;

	perf_iterate_all([](மாறிலி எழுத்து *counter_வரி, வெறுமை *user) {
		எனில் (strstr(counter_வரி, "test_perf_free_defer_a") != nullptr) {
			static_cast<மொத்தம்_Context *>(user)->வரிகள்_a++;
		}
	}, &மொத்தம்_context);

	EXPECT_EQ(மொத்தம்_context.வரிகள்_a, 0);

	perf_free(counter_b);
}

TEST(PerfCounter, FreeIsDeferredUntilLastOverlappingIterationEnds)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_counter_t counter_a = perf_alloc(PC_மொத்தம், "test_perf_overlap_a");
	perf_counter_t counter_b = perf_alloc(PC_மொத்தம், "test_perf_overlap_b");
	ASSERT_NE(counter_a, nullptr);
	ASSERT_NE(counter_b, nullptr);

	கட்டமைப்பு IterateContext {
		std::atomic<தருக்க> entered{பொய்};
		std::atomic<தருக்க> release{பொய்};
		std::atomic<தருக்க> timed_out{பொய்};
		முழு வரிகள்_a{0};
		முழு replacement_வரிகள்{0};
	} first_context, second_context;

	perf_callback callback = [](மாறிலி எழுத்து *counter_வரி, வெறுமை *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		எனில் (strstr(counter_வரி, "test_perf_overlap_b") != nullptr) {
			ctx->entered.store(உண்மை);

			எனில் (!wait_until(ctx->release)) {
				ctx->timed_out.store(உண்மை);
			}
		}

		எனில் (strstr(counter_வரி, "test_perf_overlap_a") != nullptr) {
			ctx->வரிகள்_a++;
		}

		எனில் (strstr(counter_வரி, "test_perf_overlap_replacement") != nullptr) {
			ctx->replacement_வரிகள்++;
		}
	};

	std::thread first_thread([&]() { perf_iterate_all(callback, &first_context); });

	எனில் (!wait_until(first_context.entered)) {
		first_context.release.store(உண்மை);
		first_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread second_thread([&]() { perf_iterate_all(callback, &second_context); });

	எனில் (!wait_until(second_context.entered)) {
		first_context.release.store(உண்மை);
		second_context.release.store(உண்மை);
		first_thread.join();
		second_thread.join();
		perf_free(counter_a);
		perf_free(counter_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<தருக்க> free_done{பொய்};
	std::thread free_thread([&]() {
		perf_free(counter_a);
		free_done.store(உண்மை);
	});

	எனில் (!wait_until(free_done)) {
		first_context.release.store(உண்மை);
		second_context.release.store(உண்மை);
		first_thread.join();
		second_thread.join();
		free_thread.join();
		perf_free(counter_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	first_context.release.store(உண்மை);
	first_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_counter_t replacement = perf_alloc(PC_மொத்தம், "test_perf_overlap_replacement");

	எனில் (replacement == nullptr) {
		second_context.release.store(உண்மை);
		second_thread.join();
		perf_free(counter_b);
		FAIL() << "replacement counter allocation failed";
	}

	second_context.release.store(உண்மை);
	second_thread.join();

	EXPECT_FALSE(first_context.timed_out.load());
	EXPECT_FALSE(second_context.timed_out.load());
	EXPECT_EQ(first_context.வரிகள்_a, 0);
	EXPECT_EQ(second_context.வரிகள்_a, 0);
	EXPECT_EQ(second_context.replacement_வரிகள், 0);

	perf_free(replacement);
	perf_free(counter_b);
}
