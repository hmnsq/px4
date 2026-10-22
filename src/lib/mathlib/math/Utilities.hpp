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

#অন্তর্ভাব <matrix/math.hpp>

নামাকাশ math
{

নামাকাশ Utilities
{

// return the square of two floating point numbers - used in auto coded sections
স্থির constexpr দশমিক sq(দশমিক var) { ফেরত var * var; }

// converts Tait-Bryan 312 sequence of rotations from frame 1 to frame 2
// to the corresponding rotation matrix that rotates from frame 2 to frame 1
// rot312(0) - First rotation is a RH rotation about the Z axis (rad)
// rot312(1) - Second rotation is a RH rotation about the X axis (rad)
// rot312(2) - Third rotation is a RH rotation about the Y axis (rad)
// See http://www.atacolorado.com/eulersequences.doc
template<typename T = দশমিক>
অন্তঃরেখ matrix::Dcm<T> taitBryan312ToRotMat(ধ্রুবক matrix::Vector3<T> &rot312)
{
	// Calculate the frame2 to frame 1 rotation matrix from a 312 Tait-Bryan rotation sequence
	ধ্রুবক T c2 = cosf(rot312(2)); // third rotation is pitch
	ধ্রুবক T s2 = sinf(rot312(2));
	ধ্রুবক T s1 = sinf(rot312(1)); // second rotation is roll
	ধ্রুবক T c1 = cosf(rot312(1));
	ধ্রুবক T s0 = sinf(rot312(0)); // first rotation is yaw
	ধ্রুবক T c0 = cosf(rot312(0));

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

	ফেরত R;
}

template<typename T = দশমিক>
অন্তঃরেখ matrix::Dcm<T> quatToInverseRotMat(ধ্রুবক matrix::Quaternion<T> &quat)
{
	ধ্রুবক T q00 = quat(0) * quat(0);
	ধ্রুবক T q11 = quat(1) * quat(1);
	ধ্রুবক T q22 = quat(2) * quat(2);
	ধ্রুবক T q33 = quat(3) * quat(3);
	ধ্রুবক T q01 = quat(0) * quat(1);
	ধ্রুবক T q02 = quat(0) * quat(2);
	ধ্রুবক T q03 = quat(0) * quat(3);
	ধ্রুবক T q12 = quat(1) * quat(2);
	ধ্রুবক T q13 = quat(1) * quat(3);
	ধ্রুবক T q23 = quat(2) * quat(3);

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

	ফেরত dcm;
}

// We should use a 3-2-1 Tait-Bryan (yaw-pitch-roll) rotation sequence
// when there is more roll than pitch tilt and a 3-1-2 rotation sequence
// when there is more pitch than roll tilt to avoid gimbal lock.
template<typename T = দশমিক>
অন্তঃরেখ যৌক্তিক shouldUse321RotationSequence(ধ্রুবক matrix::Dcm<T> &R)
{
	ফেরত fabsf(R(2, 0)) < fabsf(R(2, 1));
}

template<typename T = দশমিক>
অন্তঃরেখ দশমিক getEuler321Yaw(ধ্রুবক matrix::Dcm<T> &R)
{
	ফেরত atan2f(R(1, 0), R(0, 0));
}

template<typename T = দশমিক>
অন্তঃরেখ দশমিক getEuler312Yaw(ধ্রুবক matrix::Dcm<T> &R)
{
	ফেরত atan2f(-R(0, 1), R(1, 1));
}

template<typename T = দশমিক>
অন্তঃরেখ T getEuler321Yaw(ধ্রুবক matrix::Quaternion<T> &q)
{
	// Values from yaw_input_321.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw321.m
	ধ্রুবক T a = static_cast<T>(2.) * (q(0) * q(3) + q(1) * q(2));
	ধ্রুবক T b = sq(q(0)) + sq(q(1)) - sq(q(2)) - sq(q(3));
	ফেরত atan2f(a, b);
}

template<typename T = দশমিক>
অন্তঃরেখ T getEuler312Yaw(ধ্রুবক matrix::Quaternion<T> &q)
{
	// Values from yaw_input_312.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw312.m
	ধ্রুবক T a = static_cast<T>(2.) * (q(0) * q(3) - q(1) * q(2));
	ধ্রুবক T b = sq(q(0)) - sq(q(1)) + sq(q(2)) - sq(q(3));
	ফেরত atan2f(a, b);
}

template<typename T = দশমিক>
অন্তঃরেখ T getEulerYaw(ধ্রুবক matrix::Dcm<T> &R)
{
	যদি (shouldUse321RotationSequence(R)) {
		ফেরত getEuler321Yaw(R);

	} অন্যথা {
		ফেরত getEuler312Yaw(R);
	}
}

template<typename T = দশমিক>
অন্তঃরেখ T getEulerYaw(ধ্রুবক matrix::Quaternion<T> &q)
{
	ফেরত getEulerYaw(matrix::Dcm<T>(q));
}

template<typename T = দশমিক>
অন্তঃরেখ matrix::Dcm<T> updateEuler321YawInRotMat(T yaw, ধ্রুবক matrix::Dcm<T> &rot_in)
{
	matrix::Euler<T> euler321(rot_in);
	euler321(2) = yaw;
	ফেরত matrix::Dcm<T>(euler321);
}

template<typename T = দশমিক>
অন্তঃরেখ matrix::Dcm<T> updateEuler312YawInRotMat(T yaw, ধ্রুবক matrix::Dcm<T> &rot_in)
{
	ধ্রুবক matrix::Vector3<T> rotVec312(yaw,                                  // yaw
					   asinf(rot_in(2, 1)),                  // roll
					   atan2f(-rot_in(2, 0), rot_in(2, 2))); // pitch
	ফেরত taitBryan312ToRotMat(rotVec312);
}

// Checks which euler rotation sequence to use and update yaw in rotation matrix
template<typename T = দশমিক>
অন্তঃরেখ matrix::Dcm<T> updateYawInRotMat(T yaw, ধ্রুবক matrix::Dcm<T> &rot_in)
{
	যদি (shouldUse321RotationSequence(rot_in)) {
		ফেরত updateEuler321YawInRotMat(yaw, rot_in);

	} অন্যথা {
		ফেরত updateEuler312YawInRotMat(yaw, rot_in);
	}
}

} // namespace Utilities
} // namespace math

#endif // MATH_UTILITIES_H
