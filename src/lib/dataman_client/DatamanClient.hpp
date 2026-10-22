/****************************************************************************
 *
 *   Copyright (c) 2023 PX4 Development Team. All rights reserved.
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
#pragma once

#ضمن <uORB/uORB.h>
#ضمن <uORB/Publication.hpp>
#ضمن <uORB/Subscription.hpp>
#ضمن <uORB/topics/dataman_request.h>
#ضمن <uORB/topics/dataman_response.h>
#ضمن <dataman/dataman.h>
#ضمن <lib/perf/perf_counter.h>
#ضمن <px4_platform_common/posix.h>

using نطاق time_literals;

صنف DatamanClientTestPeer;
صنف DatamanCacheTestPeer;

صنف DatamanClient
{
عام:
	DatamanClient();
	~DatamanClient();

	DatamanClient(ثابت DatamanClient &) = delete;
	DatamanClient &operator=(ثابت DatamanClient &) = delete;

	/**
	 * @brief Reads data synchronously from the dataman for the specified item and index.
	 *
	 * @param[in] item The item to read data from.
	 * @param[in] index The index of the item to read data from.
	 * @param[out] buffer Pointer to the buffer to store the read data.
	 * @param[in] length The length of the data to read.
	 * @param[in] timeout The timeout in microseconds for waiting for the response.
	 *
	 * @return true if data was read successfully within the timeout, false otherwise.
	 */
	منطقي readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout = 5000_ms);

	/**
	 * @brief Write data to the dataman synchronously.
	 *
	 * @param[in] item The data item type to write.
	 * @param[in] index The index of the data item.
	 * @param[in] buffer The buffer that contains the data to write.
	 * @param[in] length The length of the data to write.
	 * @param[in] timeout The maximum time in microseconds to wait for the response.
	 *
	 * @return True if the write operation succeeded, false otherwise.
	 */
	منطقي writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout = 5000_ms);

	/**
	 * @brief Clears the data in the specified dataman item.
	 *
	 * @param[in] item The dataman item to clear.
	 * @param[in] timeout The timeout for the operation.
	 *
	 * @return True if the operation was successful, false otherwise.
	 */
	منطقي clearSync(dm_item_t item, hrt_abstime timeout = 5000_ms);

	/**
	 * @brief Initiates an asynchronous request to read the data from dataman for a specific item and index.
	 *
	 * @param[in] item The item to read from.
	 * @param[in] index The index within the item to read from.
	 * @param[out] buffer The buffer to store the read data in.
	 * @param[in] length The length of the data to read.
	 *
	 * @return True if the read request was successfully queued, false otherwise.
	 *
	 * @note The buffer must be kept alive as long as the request did not finish.
	 *       The completion status of the request can be obtained with the
	 *       lastOperationCompleted() function.
	 */
	منطقي readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length);

	/**
	 * @brief Initiates an asynchronous request to write the data to dataman for a specific item and index.
	 *
	 * @param[in] item The item to write data to.
	 * @param[in] index The index of the item to write data to.
	 * @param[in] buffer The buffer containing the data to write.
	 * @param[in] length The length of the data to write.
	 *
	 * @return True if the write request was successfully sent, false otherwise.
	 *
	 * @note The buffer must be kept alive as long as the request did not finish.
	 *       The completion status of the request can be obtained with the
	 *       lastOperationCompleted() function.
	 */
	منطقي writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length);

	/**
	 * @brief Initiates an asynchronous request to clear an item in dataman.
	 *
	 * The request is only initiated if the DatamanClient is in the Idle state.
	 *
	 * @param[in] item The item to clear.
	 * @return True if the request was successfully initiated, false otherwise.
	 */
	منطقي clearAsync(dm_item_t item);

	/**
	 * @brief Updates the state of the dataman client for asynchronous functions.
	 *
	 * This function shall be called regularly. It checks if there is any response from the dataman,
	 * and updates the state accordingly. If there is no response for a request, it retries the
	 * request after a timeout.
	 *
	 * @see readAsync(), writeAsync(), clearAsync(), lastOperationCompleted()
	 */
	فراغ update();

	/**
	 * @brief Check if the last dataman operation has completed and whether it was successful.
	 *
	 * @param[out] success Output parameter indicating whether the last operation was successful.
	 * @return true if the last operation has completed, false otherwise.
	 */
	منطقي lastOperationCompleted(منطقي &success);

	/**
	 * Abort any async operation currently in progress
	 */
	فراغ abortCurrentOperation();

	/**
	 * @brief Get the subscription fd used for Dataman responses.
	 *
	 * Dataman responses are published on a shared topic. Callers that add this fd
	 * to a poll set must only do so while they have an outstanding operation whose
	 * update() path will consume the notification.
	 */
	orb_sub_t responseSubscription() ثابت { ܦܢܐ _dataman_response_sub; }

خاص:
	friend صنف DatamanClientTestPeer;

	تعداد صنف State {
		Idle,
		RequestSent,
		ResponseReceived
	};

	بنية Request {
		hrt_abstime timestamp;
		dm_function_t request_type;
		dm_item_t item;
		uint32_t index;
		uint8_t *buffer;
		uint32_t length;
	};

	/* Synchronous response/request handler */
	منطقي syncHandler(ثابت dataman_request_s &request, dataman_response_s &response,
			 ثابت hrt_abstime &start_time, hrt_abstime timeout);
	/* Drain any queued stale replies before a new request starts. */
	فراغ clearPendingResponse();

	State _state{State::Idle};
	Request _active_request{};
	uint8_t _response_status{};

	orb_sub_t _dataman_response_sub{ORB_SUB_INVALID};
	uORB::Publication<dataman_request_s> _dataman_request_pub{ORB_ID(dataman_request)};

	px4_pollfd_struct_t _fds;

	ساكن constexpr uint16_t CLIENT_ID_NOT_SET{0};

	uint16_t _client_id{CLIENT_ID_NOT_SET};

	perf_counter_t _sync_perf{nullptr};
};


صنف DatamanCache
{
عام:
	DatamanCache(ثابت حرف *cache_miss_perf_counter_name, uint32_t num_items);
	~DatamanCache();

	/**
	 * @brief Resizes the cache to hold the specified number of items.
	 *
	 * @param[in] num_items The number of items the cache should hold.
	 */
	فراغ resize(uint32_t num_items);

	/**
	 * @brief Invalidates all cached items in the cache by resetting their cache_state to Idle.
	 */
	فراغ invalidate();

	/**
	 * @brief Adds an index for items to be cached.
	 *
	 * Calling this function will exit immediately. Data shall be acquired with 'update()' function and
	 * it will be cached at full size. Later it can be retrieved with 'loadWait()' function.
	 *
	 * @param[in] item The item to load.
	 * @param[in] index The index of the item to load.
	 *
	 * @return true if the item was added to be cached, false otherwise if the size of the cache is reached.
	 */
	منطقي load(dm_item_t item, uint32_t index);

	/**
	 * @brief Loads for a specific item from the cache or acquires and wait for it if not found in the cache.
	 *
	 * @param[in] item   Dataman item type
	 * @param[in] index  Item index
	 * @param[out] buffer Buffer for the data to be stored
	 * @param[in] length Length of the buffer in bytes to be stored
	 * @param[in] timeout Maximum time to wait for the item to be available in microseconds, 0 to return immediately
	 *
	 * @return true if item was successfully loaded from cache or acquired through the client, false otherwise.
	 *
	 * @note This function will block if timeout is set differently than 0 and data doesn't exist in cache.
	 */
	منطقي loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout = 0);

	/**
	 * @brief Write data back and update it in the cache if stored.
	 *
	 * @param[in] item The data item type to write.
	 * @param[in] index The index of the data item.
	 * @param[in] buffer The buffer that contains the data to write.
	 * @param[in] length The length of the data to write.
	 * @param[in] timeout The maximum time in microseconds to wait for the response.
	 *
	 * @return True if the write operation succeeded, false otherwise.
	 */
	منطقي writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout = 5000_ms);

	/**
	 * @brief Update a cached item in-place without issuing a new dataman request.
	 *
	 * Intended for callers that already wrote the authoritative value through another
	 * path and need to keep a second DatamanCache instance coherent.
	 *
	 * @return true if the item was fully cached and has been updated.
	 */
	منطقي updateCachedItem(dm_item_t item, uint32_t index, ثابت uint8_t *buffer, uint32_t length);

	/**
	 * @brief Updates the dataman cache by checking for responses from the DatamanClient and processing them.
	 *
	 * If there are items in the cache, this function will call the DatamanClient's 'update()' function to check for responses.
	 * Depending on the state of each item, it will either send a request, wait for a response, or report an error.
	 * If a response is received for an item, it will be marked as "response received" and the update index will be changed
	 * to the next item in the cache. This function does not block and returns immediately.
	 * The data can be acquired with the 'loadWait()' function after it has been cached.
	 */
	فراغ update();

	/**
	 * @brief Function providing info if there items to be loaded to dataman cache.
	 *
	 * @return true if there are items to be processed.
	 */
	منطقي isLoading() ثابت { ܦܢܐ (_item_counter > 0); }

	/**
	 * @brief Returns a reference to the DatamanClient instance used by the DatamanCache.
	 *
	 * @return A reference to the DatamanClient instance used by the DatamanCache.
	 */
	DatamanClient &client() { ܦܢܐ _client; }

	صحيح size() ثابت { ܦܢܐ _num_items; }

خاص:
	friend صنف DatamanCacheTestPeer;

	تعداد صنف State {
		Idle,
		RequestPrepared,
		RequestSent,
		ResponseReceived,
		Error
	};

	بنية Item {
		dataman_response_s response;
		State cache_state;
	};

	فراغ resetCacheState();
	مضمن فراغ changeUpdateIndex();

	Item *_items{nullptr};
	uint32_t _load_index{0};	///< index for tracking last index used by load function
	uint32_t _update_index{0};	///< index for tracking last index used by update function
	uint32_t _item_counter{0};	///< number of items to process with update function
	uint32_t _num_items{0};		///< number of items that cache can store

	DatamanClient _client{};

	perf_counter_t	_cache_miss_perf;
};
