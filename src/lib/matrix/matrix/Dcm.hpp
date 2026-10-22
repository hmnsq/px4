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

#निर्देशः once

#अन्तर्भाव "SquareMatrix.hpp"
#अन्तर्भाव "Vector3.hpp"

नामाकाश matrix
{

प्रतिरूपम्<प्रकारनाम Type>
वर्ग AxisAngle;

प्रतिरूपम्<प्रकारनाम Type>
वर्ग Euler;

प्रतिरूपम्<प्रकारनाम Type>
वर्ग Quaternion;

/**
 * Direction cosine matrix class
 *
 * The rotation between two coordinate frames is
 * described by this class.
 */
प्रतिरूपम्<प्रकारनाम Type>
वर्ग Dcm : सार्वजनिक वर्गफलम्_Matrix<Type, 3>
{
सार्वजनिक:
	/**
	 * Standard constructor
	 *
	 * Initializes to identity
	 */
	Dcm() : वर्गफलम्_Matrix<Type, 3>(eye<Type, 3>()) {}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	स्पष्ट Dcm(ध्रुव Type दत्त_2[3][3]) : वर्गफलम्_Matrix<Type, 3>(दत्त_2)
	{
	}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	स्पष्ट Dcm(ध्रुव Type दत्त_2[9]) : वर्गफलम्_Matrix<Type, 3>(दत्त_2)
	{
	}

	/**
	 * Copy constructor
	 *
	 * @param other Matrix33 to set dcm to
	 */
	Dcm(ध्रुव Matrix<Type, 3, 3> &other) : वर्गफलम्_Matrix<Type, 3>(other)
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
	Dcm(ध्रुव Quaternion<Type> &q)
	{
		Dcm &dcm = *एतत्;
		ध्रुव Type a = q(0);
		ध्रुव Type b = q(1);
		ध्रुव Type c = q(2);
		ध्रुव Type d = q(3);
		ध्रुव Type ab = a * b;
		ध्रुव Type ac = a * c;
		ध्रुव Type ad = a * d;
		ध्रुव Type bb = b * b;
		ध्रुव Type bc = b * c;
		ध्रुव Type bd = b * d;
		ध्रुव Type cc = c * c;
		ध्रुव Type cd = c * d;
		ध्रुव Type dd = d * d;
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
	Dcm(ध्रुव Euler<Type> &euler)
	{
		Dcm &dcm = *एतत्;
		Type cosPhi = Type(std::कोटिज्या(euler.phi()));
		Type sinPhi = Type(std::ज्या(euler.phi()));
		Type cosThe = Type(std::कोटिज्या(euler.theta()));
		Type sinThe = Type(std::ज्या(euler.theta()));
		Type cosPsi = Type(std::कोटिज्या(euler.psi()));
		Type sinPsi = Type(std::ज्या(euler.psi()));

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
	Dcm(ध्रुव AxisAngle<Type> &aa)
	{
		Dcm &dcm = *एतत्;
		dcm = Quaternion<Type>(aa);
	}

	Vector3<Type> vee() ध्रुव // inverse to Vector.hat() operation
	{
		ध्रुव Dcm &A(*एतत्);
		प्रत्यागम {-A(1, 2), A(0, 2), -A(0, 1)};
	}

	शून्य renormalize()
	{
		/* renormalize rows */
		प्रति (size_t r = 0; r < 3; r++) {
			matrix::Vector3<Type> rvec(Matrix<Type, 1, 3>(एतत्->Matrix<Type, 3, 3>::row(r)).transpose());
			एतत्->Matrix<Type, 3, 3>::row(r) = rvec.normalized();
		}
	}
};

उपयुज्य Dcmf = Dcm<भिन्न>;
उपयुज्य Dcmd = Dcm<द्विगुण>;

} // namespace matrix
