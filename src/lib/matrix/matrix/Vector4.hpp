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

#شامل "Vector.hpp"

نامخلا میٹرکس
{

سانچہ<قسم_نام Type>
جماعت Vector4 : عوامی سمتیہ<Type, 4>
{
عوامی:
	استعمال Matrix41 = میٹرکس_2<Type, 4, 1>;

	Vector4() = طےشدہ;

	Vector4(ثابت Matrix41 &other) :
		سمتیہ<Type, 4>(other)
	{
	}

	صریح Vector4(ثابت Type ڈیٹا_2[3]) :
		سمتیہ<Type, 4>(ڈیٹا_2)
	{
	}

	Vector4(Type x1, Type x2, Type x3, Type x4)
	{
		Vector4 &v(*یہ);
		v(0) = x1;
		v(1) = x2;
		v(2) = x3;
		v(3) = x4;
	}

	سانچہ<size_t P, size_t Q>
	Vector4(ثابت قاش_2<Type, 4, 1, P, Q> &قاش_اندر) : سمتیہ<Type, 4>(قاش_اندر)
	{
	}

	سانچہ<size_t P, size_t Q>
	Vector4(ثابت قاش_2<Type, 1, 4, P, Q> &قاش_اندر) : سمتیہ<Type, 4>(قاش_اندر)
	{
	}

	/**
	 * Override matrix ops so Vector4 type is returned
	 */

	Vector4 عامل+(Vector4 other) ثابت
	{
		واپس Matrix41::عامل+(other);
	}

	Vector4 عامل+(Type scalar) ثابت
	{
		واپس Matrix41::عامل+(scalar);
	}

	Vector4 عامل-(Vector4 other) ثابت
	{
		واپس Matrix41::عامل-(other);
	}

	Vector4 عامل-(Type scalar) ثابت
	{
		واپس Matrix41::عامل-(scalar);
	}

	Vector4 عامل-() ثابت
	{
		واپس Matrix41::عامل-();
	}

	Vector4 عامل*(Type scalar) ثابت
	{
		واپس Matrix41::عامل*(scalar);
	}

	Type عامل*(Vector4 b) ثابت
	{
		واپس سمتیہ<Type, 4>::عامل*(b);
	}

};

استعمال Vector4f = Vector4<اعشاری>;

} // namespace matrix
