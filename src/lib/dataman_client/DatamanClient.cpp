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

/**
 * @file DatamanClient.cpp
 */

#অন্তর্ভাৱ <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	যদি (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} অন্যথা {
		// make sure we don't get any stale response by doing an orb_copy
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

		_fds.fd = _dataman_response_sub;
		_fds.events = POLLIN;

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_SET;

		তার্কিক success = syncHandler(request, response, timestamp, 1000_ms);

		যদি (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} অন্যথা {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	যদি (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

তার্কিক DatamanClient::syncHandler(ধ্রুৱ dataman_request_s &request, dataman_response_s &response,
				ধ্রুৱ hrt_abstime &start_time, hrt_abstime timeout)
{
	তার্কিক response_received = মিছা;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	যেতিয়ালৈ (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		যদি (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			ভঙ্গ;

		} অন্যথা যদি (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} অন্যথা {

			তার্কিক updated = মিছা;
			orb_check(_dataman_response_sub, &updated);

			যদি (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				যদি (response.client_id == request.client_id) {

					যদি ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = সঁচা;
						ভঙ্গ;
					}

				} অন্যথা যদি (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					যদি (0 == memcmp(&(request.timestamp), &(response.data), পরিমাণ(hrt_abstime))) {
						response_received = সঁচা;
						ভঙ্গ;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	যদি (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	ঘূৰাই response_received;
}

তার্কিক DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ঘূৰাই মিছা;
	}

	যদি (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ঘূৰাই মিছা;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.index = index;
	request.data_length = length;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	তার্কিক success = syncHandler(request, response, timestamp, timeout);

	যদি (success) {

		যদি (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = মিছা;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} অন্যথা {
			memcpy(buffer, response.data, length);
		}
	}

	ঘূৰাই success;
}

তার্কিক DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ঘূৰাই মিছা;
	}

	যদি (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ঘূৰাই মিছা;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.index = index;
	request.data_length = length;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.item = static_cast<uint8_t>(item);

	memcpy(request.data, buffer, length);

	dataman_response_s response{};
	তার্কিক success = syncHandler(request, response, timestamp, timeout);

	যদি (success) {

		যদি (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = মিছা;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	ঘূৰাই success;
}

তার্কিক DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ঘূৰাই মিছা;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	তার্কিক success = syncHandler(request, response, timestamp, timeout);

	যদি (success) {

		যদি (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = মিছা;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	ঘূৰাই success;
}

শূন্য DatamanClient::clearPendingResponse()
{
	যদি (_dataman_response_sub < 0) {
		ঘূৰাই;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	তার্কিক updated = মিছা;
	orb_check(_dataman_response_sub, &updated);

	যেতিয়ালৈ (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

তার্কিক DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ঘূৰাই মিছা;
	}

	যদি (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ঘূৰাই মিছা;
	}

	তার্কিক success = মিছা;

	যদি (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.index = index;
		request.data_length = length;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.item = static_cast<uint8_t>(item);

		_active_request.timestamp = timestamp;
		_active_request.request_type = DM_READ;
		_active_request.item = item;
		_active_request.index = index;
		_active_request.buffer = buffer;
		_active_request.length = length;

		_state = State::RequestSent;

		_dataman_request_pub.publish(request);

		success = সঁচা;
	}

	ঘূৰাই success;
}

তার্কিক DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ঘূৰাই মিছা;
	}

	যদি (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ঘূৰাই মিছা;
	}

	তার্কিক success = মিছা;

	যদি (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.index = index;
		request.data_length = length;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.item = static_cast<uint8_t>(item);

		memcpy(request.data, buffer, length);

		_active_request.timestamp = timestamp;
		_active_request.request_type = DM_WRITE;
		_active_request.item = item;
		_active_request.index = index;
		_active_request.buffer = buffer;
		_active_request.length = length;

		_state = State::RequestSent;

		_dataman_request_pub.publish(request);

		success = সঁচা;
	}

	ঘূৰাই success;
}

তার্কিক DatamanClient::clearAsync(dm_item_t item)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ঘূৰাই মিছা;
	}

	তার্কিক success = মিছা;

	যদি (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.item = static_cast<uint8_t>(item);
		request.index = 0;

		_active_request.timestamp = timestamp;
		_active_request.request_type = DM_CLEAR;
		_active_request.item = item;
		_active_request.index = request.index;
		_state = State::RequestSent;

		_dataman_request_pub.publish(request);

		success = সঁচা;
	}

	ঘূৰাই success;
}

শূন্য DatamanClient::update()
{
	যদি (_state == State::RequestSent) {

		তার্কিক updated = মিছা;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		যদি (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			যদি ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				যদি (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				যদি (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		যদি (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			যদি (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_time(&_active_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_time();

				_active_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.index = _active_request.index;
				request.data_length = _active_request.length;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(_active_request.request_type);
				request.item = static_cast<uint8_t>(_active_request.item);

				যদি (_active_request.request_type == DM_WRITE) {
					memcpy(request.data, _active_request.buffer, _active_request.length);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

তার্কিক DatamanClient::lastOperationCompleted(তার্কিক &success)
{
	তার্কিক completed = মিছা;
	success = মিছা;

	যদি (_state == State::ResponseReceived) {

		যদি (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = সঁচা;
		}

		_state = State::Idle;
		completed = সঁচা;
	}

	ঘূৰাই completed;
}

শূন্য DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ধ্রুৱ ৱর্ণ *cache_miss_perf_counter_name, uint32_t অঙ্ক_বস্তুসমূহ)
	: _cache_miss_perf(perf_alloc(PC_গুনতি, cache_miss_perf_counter_name))
{
	যদি (অঙ্ক_বস্তুসমূহ == 0) {
		ঘূৰাই;
	}

	বস্তুসমূহ = নূতন Item[অঙ্ক_বস্তুসমূহ] {};

	যদি (বস্তুসমূহ != nullptr) {
		অঙ্ক_বস্তুসমূহ_2 = অঙ্ক_বস্তুসমূহ;

	} অন্যথা {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] বস্তুসমূহ;
	perf_free(_cache_miss_perf);
}

শূন্য DatamanCache::resize(uint32_t অঙ্ক_বস্তুসমূহ)
{
	যদি (অঙ্ক_বস্তুসমূহ == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] বস্তুসমূহ;
		বস্তুসমূহ = nullptr;
		resetCacheState();
		অঙ্ক_বস্তুসমূহ_2 = 0;
		ঘূৰাই;
	}

	Item *new_বস্তুসমূহ = নূতন Item[অঙ্ক_বস্তুসমূহ] {};

	যদি (new_বস্তুসমূহ != nullptr) {
		_client.abortCurrentOperation();

		uint32_t অঙ্ক_min = অঙ্ক_বস্তুসমূহ < অঙ্ক_বস্তুসমূহ_2 ? অঙ্ক_বস্তুসমূহ : অঙ্ক_বস্তুসমূহ_2;

		বাবে (uint32_t i = 0; i < অঙ্ক_min; ++i) {
			new_বস্তুসমূহ[i] = বস্তুসমূহ[i];

			যদি (new_বস্তুসমূহ[i].cache_state == State::RequestSent) {
				new_বস্তুসমূহ[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] বস্তুসমূহ;
		বস্তুসমূহ = new_বস্তুসমূহ;
		অঙ্ক_বস্তুসমূহ_2 = অঙ্ক_বস্তুসমূহ;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < অঙ্ক_বস্তুসমূহ_2) ? _load_index : 0;
		_update_index = (_update_index < অঙ্ক_বস্তুসমূহ_2) ? _update_index : 0;
		_item_counter = (_item_counter < অঙ্ক_বস্তুসমূহ_2) ? _item_counter : অঙ্ক_বস্তুসমূহ_2;

	} অন্যথা {
		PX4_ERR("alloc failed");
	}
}

তার্কিক DatamanCache::load(dm_item_t item, uint32_t index)
{
	যদি (!বস্তুসমূহ) {
		ঘূৰাই মিছা;
	}

	তার্কিক success = মিছা;
	তার্কিক duplicate = মিছা;

	//Prevent duplicates
	বাবে (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {

		যদি (বস্তুসমূহ[i].cache_state != State::Idle &&
		    বস্তুসমূহ[i].cache_state != State::Error &&
		    বস্তুসমূহ[i].response.item == item &&
		    বস্তুসমূহ[i].response.index == index) {
			duplicate = সঁচা;
			ভঙ্গ;
		}
	}

	যদি (!duplicate && (_item_counter < অঙ্ক_বস্তুসমূহ_2)) {

		বস্তুসমূহ[_load_index].cache_state = State::RequestPrepared;
		বস্তুসমূহ[_load_index].response.item = item;
		বস্তুসমূহ[_load_index].response.index = index;

		_load_index = (_load_index + 1) % অঙ্ক_বস্তুসমূহ_2;

		++_item_counter;

		success = সঁচা;
	}

	ঘূৰাই success;
}

তার্কিক DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	যদি (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		ঘূৰাই মিছা;
	}

	তার্কিক success = মিছা;
	তার্কিক item_found = মিছা;

	যদি (বস্তুসমূহ) {
		বাবে (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {
			যদি ((বস্তুসমূহ[i].response.item == item) &&
			    (বস্তুসমূহ[i].response.index == index)) {
				item_found = সঁচা;

				যদি (বস্তুসমূহ[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, বস্তুসমূহ[i].response.data, length);
					success = সঁচা;
					ভঙ্গ;
				}
			}
		}
	}

	যদি (!success) {
		perf_গুনতি(_cache_miss_perf);
	}

	যদি (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		যদি (success && !item_found && বস্তুসমূহ && (_item_counter < অঙ্ক_বস্তুসমূহ_2)) {
			বস্তুসমূহ[_load_index].cache_state = State::ResponseReceived;
			বস্তুসমূহ[_load_index].response.item = item;
			বস্তুসমূহ[_load_index].response.index = index;
			memcpy(বস্তুসমূহ[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % অঙ্ক_বস্তুসমূহ_2;

			++_item_counter; // Still increase the counter here
		}
	}

	ঘূৰাই success;
}

তার্কিক DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	যদি (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ঘূৰাই মিছা;
	}

	তার্কিক success = _client.writeSync(item, index, buffer, length, timeout);

	যদি (success && বস্তুসমূহ) {
		বাবে (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {
			যদি ((বস্তুসমূহ[i].response.item == item) &&
			    (বস্তুসমূহ[i].response.index == index) &&
			    ((বস্তুসমূহ[i].cache_state == State::ResponseReceived) ||
			     (বস্তুসমূহ[i].cache_state == State::RequestPrepared))) {

				memcpy(বস্তুসমূহ[i].response.data, buffer, length);
				বস্তুসমূহ[i].cache_state = State::ResponseReceived;
				ভঙ্গ;
			}
		}
	}

	ঘূৰাই success;
}

তার্কিক DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, ধ্রুৱ uint8_t *buffer, uint32_t length)
{
	যদি (!বস্তুসমূহ || buffer == nullptr || অঙ্ক_বস্তুসমূহ_2 == 0) {
		ঘূৰাই মিছা;
	}

	যদি (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ঘূৰাই মিছা;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	বাবে (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {
		যদি ((বস্তুসমূহ[i].response.item == item) && (বস্তুসমূহ[i].response.index == index)
		    && (বস্তুসমূহ[i].cache_state == State::ResponseReceived)) {
			memcpy(বস্তুসমূহ[i].response.data, buffer, length);
			ঘূৰাই সঁচা;
		}
	}

	ঘূৰাই মিছা;
}

শূন্য DatamanCache::update()
{
	যদি (_item_counter > 0) {

		_client.update();

		তার্কিক success = মিছা;
		তার্কিক response_success = মিছা;

		ৱিকল্প (বস্তুসমূহ[_update_index].cache_state) {
		পক্ষ State::Idle:
			ভঙ্গ;

		পক্ষ State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			ভঙ্গ;

		পক্ষ State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(বস্তুসমূহ[_update_index].response.item),
						    বস্তুসমূহ[_update_index].response.index,
						    বস্তুসমূহ[_update_index].response.data,
						    g_per_item_size[বস্তুসমূহ[_update_index].response.item]);

			যদি (success) {
				বস্তুসমূহ[_update_index].cache_state = State::RequestSent;

			} অন্যথা {
				বস্তুসমূহ[_update_index].cache_state = State::Error;
			}

			ভঙ্গ;

		পক্ষ State::RequestSent:

			যদি (_client.lastOperationCompleted(response_success)) {

				যদি (response_success) {

					বস্তুসমূহ[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} অন্যথা {
					বস্তুসমূহ[_update_index].cache_state = State::Error;
				}
			}

			ভঙ্গ;

		পক্ষ State::Error:
			// Handled below
			ভঙ্গ;
		}

		যদি (বস্তুসমূহ[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				বস্তুসমূহ[_update_index].response.item, বস্তুসমূহ[_update_index].response.index,
				বস্তুসমূহ[_update_index].response.status);

			changeUpdateIndex();
		}

	}
}

শূন্য DatamanCache::resetCacheState()
{
	_update_index = 0;
	_item_counter = 0;
	_load_index = 0;
}

শূন্য DatamanCache::invalidate()
{
	বাবে (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {
		বস্তুসমূহ[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

অন্তঃস্থ শূন্য DatamanCache::changeUpdateIndex()
{
	যদি (অঙ্ক_বস্তুসমূহ_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		ঘূৰাই;
	}

	_update_index = (_update_index + 1) % অঙ্ক_বস্তুসমূহ_2;

	যদি (_item_counter > 0) {
		--_item_counter;
	}
}
