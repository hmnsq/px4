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

#ಅನ್ತರ್ಭಾವ "SquareMatrix.hpp"
#ಅನ್ತರ್ಭಾವ "Vector3.hpp"

ನಾಮಾಕಾಶ matrix
{

template<typename Type>
ವರ್ಗ AxisAngle;

template<typename Type>
ವರ್ಗ Euler;

template<typename Type>
ವರ್ಗ Quaternion;

/**
 * Direction cosine matrix class
 *
 * The rotation between two coordinate frames is
 * described by this class.
 */
template<typename Type>
ವರ್ಗ Dcm : ಸಾರ್ವಜನಿಕ SquareMatrix<Type, 3>
{
ಸಾರ್ವಜನಿಕ:
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
	explicit Dcm(ಧ್ರುವ Type data_[3][3]) : SquareMatrix<Type, 3>(data_)
	{
	}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	explicit Dcm(ಧ್ರುವ Type data_[9]) : SquareMatrix<Type, 3>(data_)
	{
	}

	/**
	 * Copy constructor
	 *
	 * @param other Matrix33 to set dcm to
	 */
	Dcm(ಧ್ರುವ Matrix<Type, 3, 3> &other) : SquareMatrix<Type, 3>(other)
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
	Dcm(ಧ್ರುವ Quaternion<Type> &q)
	{
		Dcm &dcm = *ಏತತ್;
		ಧ್ರುವ Type a = q(0);
		ಧ್ರುವ Type b = q(1);
		ಧ್ರುವ Type c = q(2);
		ಧ್ರುವ Type d = q(3);
		ಧ್ರುವ Type ab = a * b;
		ಧ್ರುವ Type ac = a * c;
		ಧ್ರುವ Type ad = a * d;
		ಧ್ರುವ Type bb = b * b;
		ಧ್ರುವ Type bc = b * c;
		ಧ್ರುವ Type bd = b * d;
		ಧ್ರುವ Type cc = c * c;
		ಧ್ರುವ Type cd = c * d;
		ಧ್ರುವ Type dd = d * d;
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
	Dcm(ಧ್ರುವ Euler<Type> &euler)
	{
		Dcm &dcm = *ಏತತ್;
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
	Dcm(ಧ್ರುವ AxisAngle<Type> &aa)
	{
		Dcm &dcm = *ಏತತ್;
		dcm = Quaternion<Type>(aa);
	}

	Vector3<Type> vee() ಧ್ರುವ // inverse to Vector.hat() operation
	{
		ಧ್ರುವ Dcm &A(*ಏತತ್);
		ಪ್ರತ್ಯಾಗಮ {-A(1, 2), A(0, 2), -A(0, 1)};
	}

	ಶೂನ್ಯ renormalize()
	{
		/* renormalize rows */
		ಪ್ರತಿ (size_t r = 0; r < 3; r++) {
			matrix::Vector3<Type> rvec(Matrix<Type, 1, 3>(ಏತತ್->Matrix<Type, 3, 3>::row(r)).transpose());
			ಏತತ್->Matrix<Type, 3, 3>::row(r) = rvec.normalized();
		}
	}
};

using Dcmf = Dcm<ಭಿನ್ನ>;
using Dcmd = Dcm<ದ್ವಿಗುಣ>;

} // namespace matrix
