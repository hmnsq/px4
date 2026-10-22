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
	ध्रुव स्वचालित deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

	यावत् (!condition.आरोपय() && std::chrono::steady_clock::now() < deadline) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	प्रत्यागम condition.आरोपय();
}

} // namespace

TEST(Perf_गणक, Iterate_पङ्क्तिः_Content)
{
	perf_गणक_t गणक = perf_alloc(PC_गणनम्, "test_perf_line_content");
	ASSERT_NE(गणक, शून्यसूचकः);

	perf_गणनम्(गणक);
	perf_गणनम्(गणक);
	perf_गणनम्(गणक);

	संरचना Context {
		std::string पङ्क्तिः;
	} context;

	perf_iterate_all([](ध्रुव वर्ण *गणक_पङ्क्तिः, शून्य *user) {
		यदि (वाक्ये_अन्विष(गणक_पङ्क्तिः, "test_perf_line_content") != शून्यसूचकः) {
			static_cast<Context *>(user)->पङ्क्तिः = गणक_पङ्क्तिः;
		}
	}, &context);

	EXPECT_EQ(context.पङ्क्तिः, "test_perf_line_content: 3 events");

	perf_free(गणक);
}

TEST(Perf_गणक, Concurrent_Alloc_Does_Not_खण्डम्_On_Slow_Callback)
{
	perf_गणक_t गणक = perf_alloc(PC_गणनम्, "test_perf_slow_callback");
	ASSERT_NE(गणक, शून्यसूचकः);

	// while the first callback is running, another thread must be able to
	// allocate and free a counter without waiting for the iteration to finish
	संरचना Context {
		std::atomic<तार्किक> first{सत्य};
		std::atomic<तार्किक> alloc_समाप्तम्{असत्य};
		std::atomic<तार्किक> alloc_succeeded{असत्य};
		std::thread alloc_thread;
	} context;

	perf_iterate_all([](ध्रुव वर्ण *, शून्य *user) {
		Context *ctx = static_cast<Context *>(user);

		यदि (ctx->first.exchange(असत्य)) {
			ctx->alloc_thread = std::thread([ctx]() {
				perf_गणक_t other = perf_alloc(PC_गणनम्, "test_perf_concurrent_alloc");
				ctx->alloc_succeeded.store(other != शून्यसूचकः);
				perf_free(other);
				ctx->alloc_समाप्तम्.store(सत्य);
			});

			EXPECT_TRUE(wait_until(ctx->alloc_समाप्तम्));
		}
	}, &context);

	यदि (context.alloc_thread.joinable()) {
		context.alloc_thread.join();
	}

	EXPECT_TRUE(context.alloc_समाप्तम्.आरोपय());
	EXPECT_TRUE(context.alloc_succeeded.आरोपय());

	perf_free(गणक);
}

TEST(Perf_गणक, AllocFromCallbackDoesNotDeadlock)
{
	perf_गणक_t गणक = perf_alloc(PC_गणनम्, "test_perf_callback_alloc");
	ASSERT_NE(गणक, शून्यसूचकः);

	// calling perf_alloc() from inside the callback self-deadlocked when the
	// iteration held the mutex; run the iteration on its own thread so a
	// regression fails the test instead of hanging it
	संरचना Context {
		std::atomic<तार्किक> first{सत्य};
		std::atomic<तार्किक> समाप्तम्{असत्य};
		perf_गणक_t allocated{शून्यसूचकः};
	} context;

	std::thread iterate_thread([&]() {
		perf_iterate_all([](ध्रुव वर्ण *, शून्य *user) {
			Context *ctx = static_cast<Context *>(user);

			यदि (ctx->first.exchange(असत्य)) {
				ctx->allocated = perf_alloc(PC_गणनम्, "test_perf_callback_alloc_inner");
			}
		}, &context);

		context.समाप्तम्.store(सत्य);
	});

	यदि (!wait_until(context.समाप्तम्)) {
		iterate_thread.detach();
		FAIL() << "perf_alloc() from the callback deadlocked";
	}

	iterate_thread.join();
	EXPECT_NE(context.allocated, शून्यसूचकः);

	perf_free(context.allocated);
	perf_free(गणक);
}

TEST(Perf_गणक, FreeDuringIterationIsDeferred)
{
	// allocation order puts counter_b at the list head, so it is visited first
	perf_गणक_t गणक_a = perf_alloc(PC_गणनम्, "test_perf_free_defer_a");
	perf_गणक_t गणक_b = perf_alloc(PC_गणनम्, "test_perf_free_defer_b");
	ASSERT_NE(गणक_a, शून्यसूचकः);
	ASSERT_NE(गणक_b, शून्यसूचकः);

	संरचना Context {
		perf_गणक_t गणक_a;
		std::atomic<तार्किक> free_समाप्तम्{असत्य};
		std::thread free_thread;
		पूर्णाङ्क पङ्क्तयः_a{0};
	} context{गणक_a};

	perf_iterate_all([](ध्रुव वर्ण *गणक_पङ्क्तिः, शून्य *user) {
		Context *ctx = static_cast<Context *>(user);

		यदि (!ctx->free_thread.joinable() && वाक्ये_अन्विष(गणक_पङ्क्तिः, "test_perf_free_defer_b") != शून्यसूचकः) {
			// free a not-yet-visited counter while the iteration is running
			ctx->free_thread = std::thread([ctx]() {
				perf_free(ctx->गणक_a);
				ctx->free_समाप्तम्.store(सत्य);
			});

			EXPECT_TRUE(wait_until(ctx->free_समाप्तम्));
		}

		यदि (वाक्ये_अन्विष(गणक_पङ्क्तिः, "test_perf_free_defer_a") != शून्यसूचकः) {
			ctx->पङ्क्तयः_a++;
		}
	}, &context);

	यदि (context.free_thread.joinable()) {
		context.free_thread.join();
	}

	EXPECT_TRUE(context.free_समाप्तम्.आरोपय());
	EXPECT_EQ(context.पङ्क्तयः_a, 0);

	// the freed entry must not reappear after deferred cleanup
	संरचना गणनम्_Context {
		पूर्णाङ्क पङ्क्तयः_a{0};
	} गणनम्_context;

	perf_iterate_all([](ध्रुव वर्ण *गणक_पङ्क्तिः, शून्य *user) {
		यदि (वाक्ये_अन्विष(गणक_पङ्क्तिः, "test_perf_free_defer_a") != शून्यसूचकः) {
			static_cast<गणनम्_Context *>(user)->पङ्क्तयः_a++;
		}
	}, &गणनम्_context);

	EXPECT_EQ(गणनम्_context.पङ्क्तयः_a, 0);

	perf_free(गणक_b);
}

TEST(Perf_गणक, Free_Is_Deferred_Until_चरमः_Overlapping_Iteration_अन्ताः)
{
	// Both iterators stop on counter_b with counter_a cached as their next entry.
	perf_गणक_t गणक_a = perf_alloc(PC_गणनम्, "test_perf_overlap_a");
	perf_गणक_t गणक_b = perf_alloc(PC_गणनम्, "test_perf_overlap_b");
	ASSERT_NE(गणक_a, शून्यसूचकः);
	ASSERT_NE(गणक_b, शून्यसूचकः);

	संरचना IterateContext {
		std::atomic<तार्किक> entered{असत्य};
		std::atomic<तार्किक> release{असत्य};
		std::atomic<तार्किक> timed_बहिर्गमनम्{असत्य};
		पूर्णाङ्क पङ्क्तयः_a{0};
		पूर्णाङ्क replacement_पङ्क्तयः{0};
	} first_context, second_context;

	perf_callback callback = [](ध्रुव वर्ण *गणक_पङ्क्तिः, शून्य *user) {
		IterateContext *ctx = static_cast<IterateContext *>(user);

		यदि (वाक्ये_अन्विष(गणक_पङ्क्तिः, "test_perf_overlap_b") != शून्यसूचकः) {
			ctx->entered.store(सत्य);

			यदि (!wait_until(ctx->release)) {
				ctx->timed_बहिर्गमनम्.store(सत्य);
			}
		}

		यदि (वाक्ये_अन्विष(गणक_पङ्क्तिः, "test_perf_overlap_a") != शून्यसूचकः) {
			ctx->पङ्क्तयः_a++;
		}

		यदि (वाक्ये_अन्विष(गणक_पङ्क्तिः, "test_perf_overlap_replacement") != शून्यसूचकः) {
			ctx->replacement_पङ्क्तयः++;
		}
	};

	std::thread first_thread([&]() { perf_iterate_all(callback, &first_context); });

	यदि (!wait_until(first_context.entered)) {
		first_context.release.store(सत्य);
		first_thread.join();
		perf_free(गणक_a);
		perf_free(गणक_b);
		FAIL() << "first iterator did not reach the blocking callback";
	}

	std::thread second_thread([&]() { perf_iterate_all(callback, &second_context); });

	यदि (!wait_until(second_context.entered)) {
		first_context.release.store(सत्य);
		second_context.release.store(सत्य);
		first_thread.join();
		second_thread.join();
		perf_free(गणक_a);
		perf_free(गणक_b);
		FAIL() << "second iterator did not reach the blocking callback";
	}

	std::atomic<तार्किक> free_समाप्तम्{असत्य};
	std::thread free_thread([&]() {
		perf_free(गणक_a);
		free_समाप्तम्.store(सत्य);
	});

	यदि (!wait_until(free_समाप्तम्)) {
		first_context.release.store(सत्य);
		second_context.release.store(सत्य);
		first_thread.join();
		second_thread.join();
		free_thread.join();
		perf_free(गणक_b);
		FAIL() << "perf_free() blocked on active callbacks";
	}

	free_thread.join();
	first_context.release.store(सत्य);
	first_thread.join();

	// If the first iterator incorrectly reclaimed counter_a, a same-sized
	// allocation is likely to reuse the entry cached by the second iterator.
	perf_गणक_t replacement = perf_alloc(PC_गणनम्, "test_perf_overlap_replacement");

	यदि (replacement == शून्यसूचकः) {
		second_context.release.store(सत्य);
		second_thread.join();
		perf_free(गणक_b);
		FAIL() << "replacement counter allocation failed";
	}

	second_context.release.store(सत्य);
	second_thread.join();

	EXPECT_FALSE(first_context.timed_बहिर्गमनम्.आरोपय());
	EXPECT_FALSE(second_context.timed_बहिर्गमनम्.आरोपय());
	EXPECT_EQ(first_context.पङ्क्तयः_a, 0);
	EXPECT_EQ(second_context.पङ्क्तयः_a, 0);
	EXPECT_EQ(second_context.replacement_पङ्क्तयः, 0);

	perf_free(replacement);
	perf_free(गणक_b);
}
