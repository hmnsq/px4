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

#لارښوونه once

#ضمن "Vector.hpp"

نطاق ماتریکس
{

کينډۍ<ډول_نوم Type>
صنف Vector4 : عام ویکټور<Type, 4>
{
عام:
	کارول Matrix41 = ماتریکس_2<Type, 4, 1>;

	Vector4() = افتراضي;

	Vector4(ثابت Matrix41 &other) :
		ویکټور<Type, 4>(other)
	{
	}

	څرګند Vector4(ثابت Type معلومات_2[3]) :
		ویکټور<Type, 4>(معلومات_2)
	{
	}

	Vector4(Type x1, Type x2, Type x3, Type x4)
	{
		Vector4 &v(*هذا);
		v(0) = x1;
		v(1) = x2;
		v(2) = x3;
		v(3) = x4;
	}

	کينډۍ<size_t P, size_t Q>
	Vector4(ثابت ټوټه_2<Type, 4, 1, P, Q> &ټوټه_دننه) : ویکټور<Type, 4>(ټوټه_دننه)
	{
	}

	کينډۍ<size_t P, size_t Q>
	Vector4(ثابت ټوټه_2<Type, 1, 4, P, Q> &ټوټه_دننه) : ویکټور<Type, 4>(ټوټه_دننه)
	{
	}

	/**
	 * Override matrix ops so Vector4 type is returned
	 */

	Vector4 چلوونکی+(Vector4 other) ثابت
	{
		أرجع Matrix41::چلوونکی+(other);
	}

	Vector4 چلوونکی+(Type scalar) ثابت
	{
		أرجع Matrix41::چلوونکی+(scalar);
	}

	Vector4 چلوونکی-(Vector4 other) ثابت
	{
		أرجع Matrix41::چلوونکی-(other);
	}

	Vector4 چلوونکی-(Type scalar) ثابت
	{
		أرجع Matrix41::چلوونکی-(scalar);
	}

	Vector4 چلوونکی-() ثابت
	{
		أرجع Matrix41::چلوونکی-();
	}

	Vector4 چلوونکی*(Type scalar) ثابت
	{
		أرجع Matrix41::چلوونکی*(scalar);
	}

	Type چلوونکی*(Vector4 b) ثابت
	{
		أرجع ویکټور<Type, 4>::چلوونکی*(b);
	}

};

کارول Vector4f = Vector4<عائم>;

} // namespace matrix
