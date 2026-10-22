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

#ifndef MATH_UTILITIES_H
#define MATH_UTILITIES_H

#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

ನಾಮಾಕಾಶ math
{

ನಾಮಾಕಾಶ Utilities
{

// return the square of two floating point numbers - used in auto coded sections
ಸ್ಥಿರ constexpr ಭಿನ್ನ sq(ಭಿನ್ನ var) { ಪ್ರತ್ಯಾಗಮ var * var; }

// converts Tait-Bryan 312 sequence of rotations from frame 1 to frame 2
// to the corresponding rotation matrix that rotates from frame 2 to frame 1
// rot312(0) - First rotation is a RH rotation about the Z axis (rad)
// rot312(1) - Second rotation is a RH rotation about the X axis (rad)
// rot312(2) - Third rotation is a RH rotation about the Y axis (rad)
// See http://www.atacolorado.com/eulersequences.doc
template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ matrix::Dcm<T> taitBryan312ToRotMat(ಧ್ರುವ matrix::Vector3<T> &rot312)
{
	// Calculate the frame2 to frame 1 rotation matrix from a 312 Tait-Bryan rotation sequence
	ಧ್ರುವ T c2 = cosf(rot312(2)); // third rotation is pitch
	ಧ್ರುವ T s2 = sinf(rot312(2));
	ಧ್ರುವ T s1 = sinf(rot312(1)); // second rotation is roll
	ಧ್ರುವ T c1 = cosf(rot312(1));
	ಧ್ರುವ T s0 = sinf(rot312(0)); // first rotation is yaw
	ಧ್ರುವ T c0 = cosf(rot312(0));

	matrix::Dcm<T> R;
	R(0, 0) = c0 * c2 - s0 * s1 * s2;
	R(1, 1) = c0 * c1;
	R(2, 2) = c2 * c1;
	R(0, 1) = -c1 * s0;
	R(0, 2) = s2 * c0 + c2 * s1 * s0;
	R(1, 0) = c2 * s0 + s2 * s1 * c0;
	R(1, 2) = s0 * s2 - s1 * c0 * c2;
	R(2, 0) = -s2 * c1;
	R(2, 1) = s1;

	ಪ್ರತ್ಯಾಗಮ R;
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ matrix::Dcm<T> quatToInverseRotMat(ಧ್ರುವ matrix::Quaternion<T> &quat)
{
	ಧ್ರುವ T q00 = quat(0) * quat(0);
	ಧ್ರುವ T q11 = quat(1) * quat(1);
	ಧ್ರುವ T q22 = quat(2) * quat(2);
	ಧ್ರುವ T q33 = quat(3) * quat(3);
	ಧ್ರುವ T q01 = quat(0) * quat(1);
	ಧ್ರುವ T q02 = quat(0) * quat(2);
	ಧ್ರುವ T q03 = quat(0) * quat(3);
	ಧ್ರುವ T q12 = quat(1) * quat(2);
	ಧ್ರುವ T q13 = quat(1) * quat(3);
	ಧ್ರುವ T q23 = quat(2) * quat(3);

	matrix::Dcm<T> dcm;
	dcm(0, 0) = q00 + q11 - q22 - q33;
	dcm(1, 1) = q00 - q11 + q22 - q33;
	dcm(2, 2) = q00 - q11 - q22 + q33;
	dcm(1, 0) = 2.0f * (q12 - q03);
	dcm(2, 0) = 2.0f * (q13 + q02);
	dcm(0, 1) = 2.0f * (q12 + q03);
	dcm(2, 1) = 2.0f * (q23 - q01);
	dcm(0, 2) = 2.0f * (q13 - q02);
	dcm(1, 2) = 2.0f * (q23 + q01);

	ಪ್ರತ್ಯಾಗಮ dcm;
}

// We should use a 3-2-1 Tait-Bryan (yaw-pitch-roll) rotation sequence
// when there is more roll than pitch tilt and a 3-1-2 rotation sequence
// when there is more pitch than roll tilt to avoid gimbal lock.
template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ shouldUse321RotationSequence(ಧ್ರುವ matrix::Dcm<T> &R)
{
	ಪ್ರತ್ಯಾಗಮ fabsf(R(2, 0)) < fabsf(R(2, 1));
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ ಭಿನ್ನ getEuler321Yaw(ಧ್ರುವ matrix::Dcm<T> &R)
{
	ಪ್ರತ್ಯಾಗಮ atan2f(R(1, 0), R(0, 0));
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ ಭಿನ್ನ getEuler312Yaw(ಧ್ರುವ matrix::Dcm<T> &R)
{
	ಪ್ರತ್ಯಾಗಮ atan2f(-R(0, 1), R(1, 1));
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ T getEuler321Yaw(ಧ್ರುವ matrix::Quaternion<T> &q)
{
	// Values from yaw_input_321.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw321.m
	ಧ್ರುವ T a = static_cast<T>(2.) * (q(0) * q(3) + q(1) * q(2));
	ಧ್ರುವ T b = sq(q(0)) + sq(q(1)) - sq(q(2)) - sq(q(3));
	ಪ್ರತ್ಯಾಗಮ atan2f(a, b);
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ T getEuler312Yaw(ಧ್ರುವ matrix::Quaternion<T> &q)
{
	// Values from yaw_input_312.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw312.m
	ಧ್ರುವ T a = static_cast<T>(2.) * (q(0) * q(3) - q(1) * q(2));
	ಧ್ರುವ T b = sq(q(0)) - sq(q(1)) + sq(q(2)) - sq(q(3));
	ಪ್ರತ್ಯಾಗಮ atan2f(a, b);
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ T getEulerYaw(ಧ್ರುವ matrix::Dcm<T> &R)
{
	ಯದಿ (shouldUse321RotationSequence(R)) {
		ಪ್ರತ್ಯಾಗಮ getEuler321Yaw(R);

	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ getEuler312Yaw(R);
	}
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ T getEulerYaw(ಧ್ರುವ matrix::Quaternion<T> &q)
{
	ಪ್ರತ್ಯಾಗಮ getEulerYaw(matrix::Dcm<T>(q));
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ matrix::Dcm<T> updateEuler321YawInRotMat(T yaw, ಧ್ರುವ matrix::Dcm<T> &rot_in)
{
	matrix::Euler<T> euler321(rot_in);
	euler321(2) = yaw;
	ಪ್ರತ್ಯಾಗಮ matrix::Dcm<T>(euler321);
}

template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ matrix::Dcm<T> updateEuler312YawInRotMat(T yaw, ಧ್ರುವ matrix::Dcm<T> &rot_in)
{
	ಧ್ರುವ matrix::Vector3<T> rotVec312(yaw,                                  // yaw
					   asinf(rot_in(2, 1)),                  // roll
					   atan2f(-rot_in(2, 0), rot_in(2, 2))); // pitch
	ಪ್ರತ್ಯಾಗಮ taitBryan312ToRotMat(rotVec312);
}

// Checks which euler rotation sequence to use and update yaw in rotation matrix
template<typename T = ಭಿನ್ನ>
ಅನ್ತಃಸ್ಥ matrix::Dcm<T> updateYawInRotMat(T yaw, ಧ್ರುವ matrix::Dcm<T> &rot_in)
{
	ಯದಿ (shouldUse321RotationSequence(rot_in)) {
		ಪ್ರತ್ಯಾಗಮ updateEuler321YawInRotMat(yaw, rot_in);

	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ updateEuler312YawInRotMat(yaw, rot_in);
	}
}

} // namespace Utilities
} // namespace math

#endif // MATH_UTILITIES_H
