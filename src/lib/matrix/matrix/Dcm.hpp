/**
 * @file Dcm.hpp
 *
 * A direction cosine matrix class.
 * All rotations and axis systems follow the right-hand rule.
 *
 * This library uses the convention that premultiplying a three dimensional
 * vector represented in coordinate system 1 will apply a rotation from coordinate system
 * 1 to coordinate system 2 to the vector.
 * Likewise, a matrix instance of this class also represents a coordinate transformation
 * from frame 2 to frame 1.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#અન્તર્ભાવ "SquareMatrix.hpp"
#અન્તર્ભાવ "Vector3.hpp"

નામાકાશ matrix
{

template<typename Type>
વર્ગ AxisAngle;

template<typename Type>
વર્ગ Euler;

template<typename Type>
વર્ગ Quaternion;

/**
 * Direction cosine matrix class
 *
 * The rotation between two coordinate frames is
 * described by this class.
 */
template<typename Type>
વર્ગ Dcm : જાહેર SquareMatrix<Type, 3>
{
જાહેર:
	/**
	 * Standard constructor
	 *
	 * Initializes to identity
	 */
	Dcm() : SquareMatrix<Type, 3>(eye<Type, 3>()) {}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	explicit Dcm(અચલ Type data_[3][3]) : SquareMatrix<Type, 3>(data_)
	{
	}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	explicit Dcm(અચલ Type data_[9]) : SquareMatrix<Type, 3>(data_)
	{
	}

	/**
	 * Copy constructor
	 *
	 * @param other Matrix33 to set dcm to
	 */
	Dcm(અચલ Matrix<Type, 3, 3> &other) : SquareMatrix<Type, 3>(other)
	{
	}

	/**
	 * Constructor from quaternion
	 *
	 * Instance is initialized from quaternion representing
	 * coordinate transformation from frame 2 to frame 1.
	 *
	 * @param q quaternion to set dcm to
	 */
	Dcm(અચલ Quaternion<Type> &q)
	{
		Dcm &dcm = *આ;
		અચલ Type a = q(0);
		અચલ Type b = q(1);
		અચલ Type c = q(2);
		અચલ Type d = q(3);
		અચલ Type ab = a * b;
		અચલ Type ac = a * c;
		અચલ Type ad = a * d;
		અચલ Type bb = b * b;
		અચલ Type bc = b * c;
		અચલ Type bd = b * d;
		અચલ Type cc = c * c;
		અચલ Type cd = c * d;
		અચલ Type dd = d * d;
		dcm(0, 0) = Type(1) - Type(2) * (cc + dd);
		dcm(0, 1) = Type(2) * (bc - ad);
		dcm(0, 2) = Type(2) * (ac + bd);
		dcm(1, 0) = Type(2) * (bc + ad);
		dcm(1, 1) = Type(1) - Type(2) * (bb + dd);
		dcm(1, 2) = Type(2) * (cd - ab);
		dcm(2, 0) = Type(2) * (bd - ac);
		dcm(2, 1) = Type(2) * (ab + cd);
		dcm(2, 2) = Type(1) - Type(2) * (bb + cc);
	}

	/**
	 * Constructor from euler angles
	 *
	 * This sets the transformation matrix from frame 2 to frame 1 where the rotation
	 * from frame 1 to frame 2 is described by a 3-2-1 intrinsic Tait-Bryan rotation sequence.
	 *
	 *
	 * @param euler euler angle instance
	 */
	Dcm(અચલ Euler<Type> &euler)
	{
		Dcm &dcm = *આ;
		Type cosPhi = Type(std::cos(euler.phi()));
		Type sinPhi = Type(std::sin(euler.phi()));
		Type cosThe = Type(std::cos(euler.theta()));
		Type sinThe = Type(std::sin(euler.theta()));
		Type cosPsi = Type(std::cos(euler.psi()));
		Type sinPsi = Type(std::sin(euler.psi()));

		dcm(0, 0) = cosThe * cosPsi;
		dcm(0, 1) = -cosPhi * sinPsi + sinPhi * sinThe * cosPsi;
		dcm(0, 2) = sinPhi * sinPsi + cosPhi * sinThe * cosPsi;

		dcm(1, 0) = cosThe * sinPsi;
		dcm(1, 1) = cosPhi * cosPsi + sinPhi * sinThe * sinPsi;
		dcm(1, 2) = -sinPhi * cosPsi + cosPhi * sinThe * sinPsi;

		dcm(2, 0) = -sinThe;
		dcm(2, 1) = sinPhi * cosThe;
		dcm(2, 2) = cosPhi * cosThe;
	}


	/**
	 * Constructor from axis angle
	 *
	 * This sets the transformation matrix from frame 2 to frame 1 where the rotation
	 * from frame 1 to frame 2 is described by a 3-2-1 intrinsic Tait-Bryan rotation sequence.
	 *
	 *
	 * @param euler euler angle instance
	 */
	Dcm(અચલ AxisAngle<Type> &aa)
	{
		Dcm &dcm = *આ;
		dcm = Quaternion<Type>(aa);
	}

	Vector3<Type> vee() અચલ // inverse to Vector.hat() operation
	{
		અચલ Dcm &A(*આ);
		પાછા {-A(1, 2), A(0, 2), -A(0, 1)};
	}

	શૂન્ય renormalize()
	{
		/* renormalize rows */
		માટે (size_t r = 0; r < 3; r++) {
			matrix::Vector3<Type> rvec(Matrix<Type, 1, 3>(આ->Matrix<Type, 3, 3>::row(r)).transpose());
			આ->Matrix<Type, 3, 3>::row(r) = rvec.normalized();
		}
	}
};

using Dcmf = Dcm<દશાંશ>;
using Dcmd = Dcm<દ્વિગુણ>;

} // namespace matrix
