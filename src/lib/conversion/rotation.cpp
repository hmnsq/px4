/****************************************************************************
 *
 *   Copyright (C) 2013-2021 PX4 Development Team. All rights reserved.
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
 * @file rotation.cpp
 *
 * Vector rotation library
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "rotation.h"

__EXPORT ਮੈਟ੍ਰਿਕਸ::Dcmf
get_rot_ਮੈਟ੍ਰਿਕਸ(ਗਿਣਤੀ Rotation rot)
{
	ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::Dcmf{ਮੈਟ੍ਰਿਕਸ::Eulerf{
			math::radians((ਦਸ਼ਮਲਵ)rot_lookup[rot].roll),
			math::radians((ਦਸ਼ਮਲਵ)rot_lookup[rot].pitch),
			math::radians((ਦਸ਼ਮਲਵ)rot_lookup[rot].yaw)}};
}

__EXPORT ਮੈਟ੍ਰਿਕਸ::Quatf
get_rot_quaternion(ਗਿਣਤੀ Rotation rot)
{
	ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::Quatf{ਮੈਟ੍ਰਿਕਸ::Eulerf{
			math::radians((ਦਸ਼ਮਲਵ)rot_lookup[rot].roll),
			math::radians((ਦਸ਼ਮਲਵ)rot_lookup[rot].pitch),
			math::radians((ਦਸ਼ਮਲਵ)rot_lookup[rot].yaw)}};
}

__EXPORT ਖਾਲੀ
rotate_3i(ਗਿਣਤੀ Rotation rot, int16_t &x, int16_t &y, int16_t &z)
{
	ਜੇ (!rotate_3(rot, x, y, z)) {
		// otherwise use full rotation matrix for valid rotations
		ਜੇ (rot < ROTATION_MAX) {
			ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f r{get_rot_ਮੈਟ੍ਰਿਕਸ(rot) *ਮੈਟ੍ਰਿਕਸ::Vector3f{(ਦਸ਼ਮਲਵ)x, (ਦਸ਼ਮਲਵ)y, (ਦਸ਼ਮਲਵ)z}};
			x = math::constrain(roundf(r(0)), (ਦਸ਼ਮਲਵ)INT16_MIN, (ਦਸ਼ਮਲਵ)INT16_MAX);
			y = math::constrain(roundf(r(1)), (ਦਸ਼ਮਲਵ)INT16_MIN, (ਦਸ਼ਮਲਵ)INT16_MAX);
			z = math::constrain(roundf(r(2)), (ਦਸ਼ਮਲਵ)INT16_MIN, (ਦਸ਼ਮਲਵ)INT16_MAX);
		}
	}
}

__EXPORT ਖਾਲੀ
rotate_3f(ਗਿਣਤੀ Rotation rot, ਦਸ਼ਮਲਵ &x, ਦਸ਼ਮਲਵ &y, ਦਸ਼ਮਲਵ &z)
{
	ਜੇ (!rotate_3(rot, x, y, z)) {
		// otherwise use full rotation matrix for valid rotations
		ਜੇ (rot < ROTATION_MAX) {
			ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f r{get_rot_ਮੈਟ੍ਰਿਕਸ(rot) *ਮੈਟ੍ਰਿਕਸ::Vector3f{x, y, z}};
			x = r(0);
			y = r(1);
			z = r(2);
		}
	}
}
