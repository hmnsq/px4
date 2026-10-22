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

		hrt_abstime timestamp = hrt_absolute_وخت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_سېټ;

		منطقي success = syncHandler(request, response, timestamp, 1000_ms);

		إذا (success && (response.client_id > CLIENT_ID_NOT_سېټ)) {

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
				ثابت hrt_abstime &پیل_وخت, hrt_abstime timeout)
{
	منطقي response_received = خطأ;
	int32_t ret = 0;
	hrt_abstime وخت_elapsed = hrt_elapsed_وخت(&پیل_وخت);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	طالما (!response_received && (وخت_elapsed < timeout)) {

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
					    (response.توکی == request.توکی) &&
					    (response.شاخص == request.شاخص)) {
						response_received = ریښتیا;
						اكسر;
					}

				} وإلا إذا (request.client_id == CLIENT_ID_NOT_سېټ) {

					// validate timestamp from response.data
					إذا (0 == memcmp(&(request.timestamp), &(response.معلومات), حجم(hrt_abstime))) {
						response_received = ریښتیا;
						اكسر;
					}
				}
			}
		}

		وخت_elapsed = hrt_elapsed_وخت(&پیل_وخت);
	}

	perf_پای_ټکی(_sync_perf);

	إذا (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	أرجع response_received;
}

منطقي DatamanClient::readSync(dm_توکی_t توکی, uint32_t شاخص, uint8_t *بفر_2, uint32_t اوږدتیا, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سېټ) {
		أرجع خطأ;
	}

	إذا (اوږدتیا > g_فی_توکی_اندازه[توکی]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, اوږدتیا, static_cast<uint8_t>(توکی));
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_وخت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.شاخص = شاخص;
	request.معلومات_اوږدتیا = اوږدتیا;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.توکی = static_cast<uint8_t>(توکی);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.حالت, static_cast<uint8_t>(توکی), شاخص, اوږدتیا);

		} وإلا {
			حافظه_نقل(بفر_2, response.معلومات, اوږدتیا);
		}
	}

	أرجع success;
}

منطقي DatamanClient::writeSync(dm_توکی_t توکی, uint32_t شاخص, uint8_t *بفر_2, uint32_t اوږدتیا, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سېټ) {
		أرجع خطأ;
	}

	إذا (اوږدتیا > g_فی_توکی_اندازه[توکی]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, اوږدتیا, static_cast<uint8_t>(توکی));
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_وخت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.شاخص = شاخص;
	request.معلومات_اوږدتیا = اوږدتیا;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.توکی = static_cast<uint8_t>(توکی);

	حافظه_نقل(request.معلومات, بفر_2, اوږدتیا);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.حالت, static_cast<uint8_t>(توکی), شاخص, اوږدتیا);
		}
	}

	أرجع success;
}

منطقي DatamanClient::clearSync(dm_توکی_t توکی, hrt_abstime timeout)
{
	إذا (_client_id == CLIENT_ID_NOT_سېټ) {
		أرجع خطأ;
	}

	hrt_abstime timestamp = hrt_absolute_وخت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.توکی = static_cast<uint8_t>(توکی);

	dataman_response_s response{};
	منطقي success = syncHandler(request, response, timestamp, timeout);

	إذا (success) {

		إذا (response.حالت != dataman_response_s::حالت_SUCCESS) {

			success = خطأ;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.حالت, static_cast<uint8_t>(توکی));
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

منطقي DatamanClient::readAsync(dm_توکی_t توکی, uint32_t شاخص, uint8_t *بفر_2, uint32_t اوږدتیا)
{
	إذا (_client_id == CLIENT_ID_NOT_سېټ) {
		أرجع خطأ;
	}

	إذا (اوږدتیا > g_فی_توکی_اندازه[توکی]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, اوږدتیا, static_cast<uint8_t>(توکی));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وخت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.شاخص = شاخص;
		request.معلومات_اوږدتیا = اوږدتیا;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.توکی = static_cast<uint8_t>(توکی);

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_READ;
		فعال_request.توکی = توکی;
		فعال_request.شاخص = شاخص;
		فعال_request.بفر_2 = بفر_2;
		فعال_request.اوږدتیا = اوږدتیا;

		_state = State::Request_لېږل_شوی;

		_dataman_request_pub.publish(request);

		success = ریښتیا;
	}

	أرجع success;
}

منطقي DatamanClient::writeAsync(dm_توکی_t توکی, uint32_t شاخص, uint8_t *بفر_2, uint32_t اوږدتیا)
{
	إذا (_client_id == CLIENT_ID_NOT_سېټ) {
		أرجع خطأ;
	}

	إذا (اوږدتیا > g_فی_توکی_اندازه[توکی]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, اوږدتیا, static_cast<uint8_t>(توکی));
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وخت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.شاخص = شاخص;
		request.معلومات_اوږدتیا = اوږدتیا;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.توکی = static_cast<uint8_t>(توکی);

		حافظه_نقل(request.معلومات, بفر_2, اوږدتیا);

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_WRITE;
		فعال_request.توکی = توکی;
		فعال_request.شاخص = شاخص;
		فعال_request.بفر_2 = بفر_2;
		فعال_request.اوږدتیا = اوږدتیا;

		_state = State::Request_لېږل_شوی;

		_dataman_request_pub.publish(request);

		success = ریښتیا;
	}

	أرجع success;
}

منطقي DatamanClient::clearAsync(dm_توکی_t توکی)
{
	إذا (_client_id == CLIENT_ID_NOT_سېټ) {
		أرجع خطأ;
	}

	منطقي success = خطأ;

	إذا (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وخت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.توکی = static_cast<uint8_t>(توکی);
		request.شاخص = 0;

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_CLEAR;
		فعال_request.توکی = توکی;
		فعال_request.شاخص = request.شاخص;
		_state = State::Request_لېږل_شوی;

		_dataman_request_pub.publish(request);

		success = ریښتیا;
	}

	أرجع success;
}

فراغ DatamanClient::update()
{
	إذا (_state == State::Request_لېږل_شوی) {

		منطقي updated = خطأ;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		إذا (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			إذا ((response.client_id == _client_id) &&
			    (response.request_type == فعال_request.request_type) &&
			    (response.توکی == فعال_request.توکی) &&
			    (response.شاخص == فعال_request.شاخص)) {

				إذا (response.request_type == DM_READ) {
					حافظه_نقل(فعال_request.بفر_2, response.معلومات, فعال_request.اوږدتیا);
				}

				response_حالت = response.حالت;

				إذا (response_حالت != dataman_response_s::حالت_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.حالت, static_cast<uint8_t>(فعال_request.توکی), فعال_request.شاخص);
				}

				_state = State::ResponseReceived;
			}
		}

		إذا (_state == State::Request_لېږل_شوی) {

			/* Retry the request if there is no answer */
			إذا (((فعال_request.request_type != DM_CLEAR) && (hrt_elapsed_وخت(&فعال_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_وخت(&فعال_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_وخت();

				فعال_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.شاخص = فعال_request.شاخص;
				request.معلومات_اوږدتیا = فعال_request.اوږدتیا;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(فعال_request.request_type);
				request.توکی = static_cast<uint8_t>(فعال_request.توکی);

				إذا (فعال_request.request_type == DM_WRITE) {
					حافظه_نقل(request.معلومات, فعال_request.بفر_2, فعال_request.اوږدتیا);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_لېږل_شوی;
			}
		}
	}
}

منطقي DatamanClient::وروستی_Operation_Completed(منطقي &success)
{
	منطقي completed = خطأ;
	success = خطأ;

	إذا (_state == State::ResponseReceived) {

		إذا (response_حالت == dataman_response_s::حالت_SUCCESS) {
			success = ریښتیا;
		}

		_state = State::Idle;
		completed = ریښتیا;
	}

	أرجع completed;
}

فراغ DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ثابت حرف *cache_نه_موندنه_perf_counter_name, uint32_t عدد_توکي)
	: cache_نه_موندنه_perf(perf_alloc(PC_شمېر, cache_نه_موندنه_perf_counter_name))
{
	إذا (عدد_توکي == 0) {
		أرجع;
	}

	توکي = جديد توکی_2[عدد_توکي] {};

	إذا (توکي != تش_اشاره) {
		عدد_توکي_2 = عدد_توکي;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	ړنګول[] توکي;
	perf_free(cache_نه_موندنه_perf);
}

فراغ DatamanCache::resize(uint32_t عدد_توکي)
{
	إذا (عدد_توکي == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		ړنګول[] توکي;
		توکي = تش_اشاره;
		resetCacheState();
		عدد_توکي_2 = 0;
		أرجع;
	}

	توکی_2 *new_توکي = جديد توکی_2[عدد_توکي] {};

	إذا (new_توکي != تش_اشاره) {
		_client.abortCurrentOperation();

		uint32_t عدد_min = عدد_توکي < عدد_توکي_2 ? عدد_توکي : عدد_توکي_2;

		لكل (uint32_t i = 0; i < عدد_min; ++i) {
			new_توکي[i] = توکي[i];

			إذا (new_توکي[i].cache_state == State::Request_لېږل_شوی) {
				new_توکي[i].cache_state = State::RequestPrepared;
			}
		}

		ړنګول[] توکي;
		توکي = new_توکي;
		عدد_توکي_2 = عدد_توکي;

		// ensure new bounds are respected after resize
		بارول_شاخص = (بارول_شاخص < عدد_توکي_2) ? بارول_شاخص : 0;
		update_شاخص = (update_شاخص < عدد_توکي_2) ? update_شاخص : 0;
		توکی_counter = (توکی_counter < عدد_توکي_2) ? توکی_counter : عدد_توکي_2;

	} وإلا {
		PX4_ERR("alloc failed");
	}
}

منطقي DatamanCache::بارول(dm_توکی_t توکی, uint32_t شاخص)
{
	إذا (!توکي) {
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي duplicate = خطأ;

	//Prevent duplicates
	لكل (uint32_t i = 0; i < عدد_توکي_2; ++i) {

		إذا (توکي[i].cache_state != State::Idle &&
		    توکي[i].cache_state != State::Error &&
		    توکي[i].response.توکی == توکی &&
		    توکي[i].response.شاخص == شاخص) {
			duplicate = ریښتیا;
			اكسر;
		}
	}

	إذا (!duplicate && (توکی_counter < عدد_توکي_2)) {

		توکي[بارول_شاخص].cache_state = State::RequestPrepared;
		توکي[بارول_شاخص].response.توکی = توکی;
		توکي[بارول_شاخص].response.شاخص = شاخص;

		بارول_شاخص = (بارول_شاخص + 1) % عدد_توکي_2;

		++توکی_counter;

		success = ریښتیا;
	}

	أرجع success;
}

منطقي DatamanCache::بارول_Wait(dm_توکی_t توکی, uint32_t شاخص, uint8_t *بفر_2, uint32_t اوږدتیا, hrt_abstime timeout)
{
	إذا (اوږدتیا > g_فی_توکی_اندازه[توکی]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, اوږدتیا, static_cast<uint8_t>(توکی));
		أرجع خطأ;
	}

	منطقي success = خطأ;
	منطقي توکی_وموندل = خطأ;

	إذا (توکي) {
		لكل (uint32_t i = 0; i < عدد_توکي_2; ++i) {
			إذا ((توکي[i].response.توکی == توکی) &&
			    (توکي[i].response.شاخص == شاخص)) {
				توکی_وموندل = ریښتیا;

				إذا (توکي[i].cache_state == State::ResponseReceived) {
					حافظه_نقل(بفر_2, توکي[i].response.معلومات, اوږدتیا);
					success = ریښتیا;
					اكسر;
				}
			}
		}
	}

	إذا (!success) {
		perf_شمېر(cache_نه_موندنه_perf);
	}

	إذا (!success && (timeout > 0)) {
		success = _client.readSync(توکی, شاخص, بفر_2, اوږدتیا, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		إذا (success && !توکی_وموندل && توکي && (توکی_counter < عدد_توکي_2)) {
			توکي[بارول_شاخص].cache_state = State::ResponseReceived;
			توکي[بارول_شاخص].response.توکی = توکی;
			توکي[بارول_شاخص].response.شاخص = شاخص;
			حافظه_نقل(توکي[بارول_شاخص].response.معلومات, بفر_2, اوږدتیا);

			بارول_شاخص = (بارول_شاخص + 1) % عدد_توکي_2;

			++توکی_counter; // Still increase the counter here
		}
	}

	أرجع success;
}

منطقي DatamanCache::writeWait(dm_توکی_t توکی, uint32_t شاخص, uint8_t *بفر_2, uint32_t اوږدتیا, hrt_abstime timeout)
{
	إذا (اوږدتیا > g_فی_توکی_اندازه[توکی]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, اوږدتیا, static_cast<uint8_t>(توکی));
		أرجع خطأ;
	}

	منطقي success = _client.writeSync(توکی, شاخص, بفر_2, اوږدتیا, timeout);

	إذا (success && توکي) {
		لكل (uint32_t i = 0; i < عدد_توکي_2; ++i) {
			إذا ((توکي[i].response.توکی == توکی) &&
			    (توکي[i].response.شاخص == شاخص) &&
			    ((توکي[i].cache_state == State::ResponseReceived) ||
			     (توکي[i].cache_state == State::RequestPrepared))) {

				حافظه_نقل(توکي[i].response.معلومات, بفر_2, اوږدتیا);
				توکي[i].cache_state = State::ResponseReceived;
				اكسر;
			}
		}
	}

	أرجع success;
}

منطقي DatamanCache::update_Cached_توکی(dm_توکی_t توکی, uint32_t شاخص, ثابت uint8_t *بفر_2, uint32_t اوږدتیا)
{
	إذا (!توکي || بفر_2 == تش_اشاره || عدد_توکي_2 == 0) {
		أرجع خطأ;
	}

	إذا (اوږدتیا > g_فی_توکی_اندازه[توکی]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, اوږدتیا, static_cast<uint8_t>(توکی));
		أرجع خطأ;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	لكل (uint32_t i = 0; i < عدد_توکي_2; ++i) {
		إذا ((توکي[i].response.توکی == توکی) && (توکي[i].response.شاخص == شاخص)
		    && (توکي[i].cache_state == State::ResponseReceived)) {
			حافظه_نقل(توکي[i].response.معلومات, بفر_2, اوږدتیا);
			أرجع ریښتیا;
		}
	}

	أرجع خطأ;
}

فراغ DatamanCache::update()
{
	إذا (توکی_counter > 0) {

		_client.update();

		منطقي success = خطأ;
		منطقي response_success = خطأ;

		بدل (توکي[update_شاخص].cache_state) {
		حالة State::Idle:
			اكسر;

		حالة State::ResponseReceived:
			// Skip it
			change_Update_شاخص();
			اكسر;

		حالة State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_توکی_t>(توکي[update_شاخص].response.توکی),
						    توکي[update_شاخص].response.شاخص,
						    توکي[update_شاخص].response.معلومات,
						    g_فی_توکی_اندازه[توکي[update_شاخص].response.توکی]);

			إذا (success) {
				توکي[update_شاخص].cache_state = State::Request_لېږل_شوی;

			} وإلا {
				توکي[update_شاخص].cache_state = State::Error;
			}

			اكسر;

		حالة State::Request_لېږل_شوی:

			إذا (_client.وروستی_Operation_Completed(response_success)) {

				إذا (response_success) {

					توکي[update_شاخص].cache_state = State::ResponseReceived;
					change_Update_شاخص();

				} وإلا {
					توکي[update_شاخص].cache_state = State::Error;
				}
			}

			اكسر;

		حالة State::Error:
			// Handled below
			اكسر;
		}

		إذا (توکي[update_شاخص].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				توکي[update_شاخص].response.توکی, توکي[update_شاخص].response.شاخص,
				توکي[update_شاخص].response.حالت);

			change_Update_شاخص();
		}

	}
}

فراغ DatamanCache::resetCacheState()
{
	update_شاخص = 0;
	توکی_counter = 0;
	بارول_شاخص = 0;
}

فراغ DatamanCache::invalidate()
{
	لكل (uint32_t i = 0; i < عدد_توکي_2; ++i) {
		توکي[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

مضمن فراغ DatamanCache::change_Update_شاخص()
{
	إذا (عدد_توکي_2 == 0) {
		update_شاخص = 0;
		توکی_counter = 0;
		أرجع;
	}

	update_شاخص = (update_شاخص + 1) % عدد_توکي_2;

	إذا (توکی_counter > 0) {
		--توکی_counter;
	}
}
