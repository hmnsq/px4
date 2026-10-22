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

#رهنمود once

#ضمن <climits>
#ضمن <cstddef>
#ضمن <inttypes.h>

قالب <نام_نوع داده_type, size_t اندازه_3 = 0>
صنف Timestamped_Ring_میانگیر;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
قالب <نام_نوع داده_type, size_t اندازه_3>
صنف Timestamped_Ring_میانگیر
{
عام:
	static_assert(اندازه_3 > 0, "SIZE must be > 0");
	static_assert(اندازه_3 <= UINT8_MAX, "SIZE must fit in uint8_t");
	ساكن constexpr uint8_t k_اندازه = static_cast<uint8_t>(اندازه_3);

	Timestamped_Ring_میانگیر() = افتراضي;
	~Timestamped_Ring_میانگیر() = افتراضي;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_میانگیر(ثابت Timestamped_Ring_میانگیر &) = حذف;
	Timestamped_Ring_میانگیر &عملگر=(ثابت Timestamped_Ring_میانگیر &) = حذف;
	Timestamped_Ring_میانگیر(Timestamped_Ring_میانگیر &&) = حذف;
	Timestamped_Ring_میانگیر &عملگر=(Timestamped_Ring_میانگیر &&) = حذف;

	منطقي allocate(size_t اندازه) { أرجع اندازه == اندازه_3; }

	منطقي valid() ثابت { أرجع اندازه_3 > 0; }

	فراغ افزودن(ثابت داده_type &sample)
	{
		میانگیر[سر] = sample;
		سر = بعدی(سر);

		إذا (_entries < k_اندازه) {
			_entries++;
		}
	}

	uint8_t get_درازی() ثابت { أرجع k_اندازه; }

	داده_type &عملگر[](ثابت uint8_t شاخص) { أرجع میانگیر[شاخص]; }

	ثابت داده_type &get_newest() ثابت { أرجع میانگیر[get_newest_شاخص()]; }
	ثابت داده_type &get_oldest() ثابت { أرجع میانگیر[get_oldest_شاخص()]; }

	uint8_t get_newest_شاخص() ثابت { أرجع (_entries > 0) ? prev(سر) : 0; }
	uint8_t get_oldest_شاخص() ثابت
	{
		إذا (_entries == 0) {
			أرجع 0;
		}

		ثابت uint16_t idx = static_cast<uint16_t>(سر) + static_cast<uint16_t>(k_اندازه) - static_cast<uint16_t>(_entries);
		أرجع (idx >= k_اندازه) ? static_cast<uint8_t>(idx - k_اندازه) : static_cast<uint8_t>(idx);
	}

	uint8_t بعدی(uint8_t i) ثابت { أرجع (i + 1 >= k_اندازه) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ثابت { أرجع (i == 0) ? k_اندازه - 1 : i - 1; }

	منطقي برداشت_اولی_older_than(ثابت uint64_t &timestamp, داده_type *sample)
	{
		// start looking from newest observation data
		ثابت uint8_t newest_idx = get_newest_شاخص();
		ثابت uint8_t oldest_idx = get_oldest_شاخص();

		لكل (uint8_t i = 0; i < _entries; i++) {
			صحيح شاخص = (newest_idx - i);
			شاخص = شاخص < 0 ? k_اندازه + شاخص : شاخص;

			إذا (timestamp >= میانگیر[شاخص].زمان_us && timestamp < میانگیر[شاخص].زمان_us + (uint64_t)1e5) {
				*sample = میانگیر[شاخص];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ثابت uint8_t شاخص_u = static_cast<uint8_t>(شاخص);
				ثابت uint8_t فاصله = (شاخص_u >= oldest_idx) ? (شاخص_u - oldest_idx) : (شاخص_u + k_اندازه - oldest_idx);
				ثابت uint8_t discard_شمار = فاصله + 1;
				_entries = (_entries > discard_شمار) ? (_entries - discard_شمار) : 0;
				میانگیر[شاخص].زمان_us = 0;

				أرجع درست;
			}
		}

		أرجع خطأ;
	}

	صحيح get_مجموع_اندازه() ثابت
	{
		static_assert(حجم(Timestamped_Ring_میانگیر<داده_type, اندازه_3>) <= static_cast<size_t>(بیشترین_صحیح),
			      "static ring buffer size must fit in int");
		أرجع static_cast<صحيح>(حجم(Timestamped_Ring_میانگیر<داده_type, اندازه_3>));
	}

	صحيح entries() ثابت { أرجع _entries; }
	منطقي خالی() ثابت { أرجع _entries == 0; }

	فراغ reset()
	{
		لكل (uint8_t i = 0; i < k_اندازه; i++) {
			میانگیر[i].زمان_us = 0;
		}

		سر = 0;
		_entries = 0;
	}

خاص:
	داده_type میانگیر[اندازه_3] {};

	uint8_t سر{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
قالب <نام_نوع داده_type>
صنف Timestamped_Ring_میانگیر<داده_type, 0>
{
عام:
	صریح Timestamped_Ring_میانگیر(size_t اندازه) { allocate(اندازه); }
	Timestamped_Ring_میانگیر() = حذف;
	~Timestamped_Ring_میانگیر() { حذف[] میانگیر; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_میانگیر(ثابت Timestamped_Ring_میانگیر &) = حذف;
	Timestamped_Ring_میانگیر &عملگر=(ثابت Timestamped_Ring_میانگیر &) = حذف;
	Timestamped_Ring_میانگیر(Timestamped_Ring_میانگیر &&) = حذف;
	Timestamped_Ring_میانگیر &عملگر=(Timestamped_Ring_میانگیر &&) = حذف;

	منطقي allocate(size_t اندازه)
	{
		إذا (valid() && (اندازه == اندازه_2)) {
			// no change
			أرجع درست;
		}

		إذا (اندازه == 0 || اندازه > UINT8_MAX) {
			أرجع خطأ;
		}

		حذف[] میانگیر;

		میانگیر = جديد داده_type[اندازه] {};

		إذا (میانگیر == اشاره_تهی) {
			أرجع خطأ;
		}

		اندازه_2 = static_cast<uint8_t>(اندازه);

		// new[]{} already zeroed time_us, only reset the indices
		سر = 0;
		دم_2 = 0;
		اولی_write = درست;

		أرجع درست;
	}

	منطقي valid() ثابت { أرجع (میانگیر != اشاره_تهی) && (اندازه_2 > 0); }

	فراغ افزودن(ثابت داده_type &sample)
	{
		uint8_t سر_new = سر;

		إذا (!اولی_write) {
			سر_new = بعدی(سر);
		}

		میانگیر[سر_new] = sample;
		سر = سر_new;

		// move tail if we overwrite it
		إذا (سر == دم_2 && !اولی_write) {
			دم_2 = بعدی(دم_2);

		} وإلا {
			اولی_write = خطأ;
		}
	}

	uint8_t get_درازی() ثابت { أرجع اندازه_2; }

	داده_type &عملگر[](ثابت uint8_t شاخص) { أرجع میانگیر[شاخص]; }

	ثابت داده_type &get_newest() ثابت { أرجع میانگیر[سر]; }
	ثابت داده_type &get_oldest() ثابت { أرجع میانگیر[دم_2]; }

	uint8_t get_newest_شاخص() ثابت { أرجع سر; }
	uint8_t get_oldest_شاخص() ثابت { أرجع دم_2; }

	uint8_t بعدی(uint8_t i) ثابت { أرجع (اندازه_2 > 0) ? ((i + 1 >= اندازه_2) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ثابت { أرجع (اندازه_2 > 0) ? ((i == 0) ? (اندازه_2 - 1) : (i - 1)) : 0; }

	منطقي برداشت_اولی_older_than(ثابت uint64_t &timestamp, داده_type *sample)
	{
		// start looking from newest observation data
		لكل (uint8_t i = 0; i < اندازه_2; i++) {
			صحيح شاخص = (سر - i);
			شاخص = شاخص < 0 ? اندازه_2 + شاخص : شاخص;

			إذا (timestamp >= میانگیر[شاخص].زمان_us && timestamp < میانگیر[شاخص].زمان_us + (uint64_t)1e5) {
				*sample = میانگیر[شاخص];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				إذا (شاخص == سر) {
					دم_2 = سر;
					اولی_write = درست;

				} وإلا {
					دم_2 = بعدی(static_cast<uint8_t>(شاخص));
				}

				میانگیر[شاخص].زمان_us = 0;

				أرجع درست;
			}

			إذا (شاخص == دم_2) {
				// we have reached the tail and haven't got a
				//تطابق
				أرجع خطأ;
			}
		}

		أرجع خطأ;
	}

	صحيح get_استفاده_شده_اندازه() ثابت { أرجع حجم(*هذا) + حجم(داده_type) * entries(); }
	صحيح get_مجموع_اندازه() ثابت { أرجع حجم(*هذا) + حجم(داده_type) * اندازه_2; }

	صحيح entries() ثابت
	{
		صحيح شمار = 0;

		لكل (uint8_t i = 0; i < اندازه_2; i++) {
			إذا (میانگیر[i].زمان_us != 0) {
				شمار++;
			}
		}

		أرجع شمار;
	}

	منطقي خالی() ثابت { أرجع entries() == 0; }

	فراغ reset()
	{
		إذا (میانگیر) {
			لكل (uint8_t i = 0; i < اندازه_2; i++) {
				میانگیر[i].زمان_us = 0;
			}

			سر = 0;
			دم_2 = 0;
			اولی_write = درست;
		}
	}

خاص:
	داده_type *میانگیر{اشاره_تهی};

	uint8_t سر{0};
	uint8_t دم_2{0};
	uint8_t اندازه_2{0};

	منطقي اولی_write{درست};
};
