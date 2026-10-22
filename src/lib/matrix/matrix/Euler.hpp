/**
 * @file Euler.hpp
 *
 * All rotations and axis systems follow the right-hand rule
 *
 * An instance of this class defines a rotation from coordinate frame 1 to coordinate frame 2.
 * It follows the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.
 * In order to go from frame 1 to frame 2 we apply the following rotations consecutively.
 * 1) We rotate about our initial Z axis by an angle of _psi.
 * 2) We rotate about the newly created Y' axis by an angle of _theta.
 * 3) We rotate about the newly created X'' axis by an angle of _phi.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

ନାମାକାଶ matrix
{

/**
 * Euler angles class
 *
 * This class describes the rotation from frame 1
 * to frame 2 via 3-2-1 intrinsic Tait-Bryan rotation sequence.
 */
template<typename Type>
ଶ୍ରେଣୀ Euler : ସର୍ବସାଧାରଣ Vector<Type, 3>
{
ସର୍ବସାଧାରଣ:
	/**
	 * Standard constructor
	 */
	Euler() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	/**
	 * Copy constructor
	 *
	 * @param other vector to copy
	 */
	Euler(ଅଚଳ Vector<Type, 3> &other) :
		Vector<Type, 3>(other)
	{
	}

	/**
	 * Constructor from Matrix31
	 *
	 * @param other Matrix31 to copy
	 */
	Euler(ଅଚଳ Matrix<Type, 3, 1> &other) :
		Vector<Type, 3>(other)
	{
	}

	/**
	 * Constructor from euler angles
	 *
	 * Instance is initialized from an 3-2-1 intrinsic Tait-Bryan
	 * rotation sequence representing transformation from frame 1
	 * to frame 2.
	 *
	 * @param phi_ rotation angle about X axis
	 * @param theta_ rotation angle about Y axis
	 * @param psi_ rotation angle about Z axis
	 */
	Euler(Type phi_, Type theta_, Type psi_) : Vector<Type, 3>()
	{
		phi() = phi_;
		theta() = theta_;
		psi() = psi_;
	}

	/**
	 * Constructor from DCM matrix
	 *
	 * Instance is set from Dcm representing transformation from
	 * frame 2 to frame 1.
	 * This instance will hold the angles defining the 3-2-1 intrinsic
	 * Tait-Bryan rotation sequence from frame 1 to frame 2.
	 *
	 * @param dcm Direction cosine matrix
	*/
	Euler(ଅଚଳ Dcm<Type> &dcm)
	{
		theta() = std::asin(-dcm(2, 0));

		ଯଦି ((std::fabs(theta() - Type(M_PI / 2))) < Type(1.0e-3)) {
			phi() = 0;
			psi() = std::atan2(dcm(1, 2), dcm(0, 2));

		} ଅନ୍ୟଥା ଯଦି ((std::fabs(theta() + Type(M_PI / 2))) < Type(1.0e-3)) {
			phi() = 0;
			psi() = std::atan2(-dcm(1, 2), -dcm(0, 2));

		} ଅନ୍ୟଥା {
			phi() = std::atan2(dcm(2, 1), dcm(2, 2));
			psi() = std::atan2(dcm(1, 0), dcm(0, 0));
		}
	}

	/**
	 * Constructor from quaternion instance.
	 *
	 * Instance is set from a quaternion representing transformation
	 * from frame 2 to frame 1.
	 * This instance will hold the angles defining the 3-2-1 intrinsic
	 * Tait-Bryan rotation sequence from frame 1 to frame 2.
	 *
	 * @param q quaternion
	*/
	Euler(ଅଚଳ Quaternion<Type> &q) : Vector<Type, 3>(Euler(Dcm<Type>(q)))
	{
	}

	ଧାଡ଼ିଭିତରେ Type phi() ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି)(0);
	}
	ଧାଡ଼ିଭିତରେ Type theta() ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି)(1);
	}
	ଧାଡ଼ିଭିତରେ Type psi() ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି)(2);
	}

	ଧାଡ଼ିଭିତରେ Type &phi()
	{
		ଫେରନ୍ତୁ (*ଏହି)(0);
	}
	ଧାଡ଼ିଭିତରେ Type &theta()
	{
		ଫେରନ୍ତୁ (*ଏହି)(1);
	}
	ଧାଡ଼ିଭିତରେ Type &psi()
	{
		ଫେରନ୍ତୁ (*ଏହି)(2);
	}

};

using Eulerf = Euler<ଦଶମିକ>;
using Eulerd = Euler<ଦ୍ୱିଗୁଣ>;

} // namespace matrix
