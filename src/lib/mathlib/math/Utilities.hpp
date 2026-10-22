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

#నిర్వచితం_కాకపోతే MATH_UTILITIES_H
#నిర్వచించు MATH_UTILITIES_H

#అన్తర్భావ <matrix/math.hpp>

నామాకాశ math
{

నామాకాశ Utilities
{

// return the square of two floating point numbers - used in auto coded sections
స్థిర constexpr దశాంశం sq(దశాంశం var) { తిరిగి var * var; }

// converts Tait-Bryan 312 sequence of rotations from frame 1 to frame 2
// to the corresponding rotation matrix that rotates from frame 2 to frame 1
// rot312(0) - First rotation is a RH rotation about the Z axis (rad)
// rot312(1) - Second rotation is a RH rotation about the X axis (rad)
// rot312(2) - Third rotation is a RH rotation about the Y axis (rad)
// See http://www.atacolorado.com/eulersequences.doc
మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస మాతృక::Dcm<T> tait_Bryan312_గమ్యం_Rot_Mat(స్థిరరాశి మాతృక::Vector3<T> &rot312)
{
	// Calculate the frame2 to frame 1 rotation matrix from a 312 Tait-Bryan rotation sequence
	స్థిరరాశి T c2 = cosf(rot312(2)); // third rotation is pitch
	స్థిరరాశి T s2 = sinf(rot312(2));
	స్థిరరాశి T s1 = sinf(rot312(1)); // second rotation is roll
	స్థిరరాశి T c1 = cosf(rot312(1));
	స్థిరరాశి T s0 = sinf(rot312(0)); // first rotation is yaw
	స్థిరరాశి T c0 = cosf(rot312(0));

	మాతృక::Dcm<T> R;
	R(0, 0) = c0 * c2 - s0 * s1 * s2;
	R(1, 1) = c0 * c1;
	R(2, 2) = c2 * c1;
	R(0, 1) = -c1 * s0;
	R(0, 2) = s2 * c0 + c2 * s1 * s0;
	R(1, 0) = c2 * s0 + s2 * s1 * c0;
	R(1, 2) = s0 * s2 - s1 * c0 * c2;
	R(2, 0) = -s2 * c1;
	R(2, 1) = s1;

	తిరిగి R;
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస మాతృక::Dcm<T> quat_గమ్యం_Inverse_Rot_Mat(స్థిరరాశి మాతృక::Quaternion<T> &quat)
{
	స్థిరరాశి T q00 = quat(0) * quat(0);
	స్థిరరాశి T q11 = quat(1) * quat(1);
	స్థిరరాశి T q22 = quat(2) * quat(2);
	స్థిరరాశి T q33 = quat(3) * quat(3);
	స్థిరరాశి T q01 = quat(0) * quat(1);
	స్థిరరాశి T q02 = quat(0) * quat(2);
	స్థిరరాశి T q03 = quat(0) * quat(3);
	స్థిరరాశి T q12 = quat(1) * quat(2);
	స్థిరరాశి T q13 = quat(1) * quat(3);
	స్థిరరాశి T q23 = quat(2) * quat(3);

	మాతృక::Dcm<T> dcm;
	dcm(0, 0) = q00 + q11 - q22 - q33;
	dcm(1, 1) = q00 - q11 + q22 - q33;
	dcm(2, 2) = q00 - q11 - q22 + q33;
	dcm(1, 0) = 2.0f * (q12 - q03);
	dcm(2, 0) = 2.0f * (q13 + q02);
	dcm(0, 1) = 2.0f * (q12 + q03);
	dcm(2, 1) = 2.0f * (q23 - q01);
	dcm(0, 2) = 2.0f * (q13 - q02);
	dcm(1, 2) = 2.0f * (q23 + q01);

	తిరిగి dcm;
}

// We should use a 3-2-1 Tait-Bryan (yaw-pitch-roll) rotation sequence
// when there is more roll than pitch tilt and a 3-1-2 rotation sequence
// when there is more pitch than roll tilt to avoid gimbal lock.
మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస తార్కిక shouldUse321RotationSequence(స్థిరరాశి మాతృక::Dcm<T> &R)
{
	తిరిగి fabsf(R(2, 0)) < fabsf(R(2, 1));
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస దశాంశం getEuler321Yaw(స్థిరరాశి మాతృక::Dcm<T> &R)
{
	తిరిగి atan2f(R(1, 0), R(0, 0));
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస దశాంశం getEuler312Yaw(స్థిరరాశి మాతృక::Dcm<T> &R)
{
	తిరిగి atan2f(-R(0, 1), R(1, 1));
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస T getEuler321Yaw(స్థిరరాశి మాతృక::Quaternion<T> &q)
{
	// Values from yaw_input_321.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw321.m
	స్థిరరాశి T a = static_cast<T>(2.) * (q(0) * q(3) + q(1) * q(2));
	స్థిరరాశి T b = sq(q(0)) + sq(q(1)) - sq(q(2)) - sq(q(3));
	తిరిగి atan2f(a, b);
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస T getEuler312Yaw(స్థిరరాశి మాతృక::Quaternion<T> &q)
{
	// Values from yaw_input_312.c file produced by
	// https://github.com/PX4/ecl/blob/master/matlab/scripts/Inertial%20Nav%20EKF/quat2yaw312.m
	స్థిరరాశి T a = static_cast<T>(2.) * (q(0) * q(3) - q(1) * q(2));
	స్థిరరాశి T b = sq(q(0)) - sq(q(1)) + sq(q(2)) - sq(q(3));
	తిరిగి atan2f(a, b);
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస T getEulerYaw(స్థిరరాశి మాతృక::Dcm<T> &R)
{
	ఒకవేళ (shouldUse321RotationSequence(R)) {
		తిరిగి getEuler321Yaw(R);

	} లేకపోతే {
		తిరిగి getEuler312Yaw(R);
	}
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస T getEulerYaw(స్థిరరాశి మాతృక::Quaternion<T> &q)
{
	తిరిగి getEulerYaw(మాతృక::Dcm<T>(q));
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస మాతృక::Dcm<T> update_Euler321_Yaw_లోపల_Rot_Mat(T yaw, స్థిరరాశి మాతృక::Dcm<T> &rot_లోపల)
{
	మాతృక::Euler<T> euler321(rot_లోపల);
	euler321(2) = yaw;
	తిరిగి మాతృక::Dcm<T>(euler321);
}

మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస మాతృక::Dcm<T> update_Euler312_Yaw_లోపల_Rot_Mat(T yaw, స్థిరరాశి మాతృక::Dcm<T> &rot_లోపల)
{
	స్థిరరాశి మాతృక::Vector3<T> rotVec312(yaw,                                  // yaw
					   asinf(rot_లోపల(2, 1)),                  // roll
					   atan2f(-rot_లోపల(2, 0), rot_లోపల(2, 2))); // pitch
	తిరిగి tait_Bryan312_గమ్యం_Rot_Mat(rotVec312);
}

// Checks which euler rotation sequence to use and update yaw in rotation matrix
మూస<రకం_పేరు T = దశాంశం>
అంతఃవరుస మాతృక::Dcm<T> update_Yaw_లోపల_Rot_Mat(T yaw, స్థిరరాశి మాతృక::Dcm<T> &rot_లోపల)
{
	ఒకవేళ (shouldUse321RotationSequence(rot_లోపల)) {
		తిరిగి update_Euler321_Yaw_లోపల_Rot_Mat(yaw, rot_లోపల);

	} లేకపోతే {
		తిరిగి update_Euler312_Yaw_లోపల_Rot_Mat(yaw, rot_లోపల);
	}
}

} // namespace Utilities
} // namespace math

#అయితే_ముగింపు // MATH_UTILITIES_H
