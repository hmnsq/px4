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
 * @file Vector4.hpp
 *
 * 4D vector class.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ ಮಾತೃಕೆ
{

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು Type>
ವರ್ಗ Vector4 : ಸಾರ್ವಜನಿಕ ಸದಿಶ<Type, 4>
{
ಸಾರ್ವಜನಿಕ:
	ಬಳಸಿ Matrix41 = ಮಾತೃಕೆ_2<Type, 4, 1>;

	Vector4() = ಪೂರ್ವನಿಯೋಜಿತ;

	Vector4(ಸ್ಥಿರಾಂಕ Matrix41 &other) :
		ಸದಿಶ<Type, 4>(other)
	{
	}

	ಸ್ಪಷ್ಟ Vector4(ಸ್ಥಿರಾಂಕ Type ದತ್ತಾಂಶ_2[3]) :
		ಸದಿಶ<Type, 4>(ದತ್ತಾಂಶ_2)
	{
	}

	Vector4(Type x1, Type x2, Type x3, Type x4)
	{
		Vector4 &v(*ಇದು);
		v(0) = x1;
		v(1) = x2;
		v(2) = x3;
		v(3) = x4;
	}

	ಮಾದರಿ<size_t P, size_t Q>
	Vector4(ಸ್ಥಿರಾಂಕ ತುಂಡು_2<Type, 4, 1, P, Q> &ತುಂಡು_ಒಳ) : ಸದಿಶ<Type, 4>(ತುಂಡು_ಒಳ)
	{
	}

	ಮಾದರಿ<size_t P, size_t Q>
	Vector4(ಸ್ಥಿರಾಂಕ ತುಂಡು_2<Type, 1, 4, P, Q> &ತುಂಡು_ಒಳ) : ಸದಿಶ<Type, 4>(ತುಂಡು_ಒಳ)
	{
	}

	/**
	 * Override matrix ops so Vector4 type is returned
	 */

	Vector4 ಕ್ರಿಯಾಕಾರಕ+(Vector4 other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix41::ಕ್ರಿಯಾಕಾರಕ+(other);
	}

	Vector4 ಕ್ರಿಯಾಕಾರಕ+(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix41::ಕ್ರಿಯಾಕಾರಕ+(scalar);
	}

	Vector4 ಕ್ರಿಯಾಕಾರಕ-(Vector4 other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix41::ಕ್ರಿಯಾಕಾರಕ-(other);
	}

	Vector4 ಕ್ರಿಯಾಕಾರಕ-(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix41::ಕ್ರಿಯಾಕಾರಕ-(scalar);
	}

	Vector4 ಕ್ರಿಯಾಕಾರಕ-() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix41::ಕ್ರಿಯಾಕಾರಕ-();
	}

	Vector4 ಕ್ರಿಯಾಕಾರಕ*(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix41::ಕ್ರಿಯಾಕಾರಕ*(scalar);
	}

	Type ಕ್ರಿಯಾಕಾರಕ*(Vector4 b) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಸದಿಶ<Type, 4>::ಕ್ರಿಯಾಕಾರಕ*(b);
	}

};

ಬಳಸಿ Vector4f = Vector4<ದಶಮಾಂಶ>;

} // namespace matrix
