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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <containers/List.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <controllib/block/BlockParam.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdint>

ਨਾਮਾਕਾਸ਼ control
{

ਸਥਿਰ constexpr uint8_t max_Children_ਪ੍ਰਤੀ_ਖੰਡ = 100;
ਸਥਿਰ constexpr uint8_t max_Params_ਪ੍ਰਤੀ_ਖੰਡ = 110;
ਸਥਿਰ constexpr uint8_t ਖੰਡ_Name_ਲੰਮਾਈ_Max = 40;

// forward declaration
ਵਰਗ ਖੰਡ_Param_ਅਧਾਰ;
ਵਰਗ Super_ਖੰਡ;

/**
 */
ਵਰਗ __EXPORT ਖੰਡ : ਜਨਤਕ List_ਗੰਢ<ਖੰਡ *>
{
ਜਨਤਕ:
	ਮਿੱਤਰ ਵਰਗ ਖੰਡ_Param_ਅਧਾਰ;

	ਖੰਡ(Super_ਖੰਡ *ਮਾਪਾ, ਅਚਲ ਅੱਖਰ *name);
	ਆਭਾਸੀ ~ਖੰਡ() = ਮੂਲ;

	// no copy, assignment, move, move assignment
	ਖੰਡ(ਅਚਲ ਖੰਡ &) = ਮਿਟਾਓ;
	ਖੰਡ &ਸੰਚਾਲਕ=(ਅਚਲ ਖੰਡ &) = ਮਿਟਾਓ;
	ਖੰਡ(ਖੰਡ &&) = ਮਿਟਾਓ;
	ਖੰਡ &ਸੰਚਾਲਕ=(ਖੰਡ &&) = ਮਿਟਾਓ;

	ਖਾਲੀ getName(ਅੱਖਰ *name, size_t n);

	ਆਭਾਸੀ ਖਾਲੀ updateParams();

	ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_Dt(ਦਸ਼ਮਲਵ dt) { _dt = dt; }
	ਦਸ਼ਮਲਵ getDt() { ਵਾਪਸ _dt; }

ਸੁਰੱਖਿਅਤ:

	ਆਭਾਸੀ ਖਾਲੀ updateParamsSubclass() {}

	Super_ਖੰਡ *get_ਮਾਪਾ() { ਵਾਪਸ ਮਾਪਾ_2; }
	List<ਖੰਡ_Param_ਅਧਾਰ *> &getParams() { ਵਾਪਸ _params; }

	ਅਚਲ ਅੱਖਰ *_name;
	Super_ਖੰਡ *ਮਾਪਾ_2;
	ਦਸ਼ਮਲਵ _dt{0.0f};

	List<ਖੰਡ_Param_ਅਧਾਰ *> _params;
};

ਵਰਗ __EXPORT Super_ਖੰਡ :
	ਜਨਤਕ ਖੰਡ
{
ਜਨਤਕ:
	ਮਿੱਤਰ ਵਰਗ ਖੰਡ;

	Super_ਖੰਡ(Super_ਖੰਡ *ਮਾਪਾ, ਅਚਲ ਅੱਖਰ *name) : ਖੰਡ(ਮਾਪਾ, name) {}
	~Super_ਖੰਡ() = ਮੂਲ;

	// no copy, assignment, move, move assignment
	Super_ਖੰਡ(ਅਚਲ Super_ਖੰਡ &) = ਮਿਟਾਓ;
	Super_ਖੰਡ &ਸੰਚਾਲਕ=(ਅਚਲ Super_ਖੰਡ &) = ਮਿਟਾਓ;
	Super_ਖੰਡ(Super_ਖੰਡ &&) = ਮਿਟਾਓ;
	Super_ਖੰਡ &ਸੰਚਾਲਕ=(Super_ਖੰਡ &&) = ਮਿਟਾਓ;

	ਖਾਲੀ ਸੈੱਟ_Dt(ਦਸ਼ਮਲਵ dt) override;

	ਖਾਲੀ updateParams() override
	{
		ਖੰਡ::updateParams();

		ਜੇ (getChildren().get_ਸਿਰ() != ਖਾਲੀ_ਸੂਚਕ) { update_ਬੱਚਾ_Params(); }
	}

ਸੁਰੱਖਿਅਤ:
	List<ਖੰਡ *> &getChildren() { ਵਾਪਸ _children; }
	ਖਾਲੀ update_ਬੱਚਾ_Params();

	List<ਖੰਡ *> _children;
};


} // namespace control
