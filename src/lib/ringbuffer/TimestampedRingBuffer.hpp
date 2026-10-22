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

#هدايت once

#ضمن <climits>
#ضمن <cstddef>
#ضمن <inttypes.h>

سانچو <قسم_نالو معلومات_type, size_t سائيز_3 = 0>
صنف Timestamped_Ring_بفر;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
سانچو <قسم_نالو معلومات_type, size_t سائيز_3>
صنف Timestamped_Ring_بفر
{
عام:
	static_assert(سائيز_3 > 0, "SIZE must be > 0");
	static_assert(سائيز_3 <= UINT8_MAX, "SIZE must fit in uint8_t");
	ساكن constexpr uint8_t k_سائيز = static_cast<uint8_t>(سائيز_3);

	Timestamped_Ring_بفر() = افتراضي;
	~Timestamped_Ring_بفر() = افتراضي;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_بفر(ثابت Timestamped_Ring_بفر &) = ميٽيو;
	Timestamped_Ring_بفر &عامل=(ثابت Timestamped_Ring_بفر &) = ميٽيو;
	Timestamped_Ring_بفر(Timestamped_Ring_بفر &&) = ميٽيو;
	Timestamped_Ring_بفر &عامل=(Timestamped_Ring_بفر &&) = ميٽيو;

	منطقي allocate(size_t سائيز) { أرجع سائيز == سائيز_3; }

	منطقي valid() ثابت { أرجع سائيز_3 > 0; }

	فراغ ڌڪو(ثابت معلومات_type &sample)
	{
		بفر[سر] = sample;
		سر = ايندڙ(سر);

		إذا (_entries < k_سائيز) {
			_entries++;
		}
	}

	uint8_t get_ڊيگهائي() ثابت { أرجع k_سائيز; }

	معلومات_type &عامل[](ثابت uint8_t اشاريو) { أرجع بفر[اشاريو]; }

	ثابت معلومات_type &get_newest() ثابت { أرجع بفر[get_newest_اشاريو()]; }
	ثابت معلومات_type &get_oldest() ثابت { أرجع بفر[get_oldest_اشاريو()]; }

	uint8_t get_newest_اشاريو() ثابت { أرجع (_entries > 0) ? prev(سر) : 0; }
	uint8_t get_oldest_اشاريو() ثابت
	{
		إذا (_entries == 0) {
			أرجع 0;
		}

		ثابت uint16_t idx = static_cast<uint16_t>(سر) + static_cast<uint16_t>(k_سائيز) - static_cast<uint16_t>(_entries);
		أرجع (idx >= k_سائيز) ? static_cast<uint8_t>(idx - k_سائيز) : static_cast<uint8_t>(idx);
	}

	uint8_t ايندڙ(uint8_t i) ثابت { أرجع (i + 1 >= k_سائيز) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ثابت { أرجع (i == 0) ? k_سائيز - 1 : i - 1; }

	منطقي ڪڍو_پهريون_older_than(ثابت uint64_t &timestamp, معلومات_type *sample)
	{
		// start looking from newest observation data
		ثابت uint8_t newest_idx = get_newest_اشاريو();
		ثابت uint8_t oldest_idx = get_oldest_اشاريو();

		لكل (uint8_t i = 0; i < _entries; i++) {
			صحيح اشاريو = (newest_idx - i);
			اشاريو = اشاريو < 0 ? k_سائيز + اشاريو : اشاريو;

			إذا (timestamp >= بفر[اشاريو].وقت_us && timestamp < بفر[اشاريو].وقت_us + (uint64_t)1e5) {
				*sample = بفر[اشاريو];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ثابت uint8_t اشاريو_u = static_cast<uint8_t>(اشاريو);
				ثابت uint8_t مفاصلو = (اشاريو_u >= oldest_idx) ? (اشاريو_u - oldest_idx) : (اشاريو_u + k_سائيز - oldest_idx);
				ثابت uint8_t discard_ڳڻپ = مفاصلو + 1;
				_entries = (_entries > discard_ڳڻپ) ? (_entries - discard_ڳڻپ) : 0;
				بفر[اشاريو].وقت_us = 0;

				أرجع سچ;
			}
		}

		أرجع خطأ;
	}

	صحيح get_ڪل_سائيز() ثابت
	{
		static_assert(حجم(Timestamped_Ring_بفر<معلومات_type, سائيز_3>) <= static_cast<size_t>(وڌ_کان_وڌ_عدد),
			      "static ring buffer size must fit in int");
		أرجع static_cast<صحيح>(حجم(Timestamped_Ring_بفر<معلومات_type, سائيز_3>));
	}

	صحيح entries() ثابت { أرجع _entries; }
	منطقي خالي_جايون() ثابت { أرجع _entries == 0; }

	فراغ reset()
	{
		لكل (uint8_t i = 0; i < k_سائيز; i++) {
			بفر[i].وقت_us = 0;
		}

		سر = 0;
		_entries = 0;
	}

خاص:
	معلومات_type بفر[سائيز_3] {};

	uint8_t سر{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
سانچو <قسم_نالو معلومات_type>
صنف Timestamped_Ring_بفر<معلومات_type, 0>
{
عام:
	واضح Timestamped_Ring_بفر(size_t سائيز) { allocate(سائيز); }
	Timestamped_Ring_بفر() = ميٽيو;
	~Timestamped_Ring_بفر() { ميٽيو[] بفر; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_بفر(ثابت Timestamped_Ring_بفر &) = ميٽيو;
	Timestamped_Ring_بفر &عامل=(ثابت Timestamped_Ring_بفر &) = ميٽيو;
	Timestamped_Ring_بفر(Timestamped_Ring_بفر &&) = ميٽيو;
	Timestamped_Ring_بفر &عامل=(Timestamped_Ring_بفر &&) = ميٽيو;

	منطقي allocate(size_t سائيز)
	{
		إذا (valid() && (سائيز == سائيز_2)) {
			// no change
			أرجع سچ;
		}

		إذا (سائيز == 0 || سائيز > UINT8_MAX) {
			أرجع خطأ;
		}

		ميٽيو[] بفر;

		بفر = جديد معلومات_type[سائيز] {};

		إذا (بفر == خالي_اشارو) {
			أرجع خطأ;
		}

		سائيز_2 = static_cast<uint8_t>(سائيز);

		// new[]{} already zeroed time_us, only reset the indices
		سر = 0;
		پڇ_2 = 0;
		پهريون_write = سچ;

		أرجع سچ;
	}

	منطقي valid() ثابت { أرجع (بفر != خالي_اشارو) && (سائيز_2 > 0); }

	فراغ ڌڪو(ثابت معلومات_type &sample)
	{
		uint8_t سر_new = سر;

		إذا (!پهريون_write) {
			سر_new = ايندڙ(سر);
		}

		بفر[سر_new] = sample;
		سر = سر_new;

		// move tail if we overwrite it
		إذا (سر == پڇ_2 && !پهريون_write) {
			پڇ_2 = ايندڙ(پڇ_2);

		} وإلا {
			پهريون_write = خطأ;
		}
	}

	uint8_t get_ڊيگهائي() ثابت { أرجع سائيز_2; }

	معلومات_type &عامل[](ثابت uint8_t اشاريو) { أرجع بفر[اشاريو]; }

	ثابت معلومات_type &get_newest() ثابت { أرجع بفر[سر]; }
	ثابت معلومات_type &get_oldest() ثابت { أرجع بفر[پڇ_2]; }

	uint8_t get_newest_اشاريو() ثابت { أرجع سر; }
	uint8_t get_oldest_اشاريو() ثابت { أرجع پڇ_2; }

	uint8_t ايندڙ(uint8_t i) ثابت { أرجع (سائيز_2 > 0) ? ((i + 1 >= سائيز_2) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ثابت { أرجع (سائيز_2 > 0) ? ((i == 0) ? (سائيز_2 - 1) : (i - 1)) : 0; }

	منطقي ڪڍو_پهريون_older_than(ثابت uint64_t &timestamp, معلومات_type *sample)
	{
		// start looking from newest observation data
		لكل (uint8_t i = 0; i < سائيز_2; i++) {
			صحيح اشاريو = (سر - i);
			اشاريو = اشاريو < 0 ? سائيز_2 + اشاريو : اشاريو;

			إذا (timestamp >= بفر[اشاريو].وقت_us && timestamp < بفر[اشاريو].وقت_us + (uint64_t)1e5) {
				*sample = بفر[اشاريو];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				إذا (اشاريو == سر) {
					پڇ_2 = سر;
					پهريون_write = سچ;

				} وإلا {
					پڇ_2 = ايندڙ(static_cast<uint8_t>(اشاريو));
				}

				بفر[اشاريو].وقت_us = 0;

				أرجع سچ;
			}

			إذا (اشاريو == پڇ_2) {
				// we have reached the tail and haven't got a
				//ميلاپ
				أرجع خطأ;
			}
		}

		أرجع خطأ;
	}

	صحيح get_استعمال_ٿيل_سائيز() ثابت { أرجع حجم(*هذا) + حجم(معلومات_type) * entries(); }
	صحيح get_ڪل_سائيز() ثابت { أرجع حجم(*هذا) + حجم(معلومات_type) * سائيز_2; }

	صحيح entries() ثابت
	{
		صحيح ڳڻپ = 0;

		لكل (uint8_t i = 0; i < سائيز_2; i++) {
			إذا (بفر[i].وقت_us != 0) {
				ڳڻپ++;
			}
		}

		أرجع ڳڻپ;
	}

	منطقي خالي_جايون() ثابت { أرجع entries() == 0; }

	فراغ reset()
	{
		إذا (بفر) {
			لكل (uint8_t i = 0; i < سائيز_2; i++) {
				بفر[i].وقت_us = 0;
			}

			سر = 0;
			پڇ_2 = 0;
			پهريون_write = سچ;
		}
	}

خاص:
	معلومات_type *بفر{خالي_اشارو};

	uint8_t سر{0};
	uint8_t پڇ_2{0};
	uint8_t سائيز_2{0};

	منطقي پهريون_write{سچ};
};
