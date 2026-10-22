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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "Vector.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type>
ਵਰਗ Vector4 : ਜਨਤਕ ਸਦਿਸ਼<Type, 4>
{
ਜਨਤਕ:
	ਵਰਤੋਂ Matrix41 = ਮੈਟ੍ਰਿਕਸ_2<Type, 4, 1>;

	Vector4() = ਮੂਲ;

	Vector4(ਅਚਲ Matrix41 &other) :
		ਸਦਿਸ਼<Type, 4>(other)
	{
	}

	ਸਪਸ਼ਟ Vector4(ਅਚਲ Type ਅੰਕੜੇ_2[3]) :
		ਸਦਿਸ਼<Type, 4>(ਅੰਕੜੇ_2)
	{
	}

	Vector4(Type x1, Type x2, Type x3, Type x4)
	{
		Vector4 &v(*ਇਹ);
		v(0) = x1;
		v(1) = x2;
		v(2) = x3;
		v(3) = x4;
	}

	ਸਾਂਚਾ<size_t P, size_t Q>
	Vector4(ਅਚਲ ਟੁਕੜਾ_2<Type, 4, 1, P, Q> &ਟੁਕੜਾ_ਵਿੱਚ) : ਸਦਿਸ਼<Type, 4>(ਟੁਕੜਾ_ਵਿੱਚ)
	{
	}

	ਸਾਂਚਾ<size_t P, size_t Q>
	Vector4(ਅਚਲ ਟੁਕੜਾ_2<Type, 1, 4, P, Q> &ਟੁਕੜਾ_ਵਿੱਚ) : ਸਦਿਸ਼<Type, 4>(ਟੁਕੜਾ_ਵਿੱਚ)
	{
	}

	/**
	 * Override matrix ops so Vector4 type is returned
	 */

	Vector4 ਸੰਚਾਲਕ+(Vector4 other) ਅਚਲ
	{
		ਵਾਪਸ Matrix41::ਸੰਚਾਲਕ+(other);
	}

	Vector4 ਸੰਚਾਲਕ+(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix41::ਸੰਚਾਲਕ+(scalar);
	}

	Vector4 ਸੰਚਾਲਕ-(Vector4 other) ਅਚਲ
	{
		ਵਾਪਸ Matrix41::ਸੰਚਾਲਕ-(other);
	}

	Vector4 ਸੰਚਾਲਕ-(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix41::ਸੰਚਾਲਕ-(scalar);
	}

	Vector4 ਸੰਚਾਲਕ-() ਅਚਲ
	{
		ਵਾਪਸ Matrix41::ਸੰਚਾਲਕ-();
	}

	Vector4 ਸੰਚਾਲਕ*(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix41::ਸੰਚਾਲਕ*(scalar);
	}

	Type ਸੰਚਾਲਕ*(Vector4 b) ਅਚਲ
	{
		ਵਾਪਸ ਸਦਿਸ਼<Type, 4>::ਸੰਚਾਲਕ*(b);
	}

};

ਵਰਤੋਂ Vector4f = Vector4<ਦਸ਼ਮਲਵ>;

} // namespace matrix
