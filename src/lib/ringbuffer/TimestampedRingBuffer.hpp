/****************************************************************************
 *
 *   Copyright (C) 2015-2026 PX4 Development Team. All rights reserved.
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
* @file TimestampedRingBuffer.hpp
* @author Roman Bapst <bapstroman@gmail.com>
* @brief Template ring buffer for timestamped samples (requires data_type::time_us).
*
* Note: This is not the same as `Ringbuffer` (byte FIFO) in `src/lib/ringbuffer/Ringbuffer.hpp`.
*/

#pragma once

#অন্তর্ভাব <climits>
#অন্তর্ভাব <cstddef>
#অন্তর্ভাব <inttypes.h>

template <typename তথ্য_type, size_t আকার_4 = 0>
শ্রেণী Timestamped_Ring_বাফার;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename তথ্য_type, size_t আকার_4>
শ্রেণী Timestamped_Ring_বাফার
{
সর্বজনীন:
	static_assert(আকার_4 > 0, "SIZE must be > 0");
	static_assert(আকার_4 <= UINT8_MAX, "SIZE must fit in uint8_t");
	স্থির constexpr uint8_t k_আকার = static_cast<uint8_t>(আকার_4);

	Timestamped_Ring_বাফার() = পূর্বনির্ধারিত;
	~Timestamped_Ring_বাফার() = পূর্বনির্ধারিত;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_বাফার(ধ্রুবক Timestamped_Ring_বাফার &) = delete;
	Timestamped_Ring_বাফার &operator=(ধ্রুবক Timestamped_Ring_বাফার &) = delete;
	Timestamped_Ring_বাফার(Timestamped_Ring_বাফার &&) = delete;
	Timestamped_Ring_বাফার &operator=(Timestamped_Ring_বাফার &&) = delete;

	যৌক্তিক allocate(size_t আকার_2) { ফেরত আকার_2 == আকার_4; }

	যৌক্তিক valid() ধ্রুবক { ফেরত আকার_4 > 0; }

	শূন্য ঠেলো(ধ্রুবক তথ্য_type &sample)
	{
		বাফার[শীর্ষ_2] = sample;
		শীর্ষ_2 = পরবর্তী(শীর্ষ_2);

		যদি (_entries < k_আকার) {
			_entries++;
		}
	}

	uint8_t get_দৈর্ঘ্য() ধ্রুবক { ফেরত k_আকার; }

	তথ্য_type &operator[](ধ্রুবক uint8_t সূচক) { ফেরত বাফার[সূচক]; }

	ধ্রুবক তথ্য_type &get_newest() ধ্রুবক { ফেরত বাফার[get_newest_সূচক()]; }
	ধ্রুবক তথ্য_type &get_oldest() ধ্রুবক { ফেরত বাফার[get_oldest_সূচক()]; }

	uint8_t get_newest_সূচক() ধ্রুবক { ফেরত (_entries > 0) ? পূর্ব(শীর্ষ_2) : 0; }
	uint8_t get_oldest_সূচক() ধ্রুবক
	{
		যদি (_entries == 0) {
			ফেরত 0;
		}

		ধ্রুবক uint16_t idx = static_cast<uint16_t>(শীর্ষ_2) + static_cast<uint16_t>(k_আকার) - static_cast<uint16_t>(_entries);
		ফেরত (idx >= k_আকার) ? static_cast<uint8_t>(idx - k_আকার) : static_cast<uint8_t>(idx);
	}

	uint8_t পরবর্তী(uint8_t i) ধ্রুবক { ফেরত (i + 1 >= k_আকার) ? 0 : i + 1; }
	uint8_t পূর্ব(uint8_t i) ধ্রুবক { ফেরত (i == 0) ? k_আকার - 1 : i - 1; }

	যৌক্তিক বের_করো_first_older_than(ধ্রুবক uint64_t &timestamp, তথ্য_type *sample)
	{
		// start looking from newest observation data
		ধ্রুবক uint8_t newest_idx = get_newest_সূচক();
		ধ্রুবক uint8_t oldest_idx = get_oldest_সূচক();

		জন্য (uint8_t i = 0; i < _entries; i++) {
			পূর্ণসংখ্যা সূচক = (newest_idx - i);
			সূচক = সূচক < 0 ? k_আকার + সূচক : সূচক;

			যদি (timestamp >= বাফার[সূচক].time_us && timestamp < বাফার[সূচক].time_us + (uint64_t)1e5) {
				*sample = বাফার[সূচক];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ধ্রুবক uint8_t সূচক_u = static_cast<uint8_t>(সূচক);
				ধ্রুবক uint8_t distance = (সূচক_u >= oldest_idx) ? (সূচক_u - oldest_idx) : (সূচক_u + k_আকার - oldest_idx);
				ধ্রুবক uint8_t discard_গুনতি = distance + 1;
				_entries = (_entries > discard_গুনতি) ? (_entries - discard_গুনতি) : 0;
				বাফার[সূচক].time_us = 0;

				ফেরত সত্য;
			}
		}

		ফেরত মিথ্যা;
	}

	পূর্ণসংখ্যা get_যোগ_আকার() ধ্রুবক
	{
		static_assert(আকার(Timestamped_Ring_বাফার<তথ্য_type, আকার_4>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		ফেরত static_cast<পূর্ণসংখ্যা>(আকার(Timestamped_Ring_বাফার<তথ্য_type, আকার_4>));
	}

	পূর্ণসংখ্যা entries() ধ্রুবক { ফেরত _entries; }
	যৌক্তিক empty() ধ্রুবক { ফেরত _entries == 0; }

	শূন্য reset()
	{
		জন্য (uint8_t i = 0; i < k_আকার; i++) {
			বাফার[i].time_us = 0;
		}

		শীর্ষ_2 = 0;
		_entries = 0;
	}

ব্যক্তিগত:
	তথ্য_type বাফার[আকার_4] {};

	uint8_t শীর্ষ_2{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename তথ্য_type>
শ্রেণী Timestamped_Ring_বাফার<তথ্য_type, 0>
{
সর্বজনীন:
	explicit Timestamped_Ring_বাফার(size_t আকার_2) { allocate(আকার_2); }
	Timestamped_Ring_বাফার() = delete;
	~Timestamped_Ring_বাফার() { delete[] বাফার; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_বাফার(ধ্রুবক Timestamped_Ring_বাফার &) = delete;
	Timestamped_Ring_বাফার &operator=(ধ্রুবক Timestamped_Ring_বাফার &) = delete;
	Timestamped_Ring_বাফার(Timestamped_Ring_বাফার &&) = delete;
	Timestamped_Ring_বাফার &operator=(Timestamped_Ring_বাফার &&) = delete;

	যৌক্তিক allocate(size_t আকার_2)
	{
		যদি (valid() && (আকার_2 == আকার_3)) {
			// no change
			ফেরত সত্য;
		}

		যদি (আকার_2 == 0 || আকার_2 > UINT8_MAX) {
			ফেরত মিথ্যা;
		}

		delete[] বাফার;

		বাফার = নতুন তথ্য_type[আকার_2] {};

		যদি (বাফার == nullptr) {
			ফেরত মিথ্যা;
		}

		আকার_3 = static_cast<uint8_t>(আকার_2);

		// new[]{} already zeroed time_us, only reset the indices
		শীর্ষ_2 = 0;
		লেজ_2 = 0;
		_first_write = সত্য;

		ফেরত সত্য;
	}

	যৌক্তিক valid() ধ্রুবক { ফেরত (বাফার != nullptr) && (আকার_3 > 0); }

	শূন্য ঠেলো(ধ্রুবক তথ্য_type &sample)
	{
		uint8_t শীর্ষ_new = শীর্ষ_2;

		যদি (!_first_write) {
			শীর্ষ_new = পরবর্তী(শীর্ষ_2);
		}

		বাফার[শীর্ষ_new] = sample;
		শীর্ষ_2 = শীর্ষ_new;

		// move tail if we overwrite it
		যদি (শীর্ষ_2 == লেজ_2 && !_first_write) {
			লেজ_2 = পরবর্তী(লেজ_2);

		} অন্যথা {
			_first_write = মিথ্যা;
		}
	}

	uint8_t get_দৈর্ঘ্য() ধ্রুবক { ফেরত আকার_3; }

	তথ্য_type &operator[](ধ্রুবক uint8_t সূচক) { ফেরত বাফার[সূচক]; }

	ধ্রুবক তথ্য_type &get_newest() ধ্রুবক { ফেরত বাফার[শীর্ষ_2]; }
	ধ্রুবক তথ্য_type &get_oldest() ধ্রুবক { ফেরত বাফার[লেজ_2]; }

	uint8_t get_newest_সূচক() ধ্রুবক { ফেরত শীর্ষ_2; }
	uint8_t get_oldest_সূচক() ধ্রুবক { ফেরত লেজ_2; }

	uint8_t পরবর্তী(uint8_t i) ধ্রুবক { ফেরত (আকার_3 > 0) ? ((i + 1 >= আকার_3) ? 0 : i + 1) : 0; }
	uint8_t পূর্ব(uint8_t i) ধ্রুবক { ফেরত (আকার_3 > 0) ? ((i == 0) ? (আকার_3 - 1) : (i - 1)) : 0; }

	যৌক্তিক বের_করো_first_older_than(ধ্রুবক uint64_t &timestamp, তথ্য_type *sample)
	{
		// start looking from newest observation data
		জন্য (uint8_t i = 0; i < আকার_3; i++) {
			পূর্ণসংখ্যা সূচক = (শীর্ষ_2 - i);
			সূচক = সূচক < 0 ? আকার_3 + সূচক : সূচক;

			যদি (timestamp >= বাফার[সূচক].time_us && timestamp < বাফার[সূচক].time_us + (uint64_t)1e5) {
				*sample = বাফার[সূচক];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				যদি (সূচক == শীর্ষ_2) {
					লেজ_2 = শীর্ষ_2;
					_first_write = সত্য;

				} অন্যথা {
					লেজ_2 = পরবর্তী(static_cast<uint8_t>(সূচক));
				}

				বাফার[সূচক].time_us = 0;

				ফেরত সত্য;
			}

			যদি (সূচক == লেজ_2) {
				// we have reached the tail and haven't got a
				// match
				ফেরত মিথ্যা;
			}
		}

		ফেরত মিথ্যা;
	}

	পূর্ণসংখ্যা get_used_আকার() ধ্রুবক { ফেরত আকার(*এই) + আকার(তথ্য_type) * entries(); }
	পূর্ণসংখ্যা get_যোগ_আকার() ধ্রুবক { ফেরত আকার(*এই) + আকার(তথ্য_type) * আকার_3; }

	পূর্ণসংখ্যা entries() ধ্রুবক
	{
		পূর্ণসংখ্যা গুনতি = 0;

		জন্য (uint8_t i = 0; i < আকার_3; i++) {
			যদি (বাফার[i].time_us != 0) {
				গুনতি++;
			}
		}

		ফেরত গুনতি;
	}

	যৌক্তিক empty() ধ্রুবক { ফেরত entries() == 0; }

	শূন্য reset()
	{
		যদি (বাফার) {
			জন্য (uint8_t i = 0; i < আকার_3; i++) {
				বাফার[i].time_us = 0;
			}

			শীর্ষ_2 = 0;
			লেজ_2 = 0;
			_first_write = সত্য;
		}
	}

ব্যক্তিগত:
	তথ্য_type *বাফার{nullptr};

	uint8_t শীর্ষ_2{0};
	uint8_t লেজ_2{0};
	uint8_t আকার_3{0};

	যৌক্তিক _first_write{সত্য};
};
