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

#لارښوونه once

#ضمن <climits>
#ضمن <cstddef>
#ضمن <inttypes.h>

کينډۍ <ډول_نوم معلومات_type, size_t اندازه_3 = 0>
صنف Timestamped_Ring_بفر;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
کينډۍ <ډول_نوم معلومات_type, size_t اندازه_3>
صنف Timestamped_Ring_بفر
{
عام:
	static_assert(اندازه_3 > 0, "SIZE must be > 0");
	static_assert(اندازه_3 <= UINT8_MAX, "SIZE must fit in uint8_t");
	ساكن constexpr uint8_t k_اندازه = static_cast<uint8_t>(اندازه_3);

	Timestamped_Ring_بفر() = افتراضي;
	~Timestamped_Ring_بفر() = افتراضي;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_بفر(ثابت Timestamped_Ring_بفر &) = ړنګول;
	Timestamped_Ring_بفر &چلوونکی=(ثابت Timestamped_Ring_بفر &) = ړنګول;
	Timestamped_Ring_بفر(Timestamped_Ring_بفر &&) = ړنګول;
	Timestamped_Ring_بفر &چلوونکی=(Timestamped_Ring_بفر &&) = ړنګول;

	منطقي allocate(size_t اندازه) { أرجع اندازه == اندازه_3; }

	منطقي valid() ثابت { أرجع اندازه_3 > 0; }

	فراغ ټېل(ثابت معلومات_type &sample)
	{
		بفر[سر_2] = sample;
		سر_2 = بل(سر_2);

		إذا (_entries < k_اندازه) {
			_entries++;
		}
	}

	uint8_t get_اوږدتیا() ثابت { أرجع k_اندازه; }

	معلومات_type &چلوونکی[](ثابت uint8_t شاخص) { أرجع بفر[شاخص]; }

	ثابت معلومات_type &get_newest() ثابت { أرجع بفر[get_newest_شاخص()]; }
	ثابت معلومات_type &get_oldest() ثابت { أرجع بفر[get_oldest_شاخص()]; }

	uint8_t get_newest_شاخص() ثابت { أرجع (_entries > 0) ? prev(سر_2) : 0; }
	uint8_t get_oldest_شاخص() ثابت
	{
		إذا (_entries == 0) {
			أرجع 0;
		}

		ثابت uint16_t idx = static_cast<uint16_t>(سر_2) + static_cast<uint16_t>(k_اندازه) - static_cast<uint16_t>(_entries);
		أرجع (idx >= k_اندازه) ? static_cast<uint8_t>(idx - k_اندازه) : static_cast<uint8_t>(idx);
	}

	uint8_t بل(uint8_t i) ثابت { أرجع (i + 1 >= k_اندازه) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ثابت { أرجع (i == 0) ? k_اندازه - 1 : i - 1; }

	منطقي راوباسه_لومړی_older_than(ثابت uint64_t &timestamp, معلومات_type *sample)
	{
		// start looking from newest observation data
		ثابت uint8_t newest_idx = get_newest_شاخص();
		ثابت uint8_t oldest_idx = get_oldest_شاخص();

		لكل (uint8_t i = 0; i < _entries; i++) {
			صحيح شاخص = (newest_idx - i);
			شاخص = شاخص < 0 ? k_اندازه + شاخص : شاخص;

			إذا (timestamp >= بفر[شاخص].وخت_us && timestamp < بفر[شاخص].وخت_us + (uint64_t)1e5) {
				*sample = بفر[شاخص];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ثابت uint8_t شاخص_u = static_cast<uint8_t>(شاخص);
				ثابت uint8_t واټن = (شاخص_u >= oldest_idx) ? (شاخص_u - oldest_idx) : (شاخص_u + k_اندازه - oldest_idx);
				ثابت uint8_t discard_شمېر = واټن + 1;
				_entries = (_entries > discard_شمېر) ? (_entries - discard_شمېر) : 0;
				بفر[شاخص].وخت_us = 0;

				أرجع ریښتیا;
			}
		}

		أرجع خطأ;
	}

	صحيح get_ټول_اندازه() ثابت
	{
		static_assert(حجم(Timestamped_Ring_بفر<معلومات_type, اندازه_3>) <= static_cast<size_t>(لوی_عدد),
			      "static ring buffer size must fit in int");
		أرجع static_cast<صحيح>(حجم(Timestamped_Ring_بفر<معلومات_type, اندازه_3>));
	}

	صحيح entries() ثابت { أرجع _entries; }
	منطقي تشه() ثابت { أرجع _entries == 0; }

	فراغ reset()
	{
		لكل (uint8_t i = 0; i < k_اندازه; i++) {
			بفر[i].وخت_us = 0;
		}

		سر_2 = 0;
		_entries = 0;
	}

خاص:
	معلومات_type بفر[اندازه_3] {};

	uint8_t سر_2{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
کينډۍ <ډول_نوم معلومات_type>
صنف Timestamped_Ring_بفر<معلومات_type, 0>
{
عام:
	څرګند Timestamped_Ring_بفر(size_t اندازه) { allocate(اندازه); }
	Timestamped_Ring_بفر() = ړنګول;
	~Timestamped_Ring_بفر() { ړنګول[] بفر; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_بفر(ثابت Timestamped_Ring_بفر &) = ړنګول;
	Timestamped_Ring_بفر &چلوونکی=(ثابت Timestamped_Ring_بفر &) = ړنګول;
	Timestamped_Ring_بفر(Timestamped_Ring_بفر &&) = ړنګول;
	Timestamped_Ring_بفر &چلوونکی=(Timestamped_Ring_بفر &&) = ړنګول;

	منطقي allocate(size_t اندازه)
	{
		إذا (valid() && (اندازه == اندازه_2)) {
			// no change
			أرجع ریښتیا;
		}

		إذا (اندازه == 0 || اندازه > UINT8_MAX) {
			أرجع خطأ;
		}

		ړنګول[] بفر;

		بفر = جديد معلومات_type[اندازه] {};

		إذا (بفر == تش_اشاره) {
			أرجع خطأ;
		}

		اندازه_2 = static_cast<uint8_t>(اندازه);

		// new[]{} already zeroed time_us, only reset the indices
		سر_2 = 0;
		لکۍ_2 = 0;
		لومړی_write = ریښتیا;

		أرجع ریښتیا;
	}

	منطقي valid() ثابت { أرجع (بفر != تش_اشاره) && (اندازه_2 > 0); }

	فراغ ټېل(ثابت معلومات_type &sample)
	{
		uint8_t سر_new = سر_2;

		إذا (!لومړی_write) {
			سر_new = بل(سر_2);
		}

		بفر[سر_new] = sample;
		سر_2 = سر_new;

		// move tail if we overwrite it
		إذا (سر_2 == لکۍ_2 && !لومړی_write) {
			لکۍ_2 = بل(لکۍ_2);

		} وإلا {
			لومړی_write = خطأ;
		}
	}

	uint8_t get_اوږدتیا() ثابت { أرجع اندازه_2; }

	معلومات_type &چلوونکی[](ثابت uint8_t شاخص) { أرجع بفر[شاخص]; }

	ثابت معلومات_type &get_newest() ثابت { أرجع بفر[سر_2]; }
	ثابت معلومات_type &get_oldest() ثابت { أرجع بفر[لکۍ_2]; }

	uint8_t get_newest_شاخص() ثابت { أرجع سر_2; }
	uint8_t get_oldest_شاخص() ثابت { أرجع لکۍ_2; }

	uint8_t بل(uint8_t i) ثابت { أرجع (اندازه_2 > 0) ? ((i + 1 >= اندازه_2) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ثابت { أرجع (اندازه_2 > 0) ? ((i == 0) ? (اندازه_2 - 1) : (i - 1)) : 0; }

	منطقي راوباسه_لومړی_older_than(ثابت uint64_t &timestamp, معلومات_type *sample)
	{
		// start looking from newest observation data
		لكل (uint8_t i = 0; i < اندازه_2; i++) {
			صحيح شاخص = (سر_2 - i);
			شاخص = شاخص < 0 ? اندازه_2 + شاخص : شاخص;

			إذا (timestamp >= بفر[شاخص].وخت_us && timestamp < بفر[شاخص].وخت_us + (uint64_t)1e5) {
				*sample = بفر[شاخص];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				إذا (شاخص == سر_2) {
					لکۍ_2 = سر_2;
					لومړی_write = ریښتیا;

				} وإلا {
					لکۍ_2 = بل(static_cast<uint8_t>(شاخص));
				}

				بفر[شاخص].وخت_us = 0;

				أرجع ریښتیا;
			}

			إذا (شاخص == لکۍ_2) {
				// we have reached the tail and haven't got a
				//سمون
				أرجع خطأ;
			}
		}

		أرجع خطأ;
	}

	صحيح get_کارول_شوی_اندازه() ثابت { أرجع حجم(*هذا) + حجم(معلومات_type) * entries(); }
	صحيح get_ټول_اندازه() ثابت { أرجع حجم(*هذا) + حجم(معلومات_type) * اندازه_2; }

	صحيح entries() ثابت
	{
		صحيح شمېر = 0;

		لكل (uint8_t i = 0; i < اندازه_2; i++) {
			إذا (بفر[i].وخت_us != 0) {
				شمېر++;
			}
		}

		أرجع شمېر;
	}

	منطقي تشه() ثابت { أرجع entries() == 0; }

	فراغ reset()
	{
		إذا (بفر) {
			لكل (uint8_t i = 0; i < اندازه_2; i++) {
				بفر[i].وخت_us = 0;
			}

			سر_2 = 0;
			لکۍ_2 = 0;
			لومړی_write = ریښتیا;
		}
	}

خاص:
	معلومات_type *بفر{تش_اشاره};

	uint8_t سر_2{0};
	uint8_t لکۍ_2{0};
	uint8_t اندازه_2{0};

	منطقي لومړی_write{ریښتیا};
};
