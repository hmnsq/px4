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

#ہدایت once

#شامل <containers/List.hpp>
#شامل <px4_platform_common/defines.h>
#شامل <parameters/param.h>

نامخلا control
{

جماعت بلاک;

// A base class for block params that enables traversing linked list.
جماعت بلاک_Param_بنیاد : عوامی List_گرہ<بلاک_Param_بنیاد *>
{
عوامی:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	بلاک_Param_بنیاد(بلاک *والد, ثابت حرف *name, منطقی والد_سابقہ = سچ);
	مجازی ~بلاک_Param_بنیاد() = طےشدہ;

	مجازی منطقی update() = 0;
	ثابت حرف *getName() ثابت { واپس param_name(_handle); }

محفوظ:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
سانچہ <جماعت T>
جماعت __EXPORT بلاک_Param final : عوامی بلاک_Param_بنیاد
{
عوامی:
	بلاک_Param(بلاک *بلاک_2, ثابت حرف *name, منطقی والد_سابقہ = سچ);
	بلاک_Param(بلاک *بلاک_2, ثابت حرف *name, منطقی والد_سابقہ, T &extern_val);

	~بلاک_Param() override = طےشدہ;

	// no copy, assignment, move, move assignment
	بلاک_Param(ثابت بلاک_Param &) = حذف;
	بلاک_Param &عامل=(ثابت بلاک_Param &) = حذف;
	بلاک_Param(بلاک_Param &&) = حذف;
	بلاک_Param &عامل=(بلاک_Param &&) = حذف;

	T get() ثابت { واپس _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	منطقی commit() { واپس (param_سیٹ(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	منطقی commit_no_notification() { واپس (param_سیٹ_no_notification(_handle, &_val) == PX4_OK); }

	خالی سیٹ(T val) { _val = val; }

	منطقی update() override { واپس (param_get(_handle, &_val) == PX4_OK); }

محفوظ:
	T _val;
};

سانچہ <>
منطقی بلاک_Param<منطقی>::update();

قسمتعریف بلاک_Param<اعشاری> بلاک_Param_Float;
قسمتعریف بلاک_Param<int32_t> بلاک_Param_Int;
قسمتعریف بلاک_Param<منطقی> بلاک_Param_Bool;
قسمتعریف بلاک_Param<اعشاری &> بلاک_Param_Ext_Float;
قسمتعریف بلاک_Param<int32_t &> بلاک_Param_Ext_Int;

} // namespace control
