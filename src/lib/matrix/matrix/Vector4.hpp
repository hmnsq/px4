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

#निर्देशः once

#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

प्रतिरूपम्<प्रकारनाम Type>
वर्ग Vector4 : सार्वजनिक सदिशम्<Type, 4>
{
सार्वजनिक:
	उपयुज्य Matrix41 = Matrix<Type, 4, 1>;

	Vector4() = स्वभाव;

	Vector4(ध्रुव Matrix41 &other) :
		सदिशम्<Type, 4>(other)
	{
	}

	स्पष्ट Vector4(ध्रुव Type दत्त_2[3]) :
		सदिशम्<Type, 4>(दत्त_2)
	{
	}

	Vector4(Type x1, Type x2, Type x3, Type x4)
	{
		Vector4 &v(*एतत्);
		v(0) = x1;
		v(1) = x2;
		v(2) = x3;
		v(3) = x4;
	}

	प्रतिरूपम्<size_t P, size_t Q>
	Vector4(ध्रुव खण्डिका_2<Type, 4, 1, P, Q> &खण्डिका_अन्तर्) : सदिशम्<Type, 4>(खण्डिका_अन्तर्)
	{
	}

	प्रतिरूपम्<size_t P, size_t Q>
	Vector4(ध्रुव खण्डिका_2<Type, 1, 4, P, Q> &खण्डिका_अन्तर्) : सदिशम्<Type, 4>(खण्डिका_अन्तर्)
	{
	}

	/**
	 * Override matrix ops so Vector4 type is returned
	 */

	Vector4 संक्रियकः+(Vector4 other) ध्रुव
	{
		प्रत्यागम Matrix41::संक्रियकः+(other);
	}

	Vector4 संक्रियकः+(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix41::संक्रियकः+(scalar);
	}

	Vector4 संक्रियकः-(Vector4 other) ध्रुव
	{
		प्रत्यागम Matrix41::संक्रियकः-(other);
	}

	Vector4 संक्रियकः-(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix41::संक्रियकः-(scalar);
	}

	Vector4 संक्रियकः-() ध्रुव
	{
		प्रत्यागम Matrix41::संक्रियकः-();
	}

	Vector4 संक्रियकः*(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix41::संक्रियकः*(scalar);
	}

	Type संक्रियकः*(Vector4 b) ध्रुव
	{
		प्रत्यागम सदिशम्<Type, 4>::संक्रियकः*(b);
	}

};

उपयुज्य Vector4f = Vector4<भिन्न>;

} // namespace matrix
