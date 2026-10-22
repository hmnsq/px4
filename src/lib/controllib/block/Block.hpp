/****************************************************************************
 *
 *   Copyright (C) 2012-2017 PX4 Development Team. All rights reserved.
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
 * @file Block.hpp
 *
 * Controller library code
 */

#ہدایت once

#ضمن <containers/List.hpp>
#ضمن <controllib/block/BlockParam.hpp>
#ضمن <cstdint>

نطاق control
{

ساكن constexpr uint8_t max_Children_فی_بلاک = 100;
ساكن constexpr uint8_t max_Params_فی_بلاک = 110;
ساكن constexpr uint8_t بلاک_Name_لمائی_Max = 40;

// forward declaration
صنف بلاک_Param_ادھار;
صنف Super_بلاک;

/**
 */
صنف __EXPORT بلاک : عام List_گرہ<بلاک *>
{
عام:
	یار صنف بلاک_Param_ادھار;

	بلاک(Super_بلاک *ماپا, ثابت حرف *name);
	مجازی ~بلاک() = افتراضي;

	// no copy, assignment, move, move assignment
	بلاک(ثابت بلاک &) = مٹاؤ;
	بلاک &عامل=(ثابت بلاک &) = مٹاؤ;
	بلاک(بلاک &&) = مٹاؤ;
	بلاک &عامل=(بلاک &&) = مٹاؤ;

	فراغ getName(حرف *name, size_t n);

	مجازی فراغ updateParams();

	مجازی فراغ سیٹ_Dt(عائم dt) { _dt = dt; }
	عائم getDt() { أرجع _dt; }

محمي:

	مجازی فراغ updateParamsSubclass() {}

	Super_بلاک *get_ماپا() { أرجع ماپا_2; }
	List<بلاک_Param_ادھار *> &getParams() { أرجع _params; }

	ثابت حرف *_name;
	Super_بلاک *ماپا_2;
	عائم _dt{0.0f};

	List<بلاک_Param_ادھار *> _params;
};

صنف __EXPORT Super_بلاک :
	عام بلاک
{
عام:
	یار صنف بلاک;

	Super_بلاک(Super_بلاک *ماپا, ثابت حرف *name) : بلاک(ماپا, name) {}
	~Super_بلاک() = افتراضي;

	// no copy, assignment, move, move assignment
	Super_بلاک(ثابت Super_بلاک &) = مٹاؤ;
	Super_بلاک &عامل=(ثابت Super_بلاک &) = مٹاؤ;
	Super_بلاک(Super_بلاک &&) = مٹاؤ;
	Super_بلاک &عامل=(Super_بلاک &&) = مٹاؤ;

	فراغ سیٹ_Dt(عائم dt) override;

	فراغ updateParams() override
	{
		بلاک::updateParams();

		إذا (getChildren().get_سر() != خالی_سوچک) { update_بچہ_Params(); }
	}

محمي:
	List<بلاک *> &getChildren() { أرجع _children; }
	فراغ update_بچہ_Params();

	List<بلاک *> _children;
};


} // namespace control
