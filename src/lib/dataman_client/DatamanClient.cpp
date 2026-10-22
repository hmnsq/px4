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

#അന്തര്ഭാവ <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	എങ്കിൽ (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} അല്ലെങ്കിൽ {
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

		തർക്കം success = syncHandler(request, response, timestamp, 1000_ms);

		എങ്കിൽ (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} അല്ലെങ്കിൽ {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	എങ്കിൽ (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

തർക്കം DatamanClient::syncHandler(സ്ഥിരം dataman_request_s &request, dataman_response_s &response,
				സ്ഥിരം hrt_abstime &start_time, hrt_abstime timeout)
{
	തർക്കം response_received = അസത്യം;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	ആകുമ്പോൾ (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		എങ്കിൽ (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			നിർത്തുക;

		} അല്ലെങ്കിൽ എങ്കിൽ (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} അല്ലെങ്കിൽ {

			തർക്കം updated = അസത്യം;
			orb_check(_dataman_response_sub, &updated);

			എങ്കിൽ (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				എങ്കിൽ (response.client_id == request.client_id) {

					എങ്കിൽ ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = സത്യം;
						നിർത്തുക;
					}

				} അല്ലെങ്കിൽ എങ്കിൽ (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					എങ്കിൽ (0 == memcmp(&(request.timestamp), &(response.data), വലുപ്പം(hrt_abstime))) {
						response_received = സത്യം;
						നിർത്തുക;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	എങ്കിൽ (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	തിരികെ response_received;
}

തർക്കം DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	എങ്കിൽ (_client_id == CLIENT_ID_NOT_SET) {
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		തിരികെ അസത്യം;
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
	തർക്കം success = syncHandler(request, response, timestamp, timeout);

	എങ്കിൽ (success) {

		എങ്കിൽ (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = അസത്യം;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} അല്ലെങ്കിൽ {
			memcpy(buffer, response.data, length);
		}
	}

	തിരികെ success;
}

തർക്കം DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	എങ്കിൽ (_client_id == CLIENT_ID_NOT_SET) {
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		തിരികെ അസത്യം;
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
	തർക്കം success = syncHandler(request, response, timestamp, timeout);

	എങ്കിൽ (success) {

		എങ്കിൽ (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = അസത്യം;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	തിരികെ success;
}

തർക്കം DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	എങ്കിൽ (_client_id == CLIENT_ID_NOT_SET) {
		തിരികെ അസത്യം;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	തർക്കം success = syncHandler(request, response, timestamp, timeout);

	എങ്കിൽ (success) {

		എങ്കിൽ (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = അസത്യം;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	തിരികെ success;
}

ശൂന്യം DatamanClient::clearPendingResponse()
{
	എങ്കിൽ (_dataman_response_sub < 0) {
		തിരികെ;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	തർക്കം updated = അസത്യം;
	orb_check(_dataman_response_sub, &updated);

	ആകുമ്പോൾ (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

തർക്കം DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	എങ്കിൽ (_client_id == CLIENT_ID_NOT_SET) {
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		തിരികെ അസത്യം;
	}

	തർക്കം success = അസത്യം;

	എങ്കിൽ (_state == State::Idle) {
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

		success = സത്യം;
	}

	തിരികെ success;
}

തർക്കം DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	എങ്കിൽ (_client_id == CLIENT_ID_NOT_SET) {
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		തിരികെ അസത്യം;
	}

	തർക്കം success = അസത്യം;

	എങ്കിൽ (_state == State::Idle) {
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

		success = സത്യം;
	}

	തിരികെ success;
}

തർക്കം DatamanClient::clearAsync(dm_item_t item)
{
	എങ്കിൽ (_client_id == CLIENT_ID_NOT_SET) {
		തിരികെ അസത്യം;
	}

	തർക്കം success = അസത്യം;

	എങ്കിൽ (_state == State::Idle) {
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

		success = സത്യം;
	}

	തിരികെ success;
}

ശൂന്യം DatamanClient::update()
{
	എങ്കിൽ (_state == State::RequestSent) {

		തർക്കം updated = അസത്യം;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		എങ്കിൽ (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			എങ്കിൽ ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				എങ്കിൽ (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				എങ്കിൽ (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		എങ്കിൽ (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			എങ്കിൽ (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
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

				എങ്കിൽ (_active_request.request_type == DM_WRITE) {
					memcpy(request.data, _active_request.buffer, _active_request.length);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

തർക്കം DatamanClient::lastOperationCompleted(തർക്കം &success)
{
	തർക്കം completed = അസത്യം;
	success = അസത്യം;

	എങ്കിൽ (_state == State::ResponseReceived) {

		എങ്കിൽ (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = സത്യം;
		}

		_state = State::Idle;
		completed = സത്യം;
	}

	തിരികെ completed;
}

ശൂന്യം DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(സ്ഥിരം അക്ഷരം *cache_miss_perf_counter_name, uint32_t അക്കം_ഇനങ്ങൾ)
	: _cache_miss_perf(perf_alloc(PC_എണ്ണം, cache_miss_perf_counter_name))
{
	എങ്കിൽ (അക്കം_ഇനങ്ങൾ == 0) {
		തിരികെ;
	}

	ഇനങ്ങൾ = പുതിയ Item[അക്കം_ഇനങ്ങൾ] {};

	എങ്കിൽ (ഇനങ്ങൾ != nullptr) {
		അക്കം_ഇനങ്ങൾ_2 = അക്കം_ഇനങ്ങൾ;

	} അല്ലെങ്കിൽ {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] ഇനങ്ങൾ;
	perf_free(_cache_miss_perf);
}

ശൂന്യം DatamanCache::resize(uint32_t അക്കം_ഇനങ്ങൾ)
{
	എങ്കിൽ (അക്കം_ഇനങ്ങൾ == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] ഇനങ്ങൾ;
		ഇനങ്ങൾ = nullptr;
		resetCacheState();
		അക്കം_ഇനങ്ങൾ_2 = 0;
		തിരികെ;
	}

	Item *new_ഇനങ്ങൾ = പുതിയ Item[അക്കം_ഇനങ്ങൾ] {};

	എങ്കിൽ (new_ഇനങ്ങൾ != nullptr) {
		_client.abortCurrentOperation();

		uint32_t അക്കം_min = അക്കം_ഇനങ്ങൾ < അക്കം_ഇനങ്ങൾ_2 ? അക്കം_ഇനങ്ങൾ : അക്കം_ഇനങ്ങൾ_2;

		വേണ്ടി (uint32_t i = 0; i < അക്കം_min; ++i) {
			new_ഇനങ്ങൾ[i] = ഇനങ്ങൾ[i];

			എങ്കിൽ (new_ഇനങ്ങൾ[i].cache_state == State::RequestSent) {
				new_ഇനങ്ങൾ[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] ഇനങ്ങൾ;
		ഇനങ്ങൾ = new_ഇനങ്ങൾ;
		അക്കം_ഇനങ്ങൾ_2 = അക്കം_ഇനങ്ങൾ;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < അക്കം_ഇനങ്ങൾ_2) ? _load_index : 0;
		_update_index = (_update_index < അക്കം_ഇനങ്ങൾ_2) ? _update_index : 0;
		_item_counter = (_item_counter < അക്കം_ഇനങ്ങൾ_2) ? _item_counter : അക്കം_ഇനങ്ങൾ_2;

	} അല്ലെങ്കിൽ {
		PX4_ERR("alloc failed");
	}
}

തർക്കം DatamanCache::load(dm_item_t item, uint32_t index)
{
	എങ്കിൽ (!ഇനങ്ങൾ) {
		തിരികെ അസത്യം;
	}

	തർക്കം success = അസത്യം;
	തർക്കം duplicate = അസത്യം;

	//Prevent duplicates
	വേണ്ടി (uint32_t i = 0; i < അക്കം_ഇനങ്ങൾ_2; ++i) {

		എങ്കിൽ (ഇനങ്ങൾ[i].cache_state != State::Idle &&
		    ഇനങ്ങൾ[i].cache_state != State::Error &&
		    ഇനങ്ങൾ[i].response.item == item &&
		    ഇനങ്ങൾ[i].response.index == index) {
			duplicate = സത്യം;
			നിർത്തുക;
		}
	}

	എങ്കിൽ (!duplicate && (_item_counter < അക്കം_ഇനങ്ങൾ_2)) {

		ഇനങ്ങൾ[_load_index].cache_state = State::RequestPrepared;
		ഇനങ്ങൾ[_load_index].response.item = item;
		ഇനങ്ങൾ[_load_index].response.index = index;

		_load_index = (_load_index + 1) % അക്കം_ഇനങ്ങൾ_2;

		++_item_counter;

		success = സത്യം;
	}

	തിരികെ success;
}

തർക്കം DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	എങ്കിൽ (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		തിരികെ അസത്യം;
	}

	തർക്കം success = അസത്യം;
	തർക്കം item_found = അസത്യം;

	എങ്കിൽ (ഇനങ്ങൾ) {
		വേണ്ടി (uint32_t i = 0; i < അക്കം_ഇനങ്ങൾ_2; ++i) {
			എങ്കിൽ ((ഇനങ്ങൾ[i].response.item == item) &&
			    (ഇനങ്ങൾ[i].response.index == index)) {
				item_found = സത്യം;

				എങ്കിൽ (ഇനങ്ങൾ[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, ഇനങ്ങൾ[i].response.data, length);
					success = സത്യം;
					നിർത്തുക;
				}
			}
		}
	}

	എങ്കിൽ (!success) {
		perf_എണ്ണം(_cache_miss_perf);
	}

	എങ്കിൽ (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		എങ്കിൽ (success && !item_found && ഇനങ്ങൾ && (_item_counter < അക്കം_ഇനങ്ങൾ_2)) {
			ഇനങ്ങൾ[_load_index].cache_state = State::ResponseReceived;
			ഇനങ്ങൾ[_load_index].response.item = item;
			ഇനങ്ങൾ[_load_index].response.index = index;
			memcpy(ഇനങ്ങൾ[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % അക്കം_ഇനങ്ങൾ_2;

			++_item_counter; // Still increase the counter here
		}
	}

	തിരികെ success;
}

തർക്കം DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	എങ്കിൽ (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		തിരികെ അസത്യം;
	}

	തർക്കം success = _client.writeSync(item, index, buffer, length, timeout);

	എങ്കിൽ (success && ഇനങ്ങൾ) {
		വേണ്ടി (uint32_t i = 0; i < അക്കം_ഇനങ്ങൾ_2; ++i) {
			എങ്കിൽ ((ഇനങ്ങൾ[i].response.item == item) &&
			    (ഇനങ്ങൾ[i].response.index == index) &&
			    ((ഇനങ്ങൾ[i].cache_state == State::ResponseReceived) ||
			     (ഇനങ്ങൾ[i].cache_state == State::RequestPrepared))) {

				memcpy(ഇനങ്ങൾ[i].response.data, buffer, length);
				ഇനങ്ങൾ[i].cache_state = State::ResponseReceived;
				നിർത്തുക;
			}
		}
	}

	തിരികെ success;
}

തർക്കം DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, സ്ഥിരം uint8_t *buffer, uint32_t length)
{
	എങ്കിൽ (!ഇനങ്ങൾ || buffer == nullptr || അക്കം_ഇനങ്ങൾ_2 == 0) {
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		തിരികെ അസത്യം;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	വേണ്ടി (uint32_t i = 0; i < അക്കം_ഇനങ്ങൾ_2; ++i) {
		എങ്കിൽ ((ഇനങ്ങൾ[i].response.item == item) && (ഇനങ്ങൾ[i].response.index == index)
		    && (ഇനങ്ങൾ[i].cache_state == State::ResponseReceived)) {
			memcpy(ഇനങ്ങൾ[i].response.data, buffer, length);
			തിരികെ സത്യം;
		}
	}

	തിരികെ അസത്യം;
}

ശൂന്യം DatamanCache::update()
{
	എങ്കിൽ (_item_counter > 0) {

		_client.update();

		തർക്കം success = അസത്യം;
		തർക്കം response_success = അസത്യം;

		തിരഞ്ഞെടുപ്പ് (ഇനങ്ങൾ[_update_index].cache_state) {
		അവസ്ഥ State::Idle:
			നിർത്തുക;

		അവസ്ഥ State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			നിർത്തുക;

		അവസ്ഥ State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(ഇനങ്ങൾ[_update_index].response.item),
						    ഇനങ്ങൾ[_update_index].response.index,
						    ഇനങ്ങൾ[_update_index].response.data,
						    g_per_item_size[ഇനങ്ങൾ[_update_index].response.item]);

			എങ്കിൽ (success) {
				ഇനങ്ങൾ[_update_index].cache_state = State::RequestSent;

			} അല്ലെങ്കിൽ {
				ഇനങ്ങൾ[_update_index].cache_state = State::Error;
			}

			നിർത്തുക;

		അവസ്ഥ State::RequestSent:

			എങ്കിൽ (_client.lastOperationCompleted(response_success)) {

				എങ്കിൽ (response_success) {

					ഇനങ്ങൾ[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} അല്ലെങ്കിൽ {
					ഇനങ്ങൾ[_update_index].cache_state = State::Error;
				}
			}

			നിർത്തുക;

		അവസ്ഥ State::Error:
			// Handled below
			നിർത്തുക;
		}

		എങ്കിൽ (ഇനങ്ങൾ[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				ഇനങ്ങൾ[_update_index].response.item, ഇനങ്ങൾ[_update_index].response.index,
				ഇനങ്ങൾ[_update_index].response.status);

			changeUpdateIndex();
		}

	}
}

ശൂന്യം DatamanCache::resetCacheState()
{
	_update_index = 0;
	_item_counter = 0;
	_load_index = 0;
}

ശൂന്യം DatamanCache::invalidate()
{
	വേണ്ടി (uint32_t i = 0; i < അക്കം_ഇനങ്ങൾ_2; ++i) {
		ഇനങ്ങൾ[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

വരിയിൽ ശൂന്യം DatamanCache::changeUpdateIndex()
{
	എങ്കിൽ (അക്കം_ഇനങ്ങൾ_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		തിരികെ;
	}

	_update_index = (_update_index + 1) % അക്കം_ഇനങ്ങൾ_2;

	എങ്കിൽ (_item_counter > 0) {
		--_item_counter;
	}
}
