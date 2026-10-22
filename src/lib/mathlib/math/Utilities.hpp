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

#ଅନ୍ତର୍ଭାଵ <matrix/math.hpp>

ନାମାକାଶ math
{

ନାମାକାଶ Utilities
{

// return the square of two floating point numbers - used in auto coded sections
ସ୍ଥିର constexpr ଦଶମିକ sq(ଦଶମିକ var) { ଫେରନ୍ତୁ var * var; }

// converts Tait-Bryan 312 sequence of rotations from frame 1 to frame 2
// to the corresponding rotation matrix that rotates from frame 2 to frame 1
// rot312(0) - First rotation is a RH rotation about the Z axis (rad)
// rot312(1) - Second rotation is a RH rotation about the X axis (rad)
// rot312(2) - Third rotation is a RH rotation about the Y axis (rad)
// See http://www.atacolorado.com/eulersequences.doc
template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ matrix::Dcm<T> taitBryan312ToRotMat(ଅଚଳ matrix::Vector3<T> &rot312)
{
	// Calculate the frame2 to frame 1 rotation matrix from a 312 Tait-Bryan rotation sequence
	ଅଚଳ T c2 = cosf(rot312(2)); // third rotation is pitch
	ଅଚଳ T s2 = sinf(rot312(2));
	ଅଚଳ T s1 = sinf(rot312(1)); // second rotation is roll
	ଅଚଳ T c1 = cosf(rot312(1));
	ଅଚଳ T s0 = sinf(rot312(0)); // first rotation is yaw
	ଅଚଳ T c0 = cosf(rot312(0));

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

	ଫେରନ୍ତୁ R;
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ matrix::Dcm<T> quatToInverseRotMat(ଅଚଳ matrix::Quaternion<T> &quat)
{
	ଅଚଳ T q00 = quat(0) * quat(0);
	ଅଚଳ T q11 = quat(1) * quat(1);
	ଅଚଳ T q22 = quat(2) * quat(2);
	ଅଚଳ T q33 = quat(3) * quat(3);
	ଅଚଳ T q01 = quat(0) * quat(1);
	ଅଚଳ T q02 = quat(0) * quat(2);
	ଅଚଳ T q03 = quat(0) * quat(3);
	ଅଚଳ T q12 = quat(1) * quat(2);
	ଅଚଳ T q13 = quat(1) * quat(3);
	ଅଚଳ T q23 = quat(2) * quat(3);

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

	ଫେରନ୍ତୁ dcm;
}

// We should use a 3-2-1 Tait-Bryan (yaw-pitch-roll) rotation sequence
// when there is more roll than pitch tilt and a 3-1-2 rotation sequence
// when there is more pitch than roll tilt to avoid gimbal lock.
template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ ତାର୍କିକ shouldUse321RotationSequence(ଅଚଳ matrix::Dcm<T> &R)
{
	ଫେରନ୍ତୁ fabsf(R(2, 0)) < fabsf(R(2, 1));
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ ଦଶମିକ getEuler321Yaw(ଅଚଳ matrix::Dcm<T> &R)
{
	ଫେରନ୍ତୁ atan2f(R(1, 0), R(0, 0));
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ ଦଶମିକ getEuler312Yaw(ଅଚଳ matrix::Dcm<T> &R)
{
	ଫେରନ୍ତୁ atan2f(-R(0, 1), R(1, 1));
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ T getEuler321Yaw(ଅଚଳ matrix::Quaternion<T> &q)
{
	// Values from yaw_input_321.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw321.m
	ଅଚଳ T a = static_cast<T>(2.) * (q(0) * q(3) + q(1) * q(2));
	ଅଚଳ T b = sq(q(0)) + sq(q(1)) - sq(q(2)) - sq(q(3));
	ଫେରନ୍ତୁ atan2f(a, b);
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ T getEuler312Yaw(ଅଚଳ matrix::Quaternion<T> &q)
{
	// Values from yaw_input_312.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw312.m
	ଅଚଳ T a = static_cast<T>(2.) * (q(0) * q(3) - q(1) * q(2));
	ଅଚଳ T b = sq(q(0)) - sq(q(1)) + sq(q(2)) - sq(q(3));
	ଫେରନ୍ତୁ atan2f(a, b);
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ T getEulerYaw(ଅଚଳ matrix::Dcm<T> &R)
{
	ଯଦି (shouldUse321RotationSequence(R)) {
		ଫେରନ୍ତୁ getEuler321Yaw(R);

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ getEuler312Yaw(R);
	}
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ T getEulerYaw(ଅଚଳ matrix::Quaternion<T> &q)
{
	ଫେରନ୍ତୁ getEulerYaw(matrix::Dcm<T>(q));
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ matrix::Dcm<T> updateEuler321YawInRotMat(T yaw, ଅଚଳ matrix::Dcm<T> &rot_in)
{
	matrix::Euler<T> euler321(rot_in);
	euler321(2) = yaw;
	ଫେରନ୍ତୁ matrix::Dcm<T>(euler321);
}

template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ matrix::Dcm<T> updateEuler312YawInRotMat(T yaw, ଅଚଳ matrix::Dcm<T> &rot_in)
{
	ଅଚଳ matrix::Vector3<T> rotVec312(yaw,                                  // yaw
					   asinf(rot_in(2, 1)),                  // roll
					   atan2f(-rot_in(2, 0), rot_in(2, 2))); // pitch
	ଫେରନ୍ତୁ taitBryan312ToRotMat(rotVec312);
}

// Checks which euler rotation sequence to use and update yaw in rotation matrix
template<typename T = ଦଶମିକ>
ଧାଡ଼ିଭିତରେ matrix::Dcm<T> updateYawInRotMat(T yaw, ଅଚଳ matrix::Dcm<T> &rot_in)
{
	ଯଦି (shouldUse321RotationSequence(rot_in)) {
		ଫେରନ୍ତୁ updateEuler321YawInRotMat(yaw, rot_in);

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ updateEuler312YawInRotMat(yaw, rot_in);
	}
}

} // namespace Utilities
} // namespace math

#endif // MATH_UTILITIES_H
