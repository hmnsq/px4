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

#அந்தர்பாவ "SquareMatrix.hpp"
#அந்தர்பாவ "Vector3.hpp"

நாமாகாஶ matrix
{

template<typename Type>
வகுப்பு AxisAngle;

template<typename Type>
வகுப்பு Euler;

template<typename Type>
வகுப்பு Quaternion;

/**
 * Direction cosine matrix class
 *
 * The rotation between two coordinate frames is
 * described by this class.
 */
template<typename Type>
வகுப்பு Dcm : பொது SquareMatrix<Type, 3>
{
பொது:
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
	explicit Dcm(மாறிலி Type data_[3][3]) : SquareMatrix<Type, 3>(data_)
	{
	}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	explicit Dcm(மாறிலி Type data_[9]) : SquareMatrix<Type, 3>(data_)
	{
	}

	/**
	 * Copy constructor
	 *
	 * @param other Matrix33 to set dcm to
	 */
	Dcm(மாறிலி Matrix<Type, 3, 3> &other) : SquareMatrix<Type, 3>(other)
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
	Dcm(மாறிலி Quaternion<Type> &q)
	{
		Dcm &dcm = *இது;
		மாறிலி Type a = q(0);
		மாறிலி Type b = q(1);
		மாறிலி Type c = q(2);
		மாறிலி Type d = q(3);
		மாறிலி Type ab = a * b;
		மாறிலி Type ac = a * c;
		மாறிலி Type ad = a * d;
		மாறிலி Type bb = b * b;
		மாறிலி Type bc = b * c;
		மாறிலி Type bd = b * d;
		மாறிலி Type cc = c * c;
		மாறிலி Type cd = c * d;
		மாறிலி Type dd = d * d;
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
	Dcm(மாறிலி Euler<Type> &euler)
	{
		Dcm &dcm = *இது;
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
	Dcm(மாறிலி AxisAngle<Type> &aa)
	{
		Dcm &dcm = *இது;
		dcm = Quaternion<Type>(aa);
	}

	Vector3<Type> vee() மாறிலி // inverse to Vector.hat() operation
	{
		மாறிலி Dcm &A(*இது);
		திரும்பு {-A(1, 2), A(0, 2), -A(0, 1)};
	}

	வெறுமை renormalize()
	{
		/* renormalize rows */
		க்கு (size_t r = 0; r < 3; r++) {
			matrix::Vector3<Type> rvec(Matrix<Type, 1, 3>(இது->Matrix<Type, 3, 3>::row(r)).transpose());
			இது->Matrix<Type, 3, 3>::row(r) = rvec.normalized();
		}
	}
};

using Dcmf = Dcm<தசம>;
using Dcmd = Dcm<இரட்டை>;

} // namespace matrix
