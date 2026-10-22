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

#شامل <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	اگر (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} وگرنه {
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

		منطقی success = syncHandler(request, response, timestamp, 1000_ms);

		اگر (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} وگرنه {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	اگر (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

منطقی DatamanClient::syncHandler(ثابت dataman_request_s &request, dataman_response_s &response,
				ثابت hrt_abstime &start_time, hrt_abstime timeout)
{
	منطقی response_received = نادرست;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	وقتی (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		اگر (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			بشکن;

		} وگرنه اگر (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} وگرنه {

			منطقی updated = نادرست;
			orb_check(_dataman_response_sub, &updated);

			اگر (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				اگر (response.client_id == request.client_id) {

					اگر ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = درست;
						بشکن;
					}

				} وگرنه اگر (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					اگر (0 == memcmp(&(request.timestamp), &(response.data), اندازه(hrt_abstime))) {
						response_received = درست;
						بشکن;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	اگر (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	بازگشت response_received;
}

منطقی DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	اگر (_client_id == CLIENT_ID_NOT_SET) {
		بازگشت نادرست;
	}

	اگر (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		بازگشت نادرست;
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
	منطقی success = syncHandler(request, response, timestamp, timeout);

	اگر (success) {

		اگر (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = نادرست;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} وگرنه {
			memcpy(buffer, response.data, length);
		}
	}

	بازگشت success;
}

منطقی DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	اگر (_client_id == CLIENT_ID_NOT_SET) {
		بازگشت نادرست;
	}

	اگر (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		بازگشت نادرست;
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
	منطقی success = syncHandler(request, response, timestamp, timeout);

	اگر (success) {

		اگر (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = نادرست;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	بازگشت success;
}

منطقی DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	اگر (_client_id == CLIENT_ID_NOT_SET) {
		بازگشت نادرست;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	منطقی success = syncHandler(request, response, timestamp, timeout);

	اگر (success) {

		اگر (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = نادرست;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	بازگشت success;
}

تهی DatamanClient::clearPendingResponse()
{
	اگر (_dataman_response_sub < 0) {
		بازگشت;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	منطقی updated = نادرست;
	orb_check(_dataman_response_sub, &updated);

	وقتی (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

منطقی DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	اگر (_client_id == CLIENT_ID_NOT_SET) {
		بازگشت نادرست;
	}

	اگر (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		بازگشت نادرست;
	}

	منطقی success = نادرست;

	اگر (_state == State::Idle) {
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

		success = درست;
	}

	بازگشت success;
}

منطقی DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	اگر (_client_id == CLIENT_ID_NOT_SET) {
		بازگشت نادرست;
	}

	اگر (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		بازگشت نادرست;
	}

	منطقی success = نادرست;

	اگر (_state == State::Idle) {
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

		success = درست;
	}

	بازگشت success;
}

منطقی DatamanClient::clearAsync(dm_item_t item)
{
	اگر (_client_id == CLIENT_ID_NOT_SET) {
		بازگشت نادرست;
	}

	منطقی success = نادرست;

	اگر (_state == State::Idle) {
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

		success = درست;
	}

	بازگشت success;
}

تهی DatamanClient::update()
{
	اگر (_state == State::RequestSent) {

		منطقی updated = نادرست;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		اگر (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			اگر ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				اگر (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				اگر (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		اگر (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			اگر (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
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

				اگر (_active_request.request_type == DM_WRITE) {
					memcpy(request.data, _active_request.buffer, _active_request.length);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

منطقی DatamanClient::lastOperationCompleted(منطقی &success)
{
	منطقی completed = نادرست;
	success = نادرست;

	اگر (_state == State::ResponseReceived) {

		اگر (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = درست;
		}

		_state = State::Idle;
		completed = درست;
	}

	بازگشت completed;
}

تهی DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ثابت نویسه *cache_miss_perf_counter_name, uint32_t رقم_اقلام)
	: _cache_miss_perf(perf_alloc(PC_شمار, cache_miss_perf_counter_name))
{
	اگر (رقم_اقلام == 0) {
		بازگشت;
	}

	اقلام = جدید Item[رقم_اقلام] {};

	اگر (اقلام != nullptr) {
		رقم_اقلام_2 = رقم_اقلام;

	} وگرنه {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] اقلام;
	perf_free(_cache_miss_perf);
}

تهی DatamanCache::resize(uint32_t رقم_اقلام)
{
	اگر (رقم_اقلام == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] اقلام;
		اقلام = nullptr;
		resetCacheState();
		رقم_اقلام_2 = 0;
		بازگشت;
	}

	Item *new_اقلام = جدید Item[رقم_اقلام] {};

	اگر (new_اقلام != nullptr) {
		_client.abortCurrentOperation();

		uint32_t رقم_min = رقم_اقلام < رقم_اقلام_2 ? رقم_اقلام : رقم_اقلام_2;

		برای (uint32_t i = 0; i < رقم_min; ++i) {
			new_اقلام[i] = اقلام[i];

			اگر (new_اقلام[i].cache_state == State::RequestSent) {
				new_اقلام[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] اقلام;
		اقلام = new_اقلام;
		رقم_اقلام_2 = رقم_اقلام;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < رقم_اقلام_2) ? _load_index : 0;
		_update_index = (_update_index < رقم_اقلام_2) ? _update_index : 0;
		_item_counter = (_item_counter < رقم_اقلام_2) ? _item_counter : رقم_اقلام_2;

	} وگرنه {
		PX4_ERR("alloc failed");
	}
}

منطقی DatamanCache::load(dm_item_t item, uint32_t index)
{
	اگر (!اقلام) {
		بازگشت نادرست;
	}

	منطقی success = نادرست;
	منطقی duplicate = نادرست;

	//Prevent duplicates
	برای (uint32_t i = 0; i < رقم_اقلام_2; ++i) {

		اگر (اقلام[i].cache_state != State::Idle &&
		    اقلام[i].cache_state != State::Error &&
		    اقلام[i].response.item == item &&
		    اقلام[i].response.index == index) {
			duplicate = درست;
			بشکن;
		}
	}

	اگر (!duplicate && (_item_counter < رقم_اقلام_2)) {

		اقلام[_load_index].cache_state = State::RequestPrepared;
		اقلام[_load_index].response.item = item;
		اقلام[_load_index].response.index = index;

		_load_index = (_load_index + 1) % رقم_اقلام_2;

		++_item_counter;

		success = درست;
	}

	بازگشت success;
}

منطقی DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	اگر (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		بازگشت نادرست;
	}

	منطقی success = نادرست;
	منطقی item_found = نادرست;

	اگر (اقلام) {
		برای (uint32_t i = 0; i < رقم_اقلام_2; ++i) {
			اگر ((اقلام[i].response.item == item) &&
			    (اقلام[i].response.index == index)) {
				item_found = درست;

				اگر (اقلام[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, اقلام[i].response.data, length);
					success = درست;
					بشکن;
				}
			}
		}
	}

	اگر (!success) {
		perf_شمار(_cache_miss_perf);
	}

	اگر (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		اگر (success && !item_found && اقلام && (_item_counter < رقم_اقلام_2)) {
			اقلام[_load_index].cache_state = State::ResponseReceived;
			اقلام[_load_index].response.item = item;
			اقلام[_load_index].response.index = index;
			memcpy(اقلام[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % رقم_اقلام_2;

			++_item_counter; // Still increase the counter here
		}
	}

	بازگشت success;
}

منطقی DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	اگر (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		بازگشت نادرست;
	}

	منطقی success = _client.writeSync(item, index, buffer, length, timeout);

	اگر (success && اقلام) {
		برای (uint32_t i = 0; i < رقم_اقلام_2; ++i) {
			اگر ((اقلام[i].response.item == item) &&
			    (اقلام[i].response.index == index) &&
			    ((اقلام[i].cache_state == State::ResponseReceived) ||
			     (اقلام[i].cache_state == State::RequestPrepared))) {

				memcpy(اقلام[i].response.data, buffer, length);
				اقلام[i].cache_state = State::ResponseReceived;
				بشکن;
			}
		}
	}

	بازگشت success;
}

منطقی DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, ثابت uint8_t *buffer, uint32_t length)
{
	اگر (!اقلام || buffer == nullptr || رقم_اقلام_2 == 0) {
		بازگشت نادرست;
	}

	اگر (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		بازگشت نادرست;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	برای (uint32_t i = 0; i < رقم_اقلام_2; ++i) {
		اگر ((اقلام[i].response.item == item) && (اقلام[i].response.index == index)
		    && (اقلام[i].cache_state == State::ResponseReceived)) {
			memcpy(اقلام[i].response.data, buffer, length);
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

تهی DatamanCache::update()
{
	اگر (_item_counter > 0) {

		_client.update();

		منطقی success = نادرست;
		منطقی response_success = نادرست;

		انتخاب (اقلام[_update_index].cache_state) {
		حالت State::Idle:
			بشکن;

		حالت State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			بشکن;

		حالت State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(اقلام[_update_index].response.item),
						    اقلام[_update_index].response.index,
						    اقلام[_update_index].response.data,
						    g_per_item_size[اقلام[_update_index].response.item]);

			اگر (success) {
				اقلام[_update_index].cache_state = State::RequestSent;

			} وگرنه {
				اقلام[_update_index].cache_state = State::Error;
			}

			بشکن;

		حالت State::RequestSent:

			اگر (_client.lastOperationCompleted(response_success)) {

				اگر (response_success) {

					اقلام[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} وگرنه {
					اقلام[_update_index].cache_state = State::Error;
				}
			}

			بشکن;

		حالت State::Error:
			// Handled below
			بشکن;
		}

		اگر (اقلام[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				اقلام[_update_index].response.item, اقلام[_update_index].response.index,
				اقلام[_update_index].response.status);

			changeUpdateIndex();
		}

	}
}

تهی DatamanCache::resetCacheState()
{
	_update_index = 0;
	_item_counter = 0;
	_load_index = 0;
}

تهی DatamanCache::invalidate()
{
	برای (uint32_t i = 0; i < رقم_اقلام_2; ++i) {
		اقلام[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

درونخطی تهی DatamanCache::changeUpdateIndex()
{
	اگر (رقم_اقلام_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		بازگشت;
	}

	_update_index = (_update_index + 1) % رقم_اقلام_2;

	اگر (_item_counter > 0) {
		--_item_counter;
	}
}
