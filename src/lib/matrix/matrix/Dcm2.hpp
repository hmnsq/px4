/****************************************************************************
 *
 *   Copyright (C) 2023 PX4 Development Team. All rights reserved.
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
 * @file Dcm2.hpp
 *
 * A givens rotation matrix 2x2.
 * All rotations and axis systems follow the right-hand rule.
 *
 * This library uses the convention that premultiplying a two dimensional
 * vector represented in coordinate system 1 will apply a rotation from coordinate system
 * 1 to coordinate system 2 to the vector.
 * Likewise, a matrix instance of this class also represents a coordinate transformation
 * from frame 2 to frame 1.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#निर्देशः once

#अन्तर्भाव "SquareMatrix.hpp"
#अन्तर्भाव "Vector2.hpp"

नामाकाश matrix
{

प्रतिरूपम्<प्रकारनाम Type>
वर्ग Dcm2 : सार्वजनिक वर्गफलम्_Matrix<Type, 2>
{
सार्वजनिक:
	/**
	 * Standard constructor
	 *
	 * Initializes to identity
	 */
	Dcm2() : वर्गफलम्_Matrix<Type, 2>(eye<Type, 2>()) {}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	स्पष्ट Dcm2(ध्रुव Type दत्त_2[2][2]) : वर्गफलम्_Matrix<Type, 2>(दत्त_2)
	{
	}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	स्पष्ट Dcm2(ध्रुव Type दत्त_2[4]) : वर्गफलम्_Matrix<Type, 2>(दत्त_2)
	{
	}

	/**
	 * Copy constructor
	 *
	 * @param other Matrix22 to set dcm to
	 */
	Dcm2(ध्रुव Matrix<Type, 2, 2> &other) : वर्गफलम्_Matrix<Type, 2>(other)
	{
	}

	/**
	 * Constructor from an angle
	 *
	 * This sets the transformation matrix from frame 2 to frame 1 where the rotation
	 * from frame 1 to frame 2 is described by an angle in radians.
	 *
	 * @param angle from frame 1 to frame 2 in radians
	 */
	Dcm2(ध्रुव Type angle)
	{
		Dcm2 &dcm = *एतत्;
		Type sin_angle = std::ज्या(angle);
		Type cos_angle = std::कोटिज्या(angle);

		dcm(0, 0) = cos_angle;
		dcm(0, 1) = -sin_angle;
		dcm(1, 0) = sin_angle;
		dcm(1, 1) = cos_angle;
	}

	शून्य renormalize()
	{
		// renormalize rows
		प्रति (size_t r = 0; r < 2; r++) {
			Vector2<Type> rvec(Matrix<Type, 1, 2>(एतत्->row(r)).transpose());
			एतत्->row(r) = rvec.normalized();
		}
	}
};

उपयुज्य Dcm2f = Dcm2<भिन्न>;
उपयुज्य Dcm2d = Dcm2<द्विगुण>;

} // namespace matrix
