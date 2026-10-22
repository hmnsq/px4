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

#ضمن <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	إذا (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} وإلا {
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

		منطقي success = syncHandler(request, response, timestamp, 1000_ms);

		إذا (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} وإلا {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	إذا (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

منطقي DatamanClient::syncHandler(ثابت dataman_request_s &request, dataman_response_s &response,
				ثابت hrt_abstime &start_time, hrt_abstime timeout)
{
	منطقي response_received = خطأ;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	طالما (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		إذا (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			اكسر;

		} وإلا إذا (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} وإلا {

			منطقي updated = خطأ;
			orb_check(_dataman_response_sub, &updated);

			إذا (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				إذا (response.client_id == request.client_id) {

					إذا ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = true;
						اكسر;
					}

				} وإلا إذا (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					إذا (0 == memcmp(&(request.timestamp), &(response.data), حجم(hrt_abstime))) {
						response_received = true;
						اكسر;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	إذا (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	أرجع response_received;
}

منطقي DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_SET) {
		أرجع خطأ;
	}

	إذا (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		أرجع خطأ;
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
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = خطأ;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} وإلا {
			memcpy(buffer, response.data, length);
		}
	}

	أرجع success;
}

منطقي DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_SET) {
		أرجع خطأ;
	}

	إذا (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		أرجع خطأ;
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
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = خطأ;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	أرجع success;
}

منطقي DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_SET) {
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = خطأ;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	أرجع success;
}

فراغ DatamanClient::clearPendingResponse()
{
	إذا (_dataman_response_sub < 0) {
		أرجع;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	منطقي updated = خطأ;
	orb_check(_dataman_response_sub, &updated);

	طالما (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

منطقي DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	إذا (_client_id == CLIENT_ID_NOT_SET) {
		أرجع خطأ;
	}

	إذا (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
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

		success = true;
	}

	أرجع success;
}

منطقي DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	إذا (_client_id == CLIENT_ID_NOT_SET) {
		أرجع خطأ;
	}

	إذا (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
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

		success = true;
	}

	أرجع success;
}

منطقي DatamanClient::clearAsync(dm_item_t item)
{
	إذا (_client_id == CLIENT_ID_NOT_SET) {
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
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

		success = true;
	}

	أرجع success;
}

فراغ DatamanClient::update()
{
	إذا (_state == State::RequestSent) {

		منطقي updated = خطأ;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		إذا (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			إذا ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				إذا (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				إذا (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		إذا (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			إذا (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
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

				إذا (_active_request.request_type == DM_WRITE) {
					memcpy(request.data, _active_request.buffer, _active_request.length);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

منطقي DatamanClient::lastOperationCompleted(منطقي &success)
{
	منطقي completed = خطأ;
	success = خطأ;

	إذا (_state == State::ResponseReceived) {

		إذا (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = true;
		}

		_state = State::Idle;
		completed = true;
	}

	أرجع completed;
}

فراغ DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ثابت حرف *cache_miss_perf_counter_name, uint32_t ژمارە_کاڵاکان)
	: _cache_miss_perf(perf_alloc(PC_ژماردن, cache_miss_perf_counter_name))
{
	إذا (ژمارە_کاڵاکان == 0) {
		أرجع;
	}

	کاڵاکان = جديد Item[ژمارە_کاڵاکان] {};

	إذا (کاڵاکان != nullptr) {
		ژمارە_کاڵاکان_2 = ژمارە_کاڵاکان;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] کاڵاکان;
	perf_free(_cache_miss_perf);
}

فراغ DatamanCache::resize(uint32_t ژمارە_کاڵاکان)
{
	إذا (ژمارە_کاڵاکان == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] کاڵاکان;
		کاڵاکان = nullptr;
		resetCacheState();
		ژمارە_کاڵاکان_2 = 0;
		أرجع;
	}

	Item *new_کاڵاکان = جديد Item[ژمارە_کاڵاکان] {};

	إذا (new_کاڵاکان != nullptr) {
		_client.abortCurrentOperation();

		uint32_t ژمارە_min = ژمارە_کاڵاکان < ژمارە_کاڵاکان_2 ? ژمارە_کاڵاکان : ژمارە_کاڵاکان_2;

		لكل (uint32_t i = 0; i < ژمارە_min; ++i) {
			new_کاڵاکان[i] = کاڵاکان[i];

			إذا (new_کاڵاکان[i].cache_state == State::RequestSent) {
				new_کاڵاکان[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] کاڵاکان;
		کاڵاکان = new_کاڵاکان;
		ژمارە_کاڵاکان_2 = ژمارە_کاڵاکان;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < ژمارە_کاڵاکان_2) ? _load_index : 0;
		_update_index = (_update_index < ژمارە_کاڵاکان_2) ? _update_index : 0;
		_item_counter = (_item_counter < ژمارە_کاڵاکان_2) ? _item_counter : ژمارە_کاڵاکان_2;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

منطقي DatamanCache::load(dm_item_t item, uint32_t index)
{
	إذا (!کاڵاکان) {
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي duplicate = خطأ;

	//Prevent duplicates
	لكل (uint32_t i = 0; i < ژمارە_کاڵاکان_2; ++i) {

		إذا (کاڵاکان[i].cache_state != State::Idle &&
		    کاڵاکان[i].cache_state != State::Error &&
		    کاڵاکان[i].response.item == item &&
		    کاڵاکان[i].response.index == index) {
			duplicate = true;
			اكسر;
		}
	}

	إذا (!duplicate && (_item_counter < ژمارە_کاڵاکان_2)) {

		کاڵاکان[_load_index].cache_state = State::RequestPrepared;
		کاڵاکان[_load_index].response.item = item;
		کاڵاکان[_load_index].response.index = index;

		_load_index = (_load_index + 1) % ژمارە_کاڵاکان_2;

		++_item_counter;

		success = true;
	}

	أرجع success;
}

منطقي DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	إذا (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي item_found = خطأ;

	إذا (کاڵاکان) {
		لكل (uint32_t i = 0; i < ژمارە_کاڵاکان_2; ++i) {
			إذا ((کاڵاکان[i].response.item == item) &&
			    (کاڵاکان[i].response.index == index)) {
				item_found = true;

				إذا (کاڵاکان[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, کاڵاکان[i].response.data, length);
					success = true;
					اكسر;
				}
			}
		}
	}

	إذا (!success) {
		perf_ژماردن(_cache_miss_perf);
	}

	إذا (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		إذا (success && !item_found && کاڵاکان && (_item_counter < ژمارە_کاڵاکان_2)) {
			کاڵاکان[_load_index].cache_state = State::ResponseReceived;
			کاڵاکان[_load_index].response.item = item;
			کاڵاکان[_load_index].response.index = index;
			memcpy(کاڵاکان[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % ژمارە_کاڵاکان_2;

			++_item_counter; // Still increase the counter here
		}
	}

	أرجع success;
}

منطقي DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	إذا (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		أرجع خطأ;
	}

	منطقي success = _client.writeSync(item, index, buffer, length, timeout);

	إذا (success && کاڵاکان) {
		لكل (uint32_t i = 0; i < ژمارە_کاڵاکان_2; ++i) {
			إذا ((کاڵاکان[i].response.item == item) &&
			    (کاڵاکان[i].response.index == index) &&
			    ((کاڵاکان[i].cache_state == State::ResponseReceived) ||
			     (کاڵاکان[i].cache_state == State::RequestPrepared))) {

				memcpy(کاڵاکان[i].response.data, buffer, length);
				کاڵاکان[i].cache_state = State::ResponseReceived;
				اكسر;
			}
		}
	}

	أرجع success;
}

منطقي DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, ثابت uint8_t *buffer, uint32_t length)
{
	إذا (!کاڵاکان || buffer == nullptr || ژمارە_کاڵاکان_2 == 0) {
		أرجع خطأ;
	}

	إذا (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		أرجع خطأ;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	لكل (uint32_t i = 0; i < ژمارە_کاڵاکان_2; ++i) {
		إذا ((کاڵاکان[i].response.item == item) && (کاڵاکان[i].response.index == index)
		    && (کاڵاکان[i].cache_state == State::ResponseReceived)) {
			memcpy(کاڵاکان[i].response.data, buffer, length);
			أرجع true;
		}
	}

	أرجع خطأ;
}

فراغ DatamanCache::update()
{
	إذا (_item_counter > 0) {

		_client.update();

		منطقي success = خطأ;
		منطقي response_success = خطأ;

		بدل (کاڵاکان[_update_index].cache_state) {
		حالة State::Idle:
			اكسر;

		حالة State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			اكسر;

		حالة State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(کاڵاکان[_update_index].response.item),
						    کاڵاکان[_update_index].response.index,
						    کاڵاکان[_update_index].response.data,
						    g_per_item_size[کاڵاکان[_update_index].response.item]);

			إذا (success) {
				کاڵاکان[_update_index].cache_state = State::RequestSent;

			} وإلا {
				کاڵاکان[_update_index].cache_state = State::Error;
			}

			اكسر;

		حالة State::RequestSent:

			إذا (_client.lastOperationCompleted(response_success)) {

				إذا (response_success) {

					کاڵاکان[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} وإلا {
					کاڵاکان[_update_index].cache_state = State::Error;
				}
			}

			اكسر;

		حالة State::Error:
			// Handled below
			اكسر;
		}

		إذا (کاڵاکان[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				کاڵاکان[_update_index].response.item, کاڵاکان[_update_index].response.index,
				کاڵاکان[_update_index].response.status);

			changeUpdateIndex();
		}

	}
}

فراغ DatamanCache::resetCacheState()
{
	_update_index = 0;
	_item_counter = 0;
	_load_index = 0;
}

فراغ DatamanCache::invalidate()
{
	لكل (uint32_t i = 0; i < ژمارە_کاڵاکان_2; ++i) {
		کاڵاکان[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

مضمن فراغ DatamanCache::changeUpdateIndex()
{
	إذا (ژمارە_کاڵاکان_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		أرجع;
	}

	_update_index = (_update_index + 1) % ژمارە_کاڵاکان_2;

	إذا (_item_counter > 0) {
		--_item_counter;
	}
}
