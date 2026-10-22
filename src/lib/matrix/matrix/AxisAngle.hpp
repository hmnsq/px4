/**
 * @file AxisAngle.hpp
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#अन्तर्भाव "Vector3.hpp"

नामाकाश matrix
{

template <typename Type>
वग्ग Euler;

template<typename Type>
वग्ग Quaternion;

/**
 * AxisAngle class
 *
 * The rotation between two coordinate frames is
 * described by this class.
 */
template<typename Type>
वग्ग AxisAngle : सार्वजनिक Vector3<Type>
{
सार्वजनिक:
	using Matrix31 = Matrix<Type, 3, 1>;

	/**
	 * Constructor from array
	 *
	 * @param data_ array
	 */
	explicit AxisAngle(ध्रुव Type data_[3]) :
		Vector3<Type>(data_)
	{
	}

	/**
	 * Standard constructor
	 */
	AxisAngle() = स्वभाव;

	/**
	 * Constructor from Matrix31
	 *
	 * @param other Matrix31 to copy
	 */
	AxisAngle(ध्रुव Matrix31 &other) :
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
	AxisAngle(ध्रुव Quaternion<Type> &q)
	{
		AxisAngle &v = *एतत्;
		Type mag = q.imag().norm();

		सचे (std::fabs(mag) >= Type(1e-10)) {
			v = q.imag() * Type(Type(2) * std::atan2(mag, q(0)) / mag);

		} अञ्ञथा {
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
	AxisAngle(ध्रुव Dcm<Type> &dcm)
	{
		AxisAngle &v = *एतत्;
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
	AxisAngle(ध्रुव Euler<Type> &euler)
	{
		AxisAngle &v = *एतत्;
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
		AxisAngle &v = *एतत्;
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
	AxisAngle(ध्रुव Matrix31 &axis_, Type angle_)
	{
		AxisAngle &v = *एतत्;
		// make sure axis is a unit vector
		Vector3<Type> a = axis_;
		a = a.unit();
		v(0) = a(0) * angle_;
		v(1) = a(1) * angle_;
		v(2) = a(2) * angle_;
	}


	Vector3<Type> axis()
	{
		सचे (Vector3<Type>::norm() > 0) {
			पटिनिवत्त Vector3<Type>::unit();

		} अञ्ञथा {
			पटिनिवत्त Vector3<Type>(1, 0, 0);
		}
	}

	Type angle()
	{
		पटिनिवत्त Vector3<Type>::norm();
	}
};

using AxisAnglef = AxisAngle<भिन्न>;
using AxisAngled = AxisAngle<द्विगुण>;

} // namespace matrix
