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

#pragma once

#अन्तर्भाव "ParamLayer.h"

class ConstLayer : सार्वजनिक ParamLayer
{
सार्वजनिक:

	ConstLayer() = स्वभाव;

	bool store(param_t param, param_मूल्य_u मूल्य) override
	{
		प्रत्यागम false;
	}

	bool contains(param_t param) ध्रुव override
	{
		प्रत्यागम param < PARAM_मोजणी;
	}

	px4::AtomicBitset<PARAM_मोजणी> containedAsBitset() ध्रुव override
	{
		px4::AtomicBitset<PARAM_मोजणी> set;

		प्रति (पूर्णाङ्क i = 0; i < PARAM_मोजणी; i++) {
			set.set(i);
		}

		प्रत्यागम set;
	}

	param_मूल्य_u get(param_t param) ध्रुव override
	{
		यदि (param >= PARAM_मोजणी) {
			प्रत्यागम {0};
		}

		प्रत्यागम px4::parameters[param].val;
	}

	शून्य reset(param_t param) override
	{
		// Do nothing
	}

	शून्य refresh(param_t param) override
	{
		// Do nothing
	}

	पूर्णाङ्क size() ध्रुव override
	{
		प्रत्यागम PARAM_मोजणी;
	}

	पूर्णाङ्क byteSize() ध्रुव override
	{
		प्रत्यागम PARAM_मोजणी * परिमाण(param_info_s);
	}
};
