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

		hrt_abstime timestamp = hrt_absolute_وقت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_سيٽ;

		منطقي success = syncHandler(request, response, timestamp, 1000_ms);

		إذا (success && (response.client_id > CLIENT_ID_NOT_سيٽ)) {

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
				ثابت hrt_abstime &شروعات_وقت, hrt_abstime timeout)
{
	منطقي response_received = خطأ;
	int32_t ret = 0;
	hrt_abstime وقت_elapsed = hrt_elapsed_وقت(&شروعات_وقت);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	طالما (!response_received && (وقت_elapsed < timeout)) {

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
					    (response.شي == request.شي) &&
					    (response.اشاريو == request.اشاريو)) {
						response_received = سچ;
						اكسر;
					}

				} وإلا إذا (request.client_id == CLIENT_ID_NOT_سيٽ) {

					// validate timestamp from response.data
					إذا (0 == memcmp(&(request.timestamp), &(response.معلومات), حجم(hrt_abstime))) {
						response_received = سچ;
						اكسر;
					}
				}
			}
		}

		وقت_elapsed = hrt_elapsed_وقت(&شروعات_وقت);
	}

	perf_پڇاڙي(_sync_perf);

	إذا (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	أرجع response_received;
}

منطقي DatamanClient::readSync(dm_شي_t شي, uint32_t اشاريو, uint8_t *بفر_2, uint32_t ڊيگهائي, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سيٽ) {
		أرجع خطأ;
	}

	إذا (ڊيگهائي > g_في_شي_سائيز[شي]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ڊيگهائي, static_cast<uint8_t>(شي));
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_وقت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.اشاريو = اشاريو;
	request.معلومات_ڊيگهائي = ڊيگهائي;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.شي = static_cast<uint8_t>(شي);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.حالت, static_cast<uint8_t>(شي), اشاريو, ڊيگهائي);

		} وإلا {
			يادگيري_نقل(بفر_2, response.معلومات, ڊيگهائي);
		}
	}

	أرجع success;
}

منطقي DatamanClient::writeSync(dm_شي_t شي, uint32_t اشاريو, uint8_t *بفر_2, uint32_t ڊيگهائي, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سيٽ) {
		أرجع خطأ;
	}

	إذا (ڊيگهائي > g_في_شي_سائيز[شي]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ڊيگهائي, static_cast<uint8_t>(شي));
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_وقت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.اشاريو = اشاريو;
	request.معلومات_ڊيگهائي = ڊيگهائي;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.شي = static_cast<uint8_t>(شي);

	يادگيري_نقل(request.معلومات, بفر_2, ڊيگهائي);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.حالت, static_cast<uint8_t>(شي), اشاريو, ڊيگهائي);
		}
	}

	أرجع success;
}

منطقي DatamanClient::clearSync(dm_شي_t شي, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سيٽ) {
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_وقت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.شي = static_cast<uint8_t>(شي);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.حالت, static_cast<uint8_t>(شي));
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

منطقي DatamanClient::readAsync(dm_شي_t شي, uint32_t اشاريو, uint8_t *بفر_2, uint32_t ڊيگهائي)
{
	إذا (_client_id == CLIENT_ID_NOT_سيٽ) {
		أرجع خطأ;
	}

	إذا (ڊيگهائي > g_في_شي_سائيز[شي]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ڊيگهائي, static_cast<uint8_t>(شي));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وقت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.اشاريو = اشاريو;
		request.معلومات_ڊيگهائي = ڊيگهائي;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.شي = static_cast<uint8_t>(شي);

		سرگرم_request.timestamp = timestamp;
		سرگرم_request.request_type = DM_READ;
		سرگرم_request.شي = شي;
		سرگرم_request.اشاريو = اشاريو;
		سرگرم_request.بفر_2 = بفر_2;
		سرگرم_request.ڊيگهائي = ڊيگهائي;

		_state = State::Request_موڪليل;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	أرجع success;
}

منطقي DatamanClient::writeAsync(dm_شي_t شي, uint32_t اشاريو, uint8_t *بفر_2, uint32_t ڊيگهائي)
{
	إذا (_client_id == CLIENT_ID_NOT_سيٽ) {
		أرجع خطأ;
	}

	إذا (ڊيگهائي > g_في_شي_سائيز[شي]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ڊيگهائي, static_cast<uint8_t>(شي));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وقت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.اشاريو = اشاريو;
		request.معلومات_ڊيگهائي = ڊيگهائي;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.شي = static_cast<uint8_t>(شي);

		يادگيري_نقل(request.معلومات, بفر_2, ڊيگهائي);

		سرگرم_request.timestamp = timestamp;
		سرگرم_request.request_type = DM_WRITE;
		سرگرم_request.شي = شي;
		سرگرم_request.اشاريو = اشاريو;
		سرگرم_request.بفر_2 = بفر_2;
		سرگرم_request.ڊيگهائي = ڊيگهائي;

		_state = State::Request_موڪليل;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	أرجع success;
}

منطقي DatamanClient::clearAsync(dm_شي_t شي)
{
	إذا (_client_id == CLIENT_ID_NOT_سيٽ) {
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وقت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.شي = static_cast<uint8_t>(شي);
		request.اشاريو = 0;

		سرگرم_request.timestamp = timestamp;
		سرگرم_request.request_type = DM_CLEAR;
		سرگرم_request.شي = شي;
		سرگرم_request.اشاريو = request.اشاريو;
		_state = State::Request_موڪليل;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	أرجع success;
}

فراغ DatamanClient::update()
{
	إذا (_state == State::Request_موڪليل) {

		منطقي updated = خطأ;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		إذا (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			إذا ((response.client_id == _client_id) &&
			    (response.request_type == سرگرم_request.request_type) &&
			    (response.شي == سرگرم_request.شي) &&
			    (response.اشاريو == سرگرم_request.اشاريو)) {

				إذا (response.request_type == DM_READ) {
					يادگيري_نقل(سرگرم_request.بفر_2, response.معلومات, سرگرم_request.ڊيگهائي);
				}

				response_حالت = response.حالت;

				إذا (response_حالت != dataman_response_s::حالت_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.حالت, static_cast<uint8_t>(سرگرم_request.شي), سرگرم_request.اشاريو);
				}

				_state = State::ResponseReceived;
			}
		}

		إذا (_state == State::Request_موڪليل) {

			/* Retry the request if there is no answer */
			إذا (((سرگرم_request.request_type != DM_CLEAR) && (hrt_elapsed_وقت(&سرگرم_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_وقت(&سرگرم_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_وقت();

				سرگرم_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.اشاريو = سرگرم_request.اشاريو;
				request.معلومات_ڊيگهائي = سرگرم_request.ڊيگهائي;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(سرگرم_request.request_type);
				request.شي = static_cast<uint8_t>(سرگرم_request.شي);

				إذا (سرگرم_request.request_type == DM_WRITE) {
					يادگيري_نقل(request.معلومات, سرگرم_request.بفر_2, سرگرم_request.ڊيگهائي);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_موڪليل;
			}
		}
	}
}

منطقي DatamanClient::آخري_Operation_Completed(منطقي &success)
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

DatamanCache::DatamanCache(ثابت حرف *cache_ناڪامي_perf_counter_name, uint32_t انگ_شيون)
	: cache_ناڪامي_perf(perf_alloc(PC_ڳڻپ, cache_ناڪامي_perf_counter_name))
{
	إذا (انگ_شيون == 0) {
		أرجع;
	}

	شيون = جديد شي_2[انگ_شيون] {};

	إذا (شيون != خالي_اشارو) {
		انگ_شيون_2 = انگ_شيون;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	ميٽيو[] شيون;
	perf_free(cache_ناڪامي_perf);
}

فراغ DatamanCache::resize(uint32_t انگ_شيون)
{
	إذا (انگ_شيون == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		ميٽيو[] شيون;
		شيون = خالي_اشارو;
		resetCacheState();
		انگ_شيون_2 = 0;
		أرجع;
	}

	شي_2 *new_شيون = جديد شي_2[انگ_شيون] {};

	إذا (new_شيون != خالي_اشارو) {
		_client.abortCurrentOperation();

		uint32_t انگ_min = انگ_شيون < انگ_شيون_2 ? انگ_شيون : انگ_شيون_2;

		لكل (uint32_t i = 0; i < انگ_min; ++i) {
			new_شيون[i] = شيون[i];

			إذا (new_شيون[i].cache_state == State::Request_موڪليل) {
				new_شيون[i].cache_state = State::RequestPrepared;
			}
		}

		ميٽيو[] شيون;
		شيون = new_شيون;
		انگ_شيون_2 = انگ_شيون;

		// ensure new bounds are respected after resize
		ڀريو_اشاريو = (ڀريو_اشاريو < انگ_شيون_2) ? ڀريو_اشاريو : 0;
		update_اشاريو = (update_اشاريو < انگ_شيون_2) ? update_اشاريو : 0;
		شي_counter = (شي_counter < انگ_شيون_2) ? شي_counter : انگ_شيون_2;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

منطقي DatamanCache::ڀريو(dm_شي_t شي, uint32_t اشاريو)
{
	إذا (!شيون) {
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي duplicate = خطأ;

	//Prevent duplicates
	لكل (uint32_t i = 0; i < انگ_شيون_2; ++i) {

		إذا (شيون[i].cache_state != State::Idle &&
		    شيون[i].cache_state != State::Error &&
		    شيون[i].response.شي == شي &&
		    شيون[i].response.اشاريو == اشاريو) {
			duplicate = سچ;
			اكسر;
		}
	}

	إذا (!duplicate && (شي_counter < انگ_شيون_2)) {

		شيون[ڀريو_اشاريو].cache_state = State::RequestPrepared;
		شيون[ڀريو_اشاريو].response.شي = شي;
		شيون[ڀريو_اشاريو].response.اشاريو = اشاريو;

		ڀريو_اشاريو = (ڀريو_اشاريو + 1) % انگ_شيون_2;

		++شي_counter;

		success = سچ;
	}

	أرجع success;
}

منطقي DatamanCache::ڀريو_Wait(dm_شي_t شي, uint32_t اشاريو, uint8_t *بفر_2, uint32_t ڊيگهائي, hrt_abstime timeout)
{
	إذا (ڊيگهائي > g_في_شي_سائيز[شي]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, ڊيگهائي, static_cast<uint8_t>(شي));
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي شي_مليو = خطأ;

	إذا (شيون) {
		لكل (uint32_t i = 0; i < انگ_شيون_2; ++i) {
			إذا ((شيون[i].response.شي == شي) &&
			    (شيون[i].response.اشاريو == اشاريو)) {
				شي_مليو = سچ;

				إذا (شيون[i].cache_state == State::ResponseReceived) {
					يادگيري_نقل(بفر_2, شيون[i].response.معلومات, ڊيگهائي);
					success = سچ;
					اكسر;
				}
			}
		}
	}

	إذا (!success) {
		perf_ڳڻپ(cache_ناڪامي_perf);
	}

	إذا (!success && (timeout > 0)) {
		success = _client.readSync(شي, اشاريو, بفر_2, ڊيگهائي, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		إذا (success && !شي_مليو && شيون && (شي_counter < انگ_شيون_2)) {
			شيون[ڀريو_اشاريو].cache_state = State::ResponseReceived;
			شيون[ڀريو_اشاريو].response.شي = شي;
			شيون[ڀريو_اشاريو].response.اشاريو = اشاريو;
			يادگيري_نقل(شيون[ڀريو_اشاريو].response.معلومات, بفر_2, ڊيگهائي);

			ڀريو_اشاريو = (ڀريو_اشاريو + 1) % انگ_شيون_2;

			++شي_counter; // Still increase the counter here
		}
	}

	أرجع success;
}

منطقي DatamanCache::writeWait(dm_شي_t شي, uint32_t اشاريو, uint8_t *بفر_2, uint32_t ڊيگهائي, hrt_abstime timeout)
{
	إذا (ڊيگهائي > g_في_شي_سائيز[شي]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ڊيگهائي, static_cast<uint8_t>(شي));
		أرجع خطأ;
	}

	منطقي success = _client.writeSync(شي, اشاريو, بفر_2, ڊيگهائي, timeout);

	إذا (success && شيون) {
		لكل (uint32_t i = 0; i < انگ_شيون_2; ++i) {
			إذا ((شيون[i].response.شي == شي) &&
			    (شيون[i].response.اشاريو == اشاريو) &&
			    ((شيون[i].cache_state == State::ResponseReceived) ||
			     (شيون[i].cache_state == State::RequestPrepared))) {

				يادگيري_نقل(شيون[i].response.معلومات, بفر_2, ڊيگهائي);
				شيون[i].cache_state = State::ResponseReceived;
				اكسر;
			}
		}
	}

	أرجع success;
}

منطقي DatamanCache::update_Cached_شي(dm_شي_t شي, uint32_t اشاريو, ثابت uint8_t *بفر_2, uint32_t ڊيگهائي)
{
	إذا (!شيون || بفر_2 == خالي_اشارو || انگ_شيون_2 == 0) {
		أرجع خطأ;
	}

	إذا (ڊيگهائي > g_في_شي_سائيز[شي]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ڊيگهائي, static_cast<uint8_t>(شي));
		أرجع خطأ;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	لكل (uint32_t i = 0; i < انگ_شيون_2; ++i) {
		إذا ((شيون[i].response.شي == شي) && (شيون[i].response.اشاريو == اشاريو)
		    && (شيون[i].cache_state == State::ResponseReceived)) {
			يادگيري_نقل(شيون[i].response.معلومات, بفر_2, ڊيگهائي);
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

فراغ DatamanCache::update()
{
	إذا (شي_counter > 0) {

		_client.update();

		منطقي success = خطأ;
		منطقي response_success = خطأ;

		بدل (شيون[update_اشاريو].cache_state) {
		حالة State::Idle:
			اكسر;

		حالة State::ResponseReceived:
			// Skip it
			change_Update_اشاريو();
			اكسر;

		حالة State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_شي_t>(شيون[update_اشاريو].response.شي),
						    شيون[update_اشاريو].response.اشاريو,
						    شيون[update_اشاريو].response.معلومات,
						    g_في_شي_سائيز[شيون[update_اشاريو].response.شي]);

			إذا (success) {
				شيون[update_اشاريو].cache_state = State::Request_موڪليل;

			} وإلا {
				شيون[update_اشاريو].cache_state = State::Error;
			}

			اكسر;

		حالة State::Request_موڪليل:

			إذا (_client.آخري_Operation_Completed(response_success)) {

				إذا (response_success) {

					شيون[update_اشاريو].cache_state = State::ResponseReceived;
					change_Update_اشاريو();

				} وإلا {
					شيون[update_اشاريو].cache_state = State::Error;
				}
			}

			اكسر;

		حالة State::Error:
			// Handled below
			اكسر;
		}

		إذا (شيون[update_اشاريو].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				شيون[update_اشاريو].response.شي, شيون[update_اشاريو].response.اشاريو,
				شيون[update_اشاريو].response.حالت);

			change_Update_اشاريو();
		}

	}
}

فراغ DatamanCache::resetCacheState()
{
	update_اشاريو = 0;
	شي_counter = 0;
	ڀريو_اشاريو = 0;
}

فراغ DatamanCache::invalidate()
{
	لكل (uint32_t i = 0; i < انگ_شيون_2; ++i) {
		شيون[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

مضمن فراغ DatamanCache::change_Update_اشاريو()
{
	إذا (انگ_شيون_2 == 0) {
		update_اشاريو = 0;
		شي_counter = 0;
		أرجع;
	}

	update_اشاريو = (update_اشاريو + 1) % انگ_شيون_2;

	إذا (شي_counter > 0) {
		--شي_counter;
	}
}
