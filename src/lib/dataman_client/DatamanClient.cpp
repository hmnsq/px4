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

#अन्तर्भाव <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	यदि (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} अन्यथा {
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

		तार्किक success = syncHandler(request, response, timestamp, 1000_ms);

		यदि (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} अन्यथा {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	यदि (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

तार्किक DatamanClient::syncHandler(ध्रुव dataman_request_s &request, dataman_response_s &response,
				ध्रुव hrt_abstime &start_time, hrt_abstime timeout)
{
	तार्किक response_received = असत्य;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	यावत् (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		यदि (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			भङ्ग;

		} अन्यथा यदि (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} अन्यथा {

			तार्किक updated = असत्य;
			orb_check(_dataman_response_sub, &updated);

			यदि (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				यदि (response.client_id == request.client_id) {

					यदि ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = सत्य;
						भङ्ग;
					}

				} अन्यथा यदि (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					यदि (0 == memcmp(&(request.timestamp), &(response.data), परिमाण(hrt_abstime))) {
						response_received = सत्य;
						भङ्ग;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	यदि (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	प्रत्यागम response_received;
}

तार्किक DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	यदि (_client_id == CLIENT_ID_NOT_SET) {
		प्रत्यागम असत्य;
	}

	यदि (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
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
	तार्किक success = syncHandler(request, response, timestamp, timeout);

	यदि (success) {

		यदि (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = असत्य;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} अन्यथा {
			memcpy(buffer, response.data, length);
		}
	}

	प्रत्यागम success;
}

तार्किक DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	यदि (_client_id == CLIENT_ID_NOT_SET) {
		प्रत्यागम असत्य;
	}

	यदि (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
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
	तार्किक success = syncHandler(request, response, timestamp, timeout);

	यदि (success) {

		यदि (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = असत्य;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	प्रत्यागम success;
}

तार्किक DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	यदि (_client_id == CLIENT_ID_NOT_SET) {
		प्रत्यागम असत्य;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	तार्किक success = syncHandler(request, response, timestamp, timeout);

	यदि (success) {

		यदि (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = असत्य;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	प्रत्यागम success;
}

शून्य DatamanClient::clearPendingResponse()
{
	यदि (_dataman_response_sub < 0) {
		प्रत्यागम;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	तार्किक updated = असत्य;
	orb_check(_dataman_response_sub, &updated);

	यावत् (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

तार्किक DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	यदि (_client_id == CLIENT_ID_NOT_SET) {
		प्रत्यागम असत्य;
	}

	यदि (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;

	यदि (_state == State::Idle) {
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

		success = सत्य;
	}

	प्रत्यागम success;
}

तार्किक DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	यदि (_client_id == CLIENT_ID_NOT_SET) {
		प्रत्यागम असत्य;
	}

	यदि (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;

	यदि (_state == State::Idle) {
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

		success = सत्य;
	}

	प्रत्यागम success;
}

तार्किक DatamanClient::clearAsync(dm_item_t item)
{
	यदि (_client_id == CLIENT_ID_NOT_SET) {
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;

	यदि (_state == State::Idle) {
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

		success = सत्य;
	}

	प्रत्यागम success;
}

शून्य DatamanClient::update()
{
	यदि (_state == State::RequestSent) {

		तार्किक updated = असत्य;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		यदि (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			यदि ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				यदि (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				यदि (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		यदि (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			यदि (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
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

				यदि (_active_request.request_type == DM_WRITE) {
					memcpy(request.data, _active_request.buffer, _active_request.length);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

तार्किक DatamanClient::lastOperationCompleted(तार्किक &success)
{
	तार्किक completed = असत्य;
	success = असत्य;

	यदि (_state == State::ResponseReceived) {

		यदि (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = सत्य;
		}

		_state = State::Idle;
		completed = सत्य;
	}

	प्रत्यागम completed;
}

शून्य DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ध्रुव वर्ण *cache_miss_perf_counter_name, uint32_t अंक_वस्तुसंच)
	: _cache_miss_perf(perf_alloc(PC_मोजणी, cache_miss_perf_counter_name))
{
	यदि (अंक_वस्तुसंच == 0) {
		प्रत्यागम;
	}

	वस्तुसंच = नूतन Item[अंक_वस्तुसंच] {};

	यदि (वस्तुसंच != nullptr) {
		अंक_वस्तुसंच_2 = अंक_वस्तुसंच;

	} अन्यथा {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] वस्तुसंच;
	perf_free(_cache_miss_perf);
}

शून्य DatamanCache::resize(uint32_t अंक_वस्तुसंच)
{
	यदि (अंक_वस्तुसंच == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] वस्तुसंच;
		वस्तुसंच = nullptr;
		resetCacheState();
		अंक_वस्तुसंच_2 = 0;
		प्रत्यागम;
	}

	Item *new_वस्तुसंच = नूतन Item[अंक_वस्तुसंच] {};

	यदि (new_वस्तुसंच != nullptr) {
		_client.abortCurrentOperation();

		uint32_t अंक_min = अंक_वस्तुसंच < अंक_वस्तुसंच_2 ? अंक_वस्तुसंच : अंक_वस्तुसंच_2;

		प्रति (uint32_t i = 0; i < अंक_min; ++i) {
			new_वस्तुसंच[i] = वस्तुसंच[i];

			यदि (new_वस्तुसंच[i].cache_state == State::RequestSent) {
				new_वस्तुसंच[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] वस्तुसंच;
		वस्तुसंच = new_वस्तुसंच;
		अंक_वस्तुसंच_2 = अंक_वस्तुसंच;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < अंक_वस्तुसंच_2) ? _load_index : 0;
		_update_index = (_update_index < अंक_वस्तुसंच_2) ? _update_index : 0;
		_item_counter = (_item_counter < अंक_वस्तुसंच_2) ? _item_counter : अंक_वस्तुसंच_2;

	} अन्यथा {
		PX4_ERR("alloc failed");
	}
}

तार्किक DatamanCache::load(dm_item_t item, uint32_t index)
{
	यदि (!वस्तुसंच) {
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;
	तार्किक duplicate = असत्य;

	//Prevent duplicates
	प्रति (uint32_t i = 0; i < अंक_वस्तुसंच_2; ++i) {

		यदि (वस्तुसंच[i].cache_state != State::Idle &&
		    वस्तुसंच[i].cache_state != State::Error &&
		    वस्तुसंच[i].response.item == item &&
		    वस्तुसंच[i].response.index == index) {
			duplicate = सत्य;
			भङ्ग;
		}
	}

	यदि (!duplicate && (_item_counter < अंक_वस्तुसंच_2)) {

		वस्तुसंच[_load_index].cache_state = State::RequestPrepared;
		वस्तुसंच[_load_index].response.item = item;
		वस्तुसंच[_load_index].response.index = index;

		_load_index = (_load_index + 1) % अंक_वस्तुसंच_2;

		++_item_counter;

		success = सत्य;
	}

	प्रत्यागम success;
}

तार्किक DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	यदि (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;
	तार्किक item_found = असत्य;

	यदि (वस्तुसंच) {
		प्रति (uint32_t i = 0; i < अंक_वस्तुसंच_2; ++i) {
			यदि ((वस्तुसंच[i].response.item == item) &&
			    (वस्तुसंच[i].response.index == index)) {
				item_found = सत्य;

				यदि (वस्तुसंच[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, वस्तुसंच[i].response.data, length);
					success = सत्य;
					भङ्ग;
				}
			}
		}
	}

	यदि (!success) {
		perf_मोजणी(_cache_miss_perf);
	}

	यदि (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		यदि (success && !item_found && वस्तुसंच && (_item_counter < अंक_वस्तुसंच_2)) {
			वस्तुसंच[_load_index].cache_state = State::ResponseReceived;
			वस्तुसंच[_load_index].response.item = item;
			वस्तुसंच[_load_index].response.index = index;
			memcpy(वस्तुसंच[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % अंक_वस्तुसंच_2;

			++_item_counter; // Still increase the counter here
		}
	}

	प्रत्यागम success;
}

तार्किक DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	यदि (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	तार्किक success = _client.writeSync(item, index, buffer, length, timeout);

	यदि (success && वस्तुसंच) {
		प्रति (uint32_t i = 0; i < अंक_वस्तुसंच_2; ++i) {
			यदि ((वस्तुसंच[i].response.item == item) &&
			    (वस्तुसंच[i].response.index == index) &&
			    ((वस्तुसंच[i].cache_state == State::ResponseReceived) ||
			     (वस्तुसंच[i].cache_state == State::RequestPrepared))) {

				memcpy(वस्तुसंच[i].response.data, buffer, length);
				वस्तुसंच[i].cache_state = State::ResponseReceived;
				भङ्ग;
			}
		}
	}

	प्रत्यागम success;
}

तार्किक DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, ध्रुव uint8_t *buffer, uint32_t length)
{
	यदि (!वस्तुसंच || buffer == nullptr || अंक_वस्तुसंच_2 == 0) {
		प्रत्यागम असत्य;
	}

	यदि (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	प्रति (uint32_t i = 0; i < अंक_वस्तुसंच_2; ++i) {
		यदि ((वस्तुसंच[i].response.item == item) && (वस्तुसंच[i].response.index == index)
		    && (वस्तुसंच[i].cache_state == State::ResponseReceived)) {
			memcpy(वस्तुसंच[i].response.data, buffer, length);
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

शून्य DatamanCache::update()
{
	यदि (_item_counter > 0) {

		_client.update();

		तार्किक success = असत्य;
		तार्किक response_success = असत्य;

		विकल्प (वस्तुसंच[_update_index].cache_state) {
		पक्ष State::Idle:
			भङ्ग;

		पक्ष State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			भङ्ग;

		पक्ष State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(वस्तुसंच[_update_index].response.item),
						    वस्तुसंच[_update_index].response.index,
						    वस्तुसंच[_update_index].response.data,
						    g_per_item_size[वस्तुसंच[_update_index].response.item]);

			यदि (success) {
				वस्तुसंच[_update_index].cache_state = State::RequestSent;

			} अन्यथा {
				वस्तुसंच[_update_index].cache_state = State::Error;
			}

			भङ्ग;

		पक्ष State::RequestSent:

			यदि (_client.lastOperationCompleted(response_success)) {

				यदि (response_success) {

					वस्तुसंच[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} अन्यथा {
					वस्तुसंच[_update_index].cache_state = State::Error;
				}
			}

			भङ्ग;

		पक्ष State::Error:
			// Handled below
			भङ्ग;
		}

		यदि (वस्तुसंच[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				वस्तुसंच[_update_index].response.item, वस्तुसंच[_update_index].response.index,
				वस्तुसंच[_update_index].response.status);

			changeUpdateIndex();
		}

	}
}

शून्य DatamanCache::resetCacheState()
{
	_update_index = 0;
	_item_counter = 0;
	_load_index = 0;
}

शून्य DatamanCache::invalidate()
{
	प्रति (uint32_t i = 0; i < अंक_वस्तुसंच_2; ++i) {
		वस्तुसंच[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

अन्तःस्थ शून्य DatamanCache::changeUpdateIndex()
{
	यदि (अंक_वस्तुसंच_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		प्रत्यागम;
	}

	_update_index = (_update_index + 1) % अंक_वस्तुसंच_2;

	यदि (_item_counter > 0) {
		--_item_counter;
	}
}
