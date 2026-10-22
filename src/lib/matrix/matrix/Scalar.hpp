/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ಅನ್ತರ್ಭಾವ "Matrix.hpp"

ನಾಮಾಕಾಶ matrix
{

template<typename Type>
ವರ್ಗ Scalar
{
ಸಾರ್ವಜನಿಕ:
	Scalar() = delete;

	Scalar(ಧ್ರುವ Matrix<Type, 1, 1> &other) :
		ಮೌಲ್ಯ_2{other(0, 0)}
	{
	}

	Scalar(Type other) : ಮೌಲ್ಯ_2(other)
	{
	}

	operator ಧ್ರುವ Type &()
	{
		ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ_2;
	}

	operator Matrix<Type, 1, 1>() ಧ್ರುವ
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = ಮೌಲ್ಯ_2;
		ಪ್ರತ್ಯಾಗಮ m;
	}

	operator Vector<Type, 1>() ಧ್ರುವ
	{
		Vector<Type, 1> m;
		m(0) = ಮೌಲ್ಯ_2;
		ಪ್ರತ್ಯಾಗಮ m;
	}

	ತಾರ್ಕಿಕ operator==(ಧ್ರುವ ಭಿನ್ನ other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ isEqualF(ಮೌಲ್ಯ_2, other);
	}

ಗುಪ್ತ:
	ಧ್ರುವ Type ಮೌಲ್ಯ_2;

};

using Scalarf = Scalar<ಭಿನ್ನ>;
using Scalard = Scalar<ದ್ವಿಗುಣ>;

} // namespace matrix
