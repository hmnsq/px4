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

#అన్తర్భావ <dataman_client/DatamanClient.hpp>

DatamanClient::DatamanClient()
{
	_sync_perf = perf_alloc(PC_ELAPSED, "DatamanClient: sync");

	_dataman_request_pub.advertise();
	_dataman_response_sub = orb_subscribe(ORB_ID(dataman_response));

	ఒకవేళ (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} లేకపోతే {
		// make sure we don't get any stale response by doing an orb_copy
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

		_fds.fd = _dataman_response_sub;
		_fds.events = POLLIN;

		hrt_abstime timestamp = hrt_absolute_సమయం();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_సమితి;

		తార్కిక success = syncHandler(request, response, timestamp, 1000_ms);

		ఒకవేళ (success && (response.client_id > CLIENT_ID_NOT_సమితి)) {

			_client_id = response.client_id;

		} లేకపోతే {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	ఒకవేళ (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

తార్కిక DatamanClient::syncHandler(స్థిరరాశి dataman_request_s &request, dataman_response_s &response,
				స్థిరరాశి hrt_abstime &ప్రారంభం_సమయం, hrt_abstime timeout)
{
	తార్కిక response_received = అబద్ధం;
	int32_t ret = 0;
	hrt_abstime సమయం_elapsed = hrt_elapsed_సమయం(&ప్రారంభం_సమయం);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	అయితే (!response_received && (సమయం_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		ఒకవేళ (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			ఆపు;

		} లేకపోతే ఒకవేళ (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} లేకపోతే {

			తార్కిక updated = అబద్ధం;
			orb_check(_dataman_response_sub, &updated);

			ఒకవేళ (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				ఒకవేళ (response.client_id == request.client_id) {

					ఒకవేళ ((response.request_type == request.request_type) &&
					    (response.అంశం == request.అంశం) &&
					    (response.సూచీ == request.సూచీ)) {
						response_received = నిజం;
						ఆపు;
					}

				} లేకపోతే ఒకవేళ (request.client_id == CLIENT_ID_NOT_సమితి) {

					// validate timestamp from response.data
					ఒకవేళ (0 == memcmp(&(request.timestamp), &(response.దత్తాంశం), పరిమాణం(hrt_abstime))) {
						response_received = నిజం;
						ఆపు;
					}
				}
			}
		}

		సమయం_elapsed = hrt_elapsed_సమయం(&ప్రారంభం_సమయం);
	}

	perf_చివర(_sync_perf);

	ఒకవేళ (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	తిరిగి response_received;
}

తార్కిక DatamanClient::readSync(dm_అంశం_t అంశం, uint32_t సూచీ, uint8_t *బఫరు_2, uint32_t నిడివి, hrt_abstime timeout)
{
	ఒకవేళ (_client_id == CLIENT_ID_NOT_సమితి) {
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (నిడివి > g_చొప్పున_అంశం_కొలత[అంశం]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, నిడివి, static_cast<uint8_t>(అంశం));
		తిరిగి అబద్ధం;
	}

	hrt_abstime timestamp = hrt_absolute_సమయం();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.సూచీ = సూచీ;
	request.దత్తాంశం_నిడివి = నిడివి;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.అంశం = static_cast<uint8_t>(అంశం);

	dataman_response_s response{};
	తార్కిక success = syncHandler(request, response, timestamp, timeout);

	ఒకవేళ (success) {

		ఒకవేళ (response.స్థితి_2 != dataman_response_s::స్థితి_SUCCESS) {

			success = అబద్ధం;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.స్థితి_2, static_cast<uint8_t>(అంశం), సూచీ, నిడివి);

		} లేకపోతే {
			జ్ఞాపకం_నకలు(బఫరు_2, response.దత్తాంశం, నిడివి);
		}
	}

	తిరిగి success;
}

తార్కిక DatamanClient::writeSync(dm_అంశం_t అంశం, uint32_t సూచీ, uint8_t *బఫరు_2, uint32_t నిడివి, hrt_abstime timeout)
{
	ఒకవేళ (_client_id == CLIENT_ID_NOT_సమితి) {
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (నిడివి > g_చొప్పున_అంశం_కొలత[అంశం]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, నిడివి, static_cast<uint8_t>(అంశం));
		తిరిగి అబద్ధం;
	}

	hrt_abstime timestamp = hrt_absolute_సమయం();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.సూచీ = సూచీ;
	request.దత్తాంశం_నిడివి = నిడివి;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.అంశం = static_cast<uint8_t>(అంశం);

	జ్ఞాపకం_నకలు(request.దత్తాంశం, బఫరు_2, నిడివి);

	dataman_response_s response{};
	తార్కిక success = syncHandler(request, response, timestamp, timeout);

	ఒకవేళ (success) {

		ఒకవేళ (response.స్థితి_2 != dataman_response_s::స్థితి_SUCCESS) {

			success = అబద్ధం;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.స్థితి_2, static_cast<uint8_t>(అంశం), సూచీ, నిడివి);
		}
	}

	తిరిగి success;
}

తార్కిక DatamanClient::clearSync(dm_అంశం_t అంశం, hrt_abstime timeout)
{
	ఒకవేళ (_client_id == CLIENT_ID_NOT_సమితి) {
		తిరిగి అబద్ధం;
	}

	hrt_abstime timestamp = hrt_absolute_సమయం();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.అంశం = static_cast<uint8_t>(అంశం);

	dataman_response_s response{};
	తార్కిక success = syncHandler(request, response, timestamp, timeout);

	ఒకవేళ (success) {

		ఒకవేళ (response.స్థితి_2 != dataman_response_s::స్థితి_SUCCESS) {

			success = అబద్ధం;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.స్థితి_2, static_cast<uint8_t>(అంశం));
		}
	}

	తిరిగి success;
}

శూన్య DatamanClient::clearPendingResponse()
{
	ఒకవేళ (_dataman_response_sub < 0) {
		తిరిగి;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	తార్కిక updated = అబద్ధం;
	orb_check(_dataman_response_sub, &updated);

	అయితే (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

తార్కిక DatamanClient::readAsync(dm_అంశం_t అంశం, uint32_t సూచీ, uint8_t *బఫరు_2, uint32_t నిడివి)
{
	ఒకవేళ (_client_id == CLIENT_ID_NOT_సమితి) {
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (నిడివి > g_చొప్పున_అంశం_కొలత[అంశం]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, నిడివి, static_cast<uint8_t>(అంశం));
		తిరిగి అబద్ధం;
	}

	తార్కిక success = అబద్ధం;

	ఒకవేళ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_సమయం();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.సూచీ = సూచీ;
		request.దత్తాంశం_నిడివి = నిడివి;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.అంశం = static_cast<uint8_t>(అంశం);

		చురుకైన_request.timestamp = timestamp;
		చురుకైన_request.request_type = DM_READ;
		చురుకైన_request.అంశం = అంశం;
		చురుకైన_request.సూచీ = సూచీ;
		చురుకైన_request.బఫరు_2 = బఫరు_2;
		చురుకైన_request.నిడివి = నిడివి;

		_state = State::Request_పంపిన;

		_dataman_request_pub.publish(request);

		success = నిజం;
	}

	తిరిగి success;
}

తార్కిక DatamanClient::writeAsync(dm_అంశం_t అంశం, uint32_t సూచీ, uint8_t *బఫరు_2, uint32_t నిడివి)
{
	ఒకవేళ (_client_id == CLIENT_ID_NOT_సమితి) {
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (నిడివి > g_చొప్పున_అంశం_కొలత[అంశం]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, నిడివి, static_cast<uint8_t>(అంశం));
		తిరిగి అబద్ధం;
	}

	తార్కిక success = అబద్ధం;

	ఒకవేళ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_సమయం();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.సూచీ = సూచీ;
		request.దత్తాంశం_నిడివి = నిడివి;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.అంశం = static_cast<uint8_t>(అంశం);

		జ్ఞాపకం_నకలు(request.దత్తాంశం, బఫరు_2, నిడివి);

		చురుకైన_request.timestamp = timestamp;
		చురుకైన_request.request_type = DM_WRITE;
		చురుకైన_request.అంశం = అంశం;
		చురుకైన_request.సూచీ = సూచీ;
		చురుకైన_request.బఫరు_2 = బఫరు_2;
		చురుకైన_request.నిడివి = నిడివి;

		_state = State::Request_పంపిన;

		_dataman_request_pub.publish(request);

		success = నిజం;
	}

	తిరిగి success;
}

తార్కిక DatamanClient::clearAsync(dm_అంశం_t అంశం)
{
	ఒకవేళ (_client_id == CLIENT_ID_NOT_సమితి) {
		తిరిగి అబద్ధం;
	}

	తార్కిక success = అబద్ధం;

	ఒకవేళ (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_సమయం();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.అంశం = static_cast<uint8_t>(అంశం);
		request.సూచీ = 0;

		చురుకైన_request.timestamp = timestamp;
		చురుకైన_request.request_type = DM_CLEAR;
		చురుకైన_request.అంశం = అంశం;
		చురుకైన_request.సూచీ = request.సూచీ;
		_state = State::Request_పంపిన;

		_dataman_request_pub.publish(request);

		success = నిజం;
	}

	తిరిగి success;
}

శూన్య DatamanClient::update()
{
	ఒకవేళ (_state == State::Request_పంపిన) {

		తార్కిక updated = అబద్ధం;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		ఒకవేళ (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			ఒకవేళ ((response.client_id == _client_id) &&
			    (response.request_type == చురుకైన_request.request_type) &&
			    (response.అంశం == చురుకైన_request.అంశం) &&
			    (response.సూచీ == చురుకైన_request.సూచీ)) {

				ఒకవేళ (response.request_type == DM_READ) {
					జ్ఞాపకం_నకలు(చురుకైన_request.బఫరు_2, response.దత్తాంశం, చురుకైన_request.నిడివి);
				}

				response_స్థితి = response.స్థితి_2;

				ఒకవేళ (response_స్థితి != dataman_response_s::స్థితి_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.స్థితి_2, static_cast<uint8_t>(చురుకైన_request.అంశం), చురుకైన_request.సూచీ);
				}

				_state = State::ResponseReceived;
			}
		}

		ఒకవేళ (_state == State::Request_పంపిన) {

			/* Retry the request if there is no answer */
			ఒకవేళ (((చురుకైన_request.request_type != DM_CLEAR) && (hrt_elapsed_సమయం(&చురుకైన_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_సమయం(&చురుకైన_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_సమయం();

				చురుకైన_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.సూచీ = చురుకైన_request.సూచీ;
				request.దత్తాంశం_నిడివి = చురుకైన_request.నిడివి;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(చురుకైన_request.request_type);
				request.అంశం = static_cast<uint8_t>(చురుకైన_request.అంశం);

				ఒకవేళ (చురుకైన_request.request_type == DM_WRITE) {
					జ్ఞాపకం_నకలు(request.దత్తాంశం, చురుకైన_request.బఫరు_2, చురుకైన_request.నిడివి);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_పంపిన;
			}
		}
	}
}

తార్కిక DatamanClient::చివరి_Operation_Completed(తార్కిక &success)
{
	తార్కిక completed = అబద్ధం;
	success = అబద్ధం;

	ఒకవేళ (_state == State::ResponseReceived) {

		ఒకవేళ (response_స్థితి == dataman_response_s::స్థితి_SUCCESS) {
			success = నిజం;
		}

		_state = State::Idle;
		completed = నిజం;
	}

	తిరిగి completed;
}

శూన్య DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(స్థిరరాశి అక్షరం *cache_తప్పిపోవు_perf_counter_name, uint32_t అంకె_వస్తువులు)
	: cache_తప్పిపోవు_perf(perf_alloc(PC_లెక్క, cache_తప్పిపోవు_perf_counter_name))
{
	ఒకవేళ (అంకె_వస్తువులు == 0) {
		తిరిగి;
	}

	వస్తువులు = కొత్త అంశం_2[అంకె_వస్తువులు] {};

	ఒకవేళ (వస్తువులు != శూన్యసూచిక) {
		అంకె_వస్తువులు_2 = అంకె_వస్తువులు;

	} లేకపోతే {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	తొలగించు[] వస్తువులు;
	perf_free(cache_తప్పిపోవు_perf);
}

శూన్య DatamanCache::resize(uint32_t అంకె_వస్తువులు)
{
	ఒకవేళ (అంకె_వస్తువులు == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		తొలగించు[] వస్తువులు;
		వస్తువులు = శూన్యసూచిక;
		resetCacheState();
		అంకె_వస్తువులు_2 = 0;
		తిరిగి;
	}

	అంశం_2 *new_వస్తువులు = కొత్త అంశం_2[అంకె_వస్తువులు] {};

	ఒకవేళ (new_వస్తువులు != శూన్యసూచిక) {
		_client.abortCurrentOperation();

		uint32_t అంకె_min = అంకె_వస్తువులు < అంకె_వస్తువులు_2 ? అంకె_వస్తువులు : అంకె_వస్తువులు_2;

		కొరకు (uint32_t i = 0; i < అంకె_min; ++i) {
			new_వస్తువులు[i] = వస్తువులు[i];

			ఒకవేళ (new_వస్తువులు[i].cache_state == State::Request_పంపిన) {
				new_వస్తువులు[i].cache_state = State::RequestPrepared;
			}
		}

		తొలగించు[] వస్తువులు;
		వస్తువులు = new_వస్తువులు;
		అంకె_వస్తువులు_2 = అంకె_వస్తువులు;

		// ensure new bounds are respected after resize
		ఎక్కించు_సూచీ = (ఎక్కించు_సూచీ < అంకె_వస్తువులు_2) ? ఎక్కించు_సూచీ : 0;
		update_సూచీ = (update_సూచీ < అంకె_వస్తువులు_2) ? update_సూచీ : 0;
		అంశం_counter = (అంశం_counter < అంకె_వస్తువులు_2) ? అంశం_counter : అంకె_వస్తువులు_2;

	} లేకపోతే {
		PX4_ERR("alloc failed");
	}
}

తార్కిక DatamanCache::ఎక్కించు(dm_అంశం_t అంశం, uint32_t సూచీ)
{
	ఒకవేళ (!వస్తువులు) {
		తిరిగి అబద్ధం;
	}

	తార్కిక success = అబద్ధం;
	తార్కిక duplicate = అబద్ధం;

	//Prevent duplicates
	కొరకు (uint32_t i = 0; i < అంకె_వస్తువులు_2; ++i) {

		ఒకవేళ (వస్తువులు[i].cache_state != State::Idle &&
		    వస్తువులు[i].cache_state != State::Error &&
		    వస్తువులు[i].response.అంశం == అంశం &&
		    వస్తువులు[i].response.సూచీ == సూచీ) {
			duplicate = నిజం;
			ఆపు;
		}
	}

	ఒకవేళ (!duplicate && (అంశం_counter < అంకె_వస్తువులు_2)) {

		వస్తువులు[ఎక్కించు_సూచీ].cache_state = State::RequestPrepared;
		వస్తువులు[ఎక్కించు_సూచీ].response.అంశం = అంశం;
		వస్తువులు[ఎక్కించు_సూచీ].response.సూచీ = సూచీ;

		ఎక్కించు_సూచీ = (ఎక్కించు_సూచీ + 1) % అంకె_వస్తువులు_2;

		++అంశం_counter;

		success = నిజం;
	}

	తిరిగి success;
}

తార్కిక DatamanCache::ఎక్కించు_Wait(dm_అంశం_t అంశం, uint32_t సూచీ, uint8_t *బఫరు_2, uint32_t నిడివి, hrt_abstime timeout)
{
	ఒకవేళ (నిడివి > g_చొప్పున_అంశం_కొలత[అంశం]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, నిడివి, static_cast<uint8_t>(అంశం));
		తిరిగి అబద్ధం;
	}

	తార్కిక success = అబద్ధం;
	తార్కిక అంశం_దొరికింది = అబద్ధం;

	ఒకవేళ (వస్తువులు) {
		కొరకు (uint32_t i = 0; i < అంకె_వస్తువులు_2; ++i) {
			ఒకవేళ ((వస్తువులు[i].response.అంశం == అంశం) &&
			    (వస్తువులు[i].response.సూచీ == సూచీ)) {
				అంశం_దొరికింది = నిజం;

				ఒకవేళ (వస్తువులు[i].cache_state == State::ResponseReceived) {
					జ్ఞాపకం_నకలు(బఫరు_2, వస్తువులు[i].response.దత్తాంశం, నిడివి);
					success = నిజం;
					ఆపు;
				}
			}
		}
	}

	ఒకవేళ (!success) {
		perf_లెక్క(cache_తప్పిపోవు_perf);
	}

	ఒకవేళ (!success && (timeout > 0)) {
		success = _client.readSync(అంశం, సూచీ, బఫరు_2, నిడివి, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		ఒకవేళ (success && !అంశం_దొరికింది && వస్తువులు && (అంశం_counter < అంకె_వస్తువులు_2)) {
			వస్తువులు[ఎక్కించు_సూచీ].cache_state = State::ResponseReceived;
			వస్తువులు[ఎక్కించు_సూచీ].response.అంశం = అంశం;
			వస్తువులు[ఎక్కించు_సూచీ].response.సూచీ = సూచీ;
			జ్ఞాపకం_నకలు(వస్తువులు[ఎక్కించు_సూచీ].response.దత్తాంశం, బఫరు_2, నిడివి);

			ఎక్కించు_సూచీ = (ఎక్కించు_సూచీ + 1) % అంకె_వస్తువులు_2;

			++అంశం_counter; // Still increase the counter here
		}
	}

	తిరిగి success;
}

తార్కిక DatamanCache::writeWait(dm_అంశం_t అంశం, uint32_t సూచీ, uint8_t *బఫరు_2, uint32_t నిడివి, hrt_abstime timeout)
{
	ఒకవేళ (నిడివి > g_చొప్పున_అంశం_కొలత[అంశం]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, నిడివి, static_cast<uint8_t>(అంశం));
		తిరిగి అబద్ధం;
	}

	తార్కిక success = _client.writeSync(అంశం, సూచీ, బఫరు_2, నిడివి, timeout);

	ఒకవేళ (success && వస్తువులు) {
		కొరకు (uint32_t i = 0; i < అంకె_వస్తువులు_2; ++i) {
			ఒకవేళ ((వస్తువులు[i].response.అంశం == అంశం) &&
			    (వస్తువులు[i].response.సూచీ == సూచీ) &&
			    ((వస్తువులు[i].cache_state == State::ResponseReceived) ||
			     (వస్తువులు[i].cache_state == State::RequestPrepared))) {

				జ్ఞాపకం_నకలు(వస్తువులు[i].response.దత్తాంశం, బఫరు_2, నిడివి);
				వస్తువులు[i].cache_state = State::ResponseReceived;
				ఆపు;
			}
		}
	}

	తిరిగి success;
}

తార్కిక DatamanCache::update_Cached_అంశం(dm_అంశం_t అంశం, uint32_t సూచీ, స్థిరరాశి uint8_t *బఫరు_2, uint32_t నిడివి)
{
	ఒకవేళ (!వస్తువులు || బఫరు_2 == శూన్యసూచిక || అంకె_వస్తువులు_2 == 0) {
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (నిడివి > g_చొప్పున_అంశం_కొలత[అంశం]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, నిడివి, static_cast<uint8_t>(అంశం));
		తిరిగి అబద్ధం;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	కొరకు (uint32_t i = 0; i < అంకె_వస్తువులు_2; ++i) {
		ఒకవేళ ((వస్తువులు[i].response.అంశం == అంశం) && (వస్తువులు[i].response.సూచీ == సూచీ)
		    && (వస్తువులు[i].cache_state == State::ResponseReceived)) {
			జ్ఞాపకం_నకలు(వస్తువులు[i].response.దత్తాంశం, బఫరు_2, నిడివి);
			తిరిగి నిజం;
		}
	}

	తిరిగి అబద్ధం;
}

శూన్య DatamanCache::update()
{
	ఒకవేళ (అంశం_counter > 0) {

		_client.update();

		తార్కిక success = అబద్ధం;
		తార్కిక response_success = అబద్ధం;

		ఎంపిక (వస్తువులు[update_సూచీ].cache_state) {
		స్థితి State::Idle:
			ఆపు;

		స్థితి State::ResponseReceived:
			// Skip it
			change_Update_సూచీ();
			ఆపు;

		స్థితి State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_అంశం_t>(వస్తువులు[update_సూచీ].response.అంశం),
						    వస్తువులు[update_సూచీ].response.సూచీ,
						    వస్తువులు[update_సూచీ].response.దత్తాంశం,
						    g_చొప్పున_అంశం_కొలత[వస్తువులు[update_సూచీ].response.అంశం]);

			ఒకవేళ (success) {
				వస్తువులు[update_సూచీ].cache_state = State::Request_పంపిన;

			} లేకపోతే {
				వస్తువులు[update_సూచీ].cache_state = State::Error;
			}

			ఆపు;

		స్థితి State::Request_పంపిన:

			ఒకవేళ (_client.చివరి_Operation_Completed(response_success)) {

				ఒకవేళ (response_success) {

					వస్తువులు[update_సూచీ].cache_state = State::ResponseReceived;
					change_Update_సూచీ();

				} లేకపోతే {
					వస్తువులు[update_సూచీ].cache_state = State::Error;
				}
			}

			ఆపు;

		స్థితి State::Error:
			// Handled below
			ఆపు;
		}

		ఒకవేళ (వస్తువులు[update_సూచీ].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				వస్తువులు[update_సూచీ].response.అంశం, వస్తువులు[update_సూచీ].response.సూచీ,
				వస్తువులు[update_సూచీ].response.స్థితి_2);

			change_Update_సూచీ();
		}

	}
}

శూన్య DatamanCache::resetCacheState()
{
	update_సూచీ = 0;
	అంశం_counter = 0;
	ఎక్కించు_సూచీ = 0;
}

శూన్య DatamanCache::invalidate()
{
	కొరకు (uint32_t i = 0; i < అంకె_వస్తువులు_2; ++i) {
		వస్తువులు[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

అంతఃవరుస శూన్య DatamanCache::change_Update_సూచీ()
{
	ఒకవేళ (అంకె_వస్తువులు_2 == 0) {
		update_సూచీ = 0;
		అంశం_counter = 0;
		తిరిగి;
	}

	update_సూచీ = (update_సూచీ + 1) % అంకె_వస్తువులు_2;

	ఒకవేళ (అంశం_counter > 0) {
		--అంశం_counter;
	}
}
