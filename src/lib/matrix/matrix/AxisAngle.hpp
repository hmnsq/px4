/**
 * @file AxisAngle.hpp
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#അന്തര്ഭാവ "Vector3.hpp"

നാമാകാശ matrix
{

template <typename Type>
വർഗ്ഗം Euler;

template<typename Type>
വർഗ്ഗം Quaternion;

/**
 * AxisAngle class
 *
 * The rotation between two coordinate frames is
 * described by this class.
 */
template<typename Type>
വർഗ്ഗം AxisAngle : പൊതു Vector3<Type>
{
പൊതു:
	using Matrix31 = Matrix<Type, 3, 1>;

	/**
	 * Constructor from array
	 *
	 * @param data_ array
	 */
	explicit AxisAngle(സ്ഥിരം Type data_[3]) :
		Vector3<Type>(data_)
	{
	}

	/**
	 * Standard constructor
	 */
	AxisAngle() = സ്ഥിരസ്ഥിതി;

	/**
	 * Constructor from Matrix31
	 *
	 * @param other Matrix31 to copy
	 */
	AxisAngle(സ്ഥിരം Matrix31 &other) :
		Vector3<Type>(other)
	{
	}

	/**
	 * Constructor from quaternion
	 *
	 * This sets the instance from a quaternion representing coordinate transformation from
	 * frame 2 to frame 1 where the rotation from frame 1 to frame 2 is described
	 * by a 3-2-1 intrinsic Tait-Bryan rotation sequence.
	 *
	 * @param q quaternion
	 */
	AxisAngle(സ്ഥിരം Quaternion<Type> &q)
	{
		AxisAngle &v = *ഇത്;
		Type mag = q.imag().norm();

		എങ്കിൽ (std::fabs(mag) >= Type(1e-10)) {
			v = q.imag() * Type(Type(2) * std::atan2(mag, q(0)) / mag);

		} അല്ലെങ്കിൽ {
			v = q.imag() * Type(Type(2) * Type(sign(q(0))));
		}
	}

	/**
	 * Constructor from dcm
	 *
	 * Instance is initialized from a dcm representing coordinate transformation
	 * from frame 2 to frame 1.
	 *
	 * @param dcm dcm to set quaternion to
	 */
	AxisAngle(സ്ഥിരം Dcm<Type> &dcm)
	{
		AxisAngle &v = *ഇത്;
		v = AxisAngle<Type>(Quaternion<Type>(dcm));
	}

	/**
	 * Constructor from euler angles
	 *
	 * This sets the instance to a quaternion representing coordinate transformation from
	 * frame 2 to frame 1 where the rotation from frame 1 to frame 2 is described
	 * by a 3-2-1 intrinsic Tait-Bryan rotation sequence.
	 *
	 * @param euler euler angle instance
	 */
	AxisAngle(സ്ഥിരം Euler<Type> &euler)
	{
		AxisAngle &v = *ഇത്;
		v = AxisAngle<Type>(Quaternion<Type>(euler));
	}

	/**
	 * Constructor from 3 axis angle values (unit vector * angle)
	 *
	 * @param x r_x*angle
	 * @param y r_y*angle
	 * @param z r_z*angle
	 */
	AxisAngle(Type x, Type y, Type z)
	{
		AxisAngle &v = *ഇത്;
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	/**
	 * Constructor from axis and angle
	 *
	 * @param axis An axis of rotation, normalized if not unit length
	 * @param angle The amount to rotate
	 */
	AxisAngle(സ്ഥിരം Matrix31 &axis_, Type angle_)
	{
		AxisAngle &v = *ഇത്;
		// make sure axis is a unit vector
		Vector3<Type> a = axis_;
		a = a.unit();
		v(0) = a(0) * angle_;
		v(1) = a(1) * angle_;
		v(2) = a(2) * angle_;
	}


	Vector3<Type> axis()
	{
		എങ്കിൽ (Vector3<Type>::norm() > 0) {
			തിരികെ Vector3<Type>::unit();

		} അല്ലെങ്കിൽ {
			തിരികെ Vector3<Type>(1, 0, 0);
		}
	}

	Type angle()
	{
		തിരികെ Vector3<Type>::norm();
	}
};

using AxisAnglef = AxisAngle<ദശാംശം>;
using AxisAngled = AxisAngle<ഇരട്ട>;

} // namespace matrix
