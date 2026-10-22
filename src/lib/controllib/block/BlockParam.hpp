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
 * @file BlockParam.hpp
 *
 * Controller library code
 */

#هدايت once

#ضمن <containers/List.hpp>
#ضمن <px4_platform_common/defines.h>
#ضمن <parameters/param.h>

نطاق control
{

صنف بلاڪ;

// A base class for block params that enables traversing linked list.
صنف بلاڪ_Param_بنياد : عام List_ڳنڍ<بلاڪ_Param_بنياد *>
{
عام:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	بلاڪ_Param_بنياد(بلاڪ *والد, ثابت حرف *name, منطقي والد_اڳياڙي = سچ);
	مجازي ~بلاڪ_Param_بنياد() = افتراضي;

	مجازي منطقي update() = 0;
	ثابت حرف *getName() ثابت { أرجع param_name(_handle); }

محمي:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
سانچو <صنف T>
صنف __EXPORT بلاڪ_Param final : عام بلاڪ_Param_بنياد
{
عام:
	بلاڪ_Param(بلاڪ *بلاڪ_2, ثابت حرف *name, منطقي والد_اڳياڙي = سچ);
	بلاڪ_Param(بلاڪ *بلاڪ_2, ثابت حرف *name, منطقي والد_اڳياڙي, T &extern_val);

	~بلاڪ_Param() override = افتراضي;

	// no copy, assignment, move, move assignment
	بلاڪ_Param(ثابت بلاڪ_Param &) = ميٽيو;
	بلاڪ_Param &عامل=(ثابت بلاڪ_Param &) = ميٽيو;
	بلاڪ_Param(بلاڪ_Param &&) = ميٽيو;
	بلاڪ_Param &عامل=(بلاڪ_Param &&) = ميٽيو;

	T get() ثابت { أرجع _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	منطقي commit() { أرجع (param_سيٽ(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	منطقي commit_no_notification() { أرجع (param_سيٽ_no_notification(_handle, &_val) == PX4_OK); }

	فراغ سيٽ(T val) { _val = val; }

	منطقي update() override { أرجع (param_get(_handle, &_val) == PX4_OK); }

محمي:
	T _val;
};

سانچو <>
منطقي بلاڪ_Param<منطقي>::update();

تعريفنوع بلاڪ_Param<عائم> بلاڪ_Param_Float;
تعريفنوع بلاڪ_Param<int32_t> بلاڪ_Param_Int;
تعريفنوع بلاڪ_Param<منطقي> بلاڪ_Param_Bool;
تعريفنوع بلاڪ_Param<عائم &> بلاڪ_Param_Ext_Float;
تعريفنوع بلاڪ_Param<int32_t &> بلاڪ_Param_Ext_Int;

} // namespace control
