/**
 * @file AxisAngle.hpp
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#هدايت once

#ضمن "Vector3.hpp"

نطاق ميٽرڪس
{

سانچو <قسم_نالو Type>
صنف Euler;

سانچو<قسم_نالو Type>
صنف Quaternion;

/**
 * AxisAngle class
 *
 * The rotation between two coordinate frames is
 * described by this class.
 */
سانچو<قسم_نالو Type>
صنف AxisAngle : عام Vector3<Type>
{
عام:
	استعمال Matrix31 = ميٽرڪس_2<Type, 3, 1>;

	/**
	 * Constructor from array
	 *
	 * @param data_ array
	 */
	واضح AxisAngle(ثابت Type معلومات_2[3]) :
		Vector3<Type>(معلومات_2)
	{
	}

	/**
	 * Standard constructor
	 */
	AxisAngle() = افتراضي;

	/**
	 * Constructor from Matrix31
	 *
	 * @param other Matrix31 to copy
	 */
	AxisAngle(ثابت Matrix31 &other) :
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
	AxisAngle(ثابت Quaternion<Type> &q)
	{
		AxisAngle &v = *هذا;
		Type mag = q.imag().norm();

		إذا (std::fabs(mag) >= Type(1e-10)) {
			v = q.imag() * Type(Type(2) * std::atan2(mag, q(0)) / mag);

		} وإلا {
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
	AxisAngle(ثابت Dcm<Type> &dcm)
	{
		AxisAngle &v = *هذا;
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
	AxisAngle(ثابت Euler<Type> &euler)
	{
		AxisAngle &v = *هذا;
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
		AxisAngle &v = *هذا;
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
	AxisAngle(ثابت Matrix31 &axis_, Type angle_)
	{
		AxisAngle &v = *هذا;
		// make sure axis is a unit vector
		Vector3<Type> a = axis_;
		a = a.unit();
		v(0) = a(0) * angle_;
		v(1) = a(1) * angle_;
		v(2) = a(2) * angle_;
	}


	Vector3<Type> axis()
	{
		إذا (Vector3<Type>::norm() > 0) {
			أرجع Vector3<Type>::unit();

		} وإلا {
			أرجع Vector3<Type>(1, 0, 0);
		}
	}

	Type angle()
	{
		أرجع Vector3<Type>::norm();
	}
};

استعمال AxisAnglef = AxisAngle<عائم>;
استعمال AxisAngled = AxisAngle<مزدوج>;

} // namespace matrix
