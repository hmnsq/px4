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

#ہدایت once

#ضمن "Vector.hpp"

نطاق میٹرکس
{

سانچا<قسم_ناں Type>
صنف Vector4 : عام سمتیہ<Type, 4>
{
عام:
	ورتوں Matrix41 = میٹرکس_2<Type, 4, 1>;

	Vector4() = افتراضي;

	Vector4(ثابت Matrix41 &other) :
		سمتیہ<Type, 4>(other)
	{
	}

	واضح Vector4(ثابت Type انکڑے_2[3]) :
		سمتیہ<Type, 4>(انکڑے_2)
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

	سانچا<size_t P, size_t Q>
	Vector4(ثابت ٹکڑا_2<Type, 4, 1, P, Q> &ٹکڑا_وچ) : سمتیہ<Type, 4>(ٹکڑا_وچ)
	{
	}

	سانچا<size_t P, size_t Q>
	Vector4(ثابت ٹکڑا_2<Type, 1, 4, P, Q> &ٹکڑا_وچ) : سمتیہ<Type, 4>(ٹکڑا_وچ)
	{
	}

	/**
	 * Override matrix ops so Vector4 type is returned
	 */

	Vector4 عامل+(Vector4 other) ثابت
	{
		أرجع Matrix41::عامل+(other);
	}

	Vector4 عامل+(Type scalar) ثابت
	{
		أرجع Matrix41::عامل+(scalar);
	}

	Vector4 عامل-(Vector4 other) ثابت
	{
		أرجع Matrix41::عامل-(other);
	}

	Vector4 عامل-(Type scalar) ثابت
	{
		أرجع Matrix41::عامل-(scalar);
	}

	Vector4 عامل-() ثابت
	{
		أرجع Matrix41::عامل-();
	}

	Vector4 عامل*(Type scalar) ثابت
	{
		أرجع Matrix41::عامل*(scalar);
	}

	Type عامل*(Vector4 b) ثابت
	{
		أرجع سمتیہ<Type, 4>::عامل*(b);
	}

};

ورتوں Vector4f = Vector4<عائم>;

} // namespace matrix
