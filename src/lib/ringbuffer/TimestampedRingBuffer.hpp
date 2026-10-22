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

#ہدایت once

#ضمن <climits>
#ضمن <cstddef>
#ضمن <inttypes.h>

سانچا <قسم_ناں انکڑے_type, size_t ناپ_3 = 0>
صنف Timestamped_Ring_بفر;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
سانچا <قسم_ناں انکڑے_type, size_t ناپ_3>
صنف Timestamped_Ring_بفر
{
عام:
	static_assert(ناپ_3 > 0, "SIZE must be > 0");
	static_assert(ناپ_3 <= UINT8_MAX, "SIZE must fit in uint8_t");
	ساكن constexpr uint8_t k_ناپ = static_cast<uint8_t>(ناپ_3);

	Timestamped_Ring_بفر() = افتراضي;
	~Timestamped_Ring_بفر() = افتراضي;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_بفر(ثابت Timestamped_Ring_بفر &) = مٹاؤ;
	Timestamped_Ring_بفر &عامل=(ثابت Timestamped_Ring_بفر &) = مٹاؤ;
	Timestamped_Ring_بفر(Timestamped_Ring_بفر &&) = مٹاؤ;
	Timestamped_Ring_بفر &عامل=(Timestamped_Ring_بفر &&) = مٹاؤ;

	منطقي allocate(size_t ناپ) { أرجع ناپ == ناپ_3; }

	منطقي valid() ثابت { أرجع ناپ_3 > 0; }

	فراغ ڈالو(ثابت انکڑے_type &sample)
	{
		بفر[سر] = sample;
		سر = اگلا(سر);

		إذا (_entries < k_ناپ) {
			_entries++;
		}
	}

	uint8_t get_لمائی() ثابت { أرجع k_ناپ; }

	انکڑے_type &عامل[](ثابت uint8_t اشاریہ) { أرجع بفر[اشاریہ]; }

	ثابت انکڑے_type &get_newest() ثابت { أرجع بفر[get_newest_اشاریہ()]; }
	ثابت انکڑے_type &get_oldest() ثابت { أرجع بفر[get_oldest_اشاریہ()]; }

	uint8_t get_newest_اشاریہ() ثابت { أرجع (_entries > 0) ? prev(سر) : 0; }
	uint8_t get_oldest_اشاریہ() ثابت
	{
		إذا (_entries == 0) {
			أرجع 0;
		}

		ثابت uint16_t idx = static_cast<uint16_t>(سر) + static_cast<uint16_t>(k_ناپ) - static_cast<uint16_t>(_entries);
		أرجع (idx >= k_ناپ) ? static_cast<uint8_t>(idx - k_ناپ) : static_cast<uint8_t>(idx);
	}

	uint8_t اگلا(uint8_t i) ثابت { أرجع (i + 1 >= k_ناپ) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ثابت { أرجع (i == 0) ? k_ناپ - 1 : i - 1; }

	منطقي نکالو_پہلا_older_than(ثابت uint64_t &timestamp, انکڑے_type *sample)
	{
		// start looking from newest observation data
		ثابت uint8_t newest_idx = get_newest_اشاریہ();
		ثابت uint8_t oldest_idx = get_oldest_اشاریہ();

		لكل (uint8_t i = 0; i < _entries; i++) {
			صحيح اشاریہ = (newest_idx - i);
			اشاریہ = اشاریہ < 0 ? k_ناپ + اشاریہ : اشاریہ;

			إذا (timestamp >= بفر[اشاریہ].ویلا_us && timestamp < بفر[اشاریہ].ویلا_us + (uint64_t)1e5) {
				*sample = بفر[اشاریہ];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ثابت uint8_t اشاریہ_u = static_cast<uint8_t>(اشاریہ);
				ثابت uint8_t فاصلہ = (اشاریہ_u >= oldest_idx) ? (اشاریہ_u - oldest_idx) : (اشاریہ_u + k_ناپ - oldest_idx);
				ثابت uint8_t discard_گنتی = فاصلہ + 1;
				_entries = (_entries > discard_گنتی) ? (_entries - discard_گنتی) : 0;
				بفر[اشاریہ].ویلا_us = 0;

				أرجع سچ;
			}
		}

		أرجع خطأ;
	}

	صحيح get_کل_ناپ() ثابت
	{
		static_assert(حجم(Timestamped_Ring_بفر<انکڑے_type, ناپ_3>) <= static_cast<size_t>(ودھ_توں_ودھ_عدد),
			      "static ring buffer size must fit in int");
		أرجع static_cast<صحيح>(حجم(Timestamped_Ring_بفر<انکڑے_type, ناپ_3>));
	}

	صحيح entries() ثابت { أرجع _entries; }
	منطقي خالی_تھاواں() ثابت { أرجع _entries == 0; }

	فراغ reset()
	{
		لكل (uint8_t i = 0; i < k_ناپ; i++) {
			بفر[i].ویلا_us = 0;
		}

		سر = 0;
		_entries = 0;
	}

خاص:
	انکڑے_type بفر[ناپ_3] {};

	uint8_t سر{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
سانچا <قسم_ناں انکڑے_type>
صنف Timestamped_Ring_بفر<انکڑے_type, 0>
{
عام:
	واضح Timestamped_Ring_بفر(size_t ناپ) { allocate(ناپ); }
	Timestamped_Ring_بفر() = مٹاؤ;
	~Timestamped_Ring_بفر() { مٹاؤ[] بفر; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_بفر(ثابت Timestamped_Ring_بفر &) = مٹاؤ;
	Timestamped_Ring_بفر &عامل=(ثابت Timestamped_Ring_بفر &) = مٹاؤ;
	Timestamped_Ring_بفر(Timestamped_Ring_بفر &&) = مٹاؤ;
	Timestamped_Ring_بفر &عامل=(Timestamped_Ring_بفر &&) = مٹاؤ;

	منطقي allocate(size_t ناپ)
	{
		إذا (valid() && (ناپ == ناپ_2)) {
			// no change
			أرجع سچ;
		}

		إذا (ناپ == 0 || ناپ > UINT8_MAX) {
			أرجع خطأ;
		}

		مٹاؤ[] بفر;

		بفر = جديد انکڑے_type[ناپ] {};

		إذا (بفر == خالی_سوچک) {
			أرجع خطأ;
		}

		ناپ_2 = static_cast<uint8_t>(ناپ);

		// new[]{} already zeroed time_us, only reset the indices
		سر = 0;
		پوچھ_2 = 0;
		پہلا_write = سچ;

		أرجع سچ;
	}

	منطقي valid() ثابت { أرجع (بفر != خالی_سوچک) && (ناپ_2 > 0); }

	فراغ ڈالو(ثابت انکڑے_type &sample)
	{
		uint8_t سر_new = سر;

		إذا (!پہلا_write) {
			سر_new = اگلا(سر);
		}

		بفر[سر_new] = sample;
		سر = سر_new;

		// move tail if we overwrite it
		إذا (سر == پوچھ_2 && !پہلا_write) {
			پوچھ_2 = اگلا(پوچھ_2);

		} وإلا {
			پہلا_write = خطأ;
		}
	}

	uint8_t get_لمائی() ثابت { أرجع ناپ_2; }

	انکڑے_type &عامل[](ثابت uint8_t اشاریہ) { أرجع بفر[اشاریہ]; }

	ثابت انکڑے_type &get_newest() ثابت { أرجع بفر[سر]; }
	ثابت انکڑے_type &get_oldest() ثابت { أرجع بفر[پوچھ_2]; }

	uint8_t get_newest_اشاریہ() ثابت { أرجع سر; }
	uint8_t get_oldest_اشاریہ() ثابت { أرجع پوچھ_2; }

	uint8_t اگلا(uint8_t i) ثابت { أرجع (ناپ_2 > 0) ? ((i + 1 >= ناپ_2) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ثابت { أرجع (ناپ_2 > 0) ? ((i == 0) ? (ناپ_2 - 1) : (i - 1)) : 0; }

	منطقي نکالو_پہلا_older_than(ثابت uint64_t &timestamp, انکڑے_type *sample)
	{
		// start looking from newest observation data
		لكل (uint8_t i = 0; i < ناپ_2; i++) {
			صحيح اشاریہ = (سر - i);
			اشاریہ = اشاریہ < 0 ? ناپ_2 + اشاریہ : اشاریہ;

			إذا (timestamp >= بفر[اشاریہ].ویلا_us && timestamp < بفر[اشاریہ].ویلا_us + (uint64_t)1e5) {
				*sample = بفر[اشاریہ];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				إذا (اشاریہ == سر) {
					پوچھ_2 = سر;
					پہلا_write = سچ;

				} وإلا {
					پوچھ_2 = اگلا(static_cast<uint8_t>(اشاریہ));
				}

				بفر[اشاریہ].ویلا_us = 0;

				أرجع سچ;
			}

			إذا (اشاریہ == پوچھ_2) {
				// we have reached the tail and haven't got a
				//میل
				أرجع خطأ;
			}
		}

		أرجع خطأ;
	}

	صحيح get_ورتیا_ناپ() ثابت { أرجع حجم(*هذا) + حجم(انکڑے_type) * entries(); }
	صحيح get_کل_ناپ() ثابت { أرجع حجم(*هذا) + حجم(انکڑے_type) * ناپ_2; }

	صحيح entries() ثابت
	{
		صحيح گنتی = 0;

		لكل (uint8_t i = 0; i < ناپ_2; i++) {
			إذا (بفر[i].ویلا_us != 0) {
				گنتی++;
			}
		}

		أرجع گنتی;
	}

	منطقي خالی_تھاواں() ثابت { أرجع entries() == 0; }

	فراغ reset()
	{
		إذا (بفر) {
			لكل (uint8_t i = 0; i < ناپ_2; i++) {
				بفر[i].ویلا_us = 0;
			}

			سر = 0;
			پوچھ_2 = 0;
			پہلا_write = سچ;
		}
	}

خاص:
	انکڑے_type *بفر{خالی_سوچک};

	uint8_t سر{0};
	uint8_t پوچھ_2{0};
	uint8_t ناپ_2{0};

	منطقي پہلا_write{سچ};
};
