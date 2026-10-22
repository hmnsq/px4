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

#رهنمود once

#ضمن <containers/List.hpp>
#ضمن <controllib/block/BlockParam.hpp>
#ضمن <cstdint>

نطاق control
{

ساكن constexpr uint8_t max_Children_در_هر_بلوک = 100;
ساكن constexpr uint8_t max_Params_در_هر_بلوک = 110;
ساكن constexpr uint8_t بلوک_Name_درازی_Max = 40;

// forward declaration
صنف بلوک_Param_پایه;
صنف Super_بلوک;

/**
 */
صنف __EXPORT بلوک : عام List_گره<بلوک *>
{
عام:
	دوست صنف بلوک_Param_پایه;

	بلوک(Super_بلوک *والد, ثابت حرف *name);
	مجازی ~بلوک() = افتراضي;

	// no copy, assignment, move, move assignment
	بلوک(ثابت بلوک &) = حذف;
	بلوک &عملگر=(ثابت بلوک &) = حذف;
	بلوک(بلوک &&) = حذف;
	بلوک &عملگر=(بلوک &&) = حذف;

	فراغ getName(حرف *name, size_t n);

	مجازی فراغ updateParams();

	مجازی فراغ مجموعه_Dt(عائم dt) { _dt = dt; }
	عائم getDt() { أرجع _dt; }

محمي:

	مجازی فراغ updateParamsSubclass() {}

	Super_بلوک *get_والد() { أرجع والد_2; }
	List<بلوک_Param_پایه *> &getParams() { أرجع _params; }

	ثابت حرف *_name;
	Super_بلوک *والد_2;
	عائم _dt{0.0f};

	List<بلوک_Param_پایه *> _params;
};

صنف __EXPORT Super_بلوک :
	عام بلوک
{
عام:
	دوست صنف بلوک;

	Super_بلوک(Super_بلوک *والد, ثابت حرف *name) : بلوک(والد, name) {}
	~Super_بلوک() = افتراضي;

	// no copy, assignment, move, move assignment
	Super_بلوک(ثابت Super_بلوک &) = حذف;
	Super_بلوک &عملگر=(ثابت Super_بلوک &) = حذف;
	Super_بلوک(Super_بلوک &&) = حذف;
	Super_بلوک &عملگر=(Super_بلوک &&) = حذف;

	فراغ مجموعه_Dt(عائم dt) override;

	فراغ updateParams() override
	{
		بلوک::updateParams();

		إذا (getChildren().get_سر() != اشاره_تهی) { update_فرزند_Params(); }
	}

محمي:
	List<بلوک *> &getChildren() { أرجع _children; }
	فراغ update_فرزند_Params();

	List<بلوک *> _children;
};


} // namespace control
