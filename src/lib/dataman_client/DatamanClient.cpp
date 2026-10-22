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

#ਅਨ੍ਤਰ੍ਭਾਵ <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	ਜੇ (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} ਨਹੀਂਤਾਂ {
		// make sure we don't get any stale response by doing an orb_copy
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

		_fds.fd = _dataman_response_sub;
		_fds.events = POLLIN;

		hrt_abstime timestamp = hrt_absolute_ਸਮਾਂ();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_ਸੈੱਟ;

		ਤਾਰਕਿਕ success = syncHandler(request, response, timestamp, 1000_ms);

		ਜੇ (success && (response.client_id > CLIENT_ID_NOT_ਸੈੱਟ)) {

			_client_id = response.client_id;

		} ਨਹੀਂਤਾਂ {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	ਜੇ (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

ਤਾਰਕਿਕ DatamanClient::syncHandler(ਅਚਲ dataman_request_s &request, dataman_response_s &response,
				ਅਚਲ hrt_abstime &ਸ਼ੁਰੂ_ਸਮਾਂ, hrt_abstime timeout)
{
	ਤਾਰਕਿਕ response_received = ਝੂਠ;
	int32_t ret = 0;
	hrt_abstime ਸਮਾਂ_elapsed = hrt_elapsed_ਸਮਾਂ(&ਸ਼ੁਰੂ_ਸਮਾਂ);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	ਜਦੋਂ (!response_received && (ਸਮਾਂ_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		ਜੇ (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			ਰੋਕੋ;

		} ਨਹੀਂਤਾਂ ਜੇ (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} ਨਹੀਂਤਾਂ {

			ਤਾਰਕਿਕ updated = ਝੂਠ;
			orb_check(_dataman_response_sub, &updated);

			ਜੇ (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				ਜੇ (response.client_id == request.client_id) {

					ਜੇ ((response.request_type == request.request_type) &&
					    (response.ਵਸਤੂ_2 == request.ਵਸਤੂ_2) &&
					    (response.ਸੂਚਕਾਂਕ == request.ਸੂਚਕਾਂਕ)) {
						response_received = ਸੱਚ;
						ਰੋਕੋ;
					}

				} ਨਹੀਂਤਾਂ ਜੇ (request.client_id == CLIENT_ID_NOT_ਸੈੱਟ) {

					// validate timestamp from response.data
					ਜੇ (0 == memcmp(&(request.timestamp), &(response.ਅੰਕੜੇ), ਆਕਾਰ(hrt_abstime))) {
						response_received = ਸੱਚ;
						ਰੋਕੋ;
					}
				}
			}
		}

		ਸਮਾਂ_elapsed = hrt_elapsed_ਸਮਾਂ(&ਸ਼ੁਰੂ_ਸਮਾਂ);
	}

	perf_ਸਿਰਾ(_sync_perf);

	ਜੇ (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	ਵਾਪਸ response_received;
}

ਤਾਰਕਿਕ DatamanClient::readSync(dm_ਵਸਤੂ_t ਵਸਤੂ_2, uint32_t ਸੂਚਕਾਂਕ, uint8_t *ਬਫਰ_2, uint32_t ਲੰਮਾਈ, hrt_abstime timeout)
{
	ਜੇ (_client_id == CLIENT_ID_NOT_ਸੈੱਟ) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (ਲੰਮਾਈ > g_ਪ੍ਰਤੀ_ਵਸਤੂ_ਮਾਪ[ਵਸਤੂ_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ਲੰਮਾਈ, static_cast<uint8_t>(ਵਸਤੂ_2));
		ਵਾਪਸ ਝੂਠ;
	}

	hrt_abstime timestamp = hrt_absolute_ਸਮਾਂ();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
	request.ਅੰਕੜੇ_ਲੰਮਾਈ = ਲੰਮਾਈ;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.ਵਸਤੂ_2 = static_cast<uint8_t>(ਵਸਤੂ_2);

	dataman_response_s response{};
	ਤਾਰਕਿਕ success = syncHandler(request, response, timestamp, timeout);

	ਜੇ (success) {

		ਜੇ (response.ਸਥਿਤੀ != dataman_response_s::ਸਥਿਤੀ_SUCCESS) {

			success = ਝੂਠ;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.ਸਥਿਤੀ, static_cast<uint8_t>(ਵਸਤੂ_2), ਸੂਚਕਾਂਕ, ਲੰਮਾਈ);

		} ਨਹੀਂਤਾਂ {
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਫਰ_2, response.ਅੰਕੜੇ, ਲੰਮਾਈ);
		}
	}

	ਵਾਪਸ success;
}

ਤਾਰਕਿਕ DatamanClient::writeSync(dm_ਵਸਤੂ_t ਵਸਤੂ_2, uint32_t ਸੂਚਕਾਂਕ, uint8_t *ਬਫਰ_2, uint32_t ਲੰਮਾਈ, hrt_abstime timeout)
{
	ਜੇ (_client_id == CLIENT_ID_NOT_ਸੈੱਟ) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (ਲੰਮਾਈ > g_ਪ੍ਰਤੀ_ਵਸਤੂ_ਮਾਪ[ਵਸਤੂ_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ਲੰਮਾਈ, static_cast<uint8_t>(ਵਸਤੂ_2));
		ਵਾਪਸ ਝੂਠ;
	}

	hrt_abstime timestamp = hrt_absolute_ਸਮਾਂ();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
	request.ਅੰਕੜੇ_ਲੰਮਾਈ = ਲੰਮਾਈ;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.ਵਸਤੂ_2 = static_cast<uint8_t>(ਵਸਤੂ_2);

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(request.ਅੰਕੜੇ, ਬਫਰ_2, ਲੰਮਾਈ);

	dataman_response_s response{};
	ਤਾਰਕਿਕ success = syncHandler(request, response, timestamp, timeout);

	ਜੇ (success) {

		ਜੇ (response.ਸਥਿਤੀ != dataman_response_s::ਸਥਿਤੀ_SUCCESS) {

			success = ਝੂਠ;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.ਸਥਿਤੀ, static_cast<uint8_t>(ਵਸਤੂ_2), ਸੂਚਕਾਂਕ, ਲੰਮਾਈ);
		}
	}

	ਵਾਪਸ success;
}

ਤਾਰਕਿਕ DatamanClient::clearSync(dm_ਵਸਤੂ_t ਵਸਤੂ_2, hrt_abstime timeout)
{
	ਜੇ (_client_id == CLIENT_ID_NOT_ਸੈੱਟ) {
		ਵਾਪਸ ਝੂਠ;
	}

	hrt_abstime timestamp = hrt_absolute_ਸਮਾਂ();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.ਵਸਤੂ_2 = static_cast<uint8_t>(ਵਸਤੂ_2);

	dataman_response_s response{};
	ਤਾਰਕਿਕ success = syncHandler(request, response, timestamp, timeout);

	ਜੇ (success) {

		ਜੇ (response.ਸਥਿਤੀ != dataman_response_s::ਸਥਿਤੀ_SUCCESS) {

			success = ਝੂਠ;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.ਸਥਿਤੀ, static_cast<uint8_t>(ਵਸਤੂ_2));
		}
	}

	ਵਾਪਸ success;
}

ਖਾਲੀ DatamanClient::clearPendingResponse()
{
	ਜੇ (_dataman_response_sub < 0) {
		ਵਾਪਸ;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	ਤਾਰਕਿਕ updated = ਝੂਠ;
	orb_check(_dataman_response_sub, &updated);

	ਜਦੋਂ (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

ਤਾਰਕਿਕ DatamanClient::readAsync(dm_ਵਸਤੂ_t ਵਸਤੂ_2, uint32_t ਸੂਚਕਾਂਕ, uint8_t *ਬਫਰ_2, uint32_t ਲੰਮਾਈ)
{
	ਜੇ (_client_id == CLIENT_ID_NOT_ਸੈੱਟ) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (ਲੰਮਾਈ > g_ਪ੍ਰਤੀ_ਵਸਤੂ_ਮਾਪ[ਵਸਤੂ_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ਲੰਮਾਈ, static_cast<uint8_t>(ਵਸਤੂ_2));
		ਵਾਪਸ ਝੂਠ;
	}

	ਤਾਰਕਿਕ success = ਝੂਠ;

	ਜੇ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ਸਮਾਂ();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
		request.ਅੰਕੜੇ_ਲੰਮਾਈ = ਲੰਮਾਈ;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.ਵਸਤੂ_2 = static_cast<uint8_t>(ਵਸਤੂ_2);

		ਸਰਗਰਮ_request.timestamp = timestamp;
		ਸਰਗਰਮ_request.request_type = DM_READ;
		ਸਰਗਰਮ_request.ਵਸਤੂ_2 = ਵਸਤੂ_2;
		ਸਰਗਰਮ_request.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
		ਸਰਗਰਮ_request.ਬਫਰ_2 = ਬਫਰ_2;
		ਸਰਗਰਮ_request.ਲੰਮਾਈ = ਲੰਮਾਈ;

		_state = State::Request_ਭੇਜਿਆ;

		_dataman_request_pub.publish(request);

		success = ਸੱਚ;
	}

	ਵਾਪਸ success;
}

ਤਾਰਕਿਕ DatamanClient::writeAsync(dm_ਵਸਤੂ_t ਵਸਤੂ_2, uint32_t ਸੂਚਕਾਂਕ, uint8_t *ਬਫਰ_2, uint32_t ਲੰਮਾਈ)
{
	ਜੇ (_client_id == CLIENT_ID_NOT_ਸੈੱਟ) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (ਲੰਮਾਈ > g_ਪ੍ਰਤੀ_ਵਸਤੂ_ਮਾਪ[ਵਸਤੂ_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ਲੰਮਾਈ, static_cast<uint8_t>(ਵਸਤੂ_2));
		ਵਾਪਸ ਝੂਠ;
	}

	ਤਾਰਕਿਕ success = ਝੂਠ;

	ਜੇ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ਸਮਾਂ();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
		request.ਅੰਕੜੇ_ਲੰਮਾਈ = ਲੰਮਾਈ;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.ਵਸਤੂ_2 = static_cast<uint8_t>(ਵਸਤੂ_2);

		ਸਮ੍ਰਿਤੀ_ਨਕਲ(request.ਅੰਕੜੇ, ਬਫਰ_2, ਲੰਮਾਈ);

		ਸਰਗਰਮ_request.timestamp = timestamp;
		ਸਰਗਰਮ_request.request_type = DM_WRITE;
		ਸਰਗਰਮ_request.ਵਸਤੂ_2 = ਵਸਤੂ_2;
		ਸਰਗਰਮ_request.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
		ਸਰਗਰਮ_request.ਬਫਰ_2 = ਬਫਰ_2;
		ਸਰਗਰਮ_request.ਲੰਮਾਈ = ਲੰਮਾਈ;

		_state = State::Request_ਭੇਜਿਆ;

		_dataman_request_pub.publish(request);

		success = ਸੱਚ;
	}

	ਵਾਪਸ success;
}

ਤਾਰਕਿਕ DatamanClient::clearAsync(dm_ਵਸਤੂ_t ਵਸਤੂ_2)
{
	ਜੇ (_client_id == CLIENT_ID_NOT_ਸੈੱਟ) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਤਾਰਕਿਕ success = ਝੂਠ;

	ਜੇ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ਸਮਾਂ();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.ਵਸਤੂ_2 = static_cast<uint8_t>(ਵਸਤੂ_2);
		request.ਸੂਚਕਾਂਕ = 0;

		ਸਰਗਰਮ_request.timestamp = timestamp;
		ਸਰਗਰਮ_request.request_type = DM_CLEAR;
		ਸਰਗਰਮ_request.ਵਸਤੂ_2 = ਵਸਤੂ_2;
		ਸਰਗਰਮ_request.ਸੂਚਕਾਂਕ = request.ਸੂਚਕਾਂਕ;
		_state = State::Request_ਭੇਜਿਆ;

		_dataman_request_pub.publish(request);

		success = ਸੱਚ;
	}

	ਵਾਪਸ success;
}

ਖਾਲੀ DatamanClient::update()
{
	ਜੇ (_state == State::Request_ਭੇਜਿਆ) {

		ਤਾਰਕਿਕ updated = ਝੂਠ;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		ਜੇ (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			ਜੇ ((response.client_id == _client_id) &&
			    (response.request_type == ਸਰਗਰਮ_request.request_type) &&
			    (response.ਵਸਤੂ_2 == ਸਰਗਰਮ_request.ਵਸਤੂ_2) &&
			    (response.ਸੂਚਕਾਂਕ == ਸਰਗਰਮ_request.ਸੂਚਕਾਂਕ)) {

				ਜੇ (response.request_type == DM_READ) {
					ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਸਰਗਰਮ_request.ਬਫਰ_2, response.ਅੰਕੜੇ, ਸਰਗਰਮ_request.ਲੰਮਾਈ);
				}

				response_ਸਥਿਤੀ = response.ਸਥਿਤੀ;

				ਜੇ (response_ਸਥਿਤੀ != dataman_response_s::ਸਥਿਤੀ_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.ਸਥਿਤੀ, static_cast<uint8_t>(ਸਰਗਰਮ_request.ਵਸਤੂ_2), ਸਰਗਰਮ_request.ਸੂਚਕਾਂਕ);
				}

				_state = State::ResponseReceived;
			}
		}

		ਜੇ (_state == State::Request_ਭੇਜਿਆ) {

			/* Retry the request if there is no answer */
			ਜੇ (((ਸਰਗਰਮ_request.request_type != DM_CLEAR) && (hrt_elapsed_ਸਮਾਂ(&ਸਰਗਰਮ_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_ਸਮਾਂ(&ਸਰਗਰਮ_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_ਸਮਾਂ();

				ਸਰਗਰਮ_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.ਸੂਚਕਾਂਕ = ਸਰਗਰਮ_request.ਸੂਚਕਾਂਕ;
				request.ਅੰਕੜੇ_ਲੰਮਾਈ = ਸਰਗਰਮ_request.ਲੰਮਾਈ;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(ਸਰਗਰਮ_request.request_type);
				request.ਵਸਤੂ_2 = static_cast<uint8_t>(ਸਰਗਰਮ_request.ਵਸਤੂ_2);

				ਜੇ (ਸਰਗਰਮ_request.request_type == DM_WRITE) {
					ਸਮ੍ਰਿਤੀ_ਨਕਲ(request.ਅੰਕੜੇ, ਸਰਗਰਮ_request.ਬਫਰ_2, ਸਰਗਰਮ_request.ਲੰਮਾਈ);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_ਭੇਜਿਆ;
			}
		}
	}
}

ਤਾਰਕਿਕ DatamanClient::ਆਖਰੀ_Operation_Completed(ਤਾਰਕਿਕ &success)
{
	ਤਾਰਕਿਕ completed = ਝੂਠ;
	success = ਝੂਠ;

	ਜੇ (_state == State::ResponseReceived) {

		ਜੇ (response_ਸਥਿਤੀ == dataman_response_s::ਸਥਿਤੀ_SUCCESS) {
			success = ਸੱਚ;
		}

		_state = State::Idle;
		completed = ਸੱਚ;
	}

	ਵਾਪਸ completed;
}

ਖਾਲੀ DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ਅਚਲ ਅੱਖਰ *cache_ਖੁੰਝ_perf_counter_name, uint32_t ਅੰਕ_ਚੀਜ਼ਾਂ)
	: cache_ਖੁੰਝ_perf(perf_alloc(PC_ਗਿਣਤੀ, cache_ਖੁੰਝ_perf_counter_name))
{
	ਜੇ (ਅੰਕ_ਚੀਜ਼ਾਂ == 0) {
		ਵਾਪਸ;
	}

	ਚੀਜ਼ਾਂ = ਨਵਾਂ ਵਸਤੂ_3[ਅੰਕ_ਚੀਜ਼ਾਂ] {};

	ਜੇ (ਚੀਜ਼ਾਂ != ਖਾਲੀ_ਸੂਚਕ) {
		ਅੰਕ_ਚੀਜ਼ਾਂ_2 = ਅੰਕ_ਚੀਜ਼ਾਂ;

	} ਨਹੀਂਤਾਂ {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	ਮਿਟਾਓ[] ਚੀਜ਼ਾਂ;
	perf_free(cache_ਖੁੰਝ_perf);
}

ਖਾਲੀ DatamanCache::resize(uint32_t ਅੰਕ_ਚੀਜ਼ਾਂ)
{
	ਜੇ (ਅੰਕ_ਚੀਜ਼ਾਂ == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		ਮਿਟਾਓ[] ਚੀਜ਼ਾਂ;
		ਚੀਜ਼ਾਂ = ਖਾਲੀ_ਸੂਚਕ;
		resetCacheState();
		ਅੰਕ_ਚੀਜ਼ਾਂ_2 = 0;
		ਵਾਪਸ;
	}

	ਵਸਤੂ_3 *new_ਚੀਜ਼ਾਂ = ਨਵਾਂ ਵਸਤੂ_3[ਅੰਕ_ਚੀਜ਼ਾਂ] {};

	ਜੇ (new_ਚੀਜ਼ਾਂ != ਖਾਲੀ_ਸੂਚਕ) {
		_client.abortCurrentOperation();

		uint32_t ਅੰਕ_min = ਅੰਕ_ਚੀਜ਼ਾਂ < ਅੰਕ_ਚੀਜ਼ਾਂ_2 ? ਅੰਕ_ਚੀਜ਼ਾਂ : ਅੰਕ_ਚੀਜ਼ਾਂ_2;

		ਲਈ (uint32_t i = 0; i < ਅੰਕ_min; ++i) {
			new_ਚੀਜ਼ਾਂ[i] = ਚੀਜ਼ਾਂ[i];

			ਜੇ (new_ਚੀਜ਼ਾਂ[i].cache_state == State::Request_ਭੇਜਿਆ) {
				new_ਚੀਜ਼ਾਂ[i].cache_state = State::RequestPrepared;
			}
		}

		ਮਿਟਾਓ[] ਚੀਜ਼ਾਂ;
		ਚੀਜ਼ਾਂ = new_ਚੀਜ਼ਾਂ;
		ਅੰਕ_ਚੀਜ਼ਾਂ_2 = ਅੰਕ_ਚੀਜ਼ਾਂ;

		// ensure new bounds are respected after resize
		ਲੱਦੋ_ਸੂਚਕਾਂਕ = (ਲੱਦੋ_ਸੂਚਕਾਂਕ < ਅੰਕ_ਚੀਜ਼ਾਂ_2) ? ਲੱਦੋ_ਸੂਚਕਾਂਕ : 0;
		update_ਸੂਚਕਾਂਕ = (update_ਸੂਚਕਾਂਕ < ਅੰਕ_ਚੀਜ਼ਾਂ_2) ? update_ਸੂਚਕਾਂਕ : 0;
		ਵਸਤੂ_counter = (ਵਸਤੂ_counter < ਅੰਕ_ਚੀਜ਼ਾਂ_2) ? ਵਸਤੂ_counter : ਅੰਕ_ਚੀਜ਼ਾਂ_2;

	} ਨਹੀਂਤਾਂ {
		PX4_ERR("alloc failed");
	}
}

ਤਾਰਕਿਕ DatamanCache::ਲੱਦੋ(dm_ਵਸਤੂ_t ਵਸਤੂ_2, uint32_t ਸੂਚਕਾਂਕ)
{
	ਜੇ (!ਚੀਜ਼ਾਂ) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਤਾਰਕਿਕ success = ਝੂਠ;
	ਤਾਰਕਿਕ duplicate = ਝੂਠ;

	//Prevent duplicates
	ਲਈ (uint32_t i = 0; i < ਅੰਕ_ਚੀਜ਼ਾਂ_2; ++i) {

		ਜੇ (ਚੀਜ਼ਾਂ[i].cache_state != State::Idle &&
		    ਚੀਜ਼ਾਂ[i].cache_state != State::Error &&
		    ਚੀਜ਼ਾਂ[i].response.ਵਸਤੂ_2 == ਵਸਤੂ_2 &&
		    ਚੀਜ਼ਾਂ[i].response.ਸੂਚਕਾਂਕ == ਸੂਚਕਾਂਕ) {
			duplicate = ਸੱਚ;
			ਰੋਕੋ;
		}
	}

	ਜੇ (!duplicate && (ਵਸਤੂ_counter < ਅੰਕ_ਚੀਜ਼ਾਂ_2)) {

		ਚੀਜ਼ਾਂ[ਲੱਦੋ_ਸੂਚਕਾਂਕ].cache_state = State::RequestPrepared;
		ਚੀਜ਼ਾਂ[ਲੱਦੋ_ਸੂਚਕਾਂਕ].response.ਵਸਤੂ_2 = ਵਸਤੂ_2;
		ਚੀਜ਼ਾਂ[ਲੱਦੋ_ਸੂਚਕਾਂਕ].response.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;

		ਲੱਦੋ_ਸੂਚਕਾਂਕ = (ਲੱਦੋ_ਸੂਚਕਾਂਕ + 1) % ਅੰਕ_ਚੀਜ਼ਾਂ_2;

		++ਵਸਤੂ_counter;

		success = ਸੱਚ;
	}

	ਵਾਪਸ success;
}

ਤਾਰਕਿਕ DatamanCache::ਲੱਦੋ_Wait(dm_ਵਸਤੂ_t ਵਸਤੂ_2, uint32_t ਸੂਚਕਾਂਕ, uint8_t *ਬਫਰ_2, uint32_t ਲੰਮਾਈ, hrt_abstime timeout)
{
	ਜੇ (ਲੰਮਾਈ > g_ਪ੍ਰਤੀ_ਵਸਤੂ_ਮਾਪ[ਵਸਤੂ_2]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, ਲੰਮਾਈ, static_cast<uint8_t>(ਵਸਤੂ_2));
		ਵਾਪਸ ਝੂਠ;
	}

	ਤਾਰਕਿਕ success = ਝੂਠ;
	ਤਾਰਕਿਕ ਵਸਤੂ_ਮਿਲਿਆ = ਝੂਠ;

	ਜੇ (ਚੀਜ਼ਾਂ) {
		ਲਈ (uint32_t i = 0; i < ਅੰਕ_ਚੀਜ਼ਾਂ_2; ++i) {
			ਜੇ ((ਚੀਜ਼ਾਂ[i].response.ਵਸਤੂ_2 == ਵਸਤੂ_2) &&
			    (ਚੀਜ਼ਾਂ[i].response.ਸੂਚਕਾਂਕ == ਸੂਚਕਾਂਕ)) {
				ਵਸਤੂ_ਮਿਲਿਆ = ਸੱਚ;

				ਜੇ (ਚੀਜ਼ਾਂ[i].cache_state == State::ResponseReceived) {
					ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਫਰ_2, ਚੀਜ਼ਾਂ[i].response.ਅੰਕੜੇ, ਲੰਮਾਈ);
					success = ਸੱਚ;
					ਰੋਕੋ;
				}
			}
		}
	}

	ਜੇ (!success) {
		perf_ਗਿਣਤੀ(cache_ਖੁੰਝ_perf);
	}

	ਜੇ (!success && (timeout > 0)) {
		success = _client.readSync(ਵਸਤੂ_2, ਸੂਚਕਾਂਕ, ਬਫਰ_2, ਲੰਮਾਈ, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		ਜੇ (success && !ਵਸਤੂ_ਮਿਲਿਆ && ਚੀਜ਼ਾਂ && (ਵਸਤੂ_counter < ਅੰਕ_ਚੀਜ਼ਾਂ_2)) {
			ਚੀਜ਼ਾਂ[ਲੱਦੋ_ਸੂਚਕਾਂਕ].cache_state = State::ResponseReceived;
			ਚੀਜ਼ਾਂ[ਲੱਦੋ_ਸੂਚਕਾਂਕ].response.ਵਸਤੂ_2 = ਵਸਤੂ_2;
			ਚੀਜ਼ਾਂ[ਲੱਦੋ_ਸੂਚਕਾਂਕ].response.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਚੀਜ਼ਾਂ[ਲੱਦੋ_ਸੂਚਕਾਂਕ].response.ਅੰਕੜੇ, ਬਫਰ_2, ਲੰਮਾਈ);

			ਲੱਦੋ_ਸੂਚਕਾਂਕ = (ਲੱਦੋ_ਸੂਚਕਾਂਕ + 1) % ਅੰਕ_ਚੀਜ਼ਾਂ_2;

			++ਵਸਤੂ_counter; // Still increase the counter here
		}
	}

	ਵਾਪਸ success;
}

ਤਾਰਕਿਕ DatamanCache::writeWait(dm_ਵਸਤੂ_t ਵਸਤੂ_2, uint32_t ਸੂਚਕਾਂਕ, uint8_t *ਬਫਰ_2, uint32_t ਲੰਮਾਈ, hrt_abstime timeout)
{
	ਜੇ (ਲੰਮਾਈ > g_ਪ੍ਰਤੀ_ਵਸਤੂ_ਮਾਪ[ਵਸਤੂ_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ਲੰਮਾਈ, static_cast<uint8_t>(ਵਸਤੂ_2));
		ਵਾਪਸ ਝੂਠ;
	}

	ਤਾਰਕਿਕ success = _client.writeSync(ਵਸਤੂ_2, ਸੂਚਕਾਂਕ, ਬਫਰ_2, ਲੰਮਾਈ, timeout);

	ਜੇ (success && ਚੀਜ਼ਾਂ) {
		ਲਈ (uint32_t i = 0; i < ਅੰਕ_ਚੀਜ਼ਾਂ_2; ++i) {
			ਜੇ ((ਚੀਜ਼ਾਂ[i].response.ਵਸਤੂ_2 == ਵਸਤੂ_2) &&
			    (ਚੀਜ਼ਾਂ[i].response.ਸੂਚਕਾਂਕ == ਸੂਚਕਾਂਕ) &&
			    ((ਚੀਜ਼ਾਂ[i].cache_state == State::ResponseReceived) ||
			     (ਚੀਜ਼ਾਂ[i].cache_state == State::RequestPrepared))) {

				ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਚੀਜ਼ਾਂ[i].response.ਅੰਕੜੇ, ਬਫਰ_2, ਲੰਮਾਈ);
				ਚੀਜ਼ਾਂ[i].cache_state = State::ResponseReceived;
				ਰੋਕੋ;
			}
		}
	}

	ਵਾਪਸ success;
}

ਤਾਰਕਿਕ DatamanCache::update_Cached_ਵਸਤੂ(dm_ਵਸਤੂ_t ਵਸਤੂ_2, uint32_t ਸੂਚਕਾਂਕ, ਅਚਲ uint8_t *ਬਫਰ_2, uint32_t ਲੰਮਾਈ)
{
	ਜੇ (!ਚੀਜ਼ਾਂ || ਬਫਰ_2 == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕ_ਚੀਜ਼ਾਂ_2 == 0) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (ਲੰਮਾਈ > g_ਪ੍ਰਤੀ_ਵਸਤੂ_ਮਾਪ[ਵਸਤੂ_2]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ਲੰਮਾਈ, static_cast<uint8_t>(ਵਸਤੂ_2));
		ਵਾਪਸ ਝੂਠ;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	ਲਈ (uint32_t i = 0; i < ਅੰਕ_ਚੀਜ਼ਾਂ_2; ++i) {
		ਜੇ ((ਚੀਜ਼ਾਂ[i].response.ਵਸਤੂ_2 == ਵਸਤੂ_2) && (ਚੀਜ਼ਾਂ[i].response.ਸੂਚਕਾਂਕ == ਸੂਚਕਾਂਕ)
		    && (ਚੀਜ਼ਾਂ[i].cache_state == State::ResponseReceived)) {
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਚੀਜ਼ਾਂ[i].response.ਅੰਕੜੇ, ਬਫਰ_2, ਲੰਮਾਈ);
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ DatamanCache::update()
{
	ਜੇ (ਵਸਤੂ_counter > 0) {

		_client.update();

		ਤਾਰਕਿਕ success = ਝੂਠ;
		ਤਾਰਕਿਕ response_success = ਝੂਠ;

		ਚੋਣ (ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].cache_state) {
		ਹਾਲਤ State::Idle:
			ਰੋਕੋ;

		ਹਾਲਤ State::ResponseReceived:
			// Skip it
			change_Update_ਸੂਚਕਾਂਕ();
			ਰੋਕੋ;

		ਹਾਲਤ State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_ਵਸਤੂ_t>(ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].response.ਵਸਤੂ_2),
						    ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].response.ਸੂਚਕਾਂਕ,
						    ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].response.ਅੰਕੜੇ,
						    g_ਪ੍ਰਤੀ_ਵਸਤੂ_ਮਾਪ[ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].response.ਵਸਤੂ_2]);

			ਜੇ (success) {
				ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].cache_state = State::Request_ਭੇਜਿਆ;

			} ਨਹੀਂਤਾਂ {
				ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].cache_state = State::Error;
			}

			ਰੋਕੋ;

		ਹਾਲਤ State::Request_ਭੇਜਿਆ:

			ਜੇ (_client.ਆਖਰੀ_Operation_Completed(response_success)) {

				ਜੇ (response_success) {

					ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].cache_state = State::ResponseReceived;
					change_Update_ਸੂਚਕਾਂਕ();

				} ਨਹੀਂਤਾਂ {
					ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].cache_state = State::Error;
				}
			}

			ਰੋਕੋ;

		ਹਾਲਤ State::Error:
			// Handled below
			ਰੋਕੋ;
		}

		ਜੇ (ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].response.ਵਸਤੂ_2, ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].response.ਸੂਚਕਾਂਕ,
				ਚੀਜ਼ਾਂ[update_ਸੂਚਕਾਂਕ].response.ਸਥਿਤੀ);

			change_Update_ਸੂਚਕਾਂਕ();
		}

	}
}

ਖਾਲੀ DatamanCache::resetCacheState()
{
	update_ਸੂਚਕਾਂਕ = 0;
	ਵਸਤੂ_counter = 0;
	ਲੱਦੋ_ਸੂਚਕਾਂਕ = 0;
}

ਖਾਲੀ DatamanCache::invalidate()
{
	ਲਈ (uint32_t i = 0; i < ਅੰਕ_ਚੀਜ਼ਾਂ_2; ++i) {
		ਚੀਜ਼ਾਂ[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

ਸਤਰਅੰਦਰ ਖਾਲੀ DatamanCache::change_Update_ਸੂਚਕਾਂਕ()
{
	ਜੇ (ਅੰਕ_ਚੀਜ਼ਾਂ_2 == 0) {
		update_ਸੂਚਕਾਂਕ = 0;
		ਵਸਤੂ_counter = 0;
		ਵਾਪਸ;
	}

	update_ਸੂਚਕਾਂਕ = (update_ਸੂਚਕਾਂਕ + 1) % ਅੰਕ_ਚੀਜ਼ਾਂ_2;

	ਜੇ (ਵਸਤੂ_counter > 0) {
		--ਵਸਤੂ_counter;
	}
}
