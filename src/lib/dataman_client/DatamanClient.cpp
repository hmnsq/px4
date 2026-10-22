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

#অন্তর্ভাব <dataman_client/DatamanClient.hpp>

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

		যৌক্তিক success = syncHandler(request, response, timestamp, 1000_ms);

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

যৌক্তিক DatamanClient::syncHandler(ধ্রুবক dataman_request_s &request, dataman_response_s &response,
				ধ্রুবক hrt_abstime &start_time, hrt_abstime timeout)
{
	যৌক্তিক response_received = মিথ্যা;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	যখন (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		যদি (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			বিরতি;

		} অন্যথা যদি (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} অন্যথা {

			যৌক্তিক updated = মিথ্যা;
			orb_check(_dataman_response_sub, &updated);

			যদি (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				যদি (response.client_id == request.client_id) {

					যদি ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.সূচক == request.সূচক)) {
						response_received = সত্য;
						বিরতি;
					}

				} অন্যথা যদি (request.client_id == CLIENT_ID_NOT_SET) {

					// validate timestamp from response.data
					যদি (0 == memcmp(&(request.timestamp), &(response.তথ্য), আকার(hrt_abstime))) {
						response_received = সত্য;
						বিরতি;
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

	ফেরত response_received;
}

যৌক্তিক DatamanClient::readSync(dm_item_t item, uint32_t সূচক, uint8_t *বাফার_2, uint32_t দৈর্ঘ্য, hrt_abstime timeout)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ফেরত মিথ্যা;
	}

	যদি (দৈর্ঘ্য > g_per_item_আকার[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, দৈর্ঘ্য, static_cast<uint8_t>(item));
		ফেরত মিথ্যা;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.সূচক = সূচক;
	request.তথ্য_দৈর্ঘ্য = দৈর্ঘ্য;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	যৌক্তিক success = syncHandler(request, response, timestamp, timeout);

	যদি (success) {

		যদি (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = মিথ্যা;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), সূচক, দৈর্ঘ্য);

		} অন্যথা {
			memcpy(বাফার_2, response.তথ্য, দৈর্ঘ্য);
		}
	}

	ফেরত success;
}

যৌক্তিক DatamanClient::writeSync(dm_item_t item, uint32_t সূচক, uint8_t *বাফার_2, uint32_t দৈর্ঘ্য, hrt_abstime timeout)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ফেরত মিথ্যা;
	}

	যদি (দৈর্ঘ্য > g_per_item_আকার[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, দৈর্ঘ্য, static_cast<uint8_t>(item));
		ফেরত মিথ্যা;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.সূচক = সূচক;
	request.তথ্য_দৈর্ঘ্য = দৈর্ঘ্য;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.item = static_cast<uint8_t>(item);

	memcpy(request.তথ্য, বাফার_2, দৈর্ঘ্য);

	dataman_response_s response{};
	যৌক্তিক success = syncHandler(request, response, timestamp, timeout);

	যদি (success) {

		যদি (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = মিথ্যা;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.status, static_cast<uint8_t>(item), সূচক, দৈর্ঘ্য);
		}
	}

	ফেরত success;
}

যৌক্তিক DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ফেরত মিথ্যা;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	যৌক্তিক success = syncHandler(request, response, timestamp, timeout);

	যদি (success) {

		যদি (response.status != dataman_response_s::STATUS_SUCCESS) {

			success = মিথ্যা;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.status, static_cast<uint8_t>(item));
		}
	}

	ফেরত success;
}

শূন্য DatamanClient::clearPendingResponse()
{
	যদি (_dataman_response_sub < 0) {
		ফেরত;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	যৌক্তিক updated = মিথ্যা;
	orb_check(_dataman_response_sub, &updated);

	যখন (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

যৌক্তিক DatamanClient::readAsync(dm_item_t item, uint32_t সূচক, uint8_t *বাফার_2, uint32_t দৈর্ঘ্য)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ফেরত মিথ্যা;
	}

	যদি (দৈর্ঘ্য > g_per_item_আকার[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, দৈর্ঘ্য, static_cast<uint8_t>(item));
		ফেরত মিথ্যা;
	}

	যৌক্তিক success = মিথ্যা;

	যদি (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.সূচক = সূচক;
		request.তথ্য_দৈর্ঘ্য = দৈর্ঘ্য;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.item = static_cast<uint8_t>(item);

		_active_request.timestamp = timestamp;
		_active_request.request_type = DM_READ;
		_active_request.item = item;
		_active_request.সূচক = সূচক;
		_active_request.বাফার_2 = বাফার_2;
		_active_request.দৈর্ঘ্য = দৈর্ঘ্য;

		_state = State::RequestSent;

		_dataman_request_pub.publish(request);

		success = সত্য;
	}

	ফেরত success;
}

যৌক্তিক DatamanClient::writeAsync(dm_item_t item, uint32_t সূচক, uint8_t *বাফার_2, uint32_t দৈর্ঘ্য)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ফেরত মিথ্যা;
	}

	যদি (দৈর্ঘ্য > g_per_item_আকার[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, দৈর্ঘ্য, static_cast<uint8_t>(item));
		ফেরত মিথ্যা;
	}

	যৌক্তিক success = মিথ্যা;

	যদি (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.সূচক = সূচক;
		request.তথ্য_দৈর্ঘ্য = দৈর্ঘ্য;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.item = static_cast<uint8_t>(item);

		memcpy(request.তথ্য, বাফার_2, দৈর্ঘ্য);

		_active_request.timestamp = timestamp;
		_active_request.request_type = DM_WRITE;
		_active_request.item = item;
		_active_request.সূচক = সূচক;
		_active_request.বাফার_2 = বাফার_2;
		_active_request.দৈর্ঘ্য = দৈর্ঘ্য;

		_state = State::RequestSent;

		_dataman_request_pub.publish(request);

		success = সত্য;
	}

	ফেরত success;
}

যৌক্তিক DatamanClient::clearAsync(dm_item_t item)
{
	যদি (_client_id == CLIENT_ID_NOT_SET) {
		ফেরত মিথ্যা;
	}

	যৌক্তিক success = মিথ্যা;

	যদি (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.item = static_cast<uint8_t>(item);
		request.সূচক = 0;

		_active_request.timestamp = timestamp;
		_active_request.request_type = DM_CLEAR;
		_active_request.item = item;
		_active_request.সূচক = request.সূচক;
		_state = State::RequestSent;

		_dataman_request_pub.publish(request);

		success = সত্য;
	}

	ফেরত success;
}

শূন্য DatamanClient::update()
{
	যদি (_state == State::RequestSent) {

		যৌক্তিক updated = মিথ্যা;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		যদি (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			যদি ((response.client_id == _client_id) &&
			    (response.request_type == _active_request.request_type) &&
			    (response.item == _active_request.item) &&
			    (response.সূচক == _active_request.সূচক)) {

				যদি (response.request_type == DM_READ) {
					memcpy(_active_request.বাফার_2, response.তথ্য, _active_request.দৈর্ঘ্য);
				}

				_response_status = response.status;

				যদি (_response_status != dataman_response_s::STATUS_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.status, static_cast<uint8_t>(_active_request.item), _active_request.সূচক);
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
				request.সূচক = _active_request.সূচক;
				request.তথ্য_দৈর্ঘ্য = _active_request.দৈর্ঘ্য;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(_active_request.request_type);
				request.item = static_cast<uint8_t>(_active_request.item);

				যদি (_active_request.request_type == DM_WRITE) {
					memcpy(request.তথ্য, _active_request.বাফার_2, _active_request.দৈর্ঘ্য);
				}

				_dataman_request_pub.publish(request);

				_state = State::RequestSent;
			}
		}
	}
}

যৌক্তিক DatamanClient::lastOperationCompleted(যৌক্তিক &success)
{
	যৌক্তিক completed = মিথ্যা;
	success = মিথ্যা;

	যদি (_state == State::ResponseReceived) {

		যদি (_response_status == dataman_response_s::STATUS_SUCCESS) {
			success = সত্য;
		}

		_state = State::Idle;
		completed = সত্য;
	}

	ফেরত completed;
}

শূন্য DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ধ্রুবক অক্ষর *cache_miss_perf_গণক_name, uint32_t অঙ্ক_বস্তুসমূহ)
	: _cache_miss_perf(perf_alloc(PC_গুনতি, cache_miss_perf_গণক_name))
{
	যদি (অঙ্ক_বস্তুসমূহ == 0) {
		ফেরত;
	}

	বস্তুসমূহ = নতুন Item[অঙ্ক_বস্তুসমূহ] {};

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
		ফেরত;
	}

	Item *new_বস্তুসমূহ = নতুন Item[অঙ্ক_বস্তুসমূহ] {};

	যদি (new_বস্তুসমূহ != nullptr) {
		_client.abortCurrentOperation();

		uint32_t অঙ্ক_min = অঙ্ক_বস্তুসমূহ < অঙ্ক_বস্তুসমূহ_2 ? অঙ্ক_বস্তুসমূহ : অঙ্ক_বস্তুসমূহ_2;

		জন্য (uint32_t i = 0; i < অঙ্ক_min; ++i) {
			new_বস্তুসমূহ[i] = বস্তুসমূহ[i];

			যদি (new_বস্তুসমূহ[i].cache_state == State::RequestSent) {
				new_বস্তুসমূহ[i].cache_state = State::RequestPrepared;
			}
		}

		delete[] বস্তুসমূহ;
		বস্তুসমূহ = new_বস্তুসমূহ;
		অঙ্ক_বস্তুসমূহ_2 = অঙ্ক_বস্তুসমূহ;

		// ensure new bounds are respected after resize
		load_সূচক = (load_সূচক < অঙ্ক_বস্তুসমূহ_2) ? load_সূচক : 0;
		update_সূচক = (update_সূচক < অঙ্ক_বস্তুসমূহ_2) ? update_সূচক : 0;
		item_গণক = (item_গণক < অঙ্ক_বস্তুসমূহ_2) ? item_গণক : অঙ্ক_বস্তুসমূহ_2;

	} অন্যথা {
		PX4_ERR("alloc failed");
	}
}

যৌক্তিক DatamanCache::load(dm_item_t item, uint32_t সূচক)
{
	যদি (!বস্তুসমূহ) {
		ফেরত মিথ্যা;
	}

	যৌক্তিক success = মিথ্যা;
	যৌক্তিক duplicate = মিথ্যা;

	//Prevent duplicates
	জন্য (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {

		যদি (বস্তুসমূহ[i].cache_state != State::Idle &&
		    বস্তুসমূহ[i].cache_state != State::Error &&
		    বস্তুসমূহ[i].response.item == item &&
		    বস্তুসমূহ[i].response.সূচক == সূচক) {
			duplicate = সত্য;
			বিরতি;
		}
	}

	যদি (!duplicate && (item_গণক < অঙ্ক_বস্তুসমূহ_2)) {

		বস্তুসমূহ[load_সূচক].cache_state = State::RequestPrepared;
		বস্তুসমূহ[load_সূচক].response.item = item;
		বস্তুসমূহ[load_সূচক].response.সূচক = সূচক;

		load_সূচক = (load_সূচক + 1) % অঙ্ক_বস্তুসমূহ_2;

		++item_গণক;

		success = সত্য;
	}

	ফেরত success;
}

যৌক্তিক DatamanCache::loadWait(dm_item_t item, uint32_t সূচক, uint8_t *বাফার_2, uint32_t দৈর্ঘ্য, hrt_abstime timeout)
{
	যদি (দৈর্ঘ্য > g_per_item_আকার[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, দৈর্ঘ্য, static_cast<uint8_t>(item));
		ফেরত মিথ্যা;
	}

	যৌক্তিক success = মিথ্যা;
	যৌক্তিক item_found = মিথ্যা;

	যদি (বস্তুসমূহ) {
		জন্য (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {
			যদি ((বস্তুসমূহ[i].response.item == item) &&
			    (বস্তুসমূহ[i].response.সূচক == সূচক)) {
				item_found = সত্য;

				যদি (বস্তুসমূহ[i].cache_state == State::ResponseReceived) {
					memcpy(বাফার_2, বস্তুসমূহ[i].response.তথ্য, দৈর্ঘ্য);
					success = সত্য;
					বিরতি;
				}
			}
		}
	}

	যদি (!success) {
		perf_গুনতি(_cache_miss_perf);
	}

	যদি (!success && (timeout > 0)) {
		success = _client.readSync(item, সূচক, বাফার_2, দৈর্ঘ্য, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		যদি (success && !item_found && বস্তুসমূহ && (item_গণক < অঙ্ক_বস্তুসমূহ_2)) {
			বস্তুসমূহ[load_সূচক].cache_state = State::ResponseReceived;
			বস্তুসমূহ[load_সূচক].response.item = item;
			বস্তুসমূহ[load_সূচক].response.সূচক = সূচক;
			memcpy(বস্তুসমূহ[load_সূচক].response.তথ্য, বাফার_2, দৈর্ঘ্য);

			load_সূচক = (load_সূচক + 1) % অঙ্ক_বস্তুসমূহ_2;

			++item_গণক; // Still increase the counter here
		}
	}

	ফেরত success;
}

যৌক্তিক DatamanCache::writeWait(dm_item_t item, uint32_t সূচক, uint8_t *বাফার_2, uint32_t দৈর্ঘ্য, hrt_abstime timeout)
{
	যদি (দৈর্ঘ্য > g_per_item_আকার[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, দৈর্ঘ্য, static_cast<uint8_t>(item));
		ফেরত মিথ্যা;
	}

	যৌক্তিক success = _client.writeSync(item, সূচক, বাফার_2, দৈর্ঘ্য, timeout);

	যদি (success && বস্তুসমূহ) {
		জন্য (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {
			যদি ((বস্তুসমূহ[i].response.item == item) &&
			    (বস্তুসমূহ[i].response.সূচক == সূচক) &&
			    ((বস্তুসমূহ[i].cache_state == State::ResponseReceived) ||
			     (বস্তুসমূহ[i].cache_state == State::RequestPrepared))) {

				memcpy(বস্তুসমূহ[i].response.তথ্য, বাফার_2, দৈর্ঘ্য);
				বস্তুসমূহ[i].cache_state = State::ResponseReceived;
				বিরতি;
			}
		}
	}

	ফেরত success;
}

যৌক্তিক DatamanCache::updateCachedItem(dm_item_t item, uint32_t সূচক, ধ্রুবক uint8_t *বাফার_2, uint32_t দৈর্ঘ্য)
{
	যদি (!বস্তুসমূহ || বাফার_2 == nullptr || অঙ্ক_বস্তুসমূহ_2 == 0) {
		ফেরত মিথ্যা;
	}

	যদি (দৈর্ঘ্য > g_per_item_আকার[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, দৈর্ঘ্য, static_cast<uint8_t>(item));
		ফেরত মিথ্যা;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	জন্য (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {
		যদি ((বস্তুসমূহ[i].response.item == item) && (বস্তুসমূহ[i].response.সূচক == সূচক)
		    && (বস্তুসমূহ[i].cache_state == State::ResponseReceived)) {
			memcpy(বস্তুসমূহ[i].response.তথ্য, বাফার_2, দৈর্ঘ্য);
			ফেরত সত্য;
		}
	}

	ফেরত মিথ্যা;
}

শূন্য DatamanCache::update()
{
	যদি (item_গণক > 0) {

		_client.update();

		যৌক্তিক success = মিথ্যা;
		যৌক্তিক response_success = মিথ্যা;

		নির্বাচন (বস্তুসমূহ[update_সূচক].cache_state) {
		ক্ষেত্র State::Idle:
			বিরতি;

		ক্ষেত্র State::ResponseReceived:
			// Skip it
			change_Update_সূচক();
			বিরতি;

		ক্ষেত্র State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(বস্তুসমূহ[update_সূচক].response.item),
						    বস্তুসমূহ[update_সূচক].response.সূচক,
						    বস্তুসমূহ[update_সূচক].response.তথ্য,
						    g_per_item_আকার[বস্তুসমূহ[update_সূচক].response.item]);

			যদি (success) {
				বস্তুসমূহ[update_সূচক].cache_state = State::RequestSent;

			} অন্যথা {
				বস্তুসমূহ[update_সূচক].cache_state = State::Error;
			}

			বিরতি;

		ক্ষেত্র State::RequestSent:

			যদি (_client.lastOperationCompleted(response_success)) {

				যদি (response_success) {

					বস্তুসমূহ[update_সূচক].cache_state = State::ResponseReceived;
					change_Update_সূচক();

				} অন্যথা {
					বস্তুসমূহ[update_সূচক].cache_state = State::Error;
				}
			}

			বিরতি;

		ক্ষেত্র State::Error:
			// Handled below
			বিরতি;
		}

		যদি (বস্তুসমূহ[update_সূচক].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				বস্তুসমূহ[update_সূচক].response.item, বস্তুসমূহ[update_সূচক].response.সূচক,
				বস্তুসমূহ[update_সূচক].response.status);

			change_Update_সূচক();
		}

	}
}

শূন্য DatamanCache::resetCacheState()
{
	update_সূচক = 0;
	item_গণক = 0;
	load_সূচক = 0;
}

শূন্য DatamanCache::invalidate()
{
	জন্য (uint32_t i = 0; i < অঙ্ক_বস্তুসমূহ_2; ++i) {
		বস্তুসমূহ[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

অন্তঃরেখ শূন্য DatamanCache::change_Update_সূচক()
{
	যদি (অঙ্ক_বস্তুসমূহ_2 == 0) {
		update_সূচক = 0;
		item_গণক = 0;
		ফেরত;
	}

	update_সূচক = (update_সূচক + 1) % অঙ্ক_বস্তুসমূহ_2;

	যদি (item_গণক > 0) {
		--item_গণক;
	}
}
