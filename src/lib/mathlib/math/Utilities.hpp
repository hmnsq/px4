/****************************************************************************
 *
 *   Copyright (c) 2020-2024 PX4 Development Team. All rights reserved.
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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MATH_UTILITIES_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MATH_UTILITIES_H

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>

ਨਾਮਾਕਾਸ਼ math
{

ਨਾਮਾਕਾਸ਼ Utilities
{

// return the square of two floating point numbers - used in auto coded sections
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ sq(ਦਸ਼ਮਲਵ var) { ਵਾਪਸ var * var; }

// converts Tait-Bryan 312 sequence of rotations from frame 1 to frame 2
// to the corresponding rotation matrix that rotates from frame 2 to frame 1
// rot312(0) - First rotation is a RH rotation about the Z axis (rad)
// rot312(1) - Second rotation is a RH rotation about the X axis (rad)
// rot312(2) - Third rotation is a RH rotation about the Y axis (rad)
// See http://www.atacolorado.com/eulersequences.doc
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ::Dcm<T> tait_Bryan312_ਮੰਜ਼ਿਲ_Rot_Mat(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3<T> &rot312)
{
	// Calculate the frame2 to frame 1 rotation matrix from a 312 Tait-Bryan rotation sequence
	ਅਚਲ T c2 = cosf(rot312(2)); // third rotation is pitch
	ਅਚਲ T s2 = sinf(rot312(2));
	ਅਚਲ T s1 = sinf(rot312(1)); // second rotation is roll
	ਅਚਲ T c1 = cosf(rot312(1));
	ਅਚਲ T s0 = sinf(rot312(0)); // first rotation is yaw
	ਅਚਲ T c0 = cosf(rot312(0));

	ਮੈਟ੍ਰਿਕਸ::Dcm<T> R;
	R(0, 0) = c0 * c2 - s0 * s1 * s2;
	R(1, 1) = c0 * c1;
	R(2, 2) = c2 * c1;
	R(0, 1) = -c1 * s0;
	R(0, 2) = s2 * c0 + c2 * s1 * s0;
	R(1, 0) = c2 * s0 + s2 * s1 * c0;
	R(1, 2) = s0 * s2 - s1 * c0 * c2;
	R(2, 0) = -s2 * c1;
	R(2, 1) = s1;

	ਵਾਪਸ R;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ::Dcm<T> quat_ਮੰਜ਼ਿਲ_Inverse_Rot_Mat(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quaternion<T> &quat)
{
	ਅਚਲ T q00 = quat(0) * quat(0);
	ਅਚਲ T q11 = quat(1) * quat(1);
	ਅਚਲ T q22 = quat(2) * quat(2);
	ਅਚਲ T q33 = quat(3) * quat(3);
	ਅਚਲ T q01 = quat(0) * quat(1);
	ਅਚਲ T q02 = quat(0) * quat(2);
	ਅਚਲ T q03 = quat(0) * quat(3);
	ਅਚਲ T q12 = quat(1) * quat(2);
	ਅਚਲ T q13 = quat(1) * quat(3);
	ਅਚਲ T q23 = quat(2) * quat(3);

	ਮੈਟ੍ਰਿਕਸ::Dcm<T> dcm;
	dcm(0, 0) = q00 + q11 - q22 - q33;
	dcm(1, 1) = q00 - q11 + q22 - q33;
	dcm(2, 2) = q00 - q11 - q22 + q33;
	dcm(1, 0) = 2.0f * (q12 - q03);
	dcm(2, 0) = 2.0f * (q13 + q02);
	dcm(0, 1) = 2.0f * (q12 + q03);
	dcm(2, 1) = 2.0f * (q23 - q01);
	dcm(0, 2) = 2.0f * (q13 - q02);
	dcm(1, 2) = 2.0f * (q23 + q01);

	ਵਾਪਸ dcm;
}

// We should use a 3-2-1 Tait-Bryan (yaw-pitch-roll) rotation sequence
// when there is more roll than pitch tilt and a 3-1-2 rotation sequence
// when there is more pitch than roll tilt to avoid gimbal lock.
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ shouldUse321RotationSequence(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Dcm<T> &R)
{
	ਵਾਪਸ fabsf(R(2, 0)) < fabsf(R(2, 1));
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ ਦਸ਼ਮਲਵ getEuler321Yaw(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Dcm<T> &R)
{
	ਵਾਪਸ atan2f(R(1, 0), R(0, 0));
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ ਦਸ਼ਮਲਵ getEuler312Yaw(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Dcm<T> &R)
{
	ਵਾਪਸ atan2f(-R(0, 1), R(1, 1));
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ T getEuler321Yaw(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quaternion<T> &q)
{
	// Values from yaw_input_321.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw321.m
	ਅਚਲ T a = static_cast<T>(2.) * (q(0) * q(3) + q(1) * q(2));
	ਅਚਲ T b = sq(q(0)) + sq(q(1)) - sq(q(2)) - sq(q(3));
	ਵਾਪਸ atan2f(a, b);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ T getEuler312Yaw(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quaternion<T> &q)
{
	// Values from yaw_input_312.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw312.m
	ਅਚਲ T a = static_cast<T>(2.) * (q(0) * q(3) - q(1) * q(2));
	ਅਚਲ T b = sq(q(0)) - sq(q(1)) + sq(q(2)) - sq(q(3));
	ਵਾਪਸ atan2f(a, b);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ T getEulerYaw(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Dcm<T> &R)
{
	ਜੇ (shouldUse321RotationSequence(R)) {
		ਵਾਪਸ getEuler321Yaw(R);

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ getEuler312Yaw(R);
	}
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ T getEulerYaw(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quaternion<T> &q)
{
	ਵਾਪਸ getEulerYaw(ਮੈਟ੍ਰਿਕਸ::Dcm<T>(q));
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ::Dcm<T> update_Euler321_Yaw_ਵਿੱਚ_Rot_Mat(T yaw, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Dcm<T> &rot_ਵਿੱਚ)
{
	ਮੈਟ੍ਰਿਕਸ::Euler<T> euler321(rot_ਵਿੱਚ);
	euler321(2) = yaw;
	ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::Dcm<T>(euler321);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ::Dcm<T> update_Euler312_Yaw_ਵਿੱਚ_Rot_Mat(T yaw, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Dcm<T> &rot_ਵਿੱਚ)
{
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3<T> rotVec312(yaw,                                  // yaw
					   asinf(rot_ਵਿੱਚ(2, 1)),                  // roll
					   atan2f(-rot_ਵਿੱਚ(2, 0), rot_ਵਿੱਚ(2, 2))); // pitch
	ਵਾਪਸ tait_Bryan312_ਮੰਜ਼ਿਲ_Rot_Mat(rotVec312);
}

// Checks which euler rotation sequence to use and update yaw in rotation matrix
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T = ਦਸ਼ਮਲਵ>
ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ::Dcm<T> update_Yaw_ਵਿੱਚ_Rot_Mat(T yaw, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Dcm<T> &rot_ਵਿੱਚ)
{
	ਜੇ (shouldUse321RotationSequence(rot_ਵਿੱਚ)) {
		ਵਾਪਸ update_Euler321_Yaw_ਵਿੱਚ_Rot_Mat(yaw, rot_ਵਿੱਚ);

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ update_Euler312_Yaw_ਵਿੱਚ_Rot_Mat(yaw, rot_ਵਿੱਚ);
	}
}

} // namespace Utilities
} // namespace math

#ਜੇ_ਅੰਤ // MATH_UTILITIES_H
