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

#અન્તર્ભાવ <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	જો (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} નહીંતર {
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

		તાર્કિક success = syncHandler(request, response, timestamp, 1000_ms);

		જો (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} નહીંતર {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	જો (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

તાર્કિક DatamanClient::syncHandler(અચલ dataman_request_s &request, dataman_response_s &response,
				અચલ hrt_abstime &start_time, hrt_abstime timeout)
{
	તાર્કિક response_received = ખોટું;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	જ્યારે (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		જો (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			તોડો;

		} નહીંતર જો (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} નહીંતર {

			તાર્કિક updated = ખોટું;
			orb_check(_dataman_response_sub, &updated);

			જો (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				જો (response.client_id == request.client_id) {

					જો ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = સાચું;
						તોડો;
					}

				} નહીંતર જો (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					જો (0 == memcmp(&(request.timestamp), &(response.data), કદ(hrt_abstime))) {
						response_received = સાચું;
						તોડો;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	જો (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	પાછા response_received;
}

તાર્કિક DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	જો (_client_id == CLIENT_ID_NOT_SET) {
		પાછા ખોટું;
	}

	જો (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		પાછા ખોટું;
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
	તાર્કિક success = syncHandler(request, response, timestamp, timeout);

	જો (success) {

		જો (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = ખોટું;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} નહીંતર {
			memcpy(buffer, response.data, length);
		}
	}

	પાછા success;
}

તાર્કિક DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	જો (_client_id == CLIENT_ID_NOT_SET) {
		પાછા ખોટું;
	}

	જો (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		પાછા ખોટું;
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
	તાર્કિક success = syncHandler(request, response, timestamp, timeout);

	જો (success) {

		જો (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = ખોટું;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	પાછા success;
}

તાર્કિક DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	જો (_client_id == CLIENT_ID_NOT_SET) {
		પાછા ખોટું;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	તાર્કિક success = syncHandler(request, response, timestamp, timeout);

	જો (success) {

		જો (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = ખોટું;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	પાછા success;
}

શૂન્ય DatamanClient::clearPendingResponse()
{
	જો (_dataman_response_sub < 0) {
		પાછા;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	તાર્કિક updated = ખોટું;
	orb_check(_dataman_response_sub, &updated);

	જ્યારે (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

તાર્કિક DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	જો (_client_id == CLIENT_ID_NOT_SET) {
		પાછા ખોટું;
	}

	જો (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		પાછા ખોટું;
	}

	તાર્કિક success = ખોટું;

	જો (_state == State::Idle) {
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

		success = સાચું;
	}

	પાછા success;
}

તાર્કિક DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	જો (_client_id == CLIENT_ID_NOT_SET) {
		પાછા ખોટું;
	}

	જો (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		પાછા ખોટું;
	}

	તાર્કિક success = ખોટું;

	જો (_state == State::Idle) {
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

		success = સાચું;
	}

	પાછા success;
}

તાર્કિક DatamanClient::clearAsync(dm_item_t item)
{
	જો (_client_id == CLIENT_ID_NOT_SET) {
		પાછા ખોટું;
	}

	તાર્કિક success = ખોટું;

	જો (_state == State::Idle) {
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

		success = સાચું;
	}

	પાછા success;
}

શૂન્ય DatamanClient::update()
{
	જો (_state == State::RequestSent) {

		તાર્કિક updated = ખોટું;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		જો (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			જો ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				જો (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				જો (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		જો (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			જો (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
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

				જો (_active_request.request_type == DM_WRITE) {
					memcpy(request.data, _active_request.buffer, _active_request.length);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

તાર્કિક DatamanClient::lastOperationCompleted(તાર્કિક &success)
{
	તાર્કિક completed = ખોટું;
	success = ખોટું;

	જો (_state == State::ResponseReceived) {

		જો (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = સાચું;
		}

		_state = State::Idle;
		completed = સાચું;
	}

	પાછા completed;
}

શૂન્ય DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(અચલ અક્ષર *cache_miss_perf_counter_name, uint32_t અંક_વસ્તુઓ)
	: _cache_miss_perf(perf_alloc(PC_ગણતરી, cache_miss_perf_counter_name))
{
	જો (અંક_વસ્તુઓ == 0) {
		પાછા;
	}

	વસ્તુઓ = નવું Item[અંક_વસ્તુઓ] {};

	જો (વસ્તુઓ != nullptr) {
		અંક_વસ્તુઓ_2 = અંક_વસ્તુઓ;

	} નહીંતર {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] વસ્તુઓ;
	perf_free(_cache_miss_perf);
}

શૂન્ય DatamanCache::resize(uint32_t અંક_વસ્તુઓ)
{
	જો (અંક_વસ્તુઓ == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] વસ્તુઓ;
		વસ્તુઓ = nullptr;
		resetCacheState();
		અંક_વસ્તુઓ_2 = 0;
		પાછા;
	}

	Item *new_વસ્તુઓ = નવું Item[અંક_વસ્તુઓ] {};

	જો (new_વસ્તુઓ != nullptr) {
		_client.abortCurrentOperation();

		uint32_t અંક_min = અંક_વસ્તુઓ < અંક_વસ્તુઓ_2 ? અંક_વસ્તુઓ : અંક_વસ્તુઓ_2;

		માટે (uint32_t i = 0; i < અંક_min; ++i) {
			new_વસ્તુઓ[i] = વસ્તુઓ[i];

			જો (new_વસ્તુઓ[i].cache_state == State::RequestSent) {
				new_વસ્તુઓ[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] વસ્તુઓ;
		વસ્તુઓ = new_વસ્તુઓ;
		અંક_વસ્તુઓ_2 = અંક_વસ્તુઓ;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < અંક_વસ્તુઓ_2) ? _load_index : 0;
		_update_index = (_update_index < અંક_વસ્તુઓ_2) ? _update_index : 0;
		_item_counter = (_item_counter < અંક_વસ્તુઓ_2) ? _item_counter : અંક_વસ્તુઓ_2;

	} નહીંતર {
		PX4_ERR("alloc failed");
	}
}

તાર્કિક DatamanCache::load(dm_item_t item, uint32_t index)
{
	જો (!વસ્તુઓ) {
		પાછા ખોટું;
	}

	તાર્કિક success = ખોટું;
	તાર્કિક duplicate = ખોટું;

	//Prevent duplicates
	માટે (uint32_t i = 0; i < અંક_વસ્તુઓ_2; ++i) {

		જો (વસ્તુઓ[i].cache_state != State::Idle &&
		    વસ્તુઓ[i].cache_state != State::Error &&
		    વસ્તુઓ[i].response.item == item &&
		    વસ્તુઓ[i].response.index == index) {
			duplicate = સાચું;
			તોડો;
		}
	}

	જો (!duplicate && (_item_counter < અંક_વસ્તુઓ_2)) {

		વસ્તુઓ[_load_index].cache_state = State::RequestPrepared;
		વસ્તુઓ[_load_index].response.item = item;
		વસ્તુઓ[_load_index].response.index = index;

		_load_index = (_load_index + 1) % અંક_વસ્તુઓ_2;

		++_item_counter;

		success = સાચું;
	}

	પાછા success;
}

તાર્કિક DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	જો (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		પાછા ખોટું;
	}

	તાર્કિક success = ખોટું;
	તાર્કિક item_found = ખોટું;

	જો (વસ્તુઓ) {
		માટે (uint32_t i = 0; i < અંક_વસ્તુઓ_2; ++i) {
			જો ((વસ્તુઓ[i].response.item == item) &&
			    (વસ્તુઓ[i].response.index == index)) {
				item_found = સાચું;

				જો (વસ્તુઓ[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, વસ્તુઓ[i].response.data, length);
					success = સાચું;
					તોડો;
				}
			}
		}
	}

	જો (!success) {
		perf_ગણતરી(_cache_miss_perf);
	}

	જો (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		જો (success && !item_found && વસ્તુઓ && (_item_counter < અંક_વસ્તુઓ_2)) {
			વસ્તુઓ[_load_index].cache_state = State::ResponseReceived;
			વસ્તુઓ[_load_index].response.item = item;
			વસ્તુઓ[_load_index].response.index = index;
			memcpy(વસ્તુઓ[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % અંક_વસ્તુઓ_2;

			++_item_counter; // Still increase the counter here
		}
	}

	પાછા success;
}

તાર્કિક DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	જો (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		પાછા ખોટું;
	}

	તાર્કિક success = _client.writeSync(item, index, buffer, length, timeout);

	જો (success && વસ્તુઓ) {
		માટે (uint32_t i = 0; i < અંક_વસ્તુઓ_2; ++i) {
			જો ((વસ્તુઓ[i].response.item == item) &&
			    (વસ્તુઓ[i].response.index == index) &&
			    ((વસ્તુઓ[i].cache_state == State::ResponseReceived) ||
			     (વસ્તુઓ[i].cache_state == State::RequestPrepared))) {

				memcpy(વસ્તુઓ[i].response.data, buffer, length);
				વસ્તુઓ[i].cache_state = State::ResponseReceived;
				તોડો;
			}
		}
	}

	પાછા success;
}

તાર્કિક DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, અચલ uint8_t *buffer, uint32_t length)
{
	જો (!વસ્તુઓ || buffer == nullptr || અંક_વસ્તુઓ_2 == 0) {
		પાછા ખોટું;
	}

	જો (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		પાછા ખોટું;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	માટે (uint32_t i = 0; i < અંક_વસ્તુઓ_2; ++i) {
		જો ((વસ્તુઓ[i].response.item == item) && (વસ્તુઓ[i].response.index == index)
		    && (વસ્તુઓ[i].cache_state == State::ResponseReceived)) {
			memcpy(વસ્તુઓ[i].response.data, buffer, length);
			પાછા સાચું;
		}
	}

	પાછા ખોટું;
}

શૂન્ય DatamanCache::update()
{
	જો (_item_counter > 0) {

		_client.update();

		તાર્કિક success = ખોટું;
		તાર્કિક response_success = ખોટું;

		પસંદગી (વસ્તુઓ[_update_index].cache_state) {
		સ્થિતિ State::Idle:
			તોડો;

		સ્થિતિ State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			તોડો;

		સ્થિતિ State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(વસ્તુઓ[_update_index].response.item),
						    વસ્તુઓ[_update_index].response.index,
						    વસ્તુઓ[_update_index].response.data,
						    g_per_item_size[વસ્તુઓ[_update_index].response.item]);

			જો (success) {
				વસ્તુઓ[_update_index].cache_state = State::RequestSent;

			} નહીંતર {
				વસ્તુઓ[_update_index].cache_state = State::Error;
			}

			તોડો;

		સ્થિતિ State::RequestSent:

			જો (_client.lastOperationCompleted(response_success)) {

				જો (response_success) {

					વસ્તુઓ[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} નહીંતર {
					વસ્તુઓ[_update_index].cache_state = State::Error;
				}
			}

			તોડો;

		સ્થિતિ State::Error:
			// Handled below
			તોડો;
		}

		જો (વસ્તુઓ[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				વસ્તુઓ[_update_index].response.item, વસ્તુઓ[_update_index].response.index,
				વસ્તુઓ[_update_index].response.status);

			changeUpdateIndex();
		}

	}
}

શૂન્ય DatamanCache::resetCacheState()
{
	_update_index = 0;
	_item_counter = 0;
	_load_index = 0;
}

શૂન્ય DatamanCache::invalidate()
{
	માટે (uint32_t i = 0; i < અંક_વસ્તુઓ_2; ++i) {
		વસ્તુઓ[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

અંતઃરેખ શૂન્ય DatamanCache::changeUpdateIndex()
{
	જો (અંક_વસ્તુઓ_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		પાછા;
	}

	_update_index = (_update_index + 1) % અંક_વસ્તુઓ_2;

	જો (_item_counter > 0) {
		--_item_counter;
	}
}
