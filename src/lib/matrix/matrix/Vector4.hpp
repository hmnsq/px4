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

#సూచన once

#అన్తర్భావ "Vector.hpp"

నామాకాశ మాతృక
{

మూస<రకం_పేరు Type>
తరగతి Vector4 : బహిరంగ సదిశ<Type, 4>
{
బహిరంగ:
	ఉపయోగించి Matrix41 = మాతృక_2<Type, 4, 1>;

	Vector4() = అప్రమేయ;

	Vector4(స్థిరరాశి Matrix41 &other) :
		సదిశ<Type, 4>(other)
	{
	}

	స్పష్టమైన Vector4(స్థిరరాశి Type దత్తాంశం_2[3]) :
		సదిశ<Type, 4>(దత్తాంశం_2)
	{
	}

	Vector4(Type x1, Type x2, Type x3, Type x4)
	{
		Vector4 &v(*ఇది);
		v(0) = x1;
		v(1) = x2;
		v(2) = x3;
		v(3) = x4;
	}

	మూస<size_t P, size_t Q>
	Vector4(స్థిరరాశి ముక్క_2<Type, 4, 1, P, Q> &ముక్క_లోపల) : సదిశ<Type, 4>(ముక్క_లోపల)
	{
	}

	మూస<size_t P, size_t Q>
	Vector4(స్థిరరాశి ముక్క_2<Type, 1, 4, P, Q> &ముక్క_లోపల) : సదిశ<Type, 4>(ముక్క_లోపల)
	{
	}

	/**
	 * Override matrix ops so Vector4 type is returned
	 */

	Vector4 క్రియాకారకం+(Vector4 other) స్థిరరాశి
	{
		తిరిగి Matrix41::క్రియాకారకం+(other);
	}

	Vector4 క్రియాకారకం+(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix41::క్రియాకారకం+(scalar);
	}

	Vector4 క్రియాకారకం-(Vector4 other) స్థిరరాశి
	{
		తిరిగి Matrix41::క్రియాకారకం-(other);
	}

	Vector4 క్రియాకారకం-(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix41::క్రియాకారకం-(scalar);
	}

	Vector4 క్రియాకారకం-() స్థిరరాశి
	{
		తిరిగి Matrix41::క్రియాకారకం-();
	}

	Vector4 క్రియాకారకం*(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix41::క్రియాకారకం*(scalar);
	}

	Type క్రియాకారకం*(Vector4 b) స్థిరరాశి
	{
		తిరిగి సదిశ<Type, 4>::క్రియాకారకం*(b);
	}

};

ఉపయోగించి Vector4f = Vector4<దశాంశం>;

} // namespace matrix
