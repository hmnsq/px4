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

#ہدایت once

#شامل "ParamLayer.h"

class ConstLayer : عوامی ParamLayer
{
عوامی:

	ConstLayer() = طےشدہ;

	bool store(param_t param, param_قدر_u قدر) override
	{
		واپس false;
	}

	bool شامل_ہے(param_t param) ثابت override
	{
		واپس param < PARAM_گنتی;
	}

	px4::AtomicBitset<PARAM_گنتی> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_گنتی> سیٹ;

		برائے (صحیح i = 0; i < PARAM_گنتی; i++) {
			سیٹ.سیٹ(i);
		}

		واپس سیٹ;
	}

	param_قدر_u get(param_t param) ثابت override
	{
		اگر (param >= PARAM_گنتی) {
			واپس {0};
		}

		واپس px4::parameters[param].val;
	}

	خالی reset(param_t param) override
	{
		// Do nothing
	}

	خالی refresh(param_t param) override
	{
		// Do nothing
	}

	صحیح سائز() ثابت override
	{
		واپس PARAM_گنتی;
	}

	صحیح byte_سائز() ثابت override
	{
		واپس PARAM_گنتی * حجم(param_info_s);
	}
};
