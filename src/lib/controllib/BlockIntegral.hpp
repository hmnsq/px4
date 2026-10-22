/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
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
 * @file BlockIntegral.hpp
 *
 * Controller library code
 */

#pragma once

#ಅನ್ತರ್ಭಾವ "BlockLimitSym.hpp"

#ಅನ್ತರ್ಭಾವ "block/Block.hpp"
#ಅನ್ತರ್ಭಾವ "block/BlockParam.hpp"

#ಅನ್ತರ್ಭಾವ "matrix/math.hpp"

ನಾಮಾಕಾಶ control
{

/**
 * A rectangular integrator.
 * A limiter is built into the class to bound the
 * integral's internal state. This is important
 * for windup protection.
 * @see Limit
 */
ವರ್ಗ __EXPORT BlockIntegral: ಸಾರ್ವಜನಿಕ SuperBlock
{
ಸಾರ್ವಜನಿಕ:
// methods
	BlockIntegral(SuperBlock *parent, ಧ್ರುವ ವರ್ಣ *name) :
		SuperBlock(parent, name),
		_y(0),
		_limit(ಏತತ್, "") {}
	ಆಭಾಸ ~BlockIntegral() = ಸ್ವಭಾವ;
	ಭಿನ್ನ update(ಭಿನ್ನ ಒಳಹರಿವು);
// accessors
	ಭಿನ್ನ getY() {ಪ್ರತ್ಯಾಗಮ _y;}
	ಭಿನ್ನ getMax() {ಪ್ರತ್ಯಾಗಮ _limit.getMax();}
	ಶೂನ್ಯ setY(ಭಿನ್ನ y) {_y = y;}
ರಕ್ಷಿತ:
// attributes
	ಭಿನ್ನ _y; /**< previous output */
	BlockLimitSym _limit; /**< limiter */
};

} // namespace control
