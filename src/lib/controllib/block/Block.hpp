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

#لارښوونه once

#ضمن <containers/List.hpp>
#ضمن <controllib/block/BlockParam.hpp>
#ضمن <cstdint>

نطاق control
{

ساكن constexpr uint8_t max_Children_فی_بلاک = 100;
ساكن constexpr uint8_t max_Params_فی_بلاک = 110;
ساكن constexpr uint8_t بلاک_Name_اوږدتیا_Max = 40;

// forward declaration
صنف بلاک_Param_بنسټ;
صنف Super_بلاک;

/**
 */
صنف __EXPORT بلاک : عام List_غوټه<بلاک *>
{
عام:
	ملګری صنف بلاک_Param_بنسټ;

	بلاک(Super_بلاک *پلار, ثابت حرف *name);
	مجازي ~بلاک() = افتراضي;

	// no copy, assignment, move, move assignment
	بلاک(ثابت بلاک &) = ړنګول;
	بلاک &چلوونکی=(ثابت بلاک &) = ړنګول;
	بلاک(بلاک &&) = ړنګول;
	بلاک &چلوونکی=(بلاک &&) = ړنګول;

	فراغ getName(حرف *name, size_t n);

	مجازي فراغ updateParams();

	مجازي فراغ سېټ_Dt(عائم dt) { _dt = dt; }
	عائم getDt() { أرجع _dt; }

محمي:

	مجازي فراغ updateParamsSubclass() {}

	Super_بلاک *get_پلار() { أرجع پلار_2; }
	List<بلاک_Param_بنسټ *> &getParams() { أرجع _params; }

	ثابت حرف *_name;
	Super_بلاک *پلار_2;
	عائم _dt{0.0f};

	List<بلاک_Param_بنسټ *> _params;
};

صنف __EXPORT Super_بلاک :
	عام بلاک
{
عام:
	ملګری صنف بلاک;

	Super_بلاک(Super_بلاک *پلار, ثابت حرف *name) : بلاک(پلار, name) {}
	~Super_بلاک() = افتراضي;

	// no copy, assignment, move, move assignment
	Super_بلاک(ثابت Super_بلاک &) = ړنګول;
	Super_بلاک &چلوونکی=(ثابت Super_بلاک &) = ړنګول;
	Super_بلاک(Super_بلاک &&) = ړنګول;
	Super_بلاک &چلوونکی=(Super_بلاک &&) = ړنګول;

	فراغ سېټ_Dt(عائم dt) override;

	فراغ updateParams() override
	{
		بلاک::updateParams();

		إذا (getChildren().get_سر() != تش_اشاره) { update_ماشوم_Params(); }
	}

محمي:
	List<بلاک *> &getChildren() { أرجع _children; }
	فراغ update_ماشوم_Params();

	List<بلاک *> _children;
};


} // namespace control
