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

#ಅನ್ತರ್ಭಾವ <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	ಆದರೆ (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} ಇಲ್ಲದಿದ್ದರೆ {
		// make sure we don't get any stale response by doing an orb_copy
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

		_fds.fd = _dataman_response_sub;
		_fds.events = POLLIN;

		hrt_abstime timestamp = hrt_absolute_ಸಮಯ();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_ಗಣ;

		ತಾರ್ಕಿಕ success = syncHandler(request, response, timestamp, 1000_ms);

		ಆದರೆ (success && (response.client_id > CLIENT_ID_NOT_ಗಣ)) {

			_client_id = response.client_id;

		} ಇಲ್ಲದಿದ್ದರೆ {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	ಆದರೆ (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

ತಾರ್ಕಿಕ DatamanClient::syncHandler(ಸ್ಥಿರಾಂಕ dataman_request_s &request, dataman_response_s &response,
				ಸ್ಥಿರಾಂಕ hrt_abstime &ಆರಂಭ_ಸಮಯ, hrt_abstime timeout)
{
	ತಾರ್ಕಿಕ response_received = ಸುಳ್ಳು;
	int32_t ret = 0;
	hrt_abstime ಸಮಯ_elapsed = hrt_elapsed_ಸಮಯ(&ಆರಂಭ_ಸಮಯ);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	ಆಗ (!response_received && (ಸಮಯ_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		ಆದರೆ (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			ನಿಲ್ಲಿಸು;

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} ಇಲ್ಲದಿದ್ದರೆ {

			ತಾರ್ಕಿಕ updated = ಸುಳ್ಳು;
			orb_check(_dataman_response_sub, &updated);

			ಆದರೆ (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				ಆದರೆ (response.client_id == request.client_id) {

					ಆದರೆ ((response.request_type == request.request_type) &&
					    (response.ಅಂಶ == request.ಅಂಶ) &&
					    (response.ಸೂಚ್ಯಂಕ == request.ಸೂಚ್ಯಂಕ)) {
						response_received = ನಿಜ;
						ನಿಲ್ಲಿಸು;
					}

				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (request.client_id == CLIENT_ID_NOT_ಗಣ) {

					// validate timestamp from response.data
					ಆದರೆ (0 == memcmp(&(request.timestamp), &(response.ದತ್ತಾಂಶ), ಗಾತ್ರ(hrt_abstime))) {
						response_received = ನಿಜ;
						ನಿಲ್ಲಿಸು;
					}
				}
			}
		}

		ಸಮಯ_elapsed = hrt_elapsed_ಸಮಯ(&ಆರಂಭ_ಸಮಯ);
	}

	perf_ಕೊನೆ(_sync_perf);

	ಆದರೆ (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	ಹಿಂತಿರುಗಿ response_received;
}

ತಾರ್ಕಿಕ DatamanClient::readSync(dm_ಅಂಶ_t ಅಂಶ, uint32_t ಸೂಚ್ಯಂಕ, uint8_t *ಬಫರ್_2, uint32_t ಉದ್ದಳತೆ, hrt_abstime timeout)
{
	ಆದರೆ (_client_id == CLIENT_ID_NOT_ಗಣ) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (ಉದ್ದಳತೆ > g_ಪ್ರತಿ_ಅಂಶ_ಗಾತ್ರ[ಅಂಶ]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ಉದ್ದಳತೆ, static_cast<uint8_t>(ಅಂಶ));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	hrt_abstime timestamp = hrt_absolute_ಸಮಯ();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
	request.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.ಅಂಶ = static_cast<uint8_t>(ಅಂಶ);

	dataman_response_s response{};
	ತಾರ್ಕಿಕ success = syncHandler(request, response, timestamp, timeout);

	ಆದರೆ (success) {

		ಆದರೆ (response.ಸ್ಥಿತಿ_2 != dataman_response_s::ಸ್ಥಿತಿ_SUCCESS) {

			success = ಸುಳ್ಳು;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.ಸ್ಥಿತಿ_2, static_cast<uint8_t>(ಅಂಶ), ಸೂಚ್ಯಂಕ, ಉದ್ದಳತೆ);

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸ್ಮರಣೆ_ನಕಲು(ಬಫರ್_2, response.ದತ್ತಾಂಶ, ಉದ್ದಳತೆ);
		}
	}

	ಹಿಂತಿರುಗಿ success;
}

ತಾರ್ಕಿಕ DatamanClient::writeSync(dm_ಅಂಶ_t ಅಂಶ, uint32_t ಸೂಚ್ಯಂಕ, uint8_t *ಬಫರ್_2, uint32_t ಉದ್ದಳತೆ, hrt_abstime timeout)
{
	ಆದರೆ (_client_id == CLIENT_ID_NOT_ಗಣ) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (ಉದ್ದಳತೆ > g_ಪ್ರತಿ_ಅಂಶ_ಗಾತ್ರ[ಅಂಶ]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ಉದ್ದಳತೆ, static_cast<uint8_t>(ಅಂಶ));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	hrt_abstime timestamp = hrt_absolute_ಸಮಯ();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
	request.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.ಅಂಶ = static_cast<uint8_t>(ಅಂಶ);

	ಸ್ಮರಣೆ_ನಕಲು(request.ದತ್ತಾಂಶ, ಬಫರ್_2, ಉದ್ದಳತೆ);

	dataman_response_s response{};
	ತಾರ್ಕಿಕ success = syncHandler(request, response, timestamp, timeout);

	ಆದರೆ (success) {

		ಆದರೆ (response.ಸ್ಥಿತಿ_2 != dataman_response_s::ಸ್ಥಿತಿ_SUCCESS) {

			success = ಸುಳ್ಳು;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.ಸ್ಥಿತಿ_2, static_cast<uint8_t>(ಅಂಶ), ಸೂಚ್ಯಂಕ, ಉದ್ದಳತೆ);
		}
	}

	ಹಿಂತಿರುಗಿ success;
}

ತಾರ್ಕಿಕ DatamanClient::clearSync(dm_ಅಂಶ_t ಅಂಶ, hrt_abstime timeout)
{
	ಆದರೆ (_client_id == CLIENT_ID_NOT_ಗಣ) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	hrt_abstime timestamp = hrt_absolute_ಸಮಯ();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.ಅಂಶ = static_cast<uint8_t>(ಅಂಶ);

	dataman_response_s response{};
	ತಾರ್ಕಿಕ success = syncHandler(request, response, timestamp, timeout);

	ಆದರೆ (success) {

		ಆದರೆ (response.ಸ್ಥಿತಿ_2 != dataman_response_s::ಸ್ಥಿತಿ_SUCCESS) {

			success = ಸುಳ್ಳು;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.ಸ್ಥಿತಿ_2, static_cast<uint8_t>(ಅಂಶ));
		}
	}

	ಹಿಂತಿರುಗಿ success;
}

ಶೂನ್ಯ DatamanClient::clearPendingResponse()
{
	ಆದರೆ (_dataman_response_sub < 0) {
		ಹಿಂತಿರುಗಿ;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	ತಾರ್ಕಿಕ updated = ಸುಳ್ಳು;
	orb_check(_dataman_response_sub, &updated);

	ಆಗ (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

ತಾರ್ಕಿಕ DatamanClient::readAsync(dm_ಅಂಶ_t ಅಂಶ, uint32_t ಸೂಚ್ಯಂಕ, uint8_t *ಬಫರ್_2, uint32_t ಉದ್ದಳತೆ)
{
	ಆದರೆ (_client_id == CLIENT_ID_NOT_ಗಣ) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (ಉದ್ದಳತೆ > g_ಪ್ರತಿ_ಅಂಶ_ಗಾತ್ರ[ಅಂಶ]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ಉದ್ದಳತೆ, static_cast<uint8_t>(ಅಂಶ));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ತಾರ್ಕಿಕ success = ಸುಳ್ಳು;

	ಆದರೆ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ಸಮಯ();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
		request.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.ಅಂಶ = static_cast<uint8_t>(ಅಂಶ);

		ಸಕ್ರಿಯ_request.timestamp = timestamp;
		ಸಕ್ರಿಯ_request.request_type = DM_READ;
		ಸಕ್ರಿಯ_request.ಅಂಶ = ಅಂಶ;
		ಸಕ್ರಿಯ_request.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
		ಸಕ್ರಿಯ_request.ಬಫರ್_2 = ಬಫರ್_2;
		ಸಕ್ರಿಯ_request.ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;

		_state = State::Request_ಕಳುಹಿಸಿದ;

		_dataman_request_pub.publish(request);

		success = ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ success;
}

ತಾರ್ಕಿಕ DatamanClient::writeAsync(dm_ಅಂಶ_t ಅಂಶ, uint32_t ಸೂಚ್ಯಂಕ, uint8_t *ಬಫರ್_2, uint32_t ಉದ್ದಳತೆ)
{
	ಆದರೆ (_client_id == CLIENT_ID_NOT_ಗಣ) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (ಉದ್ದಳತೆ > g_ಪ್ರತಿ_ಅಂಶ_ಗಾತ್ರ[ಅಂಶ]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ಉದ್ದಳತೆ, static_cast<uint8_t>(ಅಂಶ));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ತಾರ್ಕಿಕ success = ಸುಳ್ಳು;

	ಆದರೆ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ಸಮಯ();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
		request.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.ಅಂಶ = static_cast<uint8_t>(ಅಂಶ);

		ಸ್ಮರಣೆ_ನಕಲು(request.ದತ್ತಾಂಶ, ಬಫರ್_2, ಉದ್ದಳತೆ);

		ಸಕ್ರಿಯ_request.timestamp = timestamp;
		ಸಕ್ರಿಯ_request.request_type = DM_WRITE;
		ಸಕ್ರಿಯ_request.ಅಂಶ = ಅಂಶ;
		ಸಕ್ರಿಯ_request.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
		ಸಕ್ರಿಯ_request.ಬಫರ್_2 = ಬಫರ್_2;
		ಸಕ್ರಿಯ_request.ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;

		_state = State::Request_ಕಳುಹಿಸಿದ;

		_dataman_request_pub.publish(request);

		success = ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ success;
}

ತಾರ್ಕಿಕ DatamanClient::clearAsync(dm_ಅಂಶ_t ಅಂಶ)
{
	ಆದರೆ (_client_id == CLIENT_ID_NOT_ಗಣ) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ತಾರ್ಕಿಕ success = ಸುಳ್ಳು;

	ಆದರೆ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_ಸಮಯ();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.ಅಂಶ = static_cast<uint8_t>(ಅಂಶ);
		request.ಸೂಚ್ಯಂಕ = 0;

		ಸಕ್ರಿಯ_request.timestamp = timestamp;
		ಸಕ್ರಿಯ_request.request_type = DM_CLEAR;
		ಸಕ್ರಿಯ_request.ಅಂಶ = ಅಂಶ;
		ಸಕ್ರಿಯ_request.ಸೂಚ್ಯಂಕ = request.ಸೂಚ್ಯಂಕ;
		_state = State::Request_ಕಳುಹಿಸಿದ;

		_dataman_request_pub.publish(request);

		success = ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ success;
}

ಶೂನ್ಯ DatamanClient::update()
{
	ಆದರೆ (_state == State::Request_ಕಳುಹಿಸಿದ) {

		ತಾರ್ಕಿಕ updated = ಸುಳ್ಳು;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		ಆದರೆ (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			ಆದರೆ ((response.client_id == _client_id) &&
			    (response.request_type == ಸಕ್ರಿಯ_request.request_type) &&
			    (response.ಅಂಶ == ಸಕ್ರಿಯ_request.ಅಂಶ) &&
			    (response.ಸೂಚ್ಯಂಕ == ಸಕ್ರಿಯ_request.ಸೂಚ್ಯಂಕ)) {

				ಆದರೆ (response.request_type == DM_READ) {
					ಸ್ಮರಣೆ_ನಕಲು(ಸಕ್ರಿಯ_request.ಬಫರ್_2, response.ದತ್ತಾಂಶ, ಸಕ್ರಿಯ_request.ಉದ್ದಳತೆ);
				}

				response_ಸ್ಥಿತಿ = response.ಸ್ಥಿತಿ_2;

				ಆದರೆ (response_ಸ್ಥಿತಿ != dataman_response_s::ಸ್ಥಿತಿ_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.ಸ್ಥಿತಿ_2, static_cast<uint8_t>(ಸಕ್ರಿಯ_request.ಅಂಶ), ಸಕ್ರಿಯ_request.ಸೂಚ್ಯಂಕ);
				}

				_state = State::ResponseReceived;
			}
		}

		ಆದರೆ (_state == State::Request_ಕಳುಹಿಸಿದ) {

			/* Retry the request if there is no answer */
			ಆದರೆ (((ಸಕ್ರಿಯ_request.request_type != DM_CLEAR) && (hrt_elapsed_ಸಮಯ(&ಸಕ್ರಿಯ_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_ಸಮಯ(&ಸಕ್ರಿಯ_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_ಸಮಯ();

				ಸಕ್ರಿಯ_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.ಸೂಚ್ಯಂಕ = ಸಕ್ರಿಯ_request.ಸೂಚ್ಯಂಕ;
				request.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ಸಕ್ರಿಯ_request.ಉದ್ದಳತೆ;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(ಸಕ್ರಿಯ_request.request_type);
				request.ಅಂಶ = static_cast<uint8_t>(ಸಕ್ರಿಯ_request.ಅಂಶ);

				ಆದರೆ (ಸಕ್ರಿಯ_request.request_type == DM_WRITE) {
					ಸ್ಮರಣೆ_ನಕಲು(request.ದತ್ತಾಂಶ, ಸಕ್ರಿಯ_request.ಬಫರ್_2, ಸಕ್ರಿಯ_request.ಉದ್ದಳತೆ);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_ಕಳುಹಿಸಿದ;
			}
		}
	}
}

ತಾರ್ಕಿಕ DatamanClient::ಕೊನೆಯ_Operation_Completed(ತಾರ್ಕಿಕ &success)
{
	ತಾರ್ಕಿಕ completed = ಸುಳ್ಳು;
	success = ಸುಳ್ಳು;

	ಆದರೆ (_state == State::ResponseReceived) {

		ಆದರೆ (response_ಸ್ಥಿತಿ == dataman_response_s::ಸ್ಥಿತಿ_SUCCESS) {
			success = ನಿಜ;
		}

		_state = State::Idle;
		completed = ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ completed;
}

ಶೂನ್ಯ DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cache_ತಪ್ಪಿದ_perf_counter_name, uint32_t ಅಂಕೆ_ವಸ್ತುಗಳು)
	: cache_ತಪ್ಪಿದ_perf(perf_alloc(PC_ಎಣಿಕೆ, cache_ತಪ್ಪಿದ_perf_counter_name))
{
	ಆದರೆ (ಅಂಕೆ_ವಸ್ತುಗಳು == 0) {
		ಹಿಂತಿರುಗಿ;
	}

	ವಸ್ತುಗಳು = ಹೊಸ ಅಂಶ_2[ಅಂಕೆ_ವಸ್ತುಗಳು] {};

	ಆದರೆ (ವಸ್ತುಗಳು != ಶೂನ್ಯಸೂಚಕ) {
		ಅಂಕೆ_ವಸ್ತುಗಳು_2 = ಅಂಕೆ_ವಸ್ತುಗಳು;

	} ಇಲ್ಲದಿದ್ದರೆ {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	ಅಳಿಸು[] ವಸ್ತುಗಳು;
	perf_free(cache_ತಪ್ಪಿದ_perf);
}

ಶೂನ್ಯ DatamanCache::resize(uint32_t ಅಂಕೆ_ವಸ್ತುಗಳು)
{
	ಆದರೆ (ಅಂಕೆ_ವಸ್ತುಗಳು == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		ಅಳಿಸು[] ವಸ್ತುಗಳು;
		ವಸ್ತುಗಳು = ಶೂನ್ಯಸೂಚಕ;
		resetCacheState();
		ಅಂಕೆ_ವಸ್ತುಗಳು_2 = 0;
		ಹಿಂತಿರುಗಿ;
	}

	ಅಂಶ_2 *new_ವಸ್ತುಗಳು = ಹೊಸ ಅಂಶ_2[ಅಂಕೆ_ವಸ್ತುಗಳು] {};

	ಆದರೆ (new_ವಸ್ತುಗಳು != ಶೂನ್ಯಸೂಚಕ) {
		_client.abortCurrentOperation();

		uint32_t ಅಂಕೆ_min = ಅಂಕೆ_ವಸ್ತುಗಳು < ಅಂಕೆ_ವಸ್ತುಗಳು_2 ? ಅಂಕೆ_ವಸ್ತುಗಳು : ಅಂಕೆ_ವಸ್ತುಗಳು_2;

		ಗಾಗಿ (uint32_t i = 0; i < ಅಂಕೆ_min; ++i) {
			new_ವಸ್ತುಗಳು[i] = ವಸ್ತುಗಳು[i];

			ಆದರೆ (new_ವಸ್ತುಗಳು[i].cache_state == State::Request_ಕಳುಹಿಸಿದ) {
				new_ವಸ್ತುಗಳು[i].cache_state = State::RequestPrepared;
			}
		}

		ಅಳಿಸು[] ವಸ್ತುಗಳು;
		ವಸ್ತುಗಳು = new_ವಸ್ತುಗಳು;
		ಅಂಕೆ_ವಸ್ತುಗಳು_2 = ಅಂಕೆ_ವಸ್ತುಗಳು;

		// ensure new bounds are respected after resize
		ತುಂಬು_ಸೂಚ್ಯಂಕ = (ತುಂಬು_ಸೂಚ್ಯಂಕ < ಅಂಕೆ_ವಸ್ತುಗಳು_2) ? ತುಂಬು_ಸೂಚ್ಯಂಕ : 0;
		update_ಸೂಚ್ಯಂಕ = (update_ಸೂಚ್ಯಂಕ < ಅಂಕೆ_ವಸ್ತುಗಳು_2) ? update_ಸೂಚ್ಯಂಕ : 0;
		ಅಂಶ_counter = (ಅಂಶ_counter < ಅಂಕೆ_ವಸ್ತುಗಳು_2) ? ಅಂಶ_counter : ಅಂಕೆ_ವಸ್ತುಗಳು_2;

	} ಇಲ್ಲದಿದ್ದರೆ {
		PX4_ERR("alloc failed");
	}
}

ತಾರ್ಕಿಕ DatamanCache::ತುಂಬು(dm_ಅಂಶ_t ಅಂಶ, uint32_t ಸೂಚ್ಯಂಕ)
{
	ಆದರೆ (!ವಸ್ತುಗಳು) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ತಾರ್ಕಿಕ success = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ duplicate = ಸುಳ್ಳು;

	//Prevent duplicates
	ಗಾಗಿ (uint32_t i = 0; i < ಅಂಕೆ_ವಸ್ತುಗಳು_2; ++i) {

		ಆದರೆ (ವಸ್ತುಗಳು[i].cache_state != State::Idle &&
		    ವಸ್ತುಗಳು[i].cache_state != State::Error &&
		    ವಸ್ತುಗಳು[i].response.ಅಂಶ == ಅಂಶ &&
		    ವಸ್ತುಗಳು[i].response.ಸೂಚ್ಯಂಕ == ಸೂಚ್ಯಂಕ) {
			duplicate = ನಿಜ;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ (!duplicate && (ಅಂಶ_counter < ಅಂಕೆ_ವಸ್ತುಗಳು_2)) {

		ವಸ್ತುಗಳು[ತುಂಬು_ಸೂಚ್ಯಂಕ].cache_state = State::RequestPrepared;
		ವಸ್ತುಗಳು[ತುಂಬು_ಸೂಚ್ಯಂಕ].response.ಅಂಶ = ಅಂಶ;
		ವಸ್ತುಗಳು[ತುಂಬು_ಸೂಚ್ಯಂಕ].response.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;

		ತುಂಬು_ಸೂಚ್ಯಂಕ = (ತುಂಬು_ಸೂಚ್ಯಂಕ + 1) % ಅಂಕೆ_ವಸ್ತುಗಳು_2;

		++ಅಂಶ_counter;

		success = ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ success;
}

ತಾರ್ಕಿಕ DatamanCache::ತುಂಬು_Wait(dm_ಅಂಶ_t ಅಂಶ, uint32_t ಸೂಚ್ಯಂಕ, uint8_t *ಬಫರ್_2, uint32_t ಉದ್ದಳತೆ, hrt_abstime timeout)
{
	ಆದರೆ (ಉದ್ದಳತೆ > g_ಪ್ರತಿ_ಅಂಶ_ಗಾತ್ರ[ಅಂಶ]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, ಉದ್ದಳತೆ, static_cast<uint8_t>(ಅಂಶ));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ತಾರ್ಕಿಕ success = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ ಅಂಶ_ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;

	ಆದರೆ (ವಸ್ತುಗಳು) {
		ಗಾಗಿ (uint32_t i = 0; i < ಅಂಕೆ_ವಸ್ತುಗಳು_2; ++i) {
			ಆದರೆ ((ವಸ್ತುಗಳು[i].response.ಅಂಶ == ಅಂಶ) &&
			    (ವಸ್ತುಗಳು[i].response.ಸೂಚ್ಯಂಕ == ಸೂಚ್ಯಂಕ)) {
				ಅಂಶ_ಸಿಕ್ಕಿತು = ನಿಜ;

				ಆದರೆ (ವಸ್ತುಗಳು[i].cache_state == State::ResponseReceived) {
					ಸ್ಮರಣೆ_ನಕಲು(ಬಫರ್_2, ವಸ್ತುಗಳು[i].response.ದತ್ತಾಂಶ, ಉದ್ದಳತೆ);
					success = ನಿಜ;
					ನಿಲ್ಲಿಸು;
				}
			}
		}
	}

	ಆದರೆ (!success) {
		perf_ಎಣಿಕೆ(cache_ತಪ್ಪಿದ_perf);
	}

	ಆದರೆ (!success && (timeout > 0)) {
		success = _client.readSync(ಅಂಶ, ಸೂಚ್ಯಂಕ, ಬಫರ್_2, ಉದ್ದಳತೆ, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		ಆದರೆ (success && !ಅಂಶ_ಸಿಕ್ಕಿತು && ವಸ್ತುಗಳು && (ಅಂಶ_counter < ಅಂಕೆ_ವಸ್ತುಗಳು_2)) {
			ವಸ್ತುಗಳು[ತುಂಬು_ಸೂಚ್ಯಂಕ].cache_state = State::ResponseReceived;
			ವಸ್ತುಗಳು[ತುಂಬು_ಸೂಚ್ಯಂಕ].response.ಅಂಶ = ಅಂಶ;
			ವಸ್ತುಗಳು[ತುಂಬು_ಸೂಚ್ಯಂಕ].response.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
			ಸ್ಮರಣೆ_ನಕಲು(ವಸ್ತುಗಳು[ತುಂಬು_ಸೂಚ್ಯಂಕ].response.ದತ್ತಾಂಶ, ಬಫರ್_2, ಉದ್ದಳತೆ);

			ತುಂಬು_ಸೂಚ್ಯಂಕ = (ತುಂಬು_ಸೂಚ್ಯಂಕ + 1) % ಅಂಕೆ_ವಸ್ತುಗಳು_2;

			++ಅಂಶ_counter; // Still increase the counter here
		}
	}

	ಹಿಂತಿರುಗಿ success;
}

ತಾರ್ಕಿಕ DatamanCache::writeWait(dm_ಅಂಶ_t ಅಂಶ, uint32_t ಸೂಚ್ಯಂಕ, uint8_t *ಬಫರ್_2, uint32_t ಉದ್ದಳತೆ, hrt_abstime timeout)
{
	ಆದರೆ (ಉದ್ದಳತೆ > g_ಪ್ರತಿ_ಅಂಶ_ಗಾತ್ರ[ಅಂಶ]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ಉದ್ದಳತೆ, static_cast<uint8_t>(ಅಂಶ));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ತಾರ್ಕಿಕ success = _client.writeSync(ಅಂಶ, ಸೂಚ್ಯಂಕ, ಬಫರ್_2, ಉದ್ದಳತೆ, timeout);

	ಆದರೆ (success && ವಸ್ತುಗಳು) {
		ಗಾಗಿ (uint32_t i = 0; i < ಅಂಕೆ_ವಸ್ತುಗಳು_2; ++i) {
			ಆದರೆ ((ವಸ್ತುಗಳು[i].response.ಅಂಶ == ಅಂಶ) &&
			    (ವಸ್ತುಗಳು[i].response.ಸೂಚ್ಯಂಕ == ಸೂಚ್ಯಂಕ) &&
			    ((ವಸ್ತುಗಳು[i].cache_state == State::ResponseReceived) ||
			     (ವಸ್ತುಗಳು[i].cache_state == State::RequestPrepared))) {

				ಸ್ಮರಣೆ_ನಕಲು(ವಸ್ತುಗಳು[i].response.ದತ್ತಾಂಶ, ಬಫರ್_2, ಉದ್ದಳತೆ);
				ವಸ್ತುಗಳು[i].cache_state = State::ResponseReceived;
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	ಹಿಂತಿರುಗಿ success;
}

ತಾರ್ಕಿಕ DatamanCache::update_Cached_ಅಂಶ(dm_ಅಂಶ_t ಅಂಶ, uint32_t ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ uint8_t *ಬಫರ್_2, uint32_t ಉದ್ದಳತೆ)
{
	ಆದರೆ (!ವಸ್ತುಗಳು || ಬಫರ್_2 == ಶೂನ್ಯಸೂಚಕ || ಅಂಕೆ_ವಸ್ತುಗಳು_2 == 0) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (ಉದ್ದಳತೆ > g_ಪ್ರತಿ_ಅಂಶ_ಗಾತ್ರ[ಅಂಶ]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, ಉದ್ದಳತೆ, static_cast<uint8_t>(ಅಂಶ));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	ಗಾಗಿ (uint32_t i = 0; i < ಅಂಕೆ_ವಸ್ತುಗಳು_2; ++i) {
		ಆದರೆ ((ವಸ್ತುಗಳು[i].response.ಅಂಶ == ಅಂಶ) && (ವಸ್ತುಗಳು[i].response.ಸೂಚ್ಯಂಕ == ಸೂಚ್ಯಂಕ)
		    && (ವಸ್ತುಗಳು[i].cache_state == State::ResponseReceived)) {
			ಸ್ಮರಣೆ_ನಕಲು(ವಸ್ತುಗಳು[i].response.ದತ್ತಾಂಶ, ಬಫರ್_2, ಉದ್ದಳತೆ);
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ DatamanCache::update()
{
	ಆದರೆ (ಅಂಶ_counter > 0) {

		_client.update();

		ತಾರ್ಕಿಕ success = ಸುಳ್ಳು;
		ತಾರ್ಕಿಕ response_success = ಸುಳ್ಳು;

		ಆಯ್ಕೆ (ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].cache_state) {
		ಸ್ಥಿತಿ State::Idle:
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ State::ResponseReceived:
			// Skip it
			change_Update_ಸೂಚ್ಯಂಕ();
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_ಅಂಶ_t>(ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].response.ಅಂಶ),
						    ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].response.ಸೂಚ್ಯಂಕ,
						    ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].response.ದತ್ತಾಂಶ,
						    g_ಪ್ರತಿ_ಅಂಶ_ಗಾತ್ರ[ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].response.ಅಂಶ]);

			ಆದರೆ (success) {
				ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].cache_state = State::Request_ಕಳುಹಿಸಿದ;

			} ಇಲ್ಲದಿದ್ದರೆ {
				ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].cache_state = State::Error;
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ State::Request_ಕಳುಹಿಸಿದ:

			ಆದರೆ (_client.ಕೊನೆಯ_Operation_Completed(response_success)) {

				ಆದರೆ (response_success) {

					ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].cache_state = State::ResponseReceived;
					change_Update_ಸೂಚ್ಯಂಕ();

				} ಇಲ್ಲದಿದ್ದರೆ {
					ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].cache_state = State::Error;
				}
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ State::Error:
			// Handled below
			ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ (ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].response.ಅಂಶ, ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].response.ಸೂಚ್ಯಂಕ,
				ವಸ್ತುಗಳು[update_ಸೂಚ್ಯಂಕ].response.ಸ್ಥಿತಿ_2);

			change_Update_ಸೂಚ್ಯಂಕ();
		}

	}
}

ಶೂನ್ಯ DatamanCache::resetCacheState()
{
	update_ಸೂಚ್ಯಂಕ = 0;
	ಅಂಶ_counter = 0;
	ತುಂಬು_ಸೂಚ್ಯಂಕ = 0;
}

ಶೂನ್ಯ DatamanCache::invalidate()
{
	ಗಾಗಿ (uint32_t i = 0; i < ಅಂಕೆ_ವಸ್ತುಗಳು_2; ++i) {
		ವಸ್ತುಗಳು[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ DatamanCache::change_Update_ಸೂಚ್ಯಂಕ()
{
	ಆದರೆ (ಅಂಕೆ_ವಸ್ತುಗಳು_2 == 0) {
		update_ಸೂಚ್ಯಂಕ = 0;
		ಅಂಶ_counter = 0;
		ಹಿಂತಿರುಗಿ;
	}

	update_ಸೂಚ್ಯಂಕ = (update_ಸೂಚ್ಯಂಕ + 1) % ಅಂಕೆ_ವಸ್ತುಗಳು_2;

	ಆದರೆ (ಅಂಶ_counter > 0) {
		--ಅಂಶ_counter;
	}
}
