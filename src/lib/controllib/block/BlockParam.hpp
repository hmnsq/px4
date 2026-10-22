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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <containers/List.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/param.h>

ਨਾਮਾਕਾਸ਼ control
{

ਵਰਗ ਖੰਡ;

// A base class for block params that enables traversing linked list.
ਵਰਗ ਖੰਡ_Param_ਅਧਾਰ : ਜਨਤਕ List_ਗੰਢ<ਖੰਡ_Param_ਅਧਾਰ *>
{
ਜਨਤਕ:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	ਖੰਡ_Param_ਅਧਾਰ(ਖੰਡ *ਮਾਪਾ, ਅਚਲ ਅੱਖਰ *name, ਤਾਰਕਿਕ ਮਾਪਾ_ਅਗੇਤਰ = ਸੱਚ);
	ਆਭਾਸੀ ~ਖੰਡ_Param_ਅਧਾਰ() = ਮੂਲ;

	ਆਭਾਸੀ ਤਾਰਕਿਕ update() = 0;
	ਅਚਲ ਅੱਖਰ *getName() ਅਚਲ { ਵਾਪਸ param_name(_handle); }

ਸੁਰੱਖਿਅਤ:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
ਸਾਂਚਾ <ਵਰਗ T>
ਵਰਗ __EXPORT ਖੰਡ_Param final : ਜਨਤਕ ਖੰਡ_Param_ਅਧਾਰ
{
ਜਨਤਕ:
	ਖੰਡ_Param(ਖੰਡ *ਖੰਡ_2, ਅਚਲ ਅੱਖਰ *name, ਤਾਰਕਿਕ ਮਾਪਾ_ਅਗੇਤਰ = ਸੱਚ);
	ਖੰਡ_Param(ਖੰਡ *ਖੰਡ_2, ਅਚਲ ਅੱਖਰ *name, ਤਾਰਕਿਕ ਮਾਪਾ_ਅਗੇਤਰ, T &extern_val);

	~ਖੰਡ_Param() override = ਮੂਲ;

	// no copy, assignment, move, move assignment
	ਖੰਡ_Param(ਅਚਲ ਖੰਡ_Param &) = ਮਿਟਾਓ;
	ਖੰਡ_Param &ਸੰਚਾਲਕ=(ਅਚਲ ਖੰਡ_Param &) = ਮਿਟਾਓ;
	ਖੰਡ_Param(ਖੰਡ_Param &&) = ਮਿਟਾਓ;
	ਖੰਡ_Param &ਸੰਚਾਲਕ=(ਖੰਡ_Param &&) = ਮਿਟਾਓ;

	T get() ਅਚਲ { ਵਾਪਸ _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	ਤਾਰਕਿਕ commit() { ਵਾਪਸ (param_ਸੈੱਟ(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	ਤਾਰਕਿਕ commit_no_notification() { ਵਾਪਸ (param_ਸੈੱਟ_no_notification(_handle, &_val) == PX4_OK); }

	ਖਾਲੀ ਸੈੱਟ(T val) { _val = val; }

	ਤਾਰਕਿਕ update() override { ਵਾਪਸ (param_get(_handle, &_val) == PX4_OK); }

ਸੁਰੱਖਿਅਤ:
	T _val;
};

ਸਾਂਚਾ <>
ਤਾਰਕਿਕ ਖੰਡ_Param<ਤਾਰਕਿਕ>::update();

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖੰਡ_Param<ਦਸ਼ਮਲਵ> ਖੰਡ_Param_Float;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖੰਡ_Param<int32_t> ਖੰਡ_Param_Int;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖੰਡ_Param<ਤਾਰਕਿਕ> ਖੰਡ_Param_Bool;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖੰਡ_Param<ਦਸ਼ਮਲਵ &> ਖੰਡ_Param_Ext_Float;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖੰਡ_Param<int32_t &> ਖੰਡ_Param_Ext_Int;

} // namespace control
