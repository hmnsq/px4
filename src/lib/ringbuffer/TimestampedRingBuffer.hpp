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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <climits>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>
#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ਅੰਕੜੇ_type, size_t ਮਾਪ_3 = 0>
ਵਰਗ Timestamped_Ring_ਬਫਰ;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ਅੰਕੜੇ_type, size_t ਮਾਪ_3>
ਵਰਗ Timestamped_Ring_ਬਫਰ
{
ਜਨਤਕ:
	static_assert(ਮਾਪ_3 > 0, "SIZE must be > 0");
	static_assert(ਮਾਪ_3 <= UINT8_MAX, "SIZE must fit in uint8_t");
	ਸਥਿਰ constexpr uint8_t k_ਮਾਪ = static_cast<uint8_t>(ਮਾਪ_3);

	Timestamped_Ring_ਬਫਰ() = ਮੂਲ;
	~Timestamped_Ring_ਬਫਰ() = ਮੂਲ;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_ਬਫਰ(ਅਚਲ Timestamped_Ring_ਬਫਰ &) = ਮਿਟਾਓ;
	Timestamped_Ring_ਬਫਰ &ਸੰਚਾਲਕ=(ਅਚਲ Timestamped_Ring_ਬਫਰ &) = ਮਿਟਾਓ;
	Timestamped_Ring_ਬਫਰ(Timestamped_Ring_ਬਫਰ &&) = ਮਿਟਾਓ;
	Timestamped_Ring_ਬਫਰ &ਸੰਚਾਲਕ=(Timestamped_Ring_ਬਫਰ &&) = ਮਿਟਾਓ;

	ਤਾਰਕਿਕ allocate(size_t ਮਾਪ) { ਵਾਪਸ ਮਾਪ == ਮਾਪ_3; }

	ਤਾਰਕਿਕ valid() ਅਚਲ { ਵਾਪਸ ਮਾਪ_3 > 0; }

	ਖਾਲੀ ਧੱਕੋ(ਅਚਲ ਅੰਕੜੇ_type &sample)
	{
		ਬਫਰ[ਸਿਰ] = sample;
		ਸਿਰ = ਅਗਲਾ(ਸਿਰ);

		ਜੇ (_entries < k_ਮਾਪ) {
			_entries++;
		}
	}

	uint8_t get_ਲੰਮਾਈ() ਅਚਲ { ਵਾਪਸ k_ਮਾਪ; }

	ਅੰਕੜੇ_type &ਸੰਚਾਲਕ[](ਅਚਲ uint8_t ਸੂਚਕਾਂਕ) { ਵਾਪਸ ਬਫਰ[ਸੂਚਕਾਂਕ]; }

	ਅਚਲ ਅੰਕੜੇ_type &get_newest() ਅਚਲ { ਵਾਪਸ ਬਫਰ[get_newest_ਸੂਚਕਾਂਕ()]; }
	ਅਚਲ ਅੰਕੜੇ_type &get_oldest() ਅਚਲ { ਵਾਪਸ ਬਫਰ[get_oldest_ਸੂਚਕਾਂਕ()]; }

	uint8_t get_newest_ਸੂਚਕਾਂਕ() ਅਚਲ { ਵਾਪਸ (_entries > 0) ? prev(ਸਿਰ) : 0; }
	uint8_t get_oldest_ਸੂਚਕਾਂਕ() ਅਚਲ
	{
		ਜੇ (_entries == 0) {
			ਵਾਪਸ 0;
		}

		ਅਚਲ uint16_t idx = static_cast<uint16_t>(ਸਿਰ) + static_cast<uint16_t>(k_ਮਾਪ) - static_cast<uint16_t>(_entries);
		ਵਾਪਸ (idx >= k_ਮਾਪ) ? static_cast<uint8_t>(idx - k_ਮਾਪ) : static_cast<uint8_t>(idx);
	}

	uint8_t ਅਗਲਾ(uint8_t i) ਅਚਲ { ਵਾਪਸ (i + 1 >= k_ਮਾਪ) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ਅਚਲ { ਵਾਪਸ (i == 0) ? k_ਮਾਪ - 1 : i - 1; }

	ਤਾਰਕਿਕ ਕੱਢੋ_ਪਹਿਲਾ_older_than(ਅਚਲ uint64_t &timestamp, ਅੰਕੜੇ_type *sample)
	{
		// start looking from newest observation data
		ਅਚਲ uint8_t newest_idx = get_newest_ਸੂਚਕਾਂਕ();
		ਅਚਲ uint8_t oldest_idx = get_oldest_ਸੂਚਕਾਂਕ();

		ਲਈ (uint8_t i = 0; i < _entries; i++) {
			ਪੂਰਨ ਸੂਚਕਾਂਕ = (newest_idx - i);
			ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ < 0 ? k_ਮਾਪ + ਸੂਚਕਾਂਕ : ਸੂਚਕਾਂਕ;

			ਜੇ (timestamp >= ਬਫਰ[ਸੂਚਕਾਂਕ].ਸਮਾਂ_us && timestamp < ਬਫਰ[ਸੂਚਕਾਂਕ].ਸਮਾਂ_us + (uint64_t)1e5) {
				*sample = ਬਫਰ[ਸੂਚਕਾਂਕ];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ਅਚਲ uint8_t ਸੂਚਕਾਂਕ_u = static_cast<uint8_t>(ਸੂਚਕਾਂਕ);
				ਅਚਲ uint8_t ਦੂਰੀ = (ਸੂਚਕਾਂਕ_u >= oldest_idx) ? (ਸੂਚਕਾਂਕ_u - oldest_idx) : (ਸੂਚਕਾਂਕ_u + k_ਮਾਪ - oldest_idx);
				ਅਚਲ uint8_t discard_ਗਿਣਤੀ = ਦੂਰੀ + 1;
				_entries = (_entries > discard_ਗਿਣਤੀ) ? (_entries - discard_ਗਿਣਤੀ) : 0;
				ਬਫਰ[ਸੂਚਕਾਂਕ].ਸਮਾਂ_us = 0;

				ਵਾਪਸ ਸੱਚ;
			}
		}

		ਵਾਪਸ ਝੂਠ;
	}

	ਪੂਰਨ get_ਕੁੱਲ_ਮਾਪ() ਅਚਲ
	{
		static_assert(ਆਕਾਰ(Timestamped_Ring_ਬਫਰ<ਅੰਕੜੇ_type, ਮਾਪ_3>) <= static_cast<size_t>(ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ),
			      "static ring buffer size must fit in int");
		ਵਾਪਸ static_cast<ਪੂਰਨ>(ਆਕਾਰ(Timestamped_Ring_ਬਫਰ<ਅੰਕੜੇ_type, ਮਾਪ_3>));
	}

	ਪੂਰਨ entries() ਅਚਲ { ਵਾਪਸ _entries; }
	ਤਾਰਕਿਕ ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ _entries == 0; }

	ਖਾਲੀ reset()
	{
		ਲਈ (uint8_t i = 0; i < k_ਮਾਪ; i++) {
			ਬਫਰ[i].ਸਮਾਂ_us = 0;
		}

		ਸਿਰ = 0;
		_entries = 0;
	}

ਨਿਜੀ:
	ਅੰਕੜੇ_type ਬਫਰ[ਮਾਪ_3] {};

	uint8_t ਸਿਰ{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ਅੰਕੜੇ_type>
ਵਰਗ Timestamped_Ring_ਬਫਰ<ਅੰਕੜੇ_type, 0>
{
ਜਨਤਕ:
	ਸਪਸ਼ਟ Timestamped_Ring_ਬਫਰ(size_t ਮਾਪ) { allocate(ਮਾਪ); }
	Timestamped_Ring_ਬਫਰ() = ਮਿਟਾਓ;
	~Timestamped_Ring_ਬਫਰ() { ਮਿਟਾਓ[] ਬਫਰ; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_ਬਫਰ(ਅਚਲ Timestamped_Ring_ਬਫਰ &) = ਮਿਟਾਓ;
	Timestamped_Ring_ਬਫਰ &ਸੰਚਾਲਕ=(ਅਚਲ Timestamped_Ring_ਬਫਰ &) = ਮਿਟਾਓ;
	Timestamped_Ring_ਬਫਰ(Timestamped_Ring_ਬਫਰ &&) = ਮਿਟਾਓ;
	Timestamped_Ring_ਬਫਰ &ਸੰਚਾਲਕ=(Timestamped_Ring_ਬਫਰ &&) = ਮਿਟਾਓ;

	ਤਾਰਕਿਕ allocate(size_t ਮਾਪ)
	{
		ਜੇ (valid() && (ਮਾਪ == ਮਾਪ_2)) {
			// no change
			ਵਾਪਸ ਸੱਚ;
		}

		ਜੇ (ਮਾਪ == 0 || ਮਾਪ > UINT8_MAX) {
			ਵਾਪਸ ਝੂਠ;
		}

		ਮਿਟਾਓ[] ਬਫਰ;

		ਬਫਰ = ਨਵਾਂ ਅੰਕੜੇ_type[ਮਾਪ] {};

		ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
			ਵਾਪਸ ਝੂਠ;
		}

		ਮਾਪ_2 = static_cast<uint8_t>(ਮਾਪ);

		// new[]{} already zeroed time_us, only reset the indices
		ਸਿਰ = 0;
		ਪੂਛ_2 = 0;
		ਪਹਿਲਾ_write = ਸੱਚ;

		ਵਾਪਸ ਸੱਚ;
	}

	ਤਾਰਕਿਕ valid() ਅਚਲ { ਵਾਪਸ (ਬਫਰ != ਖਾਲੀ_ਸੂਚਕ) && (ਮਾਪ_2 > 0); }

	ਖਾਲੀ ਧੱਕੋ(ਅਚਲ ਅੰਕੜੇ_type &sample)
	{
		uint8_t ਸਿਰ_new = ਸਿਰ;

		ਜੇ (!ਪਹਿਲਾ_write) {
			ਸਿਰ_new = ਅਗਲਾ(ਸਿਰ);
		}

		ਬਫਰ[ਸਿਰ_new] = sample;
		ਸਿਰ = ਸਿਰ_new;

		// move tail if we overwrite it
		ਜੇ (ਸਿਰ == ਪੂਛ_2 && !ਪਹਿਲਾ_write) {
			ਪੂਛ_2 = ਅਗਲਾ(ਪੂਛ_2);

		} ਨਹੀਂਤਾਂ {
			ਪਹਿਲਾ_write = ਝੂਠ;
		}
	}

	uint8_t get_ਲੰਮਾਈ() ਅਚਲ { ਵਾਪਸ ਮਾਪ_2; }

	ਅੰਕੜੇ_type &ਸੰਚਾਲਕ[](ਅਚਲ uint8_t ਸੂਚਕਾਂਕ) { ਵਾਪਸ ਬਫਰ[ਸੂਚਕਾਂਕ]; }

	ਅਚਲ ਅੰਕੜੇ_type &get_newest() ਅਚਲ { ਵਾਪਸ ਬਫਰ[ਸਿਰ]; }
	ਅਚਲ ਅੰਕੜੇ_type &get_oldest() ਅਚਲ { ਵਾਪਸ ਬਫਰ[ਪੂਛ_2]; }

	uint8_t get_newest_ਸੂਚਕਾਂਕ() ਅਚਲ { ਵਾਪਸ ਸਿਰ; }
	uint8_t get_oldest_ਸੂਚਕਾਂਕ() ਅਚਲ { ਵਾਪਸ ਪੂਛ_2; }

	uint8_t ਅਗਲਾ(uint8_t i) ਅਚਲ { ਵਾਪਸ (ਮਾਪ_2 > 0) ? ((i + 1 >= ਮਾਪ_2) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ਅਚਲ { ਵਾਪਸ (ਮਾਪ_2 > 0) ? ((i == 0) ? (ਮਾਪ_2 - 1) : (i - 1)) : 0; }

	ਤਾਰਕਿਕ ਕੱਢੋ_ਪਹਿਲਾ_older_than(ਅਚਲ uint64_t &timestamp, ਅੰਕੜੇ_type *sample)
	{
		// start looking from newest observation data
		ਲਈ (uint8_t i = 0; i < ਮਾਪ_2; i++) {
			ਪੂਰਨ ਸੂਚਕਾਂਕ = (ਸਿਰ - i);
			ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ < 0 ? ਮਾਪ_2 + ਸੂਚਕਾਂਕ : ਸੂਚਕਾਂਕ;

			ਜੇ (timestamp >= ਬਫਰ[ਸੂਚਕਾਂਕ].ਸਮਾਂ_us && timestamp < ਬਫਰ[ਸੂਚਕਾਂਕ].ਸਮਾਂ_us + (uint64_t)1e5) {
				*sample = ਬਫਰ[ਸੂਚਕਾਂਕ];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ਜੇ (ਸੂਚਕਾਂਕ == ਸਿਰ) {
					ਪੂਛ_2 = ਸਿਰ;
					ਪਹਿਲਾ_write = ਸੱਚ;

				} ਨਹੀਂਤਾਂ {
					ਪੂਛ_2 = ਅਗਲਾ(static_cast<uint8_t>(ਸੂਚਕਾਂਕ));
				}

				ਬਫਰ[ਸੂਚਕਾਂਕ].ਸਮਾਂ_us = 0;

				ਵਾਪਸ ਸੱਚ;
			}

			ਜੇ (ਸੂਚਕਾਂਕ == ਪੂਛ_2) {
				// we have reached the tail and haven't got a
				//ਮੇਲ
				ਵਾਪਸ ਝੂਠ;
			}
		}

		ਵਾਪਸ ਝੂਠ;
	}

	ਪੂਰਨ get_ਵਰਤਿਆ_ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਆਕਾਰ(*ਇਹ) + ਆਕਾਰ(ਅੰਕੜੇ_type) * entries(); }
	ਪੂਰਨ get_ਕੁੱਲ_ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਆਕਾਰ(*ਇਹ) + ਆਕਾਰ(ਅੰਕੜੇ_type) * ਮਾਪ_2; }

	ਪੂਰਨ entries() ਅਚਲ
	{
		ਪੂਰਨ ਗਿਣਤੀ_2 = 0;

		ਲਈ (uint8_t i = 0; i < ਮਾਪ_2; i++) {
			ਜੇ (ਬਫਰ[i].ਸਮਾਂ_us != 0) {
				ਗਿਣਤੀ_2++;
			}
		}

		ਵਾਪਸ ਗਿਣਤੀ_2;
	}

	ਤਾਰਕਿਕ ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ entries() == 0; }

	ਖਾਲੀ reset()
	{
		ਜੇ (ਬਫਰ) {
			ਲਈ (uint8_t i = 0; i < ਮਾਪ_2; i++) {
				ਬਫਰ[i].ਸਮਾਂ_us = 0;
			}

			ਸਿਰ = 0;
			ਪੂਛ_2 = 0;
			ਪਹਿਲਾ_write = ਸੱਚ;
		}
	}

ਨਿਜੀ:
	ਅੰਕੜੇ_type *ਬਫਰ{ਖਾਲੀ_ਸੂਚਕ};

	uint8_t ਸਿਰ{0};
	uint8_t ਪੂਛ_2{0};
	uint8_t ਮਾਪ_2{0};

	ਤਾਰਕਿਕ ਪਹਿਲਾ_write{ਸੱਚ};
};
