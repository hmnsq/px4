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

#جے_بنا_تعریف MATH_UTILITIES_H
#تعریف_کرو MATH_UTILITIES_H

#ضمن <matrix/math.hpp>

نطاق math
{

نطاق Utilities
{

// return the square of two floating point numbers - used in auto coded sections
ساكن constexpr عائم sq(عائم var) { أرجع var * var; }

// converts Tait-Bryan 312 sequence of rotations from frame 1 to frame 2
// to the corresponding rotation matrix that rotates from frame 2 to frame 1
// rot312(0) - First rotation is a RH rotation about the Z axis (rad)
// rot312(1) - Second rotation is a RH rotation about the X axis (rad)
// rot312(2) - Third rotation is a RH rotation about the Y axis (rad)
// See http://www.atacolorado.com/eulersequences.doc
سانچا<قسم_ناں T = عائم>
مضمن میٹرکس::Dcm<T> tait_Bryan312_منزل_Rot_Mat(ثابت میٹرکس::Vector3<T> &rot312)
{
	// Calculate the frame2 to frame 1 rotation matrix from a 312 Tait-Bryan rotation sequence
	ثابت T c2 = cosf(rot312(2)); // third rotation is pitch
	ثابت T s2 = sinf(rot312(2));
	ثابت T s1 = sinf(rot312(1)); // second rotation is roll
	ثابت T c1 = cosf(rot312(1));
	ثابت T s0 = sinf(rot312(0)); // first rotation is yaw
	ثابت T c0 = cosf(rot312(0));

	میٹرکس::Dcm<T> R;
	R(0, 0) = c0 * c2 - s0 * s1 * s2;
	R(1, 1) = c0 * c1;
	R(2, 2) = c2 * c1;
	R(0, 1) = -c1 * s0;
	R(0, 2) = s2 * c0 + c2 * s1 * s0;
	R(1, 0) = c2 * s0 + s2 * s1 * c0;
	R(1, 2) = s0 * s2 - s1 * c0 * c2;
	R(2, 0) = -s2 * c1;
	R(2, 1) = s1;

	أرجع R;
}

سانچا<قسم_ناں T = عائم>
مضمن میٹرکس::Dcm<T> quat_منزل_Inverse_Rot_Mat(ثابت میٹرکس::Quaternion<T> &quat)
{
	ثابت T q00 = quat(0) * quat(0);
	ثابت T q11 = quat(1) * quat(1);
	ثابت T q22 = quat(2) * quat(2);
	ثابت T q33 = quat(3) * quat(3);
	ثابت T q01 = quat(0) * quat(1);
	ثابت T q02 = quat(0) * quat(2);
	ثابت T q03 = quat(0) * quat(3);
	ثابت T q12 = quat(1) * quat(2);
	ثابت T q13 = quat(1) * quat(3);
	ثابت T q23 = quat(2) * quat(3);

	میٹرکس::Dcm<T> dcm;
	dcm(0, 0) = q00 + q11 - q22 - q33;
	dcm(1, 1) = q00 - q11 + q22 - q33;
	dcm(2, 2) = q00 - q11 - q22 + q33;
	dcm(1, 0) = 2.0f * (q12 - q03);
	dcm(2, 0) = 2.0f * (q13 + q02);
	dcm(0, 1) = 2.0f * (q12 + q03);
	dcm(2, 1) = 2.0f * (q23 - q01);
	dcm(0, 2) = 2.0f * (q13 - q02);
	dcm(1, 2) = 2.0f * (q23 + q01);

	أرجع dcm;
}

// We should use a 3-2-1 Tait-Bryan (yaw-pitch-roll) rotation sequence
// when there is more roll than pitch tilt and a 3-1-2 rotation sequence
// when there is more pitch than roll tilt to avoid gimbal lock.
سانچا<قسم_ناں T = عائم>
مضمن منطقي shouldUse321RotationSequence(ثابت میٹرکس::Dcm<T> &R)
{
	أرجع fabsf(R(2, 0)) < fabsf(R(2, 1));
}

سانچا<قسم_ناں T = عائم>
مضمن عائم getEuler321Yaw(ثابت میٹرکس::Dcm<T> &R)
{
	أرجع atan2f(R(1, 0), R(0, 0));
}

سانچا<قسم_ناں T = عائم>
مضمن عائم getEuler312Yaw(ثابت میٹرکس::Dcm<T> &R)
{
	أرجع atan2f(-R(0, 1), R(1, 1));
}

سانچا<قسم_ناں T = عائم>
مضمن T getEuler321Yaw(ثابت میٹرکس::Quaternion<T> &q)
{
	// Values from yaw_input_321.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw321.m
	ثابت T a = static_cast<T>(2.) * (q(0) * q(3) + q(1) * q(2));
	ثابت T b = sq(q(0)) + sq(q(1)) - sq(q(2)) - sq(q(3));
	أرجع atan2f(a, b);
}

سانچا<قسم_ناں T = عائم>
مضمن T getEuler312Yaw(ثابت میٹرکس::Quaternion<T> &q)
{
	// Values from yaw_input_312.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw312.m
	ثابت T a = static_cast<T>(2.) * (q(0) * q(3) - q(1) * q(2));
	ثابت T b = sq(q(0)) - sq(q(1)) + sq(q(2)) - sq(q(3));
	أرجع atan2f(a, b);
}

سانچا<قسم_ناں T = عائم>
مضمن T getEulerYaw(ثابت میٹرکس::Dcm<T> &R)
{
	إذا (shouldUse321RotationSequence(R)) {
		أرجع getEuler321Yaw(R);

	} وإلا {
		أرجع getEuler312Yaw(R);
	}
}

سانچا<قسم_ناں T = عائم>
مضمن T getEulerYaw(ثابت میٹرکس::Quaternion<T> &q)
{
	أرجع getEulerYaw(میٹرکس::Dcm<T>(q));
}

سانچا<قسم_ناں T = عائم>
مضمن میٹرکس::Dcm<T> update_Euler321_Yaw_وچ_Rot_Mat(T yaw, ثابت میٹرکس::Dcm<T> &rot_وچ)
{
	میٹرکس::Euler<T> euler321(rot_وچ);
	euler321(2) = yaw;
	أرجع میٹرکس::Dcm<T>(euler321);
}

سانچا<قسم_ناں T = عائم>
مضمن میٹرکس::Dcm<T> update_Euler312_Yaw_وچ_Rot_Mat(T yaw, ثابت میٹرکس::Dcm<T> &rot_وچ)
{
	ثابت میٹرکس::Vector3<T> rotVec312(yaw,                                  // yaw
					   asinf(rot_وچ(2, 1)),                  // roll
					   atan2f(-rot_وچ(2, 0), rot_وچ(2, 2))); // pitch
	أرجع tait_Bryan312_منزل_Rot_Mat(rotVec312);
}

// Checks which euler rotation sequence to use and update yaw in rotation matrix
سانچا<قسم_ناں T = عائم>
مضمن میٹرکس::Dcm<T> update_Yaw_وچ_Rot_Mat(T yaw, ثابت میٹرکس::Dcm<T> &rot_وچ)
{
	إذا (shouldUse321RotationSequence(rot_وچ)) {
		أرجع update_Euler321_Yaw_وچ_Rot_Mat(yaw, rot_وچ);

	} وإلا {
		أرجع update_Euler312_Yaw_وچ_Rot_Mat(yaw, rot_وچ);
	}
}

} // namespace Utilities
} // namespace math

#جے_انت // MATH_UTILITIES_H
