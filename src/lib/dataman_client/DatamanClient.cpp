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

#அந்தர்பாவ <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	எனில் (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} இல்லையெனில் {
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

		தருக்க success = syncHandler(request, response, timestamp, 1000_ms);

		எனில் (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} இல்லையெனில் {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	எனில் (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

தருக்க DatamanClient::syncHandler(மாறிலி dataman_request_s &request, dataman_response_s &response,
				மாறிலி hrt_abstime &start_time, hrt_abstime timeout)
{
	தருக்க response_received = பொய்;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	போது (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		எனில் (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			நிறுத்து;

		} இல்லையெனில் எனில் (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} இல்லையெனில் {

			தருக்க updated = பொய்;
			orb_check(_dataman_response_sub, &updated);

			எனில் (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				எனில் (response.client_id == request.client_id) {

					எனில் ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = உண்மை;
						நிறுத்து;
					}

				} இல்லையெனில் எனில் (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					எனில் (0 == memcmp(&(request.timestamp), &(response.data), அளவு(hrt_abstime))) {
						response_received = உண்மை;
						நிறுத்து;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	எனில் (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	திரும்பு response_received;
}

தருக்க DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	எனில் (_client_id == CLIENT_ID_NOT_SET) {
		திரும்பு பொய்;
	}

	எனில் (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		திரும்பு பொய்;
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
	தருக்க success = syncHandler(request, response, timestamp, timeout);

	எனில் (success) {

		எனில் (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = பொய்;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} இல்லையெனில் {
			memcpy(buffer, response.data, length);
		}
	}

	திரும்பு success;
}

தருக்க DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	எனில் (_client_id == CLIENT_ID_NOT_SET) {
		திரும்பு பொய்;
	}

	எனில் (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		திரும்பு பொய்;
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
	தருக்க success = syncHandler(request, response, timestamp, timeout);

	எனில் (success) {

		எனில் (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = பொய்;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	திரும்பு success;
}

தருக்க DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	எனில் (_client_id == CLIENT_ID_NOT_SET) {
		திரும்பு பொய்;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	தருக்க success = syncHandler(request, response, timestamp, timeout);

	எனில் (success) {

		எனில் (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = பொய்;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	திரும்பு success;
}

வெறுமை DatamanClient::clearPendingResponse()
{
	எனில் (_dataman_response_sub < 0) {
		திரும்பு;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	தருக்க updated = பொய்;
	orb_check(_dataman_response_sub, &updated);

	போது (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

தருக்க DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	எனில் (_client_id == CLIENT_ID_NOT_SET) {
		திரும்பு பொய்;
	}

	எனில் (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		திரும்பு பொய்;
	}

	தருக்க success = பொய்;

	எனில் (_state == State::Idle) {
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

		success = உண்மை;
	}

	திரும்பு success;
}

தருக்க DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	எனில் (_client_id == CLIENT_ID_NOT_SET) {
		திரும்பு பொய்;
	}

	எனில் (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		திரும்பு பொய்;
	}

	தருக்க success = பொய்;

	எனில் (_state == State::Idle) {
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

		success = உண்மை;
	}

	திரும்பு success;
}

தருக்க DatamanClient::clearAsync(dm_item_t item)
{
	எனில் (_client_id == CLIENT_ID_NOT_SET) {
		திரும்பு பொய்;
	}

	தருக்க success = பொய்;

	எனில் (_state == State::Idle) {
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

		success = உண்மை;
	}

	திரும்பு success;
}

வெறுமை DatamanClient::update()
{
	எனில் (_state == State::RequestSent) {

		தருக்க updated = பொய்;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		எனில் (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			எனில் ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				எனில் (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				எனில் (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		எனில் (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			எனில் (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
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

				எனில் (_active_request.request_type == DM_WRITE) {
					memcpy(request.data, _active_request.buffer, _active_request.length);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

தருக்க DatamanClient::lastOperationCompleted(தருக்க &success)
{
	தருக்க completed = பொய்;
	success = பொய்;

	எனில் (_state == State::ResponseReceived) {

		எனில் (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = உண்மை;
		}

		_state = State::Idle;
		completed = உண்மை;
	}

	திரும்பு completed;
}

வெறுமை DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(மாறிலி எழுத்து *cache_miss_perf_counter_name, uint32_t இலக்கம்_பொருட்கள்)
	: _cache_miss_perf(perf_alloc(PC_மொத்தம், cache_miss_perf_counter_name))
{
	எனில் (இலக்கம்_பொருட்கள் == 0) {
		திரும்பு;
	}

	பொருட்கள் = புதிய Item[இலக்கம்_பொருட்கள்] {};

	எனில் (பொருட்கள் != nullptr) {
		இலக்கம்_பொருட்கள்_2 = இலக்கம்_பொருட்கள்;

	} இல்லையெனில் {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] பொருட்கள்;
	perf_free(_cache_miss_perf);
}

வெறுமை DatamanCache::resize(uint32_t இலக்கம்_பொருட்கள்)
{
	எனில் (இலக்கம்_பொருட்கள் == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] பொருட்கள்;
		பொருட்கள் = nullptr;
		resetCacheState();
		இலக்கம்_பொருட்கள்_2 = 0;
		திரும்பு;
	}

	Item *new_பொருட்கள் = புதிய Item[இலக்கம்_பொருட்கள்] {};

	எனில் (new_பொருட்கள் != nullptr) {
		_client.abortCurrentOperation();

		uint32_t இலக்கம்_min = இலக்கம்_பொருட்கள் < இலக்கம்_பொருட்கள்_2 ? இலக்கம்_பொருட்கள் : இலக்கம்_பொருட்கள்_2;

		க்கு (uint32_t i = 0; i < இலக்கம்_min; ++i) {
			new_பொருட்கள்[i] = பொருட்கள்[i];

			எனில் (new_பொருட்கள்[i].cache_state == State::RequestSent) {
				new_பொருட்கள்[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] பொருட்கள்;
		பொருட்கள் = new_பொருட்கள்;
		இலக்கம்_பொருட்கள்_2 = இலக்கம்_பொருட்கள்;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < இலக்கம்_பொருட்கள்_2) ? _load_index : 0;
		_update_index = (_update_index < இலக்கம்_பொருட்கள்_2) ? _update_index : 0;
		_item_counter = (_item_counter < இலக்கம்_பொருட்கள்_2) ? _item_counter : இலக்கம்_பொருட்கள்_2;

	} இல்லையெனில் {
		PX4_ERR("alloc failed");
	}
}

தருக்க DatamanCache::load(dm_item_t item, uint32_t index)
{
	எனில் (!பொருட்கள்) {
		திரும்பு பொய்;
	}

	தருக்க success = பொய்;
	தருக்க duplicate = பொய்;

	//Prevent duplicates
	க்கு (uint32_t i = 0; i < இலக்கம்_பொருட்கள்_2; ++i) {

		எனில் (பொருட்கள்[i].cache_state != State::Idle &&
		    பொருட்கள்[i].cache_state != State::Error &&
		    பொருட்கள்[i].response.item == item &&
		    பொருட்கள்[i].response.index == index) {
			duplicate = உண்மை;
			நிறுத்து;
		}
	}

	எனில் (!duplicate && (_item_counter < இலக்கம்_பொருட்கள்_2)) {

		பொருட்கள்[_load_index].cache_state = State::RequestPrepared;
		பொருட்கள்[_load_index].response.item = item;
		பொருட்கள்[_load_index].response.index = index;

		_load_index = (_load_index + 1) % இலக்கம்_பொருட்கள்_2;

		++_item_counter;

		success = உண்மை;
	}

	திரும்பு success;
}

தருக்க DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	எனில் (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		திரும்பு பொய்;
	}

	தருக்க success = பொய்;
	தருக்க item_found = பொய்;

	எனில் (பொருட்கள்) {
		க்கு (uint32_t i = 0; i < இலக்கம்_பொருட்கள்_2; ++i) {
			எனில் ((பொருட்கள்[i].response.item == item) &&
			    (பொருட்கள்[i].response.index == index)) {
				item_found = உண்மை;

				எனில் (பொருட்கள்[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, பொருட்கள்[i].response.data, length);
					success = உண்மை;
					நிறுத்து;
				}
			}
		}
	}

	எனில் (!success) {
		perf_மொத்தம்(_cache_miss_perf);
	}

	எனில் (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		எனில் (success && !item_found && பொருட்கள் && (_item_counter < இலக்கம்_பொருட்கள்_2)) {
			பொருட்கள்[_load_index].cache_state = State::ResponseReceived;
			பொருட்கள்[_load_index].response.item = item;
			பொருட்கள்[_load_index].response.index = index;
			memcpy(பொருட்கள்[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % இலக்கம்_பொருட்கள்_2;

			++_item_counter; // Still increase the counter here
		}
	}

	திரும்பு success;
}

தருக்க DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	எனில் (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		திரும்பு பொய்;
	}

	தருக்க success = _client.writeSync(item, index, buffer, length, timeout);

	எனில் (success && பொருட்கள்) {
		க்கு (uint32_t i = 0; i < இலக்கம்_பொருட்கள்_2; ++i) {
			எனில் ((பொருட்கள்[i].response.item == item) &&
			    (பொருட்கள்[i].response.index == index) &&
			    ((பொருட்கள்[i].cache_state == State::ResponseReceived) ||
			     (பொருட்கள்[i].cache_state == State::RequestPrepared))) {

				memcpy(பொருட்கள்[i].response.data, buffer, length);
				பொருட்கள்[i].cache_state = State::ResponseReceived;
				நிறுத்து;
			}
		}
	}

	திரும்பு success;
}

தருக்க DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, மாறிலி uint8_t *buffer, uint32_t length)
{
	எனில் (!பொருட்கள் || buffer == nullptr || இலக்கம்_பொருட்கள்_2 == 0) {
		திரும்பு பொய்;
	}

	எனில் (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		திரும்பு பொய்;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	க்கு (uint32_t i = 0; i < இலக்கம்_பொருட்கள்_2; ++i) {
		எனில் ((பொருட்கள்[i].response.item == item) && (பொருட்கள்[i].response.index == index)
		    && (பொருட்கள்[i].cache_state == State::ResponseReceived)) {
			memcpy(பொருட்கள்[i].response.data, buffer, length);
			திரும்பு உண்மை;
		}
	}

	திரும்பு பொய்;
}

வெறுமை DatamanCache::update()
{
	எனில் (_item_counter > 0) {

		_client.update();

		தருக்க success = பொய்;
		தருக்க response_success = பொய்;

		தேர்வு (பொருட்கள்[_update_index].cache_state) {
		நிலை State::Idle:
			நிறுத்து;

		நிலை State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			நிறுத்து;

		நிலை State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(பொருட்கள்[_update_index].response.item),
						    பொருட்கள்[_update_index].response.index,
						    பொருட்கள்[_update_index].response.data,
						    g_per_item_size[பொருட்கள்[_update_index].response.item]);

			எனில் (success) {
				பொருட்கள்[_update_index].cache_state = State::RequestSent;

			} இல்லையெனில் {
				பொருட்கள்[_update_index].cache_state = State::Error;
			}

			நிறுத்து;

		நிலை State::RequestSent:

			எனில் (_client.lastOperationCompleted(response_success)) {

				எனில் (response_success) {

					பொருட்கள்[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} இல்லையெனில் {
					பொருட்கள்[_update_index].cache_state = State::Error;
				}
			}

			நிறுத்து;

		நிலை State::Error:
			// Handled below
			நிறுத்து;
		}

		எனில் (பொருட்கள்[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				பொருட்கள்[_update_index].response.item, பொருட்கள்[_update_index].response.index,
				பொருட்கள்[_update_index].response.status);

			changeUpdateIndex();
		}

	}
}

வெறுமை DatamanCache::resetCacheState()
{
	_update_index = 0;
	_item_counter = 0;
	_load_index = 0;
}

வெறுமை DatamanCache::invalidate()
{
	க்கு (uint32_t i = 0; i < இலக்கம்_பொருட்கள்_2; ++i) {
		பொருட்கள்[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

உள்வரி வெறுமை DatamanCache::changeUpdateIndex()
{
	எனில் (இலக்கம்_பொருட்கள்_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		திரும்பு;
	}

	_update_index = (_update_index + 1) % இலக்கம்_பொருட்கள்_2;

	எனில் (_item_counter > 0) {
		--_item_counter;
	}
}
