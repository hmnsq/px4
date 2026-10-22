/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#അന്തര്ഭാവ "Matrix.hpp"

നാമാകാശ matrix
{

template<typename Type>
വർഗ്ഗം Scalar
{
പൊതു:
	Scalar() = delete;

	Scalar(സ്ഥിരം Matrix<Type, 1, 1> &other) :
		മൂല്യം_2{other(0, 0)}
	{
	}

	Scalar(Type other) : മൂല്യം_2(other)
	{
	}

	operator സ്ഥിരം Type &()
	{
		തിരികെ മൂല്യം_2;
	}

	operator Matrix<Type, 1, 1>() സ്ഥിരം
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = മൂല്യം_2;
		തിരികെ m;
	}

	operator Vector<Type, 1>() സ്ഥിരം
	{
		Vector<Type, 1> m;
		m(0) = മൂല്യം_2;
		തിരികെ m;
	}

	തർക്കം operator==(സ്ഥിരം ദശാംശം other) സ്ഥിരം
	{
		തിരികെ isEqualF(മൂല്യം_2, other);
	}

സ്വകാര്യ:
	സ്ഥിരം Type മൂല്യം_2;

};

using Scalarf = Scalar<ദശാംശം>;
using Scalard = Scalar<ഇരട്ട>;

} // namespace matrix
