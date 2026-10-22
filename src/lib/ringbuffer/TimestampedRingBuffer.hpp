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

#شامل <climits>
#شامل <cstddef>
#شامل <inttypes.h>

سانچہ <قسم_نام ڈیٹا_type, size_t سائز_3 = 0>
جماعت Timestamped_Ring_بفر;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
سانچہ <قسم_نام ڈیٹا_type, size_t سائز_3>
جماعت Timestamped_Ring_بفر
{
عوامی:
	static_assert(سائز_3 > 0, "SIZE must be > 0");
	static_assert(سائز_3 <= UINT8_MAX, "SIZE must fit in uint8_t");
	ساکن constexpr uint8_t k_سائز = static_cast<uint8_t>(سائز_3);

	Timestamped_Ring_بفر() = طےشدہ;
	~Timestamped_Ring_بفر() = طےشدہ;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_بفر(ثابت Timestamped_Ring_بفر &) = حذف;
	Timestamped_Ring_بفر &عامل=(ثابت Timestamped_Ring_بفر &) = حذف;
	Timestamped_Ring_بفر(Timestamped_Ring_بفر &&) = حذف;
	Timestamped_Ring_بفر &عامل=(Timestamped_Ring_بفر &&) = حذف;

	منطقی allocate(size_t سائز) { واپس سائز == سائز_3; }

	منطقی valid() ثابت { واپس سائز_3 > 0; }

	خالی ڈالو(ثابت ڈیٹا_type &sample)
	{
		بفر[سر] = sample;
		سر = اگلا(سر);

		اگر (_entries < k_سائز) {
			_entries++;
		}
	}

	uint8_t get_لمبائی() ثابت { واپس k_سائز; }

	ڈیٹا_type &عامل[](ثابت uint8_t اشاریہ) { واپس بفر[اشاریہ]; }

	ثابت ڈیٹا_type &get_newest() ثابت { واپس بفر[get_newest_اشاریہ()]; }
	ثابت ڈیٹا_type &get_oldest() ثابت { واپس بفر[get_oldest_اشاریہ()]; }

	uint8_t get_newest_اشاریہ() ثابت { واپس (_entries > 0) ? پچھلا_2(سر) : 0; }
	uint8_t get_oldest_اشاریہ() ثابت
	{
		اگر (_entries == 0) {
			واپس 0;
		}

		ثابت uint16_t idx = static_cast<uint16_t>(سر) + static_cast<uint16_t>(k_سائز) - static_cast<uint16_t>(_entries);
		واپس (idx >= k_سائز) ? static_cast<uint8_t>(idx - k_سائز) : static_cast<uint8_t>(idx);
	}

	uint8_t اگلا(uint8_t i) ثابت { واپس (i + 1 >= k_سائز) ? 0 : i + 1; }
	uint8_t پچھلا_2(uint8_t i) ثابت { واپس (i == 0) ? k_سائز - 1 : i - 1; }

	منطقی نکالو_پہلا_older_than(ثابت uint64_t &timestamp, ڈیٹا_type *sample)
	{
		// start looking from newest observation data
		ثابت uint8_t newest_idx = get_newest_اشاریہ();
		ثابت uint8_t oldest_idx = get_oldest_اشاریہ();

		برائے (uint8_t i = 0; i < _entries; i++) {
			صحیح اشاریہ = (newest_idx - i);
			اشاریہ = اشاریہ < 0 ? k_سائز + اشاریہ : اشاریہ;

			اگر (timestamp >= بفر[اشاریہ].وقت_us && timestamp < بفر[اشاریہ].وقت_us + (uint64_t)1e5) {
				*sample = بفر[اشاریہ];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ثابت uint8_t اشاریہ_u = static_cast<uint8_t>(اشاریہ);
				ثابت uint8_t فاصلہ = (اشاریہ_u >= oldest_idx) ? (اشاریہ_u - oldest_idx) : (اشاریہ_u + k_سائز - oldest_idx);
				ثابت uint8_t discard_گنتی = فاصلہ + 1;
				_entries = (_entries > discard_گنتی) ? (_entries - discard_گنتی) : 0;
				بفر[اشاریہ].وقت_us = 0;

				واپس سچ;
			}
		}

		واپس جھوٹ;
	}

	صحیح get_کل_سائز() ثابت
	{
		static_assert(حجم(Timestamped_Ring_بفر<ڈیٹا_type, سائز_3>) <= static_cast<size_t>(زیادہ_سے_زیادہ_عدد),
			      "static ring buffer size must fit in int");
		واپس static_cast<صحیح>(حجم(Timestamped_Ring_بفر<ڈیٹا_type, سائز_3>));
	}

	صحیح entries() ثابت { واپس _entries; }
	منطقی خالی_خانے() ثابت { واپس _entries == 0; }

	خالی reset()
	{
		برائے (uint8_t i = 0; i < k_سائز; i++) {
			بفر[i].وقت_us = 0;
		}

		سر = 0;
		_entries = 0;
	}

نجی:
	ڈیٹا_type بفر[سائز_3] {};

	uint8_t سر{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
سانچہ <قسم_نام ڈیٹا_type>
جماعت Timestamped_Ring_بفر<ڈیٹا_type, 0>
{
عوامی:
	صریح Timestamped_Ring_بفر(size_t سائز) { allocate(سائز); }
	Timestamped_Ring_بفر() = حذف;
	~Timestamped_Ring_بفر() { حذف[] بفر; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_بفر(ثابت Timestamped_Ring_بفر &) = حذف;
	Timestamped_Ring_بفر &عامل=(ثابت Timestamped_Ring_بفر &) = حذف;
	Timestamped_Ring_بفر(Timestamped_Ring_بفر &&) = حذف;
	Timestamped_Ring_بفر &عامل=(Timestamped_Ring_بفر &&) = حذف;

	منطقی allocate(size_t سائز)
	{
		اگر (valid() && (سائز == سائز_2)) {
			// no change
			واپس سچ;
		}

		اگر (سائز == 0 || سائز > UINT8_MAX) {
			واپس جھوٹ;
		}

		حذف[] بفر;

		بفر = نیا ڈیٹا_type[سائز] {};

		اگر (بفر == خالی_اشاریہ) {
			واپس جھوٹ;
		}

		سائز_2 = static_cast<uint8_t>(سائز);

		// new[]{} already zeroed time_us, only reset the indices
		سر = 0;
		دم_2 = 0;
		پہلا_write = سچ;

		واپس سچ;
	}

	منطقی valid() ثابت { واپس (بفر != خالی_اشاریہ) && (سائز_2 > 0); }

	خالی ڈالو(ثابت ڈیٹا_type &sample)
	{
		uint8_t سر_new = سر;

		اگر (!پہلا_write) {
			سر_new = اگلا(سر);
		}

		بفر[سر_new] = sample;
		سر = سر_new;

		// move tail if we overwrite it
		اگر (سر == دم_2 && !پہلا_write) {
			دم_2 = اگلا(دم_2);

		} ورنہ {
			پہلا_write = جھوٹ;
		}
	}

	uint8_t get_لمبائی() ثابت { واپس سائز_2; }

	ڈیٹا_type &عامل[](ثابت uint8_t اشاریہ) { واپس بفر[اشاریہ]; }

	ثابت ڈیٹا_type &get_newest() ثابت { واپس بفر[سر]; }
	ثابت ڈیٹا_type &get_oldest() ثابت { واپس بفر[دم_2]; }

	uint8_t get_newest_اشاریہ() ثابت { واپس سر; }
	uint8_t get_oldest_اشاریہ() ثابت { واپس دم_2; }

	uint8_t اگلا(uint8_t i) ثابت { واپس (سائز_2 > 0) ? ((i + 1 >= سائز_2) ? 0 : i + 1) : 0; }
	uint8_t پچھلا_2(uint8_t i) ثابت { واپس (سائز_2 > 0) ? ((i == 0) ? (سائز_2 - 1) : (i - 1)) : 0; }

	منطقی نکالو_پہلا_older_than(ثابت uint64_t &timestamp, ڈیٹا_type *sample)
	{
		// start looking from newest observation data
		برائے (uint8_t i = 0; i < سائز_2; i++) {
			صحیح اشاریہ = (سر - i);
			اشاریہ = اشاریہ < 0 ? سائز_2 + اشاریہ : اشاریہ;

			اگر (timestamp >= بفر[اشاریہ].وقت_us && timestamp < بفر[اشاریہ].وقت_us + (uint64_t)1e5) {
				*sample = بفر[اشاریہ];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				اگر (اشاریہ == سر) {
					دم_2 = سر;
					پہلا_write = سچ;

				} ورنہ {
					دم_2 = اگلا(static_cast<uint8_t>(اشاریہ));
				}

				بفر[اشاریہ].وقت_us = 0;

				واپس سچ;
			}

			اگر (اشاریہ == دم_2) {
				// we have reached the tail and haven't got a
				//مطابقت
				واپس جھوٹ;
			}
		}

		واپس جھوٹ;
	}

	صحیح get_مستعمل_سائز() ثابت { واپس حجم(*یہ) + حجم(ڈیٹا_type) * entries(); }
	صحیح get_کل_سائز() ثابت { واپس حجم(*یہ) + حجم(ڈیٹا_type) * سائز_2; }

	صحیح entries() ثابت
	{
		صحیح گنتی = 0;

		برائے (uint8_t i = 0; i < سائز_2; i++) {
			اگر (بفر[i].وقت_us != 0) {
				گنتی++;
			}
		}

		واپس گنتی;
	}

	منطقی خالی_خانے() ثابت { واپس entries() == 0; }

	خالی reset()
	{
		اگر (بفر) {
			برائے (uint8_t i = 0; i < سائز_2; i++) {
				بفر[i].وقت_us = 0;
			}

			سر = 0;
			دم_2 = 0;
			پہلا_write = سچ;
		}
	}

نجی:
	ڈیٹا_type *بفر{خالی_اشاریہ};

	uint8_t سر{0};
	uint8_t دم_2{0};
	uint8_t سائز_2{0};

	منطقی پہلا_write{سچ};
};
