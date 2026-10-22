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

#لارښوونه once

#ضمن "ParamLayer.h"

class ConstLayer : عام ParamLayer
{
عام:

	ConstLayer() = افتراضي;

	bool store(param_t param, param_ارزښت_u ارزښت) override
	{
		أرجع false;
	}

	bool لري(param_t param) ثابت override
	{
		أرجع param < PARAM_شمېر;
	}

	px4::AtomicBitset<PARAM_شمېر> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_شمېر> سېټ;

		لكل (صحيح i = 0; i < PARAM_شمېر; i++) {
			سېټ.سېټ(i);
		}

		أرجع سېټ;
	}

	param_ارزښت_u get(param_t param) ثابت override
	{
		إذا (param >= PARAM_شمېر) {
			أرجع {0};
		}

		أرجع px4::parameters[param].val;
	}

	فراغ reset(param_t param) override
	{
		// Do nothing
	}

	فراغ refresh(param_t param) override
	{
		// Do nothing
	}

	صحيح اندازه() ثابت override
	{
		أرجع PARAM_شمېر;
	}

	صحيح byte_اندازه() ثابت override
	{
		أرجع PARAM_شمېر * حجم(param_info_s);
	}
};
