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

	यदि (!orb_sub_valid(_dataman_response_sub)) {
		PX4_ERR("Failed to subscribe (%i)", errno);

	} अन्यथा {
		// make sure we don't get any stale response by doing an orb_copy
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

		_fds.fd = _dataman_response_sub;
		_fds.events = POLLIN;

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.request_type = DM_GET_ID;
		request.client_id = CLIENT_ID_NOT_समुच्चयः;

		तार्किक success = syncHandler(request, response, timestamp, 1000_ms);

		यदि (success && (response.client_id > CLIENT_ID_NOT_समुच्चयः)) {

			_client_id = response.client_id;

		} अन्यथा {
			PX4_ERR("Failed to get client ID!");
		}
	}
}

DatamanClient::~DatamanClient()
{
	perf_free(_sync_perf);

	यदि (orb_sub_valid(_dataman_response_sub)) {
		orb_unsubscribe(_dataman_response_sub);
	}
}

तार्किक DatamanClient::syncHandler(ध्रुव dataman_request_s &request, dataman_response_s &response,
				ध्रुव hrt_abstime &start_time, hrt_abstime timeout)
{
	तार्किक response_received = असत्य;
	int32_t ret = 0;
	hrt_abstime time_elapsed = hrt_elapsed_time(&start_time);
	perf_begin(_sync_perf);
	clearPendingResponse();

	_dataman_request_pub.publish(request);

	यावत् (!response_received && (time_elapsed < timeout)) {

		uint32_t timeout_ms = 100;
		ret = px4_poll(&_fds, 1, timeout_ms);

		यदि (ret < 0) {
			PX4_ERR("px4_poll returned error: %" PRIu32, ret);
			भङ्ग;

		} अन्यथा यदि (ret == 0) {

			// No response received, send new request
			_dataman_request_pub.publish(request);

		} अन्यथा {

			तार्किक updated = असत्य;
			orb_check(_dataman_response_sub, &updated);

			यदि (updated) {
				orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

				यदि (response.client_id == request.client_id) {

					यदि ((response.request_type == request.request_type) &&
					    (response.item == request.item) &&
					    (response.सूचकांक == request.सूचकांक)) {
						response_received = सत्य;
						भङ्ग;
					}

				} अन्यथा यदि (request.client_id == CLIENT_ID_NOT_समुच्चयः) {

					// validate timestamp from response.data
					यदि (0 == memcmp(&(request.timestamp), &(response.दत्त), परिमाण(hrt_abstime))) {
						response_received = सत्य;
						भङ्ग;
					}
				}
			}
		}

		time_elapsed = hrt_elapsed_time(&start_time);
	}

	perf_अन्तः(_sync_perf);

	यदि (!response_received && ret >= 0) {
		PX4_ERR("timeout after %" PRIu32 " ms!", static_cast<uint32_t>(timeout / 1000));
	}

	प्रत्यागम response_received;
}

तार्किक DatamanClient::readSync(dm_item_t item, uint32_t सूचकांक, uint8_t *बफर_2, uint32_t दीर्घता_2, hrt_abstime timeout)
{
	यदि (_client_id == CLIENT_ID_NOT_समुच्चयः) {
		प्रत्यागम असत्य;
	}

	यदि (दीर्घता_2 > g_प्रत्येकम्_item_आकार[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, दीर्घता_2, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.सूचकांक = सूचकांक;
	request.दत्त_दीर्घता = दीर्घता_2;
	request.client_id = _client_id;
	request.request_type = DM_READ;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	तार्किक success = syncHandler(request, response, timestamp, timeout);

	यदि (success) {

		यदि (response.स्थितिः != dataman_response_s::स्थितिः_SUCCESS) {

			success = असत्य;
			PX4_ERR("readSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.स्थितिः, static_cast<uint8_t>(item), सूचकांक, दीर्घता_2);

		} अन्यथा {
			स्मृतिं_प्रतिलिख(बफर_2, response.दत्त, दीर्घता_2);
		}
	}

	प्रत्यागम success;
}

तार्किक DatamanClient::writeSync(dm_item_t item, uint32_t सूचकांक, uint8_t *बफर_2, uint32_t दीर्घता_2, hrt_abstime timeout)
{
	यदि (_client_id == CLIENT_ID_NOT_समुच्चयः) {
		प्रत्यागम असत्य;
	}

	यदि (दीर्घता_2 > g_प्रत्येकम्_item_आकार[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, दीर्घता_2, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.सूचकांक = सूचकांक;
	request.दत्त_दीर्घता = दीर्घता_2;
	request.client_id = _client_id;
	request.request_type = DM_WRITE;
	request.item = static_cast<uint8_t>(item);

	स्मृतिं_प्रतिलिख(request.दत्त, बफर_2, दीर्घता_2);

	dataman_response_s response{};
	तार्किक success = syncHandler(request, response, timestamp, timeout);

	यदि (success) {

		यदि (response.स्थितिः != dataman_response_s::स्थितिः_SUCCESS) {

			success = असत्य;
			PX4_ERR("writeSync failed! status=%" PRIu8 ", item=%" PRIu8 ", index=%" PRIu32 ", length=%" PRIu32,
				response.स्थितिः, static_cast<uint8_t>(item), सूचकांक, दीर्घता_2);
		}
	}

	प्रत्यागम success;
}

तार्किक DatamanClient::clearSync(dm_item_t item, hrt_abstime timeout)
{
	यदि (_client_id == CLIENT_ID_NOT_समुच्चयः) {
		प्रत्यागम असत्य;
	}

	hrt_abstime timestamp = hrt_absolute_time();

	dataman_request_s request;
	request.timestamp = timestamp;
	request.client_id = _client_id;
	request.request_type = DM_CLEAR;
	request.item = static_cast<uint8_t>(item);

	dataman_response_s response{};
	तार्किक success = syncHandler(request, response, timestamp, timeout);

	यदि (success) {

		यदि (response.स्थितिः != dataman_response_s::स्थितिः_SUCCESS) {

			success = असत्य;
			PX4_ERR("clearSync failed! status=%" PRIu8 ", item=%" PRIu8,
				response.स्थितिः, static_cast<uint8_t>(item));
		}
	}

	प्रत्यागम success;
}

शून्य DatamanClient::clearPendingResponse()
{
	यदि (_dataman_response_sub < 0) {
		प्रत्यागम;
	}

	// Drain queued replies first so a response from an aborted or timed-out
	// operation cannot be matched to the next request.
	तार्किक updated = असत्य;
	orb_check(_dataman_response_sub, &updated);

	यावत् (updated) {
		dataman_response_s response{};
		orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);
		orb_check(_dataman_response_sub, &updated);
	}
}

तार्किक DatamanClient::readAsync(dm_item_t item, uint32_t सूचकांक, uint8_t *बफर_2, uint32_t दीर्घता_2)
{
	यदि (_client_id == CLIENT_ID_NOT_समुच्चयः) {
		प्रत्यागम असत्य;
	}

	यदि (दीर्घता_2 > g_प्रत्येकम्_item_आकार[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, दीर्घता_2, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;

	यदि (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.सूचकांक = सूचकांक;
		request.दत्त_दीर्घता = दीर्घता_2;
		request.client_id = _client_id;
		request.request_type = DM_READ;
		request.item = static_cast<uint8_t>(item);

		सक्रिय_request.timestamp = timestamp;
		सक्रिय_request.request_type = DM_READ;
		सक्रिय_request.item = item;
		सक्रिय_request.सूचकांक = सूचकांक;
		सक्रिय_request.बफर_2 = बफर_2;
		सक्रिय_request.दीर्घता_2 = दीर्घता_2;

		_state = State::Request_प्रेषितम्;

		_dataman_request_pub.publish(request);

		success = सत्य;
	}

	प्रत्यागम success;
}

तार्किक DatamanClient::writeAsync(dm_item_t item, uint32_t सूचकांक, uint8_t *बफर_2, uint32_t दीर्घता_2)
{
	यदि (_client_id == CLIENT_ID_NOT_समुच्चयः) {
		प्रत्यागम असत्य;
	}

	यदि (दीर्घता_2 > g_प्रत्येकम्_item_आकार[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, दीर्घता_2, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;

	यदि (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.सूचकांक = सूचकांक;
		request.दत्त_दीर्घता = दीर्घता_2;
		request.client_id = _client_id;
		request.request_type = DM_WRITE;
		request.item = static_cast<uint8_t>(item);

		स्मृतिं_प्रतिलिख(request.दत्त, बफर_2, दीर्घता_2);

		सक्रिय_request.timestamp = timestamp;
		सक्रिय_request.request_type = DM_WRITE;
		सक्रिय_request.item = item;
		सक्रिय_request.सूचकांक = सूचकांक;
		सक्रिय_request.बफर_2 = बफर_2;
		सक्रिय_request.दीर्घता_2 = दीर्घता_2;

		_state = State::Request_प्रेषितम्;

		_dataman_request_pub.publish(request);

		success = सत्य;
	}

	प्रत्यागम success;
}

तार्किक DatamanClient::clearAsync(dm_item_t item)
{
	यदि (_client_id == CLIENT_ID_NOT_समुच्चयः) {
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;

	यदि (_state == State::Idle) {
		// Drop any queued stale replies before publishing a fresh async request.
		clearPendingResponse();

		hrt_abstime timestamp = hrt_absolute_time();

		dataman_request_s request;
		request.timestamp = timestamp;
		request.client_id = _client_id;
		request.request_type = DM_CLEAR;
		request.item = static_cast<uint8_t>(item);
		request.सूचकांक = 0;

		सक्रिय_request.timestamp = timestamp;
		सक्रिय_request.request_type = DM_CLEAR;
		सक्रिय_request.item = item;
		सक्रिय_request.सूचकांक = request.सूचकांक;
		_state = State::Request_प्रेषितम्;

		_dataman_request_pub.publish(request);

		success = सत्य;
	}

	प्रत्यागम success;
}

शून्य DatamanClient::update()
{
	यदि (_state == State::Request_प्रेषितम्) {

		तार्किक updated = असत्य;
		orb_check(_dataman_response_sub, &updated);

		dataman_response_s response;

		यदि (updated) {
			orb_copy(ORB_ID(dataman_response), _dataman_response_sub, &response);

			यदि ((response.client_id == _client_id) &&
			    (response.request_type == सक्रिय_request.request_type) &&
			    (response.item == सक्रिय_request.item) &&
			    (response.सूचकांक == सक्रिय_request.सूचकांक)) {

				यदि (response.request_type == DM_READ) {
					स्मृतिं_प्रतिलिख(सक्रिय_request.बफर_2, response.दत्त, सक्रिय_request.दीर्घता_2);
				}

				response_स्थितिः = response.स्थितिः;

				यदि (response_स्थितिः != dataman_response_s::स्थितिः_SUCCESS) {

					PX4_ERR("Async request type %" PRIu8 " failed! status=%" PRIu8 " item=%" PRIu8 " index=%" PRIu32,
						response.request_type, response.स्थितिः, static_cast<uint8_t>(सक्रिय_request.item), सक्रिय_request.सूचकांक);
				}

				_state = State::ResponseReceived;
			}
		}

		यदि (_state == State::Request_प्रेषितम्) {

			/* Retry the request if there is no answer */
			यदि (((सक्रिय_request.request_type != DM_CLEAR) && (hrt_elapsed_time(&सक्रिय_request.timestamp) > 100_ms)) ||
			    (hrt_elapsed_time(&सक्रिय_request.timestamp) > 1000_ms)
			   ) {

				hrt_abstime timestamp = hrt_absolute_time();

				सक्रिय_request.timestamp = timestamp;

				dataman_request_s request;
				request.timestamp = timestamp;
				request.सूचकांक = सक्रिय_request.सूचकांक;
				request.दत्त_दीर्घता = सक्रिय_request.दीर्घता_2;
				request.client_id = _client_id;
				request.request_type = static_cast<uint8_t>(सक्रिय_request.request_type);
				request.item = static_cast<uint8_t>(सक्रिय_request.item);

				यदि (सक्रिय_request.request_type == DM_WRITE) {
					स्मृतिं_प्रतिलिख(request.दत्त, सक्रिय_request.बफर_2, सक्रिय_request.दीर्घता_2);
				}

				_dataman_request_pub.publish(request);

				_state = State::Request_प्रेषितम्;
			}
		}
	}
}

तार्किक DatamanClient::चरमः_Operation_Completed(तार्किक &success)
{
	तार्किक completed = असत्य;
	success = असत्य;

	यदि (_state == State::ResponseReceived) {

		यदि (response_स्थितिः == dataman_response_s::स्थितिः_SUCCESS) {
			success = सत्य;
		}

		_state = State::Idle;
		completed = सत्य;
	}

	प्रत्यागम completed;
}

शून्य DatamanClient::abortCurrentOperation()
{
	_state = State::Idle;
}

DatamanCache::DatamanCache(ध्रुव वर्ण *cache_अलब्धिः_perf_गणक_name, uint32_t अङ्कः_वस्तूनि)
	: cache_अलब्धिः_perf(perf_alloc(PC_गणनम्, cache_अलब्धिः_perf_गणक_name))
{
	यदि (अङ्कः_वस्तूनि == 0) {
		प्रत्यागम;
	}

	वस्तूनि = नूतन Item[अङ्कः_वस्तूनि] {};

	यदि (वस्तूनि != शून्यसूचकः) {
		अङ्कः_वस्तूनि_2 = अङ्कः_वस्तूनि;

	} अन्यथा {
		PX4_ERR("alloc failed");
	}
}

DatamanCache::~DatamanCache()
{
	_client.abortCurrentOperation();
	विलोपय[] वस्तूनि;
	perf_free(cache_अलब्धिः_perf);
}

शून्य DatamanCache::resize(uint32_t अङ्कः_वस्तूनि)
{
	यदि (अङ्कः_वस्तूनि == 0) {
		// Allow callers to explicitly disable caching while leaving the DatamanClient usable.
		_client.abortCurrentOperation();
		विलोपय[] वस्तूनि;
		वस्तूनि = शून्यसूचकः;
		resetCacheState();
		अङ्कः_वस्तूनि_2 = 0;
		प्रत्यागम;
	}

	Item *new_वस्तूनि = नूतन Item[अङ्कः_वस्तूनि] {};

	यदि (new_वस्तूनि != शून्यसूचकः) {
		_client.abortCurrentOperation();

		uint32_t अङ्कः_min = अङ्कः_वस्तूनि < अङ्कः_वस्तूनि_2 ? अङ्कः_वस्तूनि : अङ्कः_वस्तूनि_2;

		प्रति (uint32_t i = 0; i < अङ्कः_min; ++i) {
			new_वस्तूनि[i] = वस्तूनि[i];

			यदि (new_वस्तूनि[i].cache_state == State::Request_प्रेषितम्) {
				new_वस्तूनि[i].cache_state = State::RequestPrepared;
			}
		}

		विलोपय[] वस्तूनि;
		वस्तूनि = new_वस्तूनि;
		अङ्कः_वस्तूनि_2 = अङ्कः_वस्तूनि;

		// ensure new bounds are respected after resize
		आरोपय_सूचकांक = (आरोपय_सूचकांक < अङ्कः_वस्तूनि_2) ? आरोपय_सूचकांक : 0;
		update_सूचकांक = (update_सूचकांक < अङ्कः_वस्तूनि_2) ? update_सूचकांक : 0;
		item_गणक = (item_गणक < अङ्कः_वस्तूनि_2) ? item_गणक : अङ्कः_वस्तूनि_2;

	} अन्यथा {
		PX4_ERR("alloc failed");
	}
}

तार्किक DatamanCache::आरोपय(dm_item_t item, uint32_t सूचकांक)
{
	यदि (!वस्तूनि) {
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;
	तार्किक duplicate = असत्य;

	//Prevent duplicates
	प्रति (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {

		यदि (वस्तूनि[i].cache_state != State::Idle &&
		    वस्तूनि[i].cache_state != State::Error &&
		    वस्तूनि[i].response.item == item &&
		    वस्तूनि[i].response.सूचकांक == सूचकांक) {
			duplicate = सत्य;
			भङ्ग;
		}
	}

	यदि (!duplicate && (item_गणक < अङ्कः_वस्तूनि_2)) {

		वस्तूनि[आरोपय_सूचकांक].cache_state = State::RequestPrepared;
		वस्तूनि[आरोपय_सूचकांक].response.item = item;
		वस्तूनि[आरोपय_सूचकांक].response.सूचकांक = सूचकांक;

		आरोपय_सूचकांक = (आरोपय_सूचकांक + 1) % अङ्कः_वस्तूनि_2;

		++item_गणक;

		success = सत्य;
	}

	प्रत्यागम success;
}

तार्किक DatamanCache::आरोपय_Wait(dm_item_t item, uint32_t सूचकांक, uint8_t *बफर_2, uint32_t दीर्घता_2, hrt_abstime timeout)
{
	यदि (दीर्घता_2 > g_प्रत्येकम्_item_आकार[item]) {
		PX4_ERR("Length %" PRIu32 " can't fit in data size for item %" PRIi8, दीर्घता_2, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	तार्किक success = असत्य;
	तार्किक item_प्राप्तम् = असत्य;

	यदि (वस्तूनि) {
		प्रति (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {
			यदि ((वस्तूनि[i].response.item == item) &&
			    (वस्तूनि[i].response.सूचकांक == सूचकांक)) {
				item_प्राप्तम् = सत्य;

				यदि (वस्तूनि[i].cache_state == State::ResponseReceived) {
					स्मृतिं_प्रतिलिख(बफर_2, वस्तूनि[i].response.दत्त, दीर्घता_2);
					success = सत्य;
					भङ्ग;
				}
			}
		}
	}

	यदि (!success) {
		perf_गणनम्(cache_अलब्धिः_perf);
	}

	यदि (!success && (timeout > 0)) {
		success = _client.readSync(item, सूचकांक, बफर_2, दीर्घता_2, timeout);

		// Cache the item if not found already (it could be in the process of being loaded)
		यदि (success && !item_प्राप्तम् && वस्तूनि && (item_गणक < अङ्कः_वस्तूनि_2)) {
			वस्तूनि[आरोपय_सूचकांक].cache_state = State::ResponseReceived;
			वस्तूनि[आरोपय_सूचकांक].response.item = item;
			वस्तूनि[आरोपय_सूचकांक].response.सूचकांक = सूचकांक;
			स्मृतिं_प्रतिलिख(वस्तूनि[आरोपय_सूचकांक].response.दत्त, बफर_2, दीर्घता_2);

			आरोपय_सूचकांक = (आरोपय_सूचकांक + 1) % अङ्कः_वस्तूनि_2;

			++item_गणक; // Still increase the counter here
		}
	}

	प्रत्यागम success;
}

तार्किक DatamanCache::writeWait(dm_item_t item, uint32_t सूचकांक, uint8_t *बफर_2, uint32_t दीर्घता_2, hrt_abstime timeout)
{
	यदि (दीर्घता_2 > g_प्रत्येकम्_item_आकार[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, दीर्घता_2, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	तार्किक success = _client.writeSync(item, सूचकांक, बफर_2, दीर्घता_2, timeout);

	यदि (success && वस्तूनि) {
		प्रति (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {
			यदि ((वस्तूनि[i].response.item == item) &&
			    (वस्तूनि[i].response.सूचकांक == सूचकांक) &&
			    ((वस्तूनि[i].cache_state == State::ResponseReceived) ||
			     (वस्तूनि[i].cache_state == State::RequestPrepared))) {

				स्मृतिं_प्रतिलिख(वस्तूनि[i].response.दत्त, बफर_2, दीर्घता_2);
				वस्तूनि[i].cache_state = State::ResponseReceived;
				भङ्ग;
			}
		}
	}

	प्रत्यागम success;
}

तार्किक DatamanCache::updateCachedItem(dm_item_t item, uint32_t सूचकांक, ध्रुव uint8_t *बफर_2, uint32_t दीर्घता_2)
{
	यदि (!वस्तूनि || बफर_2 == शून्यसूचकः || अङ्कः_वस्तूनि_2 == 0) {
		प्रत्यागम असत्य;
	}

	यदि (दीर्घता_2 > g_प्रत्येकम्_item_आकार[item]) {
		PX4_ERR("Length  %" PRIu32 " can't fit in data size for item  %" PRIi8, दीर्घता_2, static_cast<uint8_t>(item));
		प्रत्यागम असत्य;
	}

	// Only patch data that is fully received and stable. Slots in other states are skipped:
	// an in-flight async read will fetch the new SD card data shortly anyway, and invalidated
	// slots can still carry a stale key match for the same item/index.
	प्रति (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {
		यदि ((वस्तूनि[i].response.item == item) && (वस्तूनि[i].response.सूचकांक == सूचकांक)
		    && (वस्तूनि[i].cache_state == State::ResponseReceived)) {
			स्मृतिं_प्रतिलिख(वस्तूनि[i].response.दत्त, बफर_2, दीर्घता_2);
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

शून्य DatamanCache::update()
{
	यदि (item_गणक > 0) {

		_client.update();

		तार्किक success = असत्य;
		तार्किक response_success = असत्य;

		विकल्प (वस्तूनि[update_सूचकांक].cache_state) {
		पक्ष State::Idle:
			भङ्ग;

		पक्ष State::ResponseReceived:
			// Skip it
			change_Update_सूचकांक();
			भङ्ग;

		पक्ष State::RequestPrepared:

			success = _client.readAsync(static_cast<dm_item_t>(वस्तूनि[update_सूचकांक].response.item),
						    वस्तूनि[update_सूचकांक].response.सूचकांक,
						    वस्तूनि[update_सूचकांक].response.दत्त,
						    g_प्रत्येकम्_item_आकार[वस्तूनि[update_सूचकांक].response.item]);

			यदि (success) {
				वस्तूनि[update_सूचकांक].cache_state = State::Request_प्रेषितम्;

			} अन्यथा {
				वस्तूनि[update_सूचकांक].cache_state = State::Error;
			}

			भङ्ग;

		पक्ष State::Request_प्रेषितम्:

			यदि (_client.चरमः_Operation_Completed(response_success)) {

				यदि (response_success) {

					वस्तूनि[update_सूचकांक].cache_state = State::ResponseReceived;
					change_Update_सूचकांक();

				} अन्यथा {
					वस्तूनि[update_सूचकांक].cache_state = State::Error;
				}
			}

			भङ्ग;

		पक्ष State::Error:
			// Handled below
			भङ्ग;
		}

		यदि (वस्तूनि[update_सूचकांक].cache_state == State::Error) {
			PX4_ERR("Caching: item %" PRIu8 ", index %" PRIu32", status %" PRIu8,
				वस्तूनि[update_सूचकांक].response.item, वस्तूनि[update_सूचकांक].response.सूचकांक,
				वस्तूनि[update_सूचकांक].response.स्थितिः);

			change_Update_सूचकांक();
		}

	}
}

शून्य DatamanCache::resetCacheState()
{
	update_सूचकांक = 0;
	item_गणक = 0;
	आरोपय_सूचकांक = 0;
}

शून्य DatamanCache::invalidate()
{
	प्रति (uint32_t i = 0; i < अङ्कः_वस्तूनि_2; ++i) {
		वस्तूनि[i].cache_state = State::Idle;
	}

	resetCacheState();
	_client.abortCurrentOperation();
}

अन्तःस्थ शून्य DatamanCache::change_Update_सूचकांक()
{
	यदि (अङ्कः_वस्तूनि_2 == 0) {
		update_सूचकांक = 0;
		item_गणक = 0;
		प्रत्यागम;
	}

	update_सूचकांक = (update_सूचकांक + 1) % अङ्कः_वस्तूनि_2;

	यदि (item_गणक > 0) {
		--item_गणक;
	}
}
