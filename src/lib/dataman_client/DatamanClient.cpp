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

		hrt_abstime timestamp = hrt_absolute_ویلا();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_سیٹ;

		منطقي success = syncHandler(request, response, timestamp, 1000_ms);

		إذا (success && (response.client_id > CLIENT_ID_NOT_سیٹ)) {

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
				ثابت hrt_abstime &شروع_ویلا, hrt_abstime timeout)
{
	منطقي response_received = خطأ;
	int32_t ret = 0;
	hrt_abstime ویلا_elapsed = hrt_elapsed_ویلا(&شروع_ویلا);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	طالما (!response_received && (ویلا_elapsed < timeout)) {

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
					    (response.شے == request.شے) &&
					    (response.اشاریہ == request.اشاریہ)) {
						response_received = سچ;
						اكسر;
					}

				} وإلا إذا (request.client_id == CLIENT_ID_NOT_سیٹ) {

					// validate timestamp from response.data
					إذا (0 == memcmp(&(request.timestamp), &(response.انکڑے), حجم(hrt_abstime))) {
						response_received = سچ;
						اكسر;
					}
				}
			}
		}

		ویلا_elapsed = hrt_elapsed_ویلا(&شروع_ویلا);
	}

	perf_سرا(_sync_perf);

	إذا (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	أرجع response_received;
}

منطقي DatamanClient::readSync(dm_شے_t شے, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمائی, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سیٹ) {
		أرجع خطأ;
	}

	إذا (لمائی > g_فی_شے_ناپ[شے]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمائی, static_cast<uint8_t>(شے));
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_ویلا();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.اشاریہ = اشاریہ;
	request.انکڑے_لمائی = لمائی;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.شے = static_cast<uint8_t>(شے);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.حالت, static_cast<uint8_t>(شے), اشاریہ, لمائی);

		} وإلا {
			حافظہ_نقل(بفر_2, response.انکڑے, لمائی);
		}
	}

	أرجع success;
}

منطقي DatamanClient::writeSync(dm_شے_t شے, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمائی, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سیٹ) {
		أرجع خطأ;
	}

	إذا (لمائی > g_فی_شے_ناپ[شے]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمائی, static_cast<uint8_t>(شے));
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_ویلا();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.اشاریہ = اشاریہ;
	request.انکڑے_لمائی = لمائی;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.شے = static_cast<uint8_t>(شے);

	حافظہ_نقل(request.انکڑے, بفر_2, لمائی);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.حالت, static_cast<uint8_t>(شے), اشاریہ, لمائی);
		}
	}

	أرجع success;
}

منطقي DatamanClient::clearSync(dm_شے_t شے, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سیٹ) {
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_ویلا();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.شے = static_cast<uint8_t>(شے);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.حالت, static_cast<uint8_t>(شے));
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

منطقي DatamanClient::readAsync(dm_شے_t شے, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمائی)
{
	إذا (_client_id == CLIENT_ID_NOT_سیٹ) {
		أرجع خطأ;
	}

	إذا (لمائی > g_فی_شے_ناپ[شے]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمائی, static_cast<uint8_t>(شے));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ویلا();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.اشاریہ = اشاریہ;
		request.انکڑے_لمائی = لمائی;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.شے = static_cast<uint8_t>(شے);

		سرگرم_request.timestamp = timestamp;
		سرگرم_request.request_type = DM_READ;
		سرگرم_request.شے = شے;
		سرگرم_request.اشاریہ = اشاریہ;
		سرگرم_request.بفر_2 = بفر_2;
		سرگرم_request.لمائی = لمائی;

		_state = State::Request_گھلیا;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	أرجع success;
}

منطقي DatamanClient::writeAsync(dm_شے_t شے, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمائی)
{
	إذا (_client_id == CLIENT_ID_NOT_سیٹ) {
		أرجع خطأ;
	}

	إذا (لمائی > g_فی_شے_ناپ[شے]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمائی, static_cast<uint8_t>(شے));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ویلا();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.اشاریہ = اشاریہ;
		request.انکڑے_لمائی = لمائی;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.شے = static_cast<uint8_t>(شے);

		حافظہ_نقل(request.انکڑے, بفر_2, لمائی);

		سرگرم_request.timestamp = timestamp;
		سرگرم_request.request_type = DM_WRITE;
		سرگرم_request.شے = شے;
		سرگرم_request.اشاریہ = اشاریہ;
		سرگرم_request.بفر_2 = بفر_2;
		سرگرم_request.لمائی = لمائی;

		_state = State::Request_گھلیا;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	أرجع success;
}

منطقي DatamanClient::clearAsync(dm_شے_t شے)
{
	إذا (_client_id == CLIENT_ID_NOT_سیٹ) {
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ویلا();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.شے = static_cast<uint8_t>(شے);
		request.اشاریہ = 0;

		سرگرم_request.timestamp = timestamp;
		سرگرم_request.request_type = DM_CLEAR;
		سرگرم_request.شے = شے;
		سرگرم_request.اشاریہ = request.اشاریہ;
		_state = State::Request_گھلیا;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	أرجع success;
}

فراغ DatamanClient::update()
{
	إذا (_state == State::Request_گھلیا) {

		منطقي updated = خطأ;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		إذا (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			إذا ((response.client_id == _client_id) &&
			    (response.request_type == سرگرم_request.request_type) &&
			    (response.شے == سرگرم_request.شے) &&
			    (response.اشاریہ == سرگرم_request.اشاریہ)) {

				إذا (response.request_type == DM_READ) {
					حافظہ_نقل(سرگرم_request.بفر_2, response.انکڑے, سرگرم_request.لمائی);
				}

				response_حالت = response.حالت;

				إذا (response_حالت != dataman_response_s::حالت_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.حالت, static_cast<uint8_t>(سرگرم_request.شے), سرگرم_request.اشاریہ);
				}

				_state = State::ResponseReceived;
			}
		}

		إذا (_state == State::Request_گھلیا) {

			/* Retry the request if there is no answer */
			إذا (((سرگرم_request.request_type != DM_CLEAR) && (hrt_elapsed_ویلا(&سرگرم_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_ویلا(&سرگرم_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_ویلا();

				سرگرم_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.اشاریہ = سرگرم_request.اشاریہ;
				request.انکڑے_لمائی = سرگرم_request.لمائی;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(سرگرم_request.request_type);
				request.شے = static_cast<uint8_t>(سرگرم_request.شے);

				إذا (سرگرم_request.request_type == DM_WRITE) {
					حافظہ_نقل(request.انکڑے, سرگرم_request.بفر_2, سرگرم_request.لمائی);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_گھلیا;
			}
		}
	}
}

منطقي DatamanClient::آخری_Operation_Completed(منطقي &success)
{
	منطقي completed = خطأ;
	success = خطأ;

	إذا (_state == State::ResponseReceived) {

		إذا (response_حالت == dataman_response_s::حالت_SUCCESS) {
			success = سچ;
		}

		_state = State::Idle;
		completed = سچ;
	}

	أرجع completed;
}

فراغ DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ثابت حرف *cache_کھنج_perf_counter_name, uint32_t عدد_اشیاء)
	: cache_کھنج_perf(perf_alloc(PC_گنتی, cache_کھنج_perf_counter_name))
{
	إذا (عدد_اشیاء == 0) {
		أرجع;
	}

	اشیاء = جديد شے_2[عدد_اشیاء] {};

	إذا (اشیاء != خالی_سوچک) {
		عدد_اشیاء_2 = عدد_اشیاء;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	مٹاؤ[] اشیاء;
	perf_free(cache_کھنج_perf);
}

فراغ DatamanCache::resize(uint32_t عدد_اشیاء)
{
	إذا (عدد_اشیاء == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		مٹاؤ[] اشیاء;
		اشیاء = خالی_سوچک;
		resetCacheState();
		عدد_اشیاء_2 = 0;
		أرجع;
	}

	شے_2 *new_اشیاء = جديد شے_2[عدد_اشیاء] {};

	إذا (new_اشیاء != خالی_سوچک) {
		_client.abortCurrentOperation();

		uint32_t عدد_min = عدد_اشیاء < عدد_اشیاء_2 ? عدد_اشیاء : عدد_اشیاء_2;

		لكل (uint32_t i = 0; i < عدد_min; ++i) {
			new_اشیاء[i] = اشیاء[i];

			إذا (new_اشیاء[i].cache_state == State::Request_گھلیا) {
				new_اشیاء[i].cache_state = State::RequestPrepared;
			}
		}

		مٹاؤ[] اشیاء;
		اشیاء = new_اشیاء;
		عدد_اشیاء_2 = عدد_اشیاء;

		// ensure new bounds are respected after resize
		لدو_اشاریہ = (لدو_اشاریہ < عدد_اشیاء_2) ? لدو_اشاریہ : 0;
		update_اشاریہ = (update_اشاریہ < عدد_اشیاء_2) ? update_اشاریہ : 0;
		شے_counter = (شے_counter < عدد_اشیاء_2) ? شے_counter : عدد_اشیاء_2;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

منطقي DatamanCache::لدو(dm_شے_t شے, uint32_t اشاریہ)
{
	إذا (!اشیاء) {
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي duplicate = خطأ;

	//Prevent duplicates
	لكل (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {

		إذا (اشیاء[i].cache_state != State::Idle &&
		    اشیاء[i].cache_state != State::Error &&
		    اشیاء[i].response.شے == شے &&
		    اشیاء[i].response.اشاریہ == اشاریہ) {
			duplicate = سچ;
			اكسر;
		}
	}

	إذا (!duplicate && (شے_counter < عدد_اشیاء_2)) {

		اشیاء[لدو_اشاریہ].cache_state = State::RequestPrepared;
		اشیاء[لدو_اشاریہ].response.شے = شے;
		اشیاء[لدو_اشاریہ].response.اشاریہ = اشاریہ;

		لدو_اشاریہ = (لدو_اشاریہ + 1) % عدد_اشیاء_2;

		++شے_counter;

		success = سچ;
	}

	أرجع success;
}

منطقي DatamanCache::لدو_Wait(dm_شے_t شے, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمائی, hrt_abstime timeout)
{
	إذا (لمائی > g_فی_شے_ناپ[شے]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, لمائی, static_cast<uint8_t>(شے));
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي شے_لبھیا = خطأ;

	إذا (اشیاء) {
		لكل (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {
			إذا ((اشیاء[i].response.شے == شے) &&
			    (اشیاء[i].response.اشاریہ == اشاریہ)) {
				شے_لبھیا = سچ;

				إذا (اشیاء[i].cache_state == State::ResponseReceived) {
					حافظہ_نقل(بفر_2, اشیاء[i].response.انکڑے, لمائی);
					success = سچ;
					اكسر;
				}
			}
		}
	}

	إذا (!success) {
		perf_گنتی(cache_کھنج_perf);
	}

	إذا (!success && (timeout > 0)) {
		success = _client.readSync(شے, اشاریہ, بفر_2, لمائی, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		إذا (success && !شے_لبھیا && اشیاء && (شے_counter < عدد_اشیاء_2)) {
			اشیاء[لدو_اشاریہ].cache_state = State::ResponseReceived;
			اشیاء[لدو_اشاریہ].response.شے = شے;
			اشیاء[لدو_اشاریہ].response.اشاریہ = اشاریہ;
			حافظہ_نقل(اشیاء[لدو_اشاریہ].response.انکڑے, بفر_2, لمائی);

			لدو_اشاریہ = (لدو_اشاریہ + 1) % عدد_اشیاء_2;

			++شے_counter; // Still increase the counter here
		}
	}

	أرجع success;
}

منطقي DatamanCache::writeWait(dm_شے_t شے, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمائی, hrt_abstime timeout)
{
	إذا (لمائی > g_فی_شے_ناپ[شے]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمائی, static_cast<uint8_t>(شے));
		أرجع خطأ;
	}

	منطقي success = _client.writeSync(شے, اشاریہ, بفر_2, لمائی, timeout);

	إذا (success && اشیاء) {
		لكل (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {
			إذا ((اشیاء[i].response.شے == شے) &&
			    (اشیاء[i].response.اشاریہ == اشاریہ) &&
			    ((اشیاء[i].cache_state == State::ResponseReceived) ||
			     (اشیاء[i].cache_state == State::RequestPrepared))) {

				حافظہ_نقل(اشیاء[i].response.انکڑے, بفر_2, لمائی);
				اشیاء[i].cache_state = State::ResponseReceived;
				اكسر;
			}
		}
	}

	أرجع success;
}

منطقي DatamanCache::update_Cached_شے(dm_شے_t شے, uint32_t اشاریہ, ثابت uint8_t *بفر_2, uint32_t لمائی)
{
	إذا (!اشیاء || بفر_2 == خالی_سوچک || عدد_اشیاء_2 == 0) {
		أرجع خطأ;
	}

	إذا (لمائی > g_فی_شے_ناپ[شے]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمائی, static_cast<uint8_t>(شے));
		أرجع خطأ;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	لكل (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {
		إذا ((اشیاء[i].response.شے == شے) && (اشیاء[i].response.اشاریہ == اشاریہ)
		    && (اشیاء[i].cache_state == State::ResponseReceived)) {
			حافظہ_نقل(اشیاء[i].response.انکڑے, بفر_2, لمائی);
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

فراغ DatamanCache::update()
{
	إذا (شے_counter > 0) {

		_client.update();

		منطقي success = خطأ;
		منطقي response_success = خطأ;

		بدل (اشیاء[update_اشاریہ].cache_state) {
		حالة State::Idle:
			اكسر;

		حالة State::ResponseReceived:
			// Skip it
			change_Update_اشاریہ();
			اكسر;

		حالة State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_شے_t>(اشیاء[update_اشاریہ].response.شے),
						    اشیاء[update_اشاریہ].response.اشاریہ,
						    اشیاء[update_اشاریہ].response.انکڑے,
						    g_فی_شے_ناپ[اشیاء[update_اشاریہ].response.شے]);

			إذا (success) {
				اشیاء[update_اشاریہ].cache_state = State::Request_گھلیا;

			} وإلا {
				اشیاء[update_اشاریہ].cache_state = State::Error;
			}

			اكسر;

		حالة State::Request_گھلیا:

			إذا (_client.آخری_Operation_Completed(response_success)) {

				إذا (response_success) {

					اشیاء[update_اشاریہ].cache_state = State::ResponseReceived;
					change_Update_اشاریہ();

				} وإلا {
					اشیاء[update_اشاریہ].cache_state = State::Error;
				}
			}

			اكسر;

		حالة State::Error:
			// Handled below
			اكسر;
		}

		إذا (اشیاء[update_اشاریہ].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				اشیاء[update_اشاریہ].response.شے, اشیاء[update_اشاریہ].response.اشاریہ,
				اشیاء[update_اشاریہ].response.حالت);

			change_Update_اشاریہ();
		}

	}
}

فراغ DatamanCache::resetCacheState()
{
	update_اشاریہ = 0;
	شے_counter = 0;
	لدو_اشاریہ = 0;
}

فراغ DatamanCache::invalidate()
{
	لكل (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {
		اشیاء[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

مضمن فراغ DatamanCache::change_Update_اشاریہ()
{
	إذا (عدد_اشیاء_2 == 0) {
		update_اشاریہ = 0;
		شے_counter = 0;
		أرجع;
	}

	update_اشاریہ = (update_اشاریہ + 1) % عدد_اشیاء_2;

	إذا (شے_counter > 0) {
		--شے_counter;
	}
}
