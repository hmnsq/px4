/**
 * @file AxisAngle.hpp
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ "Vector3.hpp"

ନାମାକାଶ matrix
{

template <typename Type>
ଶ୍ରେଣୀ Euler;

template<typename Type>
ଶ୍ରେଣୀ Quaternion;

/**
 * AxisAngle class
 *
 * The rotation between two coordinate frames is
 * described by this class.
 */
template<typename Type>
ଶ୍ରେଣୀ AxisAngle : ସର୍ବସାଧାରଣ Vector3<Type>
{
ସର୍ବସାଧାରଣ:
	using Matrix31 = Matrix<Type, 3, 1>;

	/**
	 * Constructor from array
	 *
	 * @param data_ array
	 */
	explicit AxisAngle(ଅଚଳ Type data_[3]) :
		Vector3<Type>(data_)
	{
	}

	/**
	 * Standard constructor
	 */
	AxisAngle() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	/**
	 * Constructor from Matrix31
	 *
	 * @param other Matrix31 to copy
	 */
	AxisAngle(ଅଚଳ Matrix31 &other) :
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
	AxisAngle(ଅଚଳ Quaternion<Type> &q)
	{
		AxisAngle &v = *ଏହି;
		Type mag = q.imag().norm();

		ଯଦି (std::fabs(mag) >= Type(1e-10)) {
			v = q.imag() * Type(Type(2) * std::atan2(mag, q(0)) / mag);

		} ଅନ୍ୟଥା {
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
	AxisAngle(ଅଚଳ Dcm<Type> &dcm)
	{
		AxisAngle &v = *ଏହି;
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
	AxisAngle(ଅଚଳ Euler<Type> &euler)
	{
		AxisAngle &v = *ଏହି;
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
		AxisAngle &v = *ଏହି;
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
	AxisAngle(ଅଚଳ Matrix31 &axis_, Type angle_)
	{
		AxisAngle &v = *ଏହି;
		// make sure axis is a unit vector
		Vector3<Type> a = axis_;
		a = a.unit();
		v(0) = a(0) * angle_;
		v(1) = a(1) * angle_;
		v(2) = a(2) * angle_;
	}


	Vector3<Type> axis()
	{
		ଯଦି (Vector3<Type>::norm() > 0) {
			ଫେରନ୍ତୁ Vector3<Type>::unit();

		} ଅନ୍ୟଥା {
			ଫେରନ୍ତୁ Vector3<Type>(1, 0, 0);
		}
	}

	Type angle()
	{
		ଫେରନ୍ତୁ Vector3<Type>::norm();
	}
};

using AxisAnglef = AxisAngle<ଦଶମିକ>;
using AxisAngled = AxisAngle<ଦ୍ୱିଗୁଣ>;

} // namespace matrix
