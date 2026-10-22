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

#సూచన once

#అన్తర్భావ <climits>
#అన్తర్భావ <cstddef>
#అన్తర్భావ <inttypes.h>

మూస <రకం_పేరు దత్తాంశం_type, size_t కొలత_3 = 0>
తరగతి Timestamped_Ring_బఫరు;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
మూస <రకం_పేరు దత్తాంశం_type, size_t కొలత_3>
తరగతి Timestamped_Ring_బఫరు
{
బహిరంగ:
	static_assert(కొలత_3 > 0, "SIZE must be > 0");
	static_assert(కొలత_3 <= UINT8_MAX, "SIZE must fit in uint8_t");
	స్థిర constexpr uint8_t k_కొలత = static_cast<uint8_t>(కొలత_3);

	Timestamped_Ring_బఫరు() = అప్రమేయ;
	~Timestamped_Ring_బఫరు() = అప్రమేయ;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_బఫరు(స్థిరరాశి Timestamped_Ring_బఫరు &) = తొలగించు;
	Timestamped_Ring_బఫరు &క్రియాకారకం=(స్థిరరాశి Timestamped_Ring_బఫరు &) = తొలగించు;
	Timestamped_Ring_బఫరు(Timestamped_Ring_బఫరు &&) = తొలగించు;
	Timestamped_Ring_బఫరు &క్రియాకారకం=(Timestamped_Ring_బఫరు &&) = తొలగించు;

	తార్కిక allocate(size_t కొలత) { తిరిగి కొలత == కొలత_3; }

	తార్కిక valid() స్థిరరాశి { తిరిగి కొలత_3 > 0; }

	శూన్య నెట్టు(స్థిరరాశి దత్తాంశం_type &sample)
	{
		బఫరు[తల] = sample;
		తల = తదుపరి(తల);

		ఒకవేళ (_entries < k_కొలత) {
			_entries++;
		}
	}

	uint8_t get_నిడివి() స్థిరరాశి { తిరిగి k_కొలత; }

	దత్తాంశం_type &క్రియాకారకం[](స్థిరరాశి uint8_t సూచీ) { తిరిగి బఫరు[సూచీ]; }

	స్థిరరాశి దత్తాంశం_type &get_newest() స్థిరరాశి { తిరిగి బఫరు[get_newest_సూచీ()]; }
	స్థిరరాశి దత్తాంశం_type &get_oldest() స్థిరరాశి { తిరిగి బఫరు[get_oldest_సూచీ()]; }

	uint8_t get_newest_సూచీ() స్థిరరాశి { తిరిగి (_entries > 0) ? prev(తల) : 0; }
	uint8_t get_oldest_సూచీ() స్థిరరాశి
	{
		ఒకవేళ (_entries == 0) {
			తిరిగి 0;
		}

		స్థిరరాశి uint16_t idx = static_cast<uint16_t>(తల) + static_cast<uint16_t>(k_కొలత) - static_cast<uint16_t>(_entries);
		తిరిగి (idx >= k_కొలత) ? static_cast<uint8_t>(idx - k_కొలత) : static_cast<uint8_t>(idx);
	}

	uint8_t తదుపరి(uint8_t i) స్థిరరాశి { తిరిగి (i + 1 >= k_కొలత) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) స్థిరరాశి { తిరిగి (i == 0) ? k_కొలత - 1 : i - 1; }

	తార్కిక తీయి_మొదటి_older_than(స్థిరరాశి uint64_t &timestamp, దత్తాంశం_type *sample)
	{
		// start looking from newest observation data
		స్థిరరాశి uint8_t newest_idx = get_newest_సూచీ();
		స్థిరరాశి uint8_t oldest_idx = get_oldest_సూచీ();

		కొరకు (uint8_t i = 0; i < _entries; i++) {
			పూర్ణాంకం సూచీ = (newest_idx - i);
			సూచీ = సూచీ < 0 ? k_కొలత + సూచీ : సూచీ;

			ఒకవేళ (timestamp >= బఫరు[సూచీ].సమయం_us && timestamp < బఫరు[సూచీ].సమయం_us + (uint64_t)1e5) {
				*sample = బఫరు[సూచీ];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				స్థిరరాశి uint8_t సూచీ_u = static_cast<uint8_t>(సూచీ);
				స్థిరరాశి uint8_t దూరం = (సూచీ_u >= oldest_idx) ? (సూచీ_u - oldest_idx) : (సూచీ_u + k_కొలత - oldest_idx);
				స్థిరరాశి uint8_t discard_లెక్క = దూరం + 1;
				_entries = (_entries > discard_లెక్క) ? (_entries - discard_లెక్క) : 0;
				బఫరు[సూచీ].సమయం_us = 0;

				తిరిగి నిజం;
			}
		}

		తిరిగి అబద్ధం;
	}

	పూర్ణాంకం get_మొత్తం_కొలత() స్థిరరాశి
	{
		static_assert(పరిమాణం(Timestamped_Ring_బఫరు<దత్తాంశం_type, కొలత_3>) <= static_cast<size_t>(గరిష్ఠ_పూర్ణాంకం),
			      "static ring buffer size must fit in int");
		తిరిగి static_cast<పూర్ణాంకం>(పరిమాణం(Timestamped_Ring_బఫరు<దత్తాంశం_type, కొలత_3>));
	}

	పూర్ణాంకం entries() స్థిరరాశి { తిరిగి _entries; }
	తార్కిక ఖాళీలు() స్థిరరాశి { తిరిగి _entries == 0; }

	శూన్య reset()
	{
		కొరకు (uint8_t i = 0; i < k_కొలత; i++) {
			బఫరు[i].సమయం_us = 0;
		}

		తల = 0;
		_entries = 0;
	}

అంతరంగ:
	దత్తాంశం_type బఫరు[కొలత_3] {};

	uint8_t తల{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
మూస <రకం_పేరు దత్తాంశం_type>
తరగతి Timestamped_Ring_బఫరు<దత్తాంశం_type, 0>
{
బహిరంగ:
	స్పష్టమైన Timestamped_Ring_బఫరు(size_t కొలత) { allocate(కొలత); }
	Timestamped_Ring_బఫరు() = తొలగించు;
	~Timestamped_Ring_బఫరు() { తొలగించు[] బఫరు; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_బఫరు(స్థిరరాశి Timestamped_Ring_బఫరు &) = తొలగించు;
	Timestamped_Ring_బఫరు &క్రియాకారకం=(స్థిరరాశి Timestamped_Ring_బఫరు &) = తొలగించు;
	Timestamped_Ring_బఫరు(Timestamped_Ring_బఫరు &&) = తొలగించు;
	Timestamped_Ring_బఫరు &క్రియాకారకం=(Timestamped_Ring_బఫరు &&) = తొలగించు;

	తార్కిక allocate(size_t కొలత)
	{
		ఒకవేళ (valid() && (కొలత == కొలత_2)) {
			// no change
			తిరిగి నిజం;
		}

		ఒకవేళ (కొలత == 0 || కొలత > UINT8_MAX) {
			తిరిగి అబద్ధం;
		}

		తొలగించు[] బఫరు;

		బఫరు = కొత్త దత్తాంశం_type[కొలత] {};

		ఒకవేళ (బఫరు == శూన్యసూచిక) {
			తిరిగి అబద్ధం;
		}

		కొలత_2 = static_cast<uint8_t>(కొలత);

		// new[]{} already zeroed time_us, only reset the indices
		తల = 0;
		తోక_2 = 0;
		మొదటి_write = నిజం;

		తిరిగి నిజం;
	}

	తార్కిక valid() స్థిరరాశి { తిరిగి (బఫరు != శూన్యసూచిక) && (కొలత_2 > 0); }

	శూన్య నెట్టు(స్థిరరాశి దత్తాంశం_type &sample)
	{
		uint8_t తల_new = తల;

		ఒకవేళ (!మొదటి_write) {
			తల_new = తదుపరి(తల);
		}

		బఫరు[తల_new] = sample;
		తల = తల_new;

		// move tail if we overwrite it
		ఒకవేళ (తల == తోక_2 && !మొదటి_write) {
			తోక_2 = తదుపరి(తోక_2);

		} లేకపోతే {
			మొదటి_write = అబద్ధం;
		}
	}

	uint8_t get_నిడివి() స్థిరరాశి { తిరిగి కొలత_2; }

	దత్తాంశం_type &క్రియాకారకం[](స్థిరరాశి uint8_t సూచీ) { తిరిగి బఫరు[సూచీ]; }

	స్థిరరాశి దత్తాంశం_type &get_newest() స్థిరరాశి { తిరిగి బఫరు[తల]; }
	స్థిరరాశి దత్తాంశం_type &get_oldest() స్థిరరాశి { తిరిగి బఫరు[తోక_2]; }

	uint8_t get_newest_సూచీ() స్థిరరాశి { తిరిగి తల; }
	uint8_t get_oldest_సూచీ() స్థిరరాశి { తిరిగి తోక_2; }

	uint8_t తదుపరి(uint8_t i) స్థిరరాశి { తిరిగి (కొలత_2 > 0) ? ((i + 1 >= కొలత_2) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) స్థిరరాశి { తిరిగి (కొలత_2 > 0) ? ((i == 0) ? (కొలత_2 - 1) : (i - 1)) : 0; }

	తార్కిక తీయి_మొదటి_older_than(స్థిరరాశి uint64_t &timestamp, దత్తాంశం_type *sample)
	{
		// start looking from newest observation data
		కొరకు (uint8_t i = 0; i < కొలత_2; i++) {
			పూర్ణాంకం సూచీ = (తల - i);
			సూచీ = సూచీ < 0 ? కొలత_2 + సూచీ : సూచీ;

			ఒకవేళ (timestamp >= బఫరు[సూచీ].సమయం_us && timestamp < బఫరు[సూచీ].సమయం_us + (uint64_t)1e5) {
				*sample = బఫరు[సూచీ];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ఒకవేళ (సూచీ == తల) {
					తోక_2 = తల;
					మొదటి_write = నిజం;

				} లేకపోతే {
					తోక_2 = తదుపరి(static_cast<uint8_t>(సూచీ));
				}

				బఫరు[సూచీ].సమయం_us = 0;

				తిరిగి నిజం;
			}

			ఒకవేళ (సూచీ == తోక_2) {
				// we have reached the tail and haven't got a
				//సరిపోలిక
				తిరిగి అబద్ధం;
			}
		}

		తిరిగి అబద్ధం;
	}

	పూర్ణాంకం get_వాడిన_కొలత() స్థిరరాశి { తిరిగి పరిమాణం(*ఇది) + పరిమాణం(దత్తాంశం_type) * entries(); }
	పూర్ణాంకం get_మొత్తం_కొలత() స్థిరరాశి { తిరిగి పరిమాణం(*ఇది) + పరిమాణం(దత్తాంశం_type) * కొలత_2; }

	పూర్ణాంకం entries() స్థిరరాశి
	{
		పూర్ణాంకం లెక్క = 0;

		కొరకు (uint8_t i = 0; i < కొలత_2; i++) {
			ఒకవేళ (బఫరు[i].సమయం_us != 0) {
				లెక్క++;
			}
		}

		తిరిగి లెక్క;
	}

	తార్కిక ఖాళీలు() స్థిరరాశి { తిరిగి entries() == 0; }

	శూన్య reset()
	{
		ఒకవేళ (బఫరు) {
			కొరకు (uint8_t i = 0; i < కొలత_2; i++) {
				బఫరు[i].సమయం_us = 0;
			}

			తల = 0;
			తోక_2 = 0;
			మొదటి_write = నిజం;
		}
	}

అంతరంగ:
	దత్తాంశం_type *బఫరు{శూన్యసూచిక};

	uint8_t తల{0};
	uint8_t తోక_2{0};
	uint8_t కొలత_2{0};

	తార్కిక మొదటి_write{నిజం};
};
