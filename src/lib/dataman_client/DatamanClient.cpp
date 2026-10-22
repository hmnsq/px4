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

		hrt_abstime timestamp = hrt_absolute_زمان();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_مجموعه;

		منطقي success = syncHandler(request, response, timestamp, 1000_ms);

		إذا (success && (response.client_id > CLIENT_ID_NOT_مجموعه)) {

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
				ثابت hrt_abstime &شروع_زمان, hrt_abstime timeout)
{
	منطقي response_received = خطأ;
	int32_t ret = 0;
	hrt_abstime زمان_elapsed = hrt_elapsed_زمان(&شروع_زمان);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	طالما (!response_received && (زمان_elapsed < timeout)) {

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
					    (response.قلم == request.قلم) &&
					    (response.شاخص == request.شاخص)) {
						response_received = درست;
						اكسر;
					}

				} وإلا إذا (request.client_id == CLIENT_ID_NOT_مجموعه) {

					// validate timestamp from response.data
					إذا (0 == memcmp(&(request.timestamp), &(response.داده), حجم(hrt_abstime))) {
						response_received = درست;
						اكسر;
					}
				}
			}
		}

		زمان_elapsed = hrt_elapsed_زمان(&شروع_زمان);
	}

	perf_انتها(_sync_perf);

	إذا (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	أرجع response_received;
}

منطقي DatamanClient::readSync(dm_قلم_t قلم, uint32_t شاخص, uint8_t *میانگیر_2, uint32_t درازی, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_مجموعه) {
		أرجع خطأ;
	}

	إذا (درازی > g_در_هر_قلم_اندازه[قلم]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, درازی, static_cast<uint8_t>(قلم));
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_زمان();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.شاخص = شاخص;
	request.داده_درازی = درازی;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.قلم = static_cast<uint8_t>(قلم);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.وضعیت != dataman_response_s::وضعیت_SUCCESS) {

			success = خطأ;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.وضعیت, static_cast<uint8_t>(قلم), شاخص, درازی);

		} وإلا {
			کپی_حافظه(میانگیر_2, response.داده, درازی);
		}
	}

	أرجع success;
}

منطقي DatamanClient::writeSync(dm_قلم_t قلم, uint32_t شاخص, uint8_t *میانگیر_2, uint32_t درازی, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_مجموعه) {
		أرجع خطأ;
	}

	إذا (درازی > g_در_هر_قلم_اندازه[قلم]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, درازی, static_cast<uint8_t>(قلم));
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_زمان();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.شاخص = شاخص;
	request.داده_درازی = درازی;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.قلم = static_cast<uint8_t>(قلم);

	کپی_حافظه(request.داده, میانگیر_2, درازی);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.وضعیت != dataman_response_s::وضعیت_SUCCESS) {

			success = خطأ;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.وضعیت, static_cast<uint8_t>(قلم), شاخص, درازی);
		}
	}

	أرجع success;
}

منطقي DatamanClient::clearSync(dm_قلم_t قلم, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_مجموعه) {
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_زمان();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.قلم = static_cast<uint8_t>(قلم);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.وضعیت != dataman_response_s::وضعیت_SUCCESS) {

			success = خطأ;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.وضعیت, static_cast<uint8_t>(قلم));
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

منطقي DatamanClient::readAsync(dm_قلم_t قلم, uint32_t شاخص, uint8_t *میانگیر_2, uint32_t درازی)
{
	إذا (_client_id == CLIENT_ID_NOT_مجموعه) {
		أرجع خطأ;
	}

	إذا (درازی > g_در_هر_قلم_اندازه[قلم]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, درازی, static_cast<uint8_t>(قلم));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_زمان();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.شاخص = شاخص;
		request.داده_درازی = درازی;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.قلم = static_cast<uint8_t>(قلم);

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_READ;
		فعال_request.قلم = قلم;
		فعال_request.شاخص = شاخص;
		فعال_request.میانگیر_2 = میانگیر_2;
		فعال_request.درازی = درازی;

		_state = State::Request_فرستاده;

		_dataman_request_pub.publish(request);

		success = درست;
	}

	أرجع success;
}

منطقي DatamanClient::writeAsync(dm_قلم_t قلم, uint32_t شاخص, uint8_t *میانگیر_2, uint32_t درازی)
{
	إذا (_client_id == CLIENT_ID_NOT_مجموعه) {
		أرجع خطأ;
	}

	إذا (درازی > g_در_هر_قلم_اندازه[قلم]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, درازی, static_cast<uint8_t>(قلم));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_زمان();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.شاخص = شاخص;
		request.داده_درازی = درازی;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.قلم = static_cast<uint8_t>(قلم);

		کپی_حافظه(request.داده, میانگیر_2, درازی);

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_WRITE;
		فعال_request.قلم = قلم;
		فعال_request.شاخص = شاخص;
		فعال_request.میانگیر_2 = میانگیر_2;
		فعال_request.درازی = درازی;

		_state = State::Request_فرستاده;

		_dataman_request_pub.publish(request);

		success = درست;
	}

	أرجع success;
}

منطقي DatamanClient::clearAsync(dm_قلم_t قلم)
{
	إذا (_client_id == CLIENT_ID_NOT_مجموعه) {
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_زمان();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.قلم = static_cast<uint8_t>(قلم);
		request.شاخص = 0;

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_CLEAR;
		فعال_request.قلم = قلم;
		فعال_request.شاخص = request.شاخص;
		_state = State::Request_فرستاده;

		_dataman_request_pub.publish(request);

		success = درست;
	}

	أرجع success;
}

فراغ DatamanClient::update()
{
	إذا (_state == State::Request_فرستاده) {

		منطقي updated = خطأ;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		إذا (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			إذا ((response.client_id == _client_id) &&
			    (response.request_type == فعال_request.request_type) &&
			    (response.قلم == فعال_request.قلم) &&
			    (response.شاخص == فعال_request.شاخص)) {

				إذا (response.request_type == DM_READ) {
					کپی_حافظه(فعال_request.میانگیر_2, response.داده, فعال_request.درازی);
				}

				response_وضعیت = response.وضعیت;

				إذا (response_وضعیت != dataman_response_s::وضعیت_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.وضعیت, static_cast<uint8_t>(فعال_request.قلم), فعال_request.شاخص);
				}

				_state = State::ResponseReceived;
			}
		}

		إذا (_state == State::Request_فرستاده) {

			/* Retry the request if there is no answer */
			إذا (((فعال_request.request_type != DM_CLEAR) && (hrt_elapsed_زمان(&فعال_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_زمان(&فعال_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_زمان();

				فعال_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.شاخص = فعال_request.شاخص;
				request.داده_درازی = فعال_request.درازی;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(فعال_request.request_type);
				request.قلم = static_cast<uint8_t>(فعال_request.قلم);

				إذا (فعال_request.request_type == DM_WRITE) {
					کپی_حافظه(request.داده, فعال_request.میانگیر_2, فعال_request.درازی);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_فرستاده;
			}
		}
	}
}

منطقي DatamanClient::آخرین_Operation_Completed(منطقي &success)
{
	منطقي completed = خطأ;
	success = خطأ;

	إذا (_state == State::ResponseReceived) {

		إذا (response_وضعیت == dataman_response_s::وضعیت_SUCCESS) {
			success = درست;
		}

		_state = State::Idle;
		completed = درست;
	}

	أرجع completed;
}

فراغ DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ثابت حرف *cache_نیافتن_perf_counter_name, uint32_t رقم_اقلام)
	: cache_نیافتن_perf(perf_alloc(PC_شمار, cache_نیافتن_perf_counter_name))
{
	إذا (رقم_اقلام == 0) {
		أرجع;
	}

	اقلام = جديد قلم_2[رقم_اقلام] {};

	إذا (اقلام != اشاره_تهی) {
		رقم_اقلام_2 = رقم_اقلام;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	حذف[] اقلام;
	perf_free(cache_نیافتن_perf);
}

فراغ DatamanCache::resize(uint32_t رقم_اقلام)
{
	إذا (رقم_اقلام == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		حذف[] اقلام;
		اقلام = اشاره_تهی;
		resetCacheState();
		رقم_اقلام_2 = 0;
		أرجع;
	}

	قلم_2 *new_اقلام = جديد قلم_2[رقم_اقلام] {};

	إذا (new_اقلام != اشاره_تهی) {
		_client.abortCurrentOperation();

		uint32_t رقم_min = رقم_اقلام < رقم_اقلام_2 ? رقم_اقلام : رقم_اقلام_2;

		لكل (uint32_t i = 0; i < رقم_min; ++i) {
			new_اقلام[i] = اقلام[i];

			إذا (new_اقلام[i].cache_state == State::Request_فرستاده) {
				new_اقلام[i].cache_state = State::RequestPrepared;
			}
		}

		حذف[] اقلام;
		اقلام = new_اقلام;
		رقم_اقلام_2 = رقم_اقلام;

		// ensure new bounds are respected after resize
		بارگیری_شاخص = (بارگیری_شاخص < رقم_اقلام_2) ? بارگیری_شاخص : 0;
		update_شاخص = (update_شاخص < رقم_اقلام_2) ? update_شاخص : 0;
		قلم_counter = (قلم_counter < رقم_اقلام_2) ? قلم_counter : رقم_اقلام_2;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

منطقي DatamanCache::بارگیری(dm_قلم_t قلم, uint32_t شاخص)
{
	إذا (!اقلام) {
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي duplicate = خطأ;

	//Prevent duplicates
	لكل (uint32_t i = 0; i < رقم_اقلام_2; ++i) {

		إذا (اقلام[i].cache_state != State::Idle &&
		    اقلام[i].cache_state != State::Error &&
		    اقلام[i].response.قلم == قلم &&
		    اقلام[i].response.شاخص == شاخص) {
			duplicate = درست;
			اكسر;
		}
	}

	إذا (!duplicate && (قلم_counter < رقم_اقلام_2)) {

		اقلام[بارگیری_شاخص].cache_state = State::RequestPrepared;
		اقلام[بارگیری_شاخص].response.قلم = قلم;
		اقلام[بارگیری_شاخص].response.شاخص = شاخص;

		بارگیری_شاخص = (بارگیری_شاخص + 1) % رقم_اقلام_2;

		++قلم_counter;

		success = درست;
	}

	أرجع success;
}

منطقي DatamanCache::بارگیری_Wait(dm_قلم_t قلم, uint32_t شاخص, uint8_t *میانگیر_2, uint32_t درازی, hrt_abstime timeout)
{
	إذا (درازی > g_در_هر_قلم_اندازه[قلم]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, درازی, static_cast<uint8_t>(قلم));
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي قلم_یافت = خطأ;

	إذا (اقلام) {
		لكل (uint32_t i = 0; i < رقم_اقلام_2; ++i) {
			إذا ((اقلام[i].response.قلم == قلم) &&
			    (اقلام[i].response.شاخص == شاخص)) {
				قلم_یافت = درست;

				إذا (اقلام[i].cache_state == State::ResponseReceived) {
					کپی_حافظه(میانگیر_2, اقلام[i].response.داده, درازی);
					success = درست;
					اكسر;
				}
			}
		}
	}

	إذا (!success) {
		perf_شمار(cache_نیافتن_perf);
	}

	إذا (!success && (timeout > 0)) {
		success = _client.readSync(قلم, شاخص, میانگیر_2, درازی, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		إذا (success && !قلم_یافت && اقلام && (قلم_counter < رقم_اقلام_2)) {
			اقلام[بارگیری_شاخص].cache_state = State::ResponseReceived;
			اقلام[بارگیری_شاخص].response.قلم = قلم;
			اقلام[بارگیری_شاخص].response.شاخص = شاخص;
			کپی_حافظه(اقلام[بارگیری_شاخص].response.داده, میانگیر_2, درازی);

			بارگیری_شاخص = (بارگیری_شاخص + 1) % رقم_اقلام_2;

			++قلم_counter; // Still increase the counter here
		}
	}

	أرجع success;
}

منطقي DatamanCache::writeWait(dm_قلم_t قلم, uint32_t شاخص, uint8_t *میانگیر_2, uint32_t درازی, hrt_abstime timeout)
{
	إذا (درازی > g_در_هر_قلم_اندازه[قلم]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, درازی, static_cast<uint8_t>(قلم));
		أرجع خطأ;
	}

	منطقي success = _client.writeSync(قلم, شاخص, میانگیر_2, درازی, timeout);

	إذا (success && اقلام) {
		لكل (uint32_t i = 0; i < رقم_اقلام_2; ++i) {
			إذا ((اقلام[i].response.قلم == قلم) &&
			    (اقلام[i].response.شاخص == شاخص) &&
			    ((اقلام[i].cache_state == State::ResponseReceived) ||
			     (اقلام[i].cache_state == State::RequestPrepared))) {

				کپی_حافظه(اقلام[i].response.داده, میانگیر_2, درازی);
				اقلام[i].cache_state = State::ResponseReceived;
				اكسر;
			}
		}
	}

	أرجع success;
}

منطقي DatamanCache::update_Cached_قلم(dm_قلم_t قلم, uint32_t شاخص, ثابت uint8_t *میانگیر_2, uint32_t درازی)
{
	إذا (!اقلام || میانگیر_2 == اشاره_تهی || رقم_اقلام_2 == 0) {
		أرجع خطأ;
	}

	إذا (درازی > g_در_هر_قلم_اندازه[قلم]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, درازی, static_cast<uint8_t>(قلم));
		أرجع خطأ;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	لكل (uint32_t i = 0; i < رقم_اقلام_2; ++i) {
		إذا ((اقلام[i].response.قلم == قلم) && (اقلام[i].response.شاخص == شاخص)
		    && (اقلام[i].cache_state == State::ResponseReceived)) {
			کپی_حافظه(اقلام[i].response.داده, میانگیر_2, درازی);
			أرجع درست;
		}
	}

	أرجع خطأ;
}

فراغ DatamanCache::update()
{
	إذا (قلم_counter > 0) {

		_client.update();

		منطقي success = خطأ;
		منطقي response_success = خطأ;

		بدل (اقلام[update_شاخص].cache_state) {
		حالة State::Idle:
			اكسر;

		حالة State::ResponseReceived:
			// Skip it
			change_Update_شاخص();
			اكسر;

		حالة State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_قلم_t>(اقلام[update_شاخص].response.قلم),
						    اقلام[update_شاخص].response.شاخص,
						    اقلام[update_شاخص].response.داده,
						    g_در_هر_قلم_اندازه[اقلام[update_شاخص].response.قلم]);

			إذا (success) {
				اقلام[update_شاخص].cache_state = State::Request_فرستاده;

			} وإلا {
				اقلام[update_شاخص].cache_state = State::Error;
			}

			اكسر;

		حالة State::Request_فرستاده:

			إذا (_client.آخرین_Operation_Completed(response_success)) {

				إذا (response_success) {

					اقلام[update_شاخص].cache_state = State::ResponseReceived;
					change_Update_شاخص();

				} وإلا {
					اقلام[update_شاخص].cache_state = State::Error;
				}
			}

			اكسر;

		حالة State::Error:
			// Handled below
			اكسر;
		}

		إذا (اقلام[update_شاخص].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				اقلام[update_شاخص].response.قلم, اقلام[update_شاخص].response.شاخص,
				اقلام[update_شاخص].response.وضعیت);

			change_Update_شاخص();
		}

	}
}

فراغ DatamanCache::resetCacheState()
{
	update_شاخص = 0;
	قلم_counter = 0;
	بارگیری_شاخص = 0;
}

فراغ DatamanCache::invalidate()
{
	لكل (uint32_t i = 0; i < رقم_اقلام_2; ++i) {
		اقلام[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

مضمن فراغ DatamanCache::change_Update_شاخص()
{
	إذا (رقم_اقلام_2 == 0) {
		update_شاخص = 0;
		قلم_counter = 0;
		أرجع;
	}

	update_شاخص = (update_شاخص + 1) % رقم_اقلام_2;

	إذا (قلم_counter > 0) {
		--قلم_counter;
	}
}
