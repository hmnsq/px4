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

#ଅନ୍ତର୍ଭାଵ <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	ଯଦି (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} ଅନ୍ୟଥା {
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

		ତାର୍କିକ success = syncHandler(request, response, timestamp, 1000_ms);

		ଯଦି (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} ଅନ୍ୟଥା {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	ଯଦି (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

ତାର୍କିକ DatamanClient::syncHandler(ଅଚଳ dataman_request_s &request, dataman_response_s &response,
				ଅଚଳ hrt_abstime &start_time, hrt_abstime timeout)
{
	ତାର୍କିକ response_received = ମିଥ୍ୟା;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	ଯେତେବେଳେ (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		ଯଦି (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			ଭାଙ୍ଗ;

		} ଅନ୍ୟଥା ଯଦି (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} ଅନ୍ୟଥା {

			ତାର୍କିକ updated = ମିଥ୍ୟା;
			orb_check(_dataman_response_sub, &updated);

			ଯଦି (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				ଯଦି (response.client_id == request.client_id) {

					ଯଦି ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = ସତ୍ୟ;
						ଭାଙ୍ଗ;
					}

				} ଅନ୍ୟଥା ଯଦି (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					ଯଦି (0 == memcmp(&(request.timestamp), &(response.data), ଆକାର(hrt_abstime))) {
						response_received = ସତ୍ୟ;
						ଭାଙ୍ଗ;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	ଯଦି (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	ଫେରନ୍ତୁ response_received;
}

ତାର୍କିକ DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	ଯଦି (_client_id == CLIENT_ID_NOT_SET) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଯଦି (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
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
	ତାର୍କିକ success = syncHandler(request, response, timestamp, timeout);

	ଯଦି (success) {

		ଯଦି (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = ମିଥ୍ୟା;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} ଅନ୍ୟଥା {
			memcpy(buffer, response.data, length);
		}
	}

	ଫେରନ୍ତୁ success;
}

ତାର୍କିକ DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	ଯଦି (_client_id == CLIENT_ID_NOT_SET) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଯଦି (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
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
	ତାର୍କିକ success = syncHandler(request, response, timestamp, timeout);

	ଯଦି (success) {

		ଯଦି (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = ମିଥ୍ୟା;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	ଫେରନ୍ତୁ success;
}

ତାର୍କିକ DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	ଯଦି (_client_id == CLIENT_ID_NOT_SET) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	ତାର୍କିକ success = syncHandler(request, response, timestamp, timeout);

	ଯଦି (success) {

		ଯଦି (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = ମିଥ୍ୟା;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	ଫେରନ୍ତୁ success;
}

ଶୂନ୍ୟ DatamanClient::clearPendingResponse()
{
	ଯଦି (_dataman_response_sub < 0) {
		ଫେରନ୍ତୁ;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	ତାର୍କିକ updated = ମିଥ୍ୟା;
	orb_check(_dataman_response_sub, &updated);

	ଯେତେବେଳେ (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

ତାର୍କିକ DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	ଯଦି (_client_id == CLIENT_ID_NOT_SET) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଯଦି (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ତାର୍କିକ success = ମିଥ୍ୟା;

	ଯଦି (_state == State::Idle) {
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

		success = ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ success;
}

ତାର୍କିକ DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	ଯଦି (_client_id == CLIENT_ID_NOT_SET) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଯଦି (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ତାର୍କିକ success = ମିଥ୍ୟା;

	ଯଦି (_state == State::Idle) {
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

		success = ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ success;
}

ତାର୍କିକ DatamanClient::clearAsync(dm_item_t item)
{
	ଯଦି (_client_id == CLIENT_ID_NOT_SET) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ତାର୍କିକ success = ମିଥ୍ୟା;

	ଯଦି (_state == State::Idle) {
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

		success = ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ success;
}

ଶୂନ୍ୟ DatamanClient::update()
{
	ଯଦି (_state == State::RequestSent) {

		ତାର୍କିକ updated = ମିଥ୍ୟା;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		ଯଦି (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			ଯଦି ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				ଯଦି (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				ଯଦି (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		ଯଦି (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			ଯଦି (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
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

				ଯଦି (_active_request.request_type == DM_WRITE) {
					memcpy(request.data, _active_request.buffer, _active_request.length);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

ତାର୍କିକ DatamanClient::lastOperationCompleted(ତାର୍କିକ &success)
{
	ତାର୍କିକ completed = ମିଥ୍ୟା;
	success = ମିଥ୍ୟା;

	ଯଦି (_state == State::ResponseReceived) {

		ଯଦି (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = ସତ୍ୟ;
		}

		_state = State::Idle;
		completed = ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ completed;
}

ଶୂନ୍ୟ DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ଅଚଳ ଅକ୍ଷର *cache_miss_perf_counter_name, uint32_t ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ)
	: _cache_miss_perf(perf_alloc(PC_ଗଣନା, cache_miss_perf_counter_name))
{
	ଯଦି (ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ == 0) {
		ଫେରନ୍ତୁ;
	}

	ବସ୍ତୁଗୁଡ଼ିକ = ନୂତନ Item[ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ] {};

	ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ != nullptr) {
		ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2 = ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ;

	} ଅନ୍ୟଥା {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] ବସ୍ତୁଗୁଡ଼ିକ;
	perf_free(_cache_miss_perf);
}

ଶୂନ୍ୟ DatamanCache::resize(uint32_t ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ)
{
	ଯଦି (ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] ବସ୍ତୁଗୁଡ଼ିକ;
		ବସ୍ତୁଗୁଡ଼ିକ = nullptr;
		resetCacheState();
		ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2 = 0;
		ଫେରନ୍ତୁ;
	}

	Item *new_ବସ୍ତୁଗୁଡ଼ିକ = ନୂତନ Item[ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ] {};

	ଯଦି (new_ବସ୍ତୁଗୁଡ଼ିକ != nullptr) {
		_client.abortCurrentOperation();

		uint32_t ଅଙ୍କ_min = ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2 ? ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ : ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2;

		ପାଇଁ (uint32_t i = 0; i < ଅଙ୍କ_min; ++i) {
			new_ବସ୍ତୁଗୁଡ଼ିକ[i] = ବସ୍ତୁଗୁଡ଼ିକ[i];

			ଯଦି (new_ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state == State::RequestSent) {
				new_ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] ବସ୍ତୁଗୁଡ଼ିକ;
		ବସ୍ତୁଗୁଡ଼ିକ = new_ବସ୍ତୁଗୁଡ଼ିକ;
		ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2 = ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2) ? _load_index : 0;
		_update_index = (_update_index < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2) ? _update_index : 0;
		_item_counter = (_item_counter < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2) ? _item_counter : ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2;

	} ଅନ୍ୟଥା {
		PX4_ERR("alloc failed");
	}
}

ତାର୍କିକ DatamanCache::load(dm_item_t item, uint32_t index)
{
	ଯଦି (!ବସ୍ତୁଗୁଡ଼ିକ) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ତାର୍କିକ success = ମିଥ୍ୟା;
	ତାର୍କିକ duplicate = ମିଥ୍ୟା;

	//Prevent duplicates
	ପାଇଁ (uint32_t i = 0; i < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2; ++i) {

		ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state != State::Idle &&
		    ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state != State::Error &&
		    ବସ୍ତୁଗୁଡ଼ିକ[i].response.item == item &&
		    ବସ୍ତୁଗୁଡ଼ିକ[i].response.index == index) {
			duplicate = ସତ୍ୟ;
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (!duplicate && (_item_counter < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2)) {

		ବସ୍ତୁଗୁଡ଼ିକ[_load_index].cache_state = State::RequestPrepared;
		ବସ୍ତୁଗୁଡ଼ିକ[_load_index].response.item = item;
		ବସ୍ତୁଗୁଡ଼ିକ[_load_index].response.index = index;

		_load_index = (_load_index + 1) % ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2;

		++_item_counter;

		success = ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ success;
}

ତାର୍କିକ DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	ଯଦି (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ତାର୍କିକ success = ମିଥ୍ୟା;
	ତାର୍କିକ item_found = ମିଥ୍ୟା;

	ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ) {
		ପାଇଁ (uint32_t i = 0; i < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2; ++i) {
			ଯଦି ((ବସ୍ତୁଗୁଡ଼ିକ[i].response.item == item) &&
			    (ବସ୍ତୁଗୁଡ଼ିକ[i].response.index == index)) {
				item_found = ସତ୍ୟ;

				ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, ବସ୍ତୁଗୁଡ଼ିକ[i].response.data, length);
					success = ସତ୍ୟ;
					ଭାଙ୍ଗ;
				}
			}
		}
	}

	ଯଦି (!success) {
		perf_ଗଣନା(_cache_miss_perf);
	}

	ଯଦି (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		ଯଦି (success && !item_found && ବସ୍ତୁଗୁଡ଼ିକ && (_item_counter < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2)) {
			ବସ୍ତୁଗୁଡ଼ିକ[_load_index].cache_state = State::ResponseReceived;
			ବସ୍ତୁଗୁଡ଼ିକ[_load_index].response.item = item;
			ବସ୍ତୁଗୁଡ଼ିକ[_load_index].response.index = index;
			memcpy(ବସ୍ତୁଗୁଡ଼ିକ[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2;

			++_item_counter; // Still increase the counter here
		}
	}

	ଫେରନ୍ତୁ success;
}

ତାର୍କିକ DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	ଯଦି (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ତାର୍କିକ success = _client.writeSync(item, index, buffer, length, timeout);

	ଯଦି (success && ବସ୍ତୁଗୁଡ଼ିକ) {
		ପାଇଁ (uint32_t i = 0; i < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2; ++i) {
			ଯଦି ((ବସ୍ତୁଗୁଡ଼ିକ[i].response.item == item) &&
			    (ବସ୍ତୁଗୁଡ଼ିକ[i].response.index == index) &&
			    ((ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state == State::ResponseReceived) ||
			     (ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state == State::RequestPrepared))) {

				memcpy(ବସ୍ତୁଗୁଡ଼ିକ[i].response.data, buffer, length);
				ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state = State::ResponseReceived;
				ଭାଙ୍ଗ;
			}
		}
	}

	ଫେରନ୍ତୁ success;
}

ତାର୍କିକ DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, ଅଚଳ uint8_t *buffer, uint32_t length)
{
	ଯଦି (!ବସ୍ତୁଗୁଡ଼ିକ || buffer == nullptr || ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2 == 0) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଯଦି (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	ପାଇଁ (uint32_t i = 0; i < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2; ++i) {
		ଯଦି ((ବସ୍ତୁଗୁଡ଼ିକ[i].response.item == item) && (ବସ୍ତୁଗୁଡ଼ିକ[i].response.index == index)
		    && (ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state == State::ResponseReceived)) {
			memcpy(ବସ୍ତୁଗୁଡ଼ିକ[i].response.data, buffer, length);
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ DatamanCache::update()
{
	ଯଦି (_item_counter > 0) {

		_client.update();

		ତାର୍କିକ success = ମିଥ୍ୟା;
		ତାର୍କିକ response_success = ମିଥ୍ୟା;

		ବାଛ (ବସ୍ତୁଗୁଡ଼ିକ[_update_index].cache_state) {
		ସ୍ଥିତି State::Idle:
			ଭାଙ୍ଗ;

		ସ୍ଥିତି State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			ଭାଙ୍ଗ;

		ସ୍ଥିତି State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(ବସ୍ତୁଗୁଡ଼ିକ[_update_index].response.item),
						    ବସ୍ତୁଗୁଡ଼ିକ[_update_index].response.index,
						    ବସ୍ତୁଗୁଡ଼ିକ[_update_index].response.data,
						    g_per_item_size[ବସ୍ତୁଗୁଡ଼ିକ[_update_index].response.item]);

			ଯଦି (success) {
				ବସ୍ତୁଗୁଡ଼ିକ[_update_index].cache_state = State::RequestSent;

			} ଅନ୍ୟଥା {
				ବସ୍ତୁଗୁଡ଼ିକ[_update_index].cache_state = State::Error;
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି State::RequestSent:

			ଯଦି (_client.lastOperationCompleted(response_success)) {

				ଯଦି (response_success) {

					ବସ୍ତୁଗୁଡ଼ିକ[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} ଅନ୍ୟଥା {
					ବସ୍ତୁଗୁଡ଼ିକ[_update_index].cache_state = State::Error;
				}
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି State::Error:
			// Handled below
			ଭାଙ୍ଗ;
		}

		ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				ବସ୍ତୁଗୁଡ଼ିକ[_update_index].response.item, ବସ୍ତୁଗୁଡ଼ିକ[_update_index].response.index,
				ବସ୍ତୁଗୁଡ଼ିକ[_update_index].response.status);

			changeUpdateIndex();
		}

	}
}

ଶୂନ୍ୟ DatamanCache::resetCacheState()
{
	_update_index = 0;
	_item_counter = 0;
	_load_index = 0;
}

ଶୂନ୍ୟ DatamanCache::invalidate()
{
	ପାଇଁ (uint32_t i = 0; i < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2; ++i) {
		ବସ୍ତୁଗୁଡ଼ିକ[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ DatamanCache::changeUpdateIndex()
{
	ଯଦି (ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		ଫେରନ୍ତୁ;
	}

	_update_index = (_update_index + 1) % ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ_2;

	ଯଦି (_item_counter > 0) {
		--_item_counter;
	}
}
