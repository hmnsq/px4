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

	सचे (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} अञ्ञथा {
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

		सचे (success && (response.client_id > CLIENT_ID_NOT_SET)) {

			_client_id = response.client_id;

		} अञ्ञथा {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	सचे (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

तार्किक DatamanClient::syncHandler(ध्रुव dataman_request_s &request, dataman_response_s &response,
				ध्रुव hrt_abstime &start_time, hrt_abstime timeout)
{
	तार्किक response_received = मिच्छा;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	याव (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		सचे (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			भङ्ग;

		} अञ्ञथा सचे (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} अञ्ञथा {

			तार्किक updated = मिच्छा;
			orb_check(_dataman_response_sub, &updated);

			सचे (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				सचे (response.client_id == request.client_id) {

					सचे ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.index == request.index)) {
						response_received = सच्च;
						भङ्ग;
					}

				} अञ्ञथा सचे (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					सचे (0 == memcmp(&(request.timestamp), &(response.data), परिमाण(hrt_abstime))) {
						response_received = सच्च;
						भङ्ग;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_end(_sync_perf);

	सचे (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	पटिनिवत्त response_received;
}

तार्किक DatamanClient::readSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	सचे (_client_id == CLIENT_ID_NOT_SET) {
		पटिनिवत्त मिच्छा;
	}

	सचे (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		पटिनिवत्त मिच्छा;
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

	सचे (success) {

		सचे (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = मिच्छा;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);

		} अञ्ञथा {
			memcpy(buffer, response.data, length);
		}
	}

	पटिनिवत्त success;
}

तार्किक DatamanClient::writeSync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	सचे (_client_id == CLIENT_ID_NOT_SET) {
		पटिनिवत्त मिच्छा;
	}

	सचे (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		पटिनिवत्त मिच्छा;
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

	सचे (success) {

		सचे (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = मिच्छा;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), index, length);
		}
	}

	पटिनिवत्त success;
}

तार्किक DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	सचे (_client_id == CLIENT_ID_NOT_SET) {
		पटिनिवत्त मिच्छा;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	तार्किक success = syncHandler(request, response, timestamp, timeout);

	सचे (success) {

		सचे (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = मिच्छा;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	पटिनिवत्त success;
}

शून्य DatamanClient::clearPendingResponse()
{
	सचे (_dataman_response_sub < 0) {
		पटिनिवत्त;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	तार्किक updated = मिच्छा;
	orb_check(_dataman_response_sub, &updated);

	याव (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

तार्किक DatamanClient::readAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	सचे (_client_id == CLIENT_ID_NOT_SET) {
		पटिनिवत्त मिच्छा;
	}

	सचे (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		पटिनिवत्त मिच्छा;
	}

	तार्किक success = मिच्छा;

	सचे (_state == State::Idle) {
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

		success = सच्च;
	}

	पटिनिवत्त success;
}

तार्किक DatamanClient::writeAsync(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length)
{
	सचे (_client_id == CLIENT_ID_NOT_SET) {
		पटिनिवत्त मिच्छा;
	}

	सचे (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		पटिनिवत्त मिच्छा;
	}

	तार्किक success = मिच्छा;

	सचे (_state == State::Idle) {
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

		success = सच्च;
	}

	पटिनिवत्त success;
}

तार्किक DatamanClient::clearAsync(dm_item_t item)
{
	सचे (_client_id == CLIENT_ID_NOT_SET) {
		पटिनिवत्त मिच्छा;
	}

	तार्किक success = मिच्छा;

	सचे (_state == State::Idle) {
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

		success = सच्च;
	}

	पटिनिवत्त success;
}

शून्य DatamanClient::update()
{
	सचे (_state == State::RequestSent) {

		तार्किक updated = मिच्छा;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		सचे (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			सचे ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.index == _active_request.index)) {

				सचे (response.request_type == DM_READ) {
					memcpy(_active_request.buffer, response.data, _active_request.length);
				}

				_response_status = response.status;

				सचे (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.index);
				}

				_state = State::ResponseReceived;
			}
		}

		सचे (_state == State::RequestSent) {

			/* Retry the request if there is no answer */
			सचे (((_active_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&_active_request.timestamp) > 100_ms)) ||
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

				सचे (_active_request.request_type == DM_WRITE) {
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
	तार्किक completed = मिच्छा;
	success = मिच्छा;

	सचे (_state == State::ResponseReceived) {

		सचे (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = सच्च;
		}

		_state = State::Idle;
		completed = सच्च;
	}

	पटिनिवत्त completed;
}

शून्य DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ध्रुव वर्ण *cache_miss_perf_counter_name, uint32_t अङ्कः_वस्तूनि)
	: _cache_miss_perf(perf_alloc(PC_गणनम्, cache_miss_perf_counter_name))
{
	सचे (अङ्कः_वस्तूनि == 0) {
		पटिनिवत्त;
	}

	वस्तूनि = नूतन Item[अङ्कः_वस्तूनि] {};

	सचे (वस्तूनि != nullptr) {
		अङ्कः_वस्तूनि_2 = अङ्कः_वस्तूनि;

	} अञ्ञथा {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	delete[] वस्तूनि;
	perf_free(_cache_miss_perf);
}

शून्य DatamanCache::resize(uint32_t अङ्कः_वस्तूनि)
{
	सचे (अङ्कः_वस्तूनि == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		delete[] वस्तूनि;
		वस्तूनि = nullptr;
		resetCacheState();
		अङ्कः_वस्तूनि_2 = 0;
		पटिनिवत्त;
	}

	Item *new_वस्तूनि = नूतन Item[अङ्कः_वस्तूनि] {};

	सचे (new_वस्तूनि != nullptr) {
		_client.abortCurrentOperation();

		uint32_t अङ्कः_min = अङ्कः_वस्तूनि < अङ्कः_वस्तूनि_2 ? अङ्कः_वस्तूनि : अङ्कः_वस्तूनि_2;

		अत्थाय (uint32_t i = 0; i < अङ्कः_min; ++i) {
			new_वस्तूनि[i] = वस्तूनि[i];

			सचे (new_वस्तूनि[i].cache_state == State::RequestSent) {
				new_वस्तूनि[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] वस्तूनि;
		वस्तूनि = new_वस्तूनि;
		अङ्कः_वस्तूनि_2 = अङ्कः_वस्तूनि;

		// ensure new bounds are respected after resize
		_load_index = (_load_index < अङ्कः_वस्तूनि_2) ? _load_index : 0;
		_update_index = (_update_index < अङ्कः_वस्तूनि_2) ? _update_index : 0;
		_item_counter = (_item_counter < अङ्कः_वस्तूनि_2) ? _item_counter : अङ्कः_वस्तूनि_2;

	} अञ्ञथा {
		PX4_ERR("alloc failed");
	}
}

तार्किक DatamanCache::load(dm_item_t item, uint32_t index)
{
	सचे (!वस्तूनि) {
		पटिनिवत्त मिच्छा;
	}

	तार्किक success = मिच्छा;
	तार्किक duplicate = मिच्छा;

	//Prevent duplicates
	अत्थाय (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {

		सचे (वस्तूनि[i].cache_state != State::Idle &&
		    वस्तूनि[i].cache_state != State::Error &&
		    वस्तूनि[i].response.item == item &&
		    वस्तूनि[i].response.index == index) {
			duplicate = सच्च;
			भङ्ग;
		}
	}

	सचे (!duplicate && (_item_counter < अङ्कः_वस्तूनि_2)) {

		वस्तूनि[_load_index].cache_state = State::RequestPrepared;
		वस्तूनि[_load_index].response.item = item;
		वस्तूनि[_load_index].response.index = index;

		_load_index = (_load_index + 1) % अङ्कः_वस्तूनि_2;

		++_item_counter;

		success = सच्च;
	}

	पटिनिवत्त success;
}

तार्किक DatamanCache::loadWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	सचे (length > g_per_item_size[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, length, static_cast<uint8_t>(item));
		पटिनिवत्त मिच्छा;
	}

	तार्किक success = मिच्छा;
	तार्किक item_found = मिच्छा;

	सचे (वस्तूनि) {
		अत्थाय (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {
			सचे ((वस्तूनि[i].response.item == item) &&
			    (वस्तूनि[i].response.index == index)) {
				item_found = सच्च;

				सचे (वस्तूनि[i].cache_state == State::ResponseReceived) {
					memcpy(buffer, वस्तूनि[i].response.data, length);
					success = सच्च;
					भङ्ग;
				}
			}
		}
	}

	सचे (!success) {
		perf_गणनम्(_cache_miss_perf);
	}

	सचे (!success && (timeout > 0)) {
		success = _client.readSync(item, index, buffer, length, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		सचे (success && !item_found && वस्तूनि && (_item_counter < अङ्कः_वस्तूनि_2)) {
			वस्तूनि[_load_index].cache_state = State::ResponseReceived;
			वस्तूनि[_load_index].response.item = item;
			वस्तूनि[_load_index].response.index = index;
			memcpy(वस्तूनि[_load_index].response.data, buffer, length);

			_load_index = (_load_index + 1) % अङ्कः_वस्तूनि_2;

			++_item_counter; // Still increase the counter here
		}
	}

	पटिनिवत्त success;
}

तार्किक DatamanCache::writeWait(dm_item_t item, uint32_t index, uint8_t *buffer, uint32_t length, hrt_abstime timeout)
{
	सचे (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		पटिनिवत्त मिच्छा;
	}

	तार्किक success = _client.writeSync(item, index, buffer, length, timeout);

	सचे (success && वस्तूनि) {
		अत्थाय (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {
			सचे ((वस्तूनि[i].response.item == item) &&
			    (वस्तूनि[i].response.index == index) &&
			    ((वस्तूनि[i].cache_state == State::ResponseReceived) ||
			     (वस्तूनि[i].cache_state == State::RequestPrepared))) {

				memcpy(वस्तूनि[i].response.data, buffer, length);
				वस्तूनि[i].cache_state = State::ResponseReceived;
				भङ्ग;
			}
		}
	}

	पटिनिवत्त success;
}

तार्किक DatamanCache::updateCachedItem(dm_item_t item, uint32_t index, ध्रुव uint8_t *buffer, uint32_t length)
{
	सचे (!वस्तूनि || buffer == nullptr || अङ्कः_वस्तूनि_2 == 0) {
		पटिनिवत्त मिच्छा;
	}

	सचे (length > g_per_item_size[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, length, static_cast<uint8_t>(item));
		पटिनिवत्त मिच्छा;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	अत्थाय (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {
		सचे ((वस्तूनि[i].response.item == item) && (वस्तूनि[i].response.index == index)
		    && (वस्तूनि[i].cache_state == State::ResponseReceived)) {
			memcpy(वस्तूनि[i].response.data, buffer, length);
			पटिनिवत्त सच्च;
		}
	}

	पटिनिवत्त मिच्छा;
}

शून्य DatamanCache::update()
{
	सचे (_item_counter > 0) {

		_client.update();

		तार्किक success = मिच्छा;
		तार्किक response_success = मिच्छा;

		विकल्प (वस्तूनि[_update_index].cache_state) {
		पक्ष State::Idle:
			भङ्ग;

		पक्ष State::ResponseReceived:
			// Skip it
			changeUpdateIndex();
			भङ्ग;

		पक्ष State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(वस्तूनि[_update_index].response.item),
						    वस्तूनि[_update_index].response.index,
						    वस्तूनि[_update_index].response.data,
						    g_per_item_size[वस्तूनि[_update_index].response.item]);

			सचे (success) {
				वस्तूनि[_update_index].cache_state = State::RequestSent;

			} अञ्ञथा {
				वस्तूनि[_update_index].cache_state = State::Error;
			}

			भङ्ग;

		पक्ष State::RequestSent:

			सचे (_client.lastOperationCompleted(response_success)) {

				सचे (response_success) {

					वस्तूनि[_update_index].cache_state = State::ResponseReceived;
					changeUpdateIndex();

				} अञ्ञथा {
					वस्तूनि[_update_index].cache_state = State::Error;
				}
			}

			भङ्ग;

		पक्ष State::Error:
			// Handled below
			भङ्ग;
		}

		सचे (वस्तूनि[_update_index].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				वस्तूनि[_update_index].response.item, वस्तूनि[_update_index].response.index,
				वस्तूनि[_update_index].response.status);

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
	अत्थाय (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {
		वस्तूनि[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

अन्तःस्थ शून्य DatamanCache::changeUpdateIndex()
{
	सचे (अङ्कः_वस्तूनि_2 == 0) {
		_update_index = 0;
		_item_counter = 0;
		पटिनिवत्त;
	}

	_update_index = (_update_index + 1) % अङ्कः_वस्तूनि_2;

	सचे (_item_counter > 0) {
		--_item_counter;
	}
}
