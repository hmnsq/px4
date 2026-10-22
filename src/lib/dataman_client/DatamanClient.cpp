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

#شامل <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	اگر (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} ورنہ {
		// make sure we don't get any stale response by doing an orb_copy
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

		_fds.fd = _dataman_response_sub;
		_fds.events = POLLIN;

		hrt_abstime timestamp = hrt_absolute_وقت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_سیٹ;

		منطقی success = syncHandler(request, response, timestamp, 1000_ms);

		اگر (success && (response.client_id > CLIENT_ID_NOT_سیٹ)) {

			_client_id = response.client_id;

		} ورنہ {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	اگر (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

منطقی DatamanClient::syncHandler(ثابت dataman_request_s &request, dataman_response_s &response,
				ثابت hrt_abstime &آغاز_وقت, hrt_abstime timeout)
{
	منطقی response_received = جھوٹ;
	int32_t ret = 0;
	hrt_abstime وقت_elapsed = hrt_elapsed_وقت(&آغاز_وقت);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	جب (!response_received && (وقت_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		اگر (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			توڑو;

		} ورنہ اگر (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} ورنہ {

			منطقی updated = جھوٹ;
			orb_check(_dataman_response_sub, &updated);

			اگر (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				اگر (response.client_id == request.client_id) {

					اگر ((response.request_type == request.request_type) &&
					    (response.شے_2 == request.شے_2) &&
					    (response.اشاریہ == request.اشاریہ)) {
						response_received = سچ;
						توڑو;
					}

				} ورنہ اگر (request.client_id == CLIENT_ID_NOT_سیٹ) {

					// validate timestamp from response.data
					اگر (0 == memcmp(&(request.timestamp), &(response.ڈیٹا), حجم(hrt_abstime))) {
						response_received = سچ;
						توڑو;
					}
				}
			}
		}

		وقت_elapsed = hrt_elapsed_وقت(&آغاز_وقت);
	}

	perf_انتہا(_sync_perf);

	اگر (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	واپس response_received;
}

منطقی DatamanClient::readSync(dm_شے_t شے_2, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمبائی_2, hrt_abstime timeout)
{
	اگر (_client_id == CLIENT_ID_NOT_سیٹ) {
		واپس جھوٹ;
	}

	اگر (لمبائی_2 > g_فی_شے_سائز[شے_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمبائی_2, static_cast<uint8_t>(شے_2));
		واپس جھوٹ;
	}

	hrt_abstime timestamp = hrt_absolute_وقت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.اشاریہ = اشاریہ;
	request.ڈیٹا_لمبائی = لمبائی_2;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.شے_2 = static_cast<uint8_t>(شے_2);

	dataman_response_s response{};
	منطقی success = syncHandler(request, response, timestamp, timeout);

	اگر (success) {

		اگر (response.حالت_2 != dataman_response_s::حالت_SUCCESS) {

			success = جھوٹ;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.حالت_2, static_cast<uint8_t>(شے_2), اشاریہ, لمبائی_2);

		} ورنہ {
			حافظہ_نقل(بفر_2, response.ڈیٹا, لمبائی_2);
		}
	}

	واپس success;
}

منطقی DatamanClient::writeSync(dm_شے_t شے_2, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمبائی_2, hrt_abstime timeout)
{
	اگر (_client_id == CLIENT_ID_NOT_سیٹ) {
		واپس جھوٹ;
	}

	اگر (لمبائی_2 > g_فی_شے_سائز[شے_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمبائی_2, static_cast<uint8_t>(شے_2));
		واپس جھوٹ;
	}

	hrt_abstime timestamp = hrt_absolute_وقت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.اشاریہ = اشاریہ;
	request.ڈیٹا_لمبائی = لمبائی_2;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.شے_2 = static_cast<uint8_t>(شے_2);

	حافظہ_نقل(request.ڈیٹا, بفر_2, لمبائی_2);

	dataman_response_s response{};
	منطقی success = syncHandler(request, response, timestamp, timeout);

	اگر (success) {

		اگر (response.حالت_2 != dataman_response_s::حالت_SUCCESS) {

			success = جھوٹ;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.حالت_2, static_cast<uint8_t>(شے_2), اشاریہ, لمبائی_2);
		}
	}

	واپس success;
}

منطقی DatamanClient::clearSync(dm_شے_t شے_2, hrt_abstime timeout)
{
	اگر (_client_id == CLIENT_ID_NOT_سیٹ) {
		واپس جھوٹ;
	}

	hrt_abstime timestamp = hrt_absolute_وقت();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.شے_2 = static_cast<uint8_t>(شے_2);

	dataman_response_s response{};
	منطقی success = syncHandler(request, response, timestamp, timeout);

	اگر (success) {

		اگر (response.حالت_2 != dataman_response_s::حالت_SUCCESS) {

			success = جھوٹ;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.حالت_2, static_cast<uint8_t>(شے_2));
		}
	}

	واپس success;
}

خالی DatamanClient::clearPendingResponse()
{
	اگر (_dataman_response_sub < 0) {
		واپس;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	منطقی updated = جھوٹ;
	orb_check(_dataman_response_sub, &updated);

	جب (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

منطقی DatamanClient::readAsync(dm_شے_t شے_2, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمبائی_2)
{
	اگر (_client_id == CLIENT_ID_NOT_سیٹ) {
		واپس جھوٹ;
	}

	اگر (لمبائی_2 > g_فی_شے_سائز[شے_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمبائی_2, static_cast<uint8_t>(شے_2));
		واپس جھوٹ;
	}

	منطقی success = جھوٹ;

	اگر (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وقت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.اشاریہ = اشاریہ;
		request.ڈیٹا_لمبائی = لمبائی_2;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.شے_2 = static_cast<uint8_t>(شے_2);

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_READ;
		فعال_request.شے_2 = شے_2;
		فعال_request.اشاریہ = اشاریہ;
		فعال_request.بفر_2 = بفر_2;
		فعال_request.لمبائی_2 = لمبائی_2;

		_state = State::Request_بھیجا;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	واپس success;
}

منطقی DatamanClient::writeAsync(dm_شے_t شے_2, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمبائی_2)
{
	اگر (_client_id == CLIENT_ID_NOT_سیٹ) {
		واپس جھوٹ;
	}

	اگر (لمبائی_2 > g_فی_شے_سائز[شے_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمبائی_2, static_cast<uint8_t>(شے_2));
		واپس جھوٹ;
	}

	منطقی success = جھوٹ;

	اگر (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وقت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.اشاریہ = اشاریہ;
		request.ڈیٹا_لمبائی = لمبائی_2;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.شے_2 = static_cast<uint8_t>(شے_2);

		حافظہ_نقل(request.ڈیٹا, بفر_2, لمبائی_2);

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_WRITE;
		فعال_request.شے_2 = شے_2;
		فعال_request.اشاریہ = اشاریہ;
		فعال_request.بفر_2 = بفر_2;
		فعال_request.لمبائی_2 = لمبائی_2;

		_state = State::Request_بھیجا;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	واپس success;
}

منطقی DatamanClient::clearAsync(dm_شے_t شے_2)
{
	اگر (_client_id == CLIENT_ID_NOT_سیٹ) {
		واپس جھوٹ;
	}

	منطقی success = جھوٹ;

	اگر (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_وقت();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.شے_2 = static_cast<uint8_t>(شے_2);
		request.اشاریہ = 0;

		فعال_request.timestamp = timestamp;
		فعال_request.request_type = DM_CLEAR;
		فعال_request.شے_2 = شے_2;
		فعال_request.اشاریہ = request.اشاریہ;
		_state = State::Request_بھیجا;

		_dataman_request_pub.publish(request);

		success = سچ;
	}

	واپس success;
}

خالی DatamanClient::update()
{
	اگر (_state == State::Request_بھیجا) {

		منطقی updated = جھوٹ;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		اگر (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			اگر ((response.client_id == _client_id) &&
			    (response.request_type == فعال_request.request_type) &&
			    (response.شے_2 == فعال_request.شے_2) &&
			    (response.اشاریہ == فعال_request.اشاریہ)) {

				اگر (response.request_type == DM_READ) {
					حافظہ_نقل(فعال_request.بفر_2, response.ڈیٹا, فعال_request.لمبائی_2);
				}

				response_حالت = response.حالت_2;

				اگر (response_حالت != dataman_response_s::حالت_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.حالت_2, static_cast<uint8_t>(فعال_request.شے_2), فعال_request.اشاریہ);
				}

				_state = State::ResponseReceived;
			}
		}

		اگر (_state == State::Request_بھیجا) {

			/* Retry the request if there is no answer */
			اگر (((فعال_request.request_type != DM_CLEAR) && (hrt_elapsed_وقت(&فعال_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_وقت(&فعال_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_وقت();

				فعال_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.اشاریہ = فعال_request.اشاریہ;
				request.ڈیٹا_لمبائی = فعال_request.لمبائی_2;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(فعال_request.request_type);
				request.شے_2 = static_cast<uint8_t>(فعال_request.شے_2);

				اگر (فعال_request.request_type == DM_WRITE) {
					حافظہ_نقل(request.ڈیٹا, فعال_request.بفر_2, فعال_request.لمبائی_2);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_بھیجا;
			}
		}
	}
}

منطقی DatamanClient::آخری_Operation_Completed(منطقی &success)
{
	منطقی completed = جھوٹ;
	success = جھوٹ;

	اگر (_state == State::ResponseReceived) {

		اگر (response_حالت == dataman_response_s::حالت_SUCCESS) {
			success = سچ;
		}

		_state = State::Idle;
		completed = سچ;
	}

	واپس completed;
}

خالی DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ثابت حرف *cache_ناکامی_perf_شمار_name, uint32_t عدد_اشیاء)
	: cache_ناکامی_perf(perf_alloc(PC_گنتی, cache_ناکامی_perf_شمار_name))
{
	اگر (عدد_اشیاء == 0) {
		واپس;
	}

	اشیاء = نیا شے_3[عدد_اشیاء] {};

	اگر (اشیاء != خالی_اشاریہ) {
		عدد_اشیاء_2 = عدد_اشیاء;

	} ورنہ {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	حذف[] اشیاء;
	perf_free(cache_ناکامی_perf);
}

خالی DatamanCache::resize(uint32_t عدد_اشیاء)
{
	اگر (عدد_اشیاء == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		حذف[] اشیاء;
		اشیاء = خالی_اشاریہ;
		resetCacheState();
		عدد_اشیاء_2 = 0;
		واپس;
	}

	شے_3 *new_اشیاء = نیا شے_3[عدد_اشیاء] {};

	اگر (new_اشیاء != خالی_اشاریہ) {
		_client.abortCurrentOperation();

		uint32_t عدد_min = عدد_اشیاء < عدد_اشیاء_2 ? عدد_اشیاء : عدد_اشیاء_2;

		برائے (uint32_t i = 0; i < عدد_min; ++i) {
			new_اشیاء[i] = اشیاء[i];

			اگر (new_اشیاء[i].cache_state == State::Request_بھیجا) {
				new_اشیاء[i].cache_state = State::RequestPrepared;
			}
		}

		حذف[] اشیاء;
		اشیاء = new_اشیاء;
		عدد_اشیاء_2 = عدد_اشیاء;

		// ensure new bounds are respected after resize
		بھرو_اشاریہ = (بھرو_اشاریہ < عدد_اشیاء_2) ? بھرو_اشاریہ : 0;
		update_اشاریہ = (update_اشاریہ < عدد_اشیاء_2) ? update_اشاریہ : 0;
		شے_شمار = (شے_شمار < عدد_اشیاء_2) ? شے_شمار : عدد_اشیاء_2;

	} ورنہ {
		PX4_ERR("alloc failed");
	}
}

منطقی DatamanCache::بھرو(dm_شے_t شے_2, uint32_t اشاریہ)
{
	اگر (!اشیاء) {
		واپس جھوٹ;
	}

	منطقی success = جھوٹ;
	منطقی duplicate = جھوٹ;

	//Prevent duplicates
	برائے (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {

		اگر (اشیاء[i].cache_state != State::Idle &&
		    اشیاء[i].cache_state != State::Error &&
		    اشیاء[i].response.شے_2 == شے_2 &&
		    اشیاء[i].response.اشاریہ == اشاریہ) {
			duplicate = سچ;
			توڑو;
		}
	}

	اگر (!duplicate && (شے_شمار < عدد_اشیاء_2)) {

		اشیاء[بھرو_اشاریہ].cache_state = State::RequestPrepared;
		اشیاء[بھرو_اشاریہ].response.شے_2 = شے_2;
		اشیاء[بھرو_اشاریہ].response.اشاریہ = اشاریہ;

		بھرو_اشاریہ = (بھرو_اشاریہ + 1) % عدد_اشیاء_2;

		++شے_شمار;

		success = سچ;
	}

	واپس success;
}

منطقی DatamanCache::بھرو_Wait(dm_شے_t شے_2, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمبائی_2, hrt_abstime timeout)
{
	اگر (لمبائی_2 > g_فی_شے_سائز[شے_2]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, لمبائی_2, static_cast<uint8_t>(شے_2));
		واپس جھوٹ;
	}

	منطقی success = جھوٹ;
	منطقی شے_ملا = جھوٹ;

	اگر (اشیاء) {
		برائے (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {
			اگر ((اشیاء[i].response.شے_2 == شے_2) &&
			    (اشیاء[i].response.اشاریہ == اشاریہ)) {
				شے_ملا = سچ;

				اگر (اشیاء[i].cache_state == State::ResponseReceived) {
					حافظہ_نقل(بفر_2, اشیاء[i].response.ڈیٹا, لمبائی_2);
					success = سچ;
					توڑو;
				}
			}
		}
	}

	اگر (!success) {
		perf_گنتی(cache_ناکامی_perf);
	}

	اگر (!success && (timeout > 0)) {
		success = _client.readSync(شے_2, اشاریہ, بفر_2, لمبائی_2, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		اگر (success && !شے_ملا && اشیاء && (شے_شمار < عدد_اشیاء_2)) {
			اشیاء[بھرو_اشاریہ].cache_state = State::ResponseReceived;
			اشیاء[بھرو_اشاریہ].response.شے_2 = شے_2;
			اشیاء[بھرو_اشاریہ].response.اشاریہ = اشاریہ;
			حافظہ_نقل(اشیاء[بھرو_اشاریہ].response.ڈیٹا, بفر_2, لمبائی_2);

			بھرو_اشاریہ = (بھرو_اشاریہ + 1) % عدد_اشیاء_2;

			++شے_شمار; // Still increase the counter here
		}
	}

	واپس success;
}

منطقی DatamanCache::writeWait(dm_شے_t شے_2, uint32_t اشاریہ, uint8_t *بفر_2, uint32_t لمبائی_2, hrt_abstime timeout)
{
	اگر (لمبائی_2 > g_فی_شے_سائز[شے_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمبائی_2, static_cast<uint8_t>(شے_2));
		واپس جھوٹ;
	}

	منطقی success = _client.writeSync(شے_2, اشاریہ, بفر_2, لمبائی_2, timeout);

	اگر (success && اشیاء) {
		برائے (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {
			اگر ((اشیاء[i].response.شے_2 == شے_2) &&
			    (اشیاء[i].response.اشاریہ == اشاریہ) &&
			    ((اشیاء[i].cache_state == State::ResponseReceived) ||
			     (اشیاء[i].cache_state == State::RequestPrepared))) {

				حافظہ_نقل(اشیاء[i].response.ڈیٹا, بفر_2, لمبائی_2);
				اشیاء[i].cache_state = State::ResponseReceived;
				توڑو;
			}
		}
	}

	واپس success;
}

منطقی DatamanCache::update_Cached_شے(dm_شے_t شے_2, uint32_t اشاریہ, ثابت uint8_t *بفر_2, uint32_t لمبائی_2)
{
	اگر (!اشیاء || بفر_2 == خالی_اشاریہ || عدد_اشیاء_2 == 0) {
		واپس جھوٹ;
	}

	اگر (لمبائی_2 > g_فی_شے_سائز[شے_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, لمبائی_2, static_cast<uint8_t>(شے_2));
		واپس جھوٹ;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	برائے (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {
		اگر ((اشیاء[i].response.شے_2 == شے_2) && (اشیاء[i].response.اشاریہ == اشاریہ)
		    && (اشیاء[i].cache_state == State::ResponseReceived)) {
			حافظہ_نقل(اشیاء[i].response.ڈیٹا, بفر_2, لمبائی_2);
			واپس سچ;
		}
	}

	واپس جھوٹ;
}

خالی DatamanCache::update()
{
	اگر (شے_شمار > 0) {

		_client.update();

		منطقی success = جھوٹ;
		منطقی response_success = جھوٹ;

		انتخاب (اشیاء[update_اشاریہ].cache_state) {
		حالت State::Idle:
			توڑو;

		حالت State::ResponseReceived:
			// Skip it
			change_Update_اشاریہ();
			توڑو;

		حالت State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_شے_t>(اشیاء[update_اشاریہ].response.شے_2),
						    اشیاء[update_اشاریہ].response.اشاریہ,
						    اشیاء[update_اشاریہ].response.ڈیٹا,
						    g_فی_شے_سائز[اشیاء[update_اشاریہ].response.شے_2]);

			اگر (success) {
				اشیاء[update_اشاریہ].cache_state = State::Request_بھیجا;

			} ورنہ {
				اشیاء[update_اشاریہ].cache_state = State::Error;
			}

			توڑو;

		حالت State::Request_بھیجا:

			اگر (_client.آخری_Operation_Completed(response_success)) {

				اگر (response_success) {

					اشیاء[update_اشاریہ].cache_state = State::ResponseReceived;
					change_Update_اشاریہ();

				} ورنہ {
					اشیاء[update_اشاریہ].cache_state = State::Error;
				}
			}

			توڑو;

		حالت State::Error:
			// Handled below
			توڑو;
		}

		اگر (اشیاء[update_اشاریہ].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				اشیاء[update_اشاریہ].response.شے_2, اشیاء[update_اشاریہ].response.اشاریہ,
				اشیاء[update_اشاریہ].response.حالت_2);

			change_Update_اشاریہ();
		}

	}
}

خالی DatamanCache::resetCacheState()
{
	update_اشاریہ = 0;
	شے_شمار = 0;
	بھرو_اشاریہ = 0;
}

خالی DatamanCache::invalidate()
{
	برائے (uint32_t i = 0; i < عدد_اشیاء_2; ++i) {
		اشیاء[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

اندرونی خالی DatamanCache::change_Update_اشاریہ()
{
	اگر (عدد_اشیاء_2 == 0) {
		update_اشاریہ = 0;
		شے_شمار = 0;
		واپس;
	}

	update_اشاریہ = (update_اشاریہ + 1) % عدد_اشیاء_2;

	اگر (شے_شمار > 0) {
		--شے_شمار;
	}
}
