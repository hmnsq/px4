/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ ಮಾತೃಕೆ
{

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Type>
ವರ್ಗ Dcm;

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು Type>
ವರ್ಗ Vector3 : ಸಾರ್ವಜನಿಕ ಸದಿಶ<Type, 3>
{
ಸಾರ್ವಜನಿಕ:

	ಬಳಸಿ Matrix31 = ಮಾತೃಕೆ_2<Type, 3, 1>;

	Vector3() = ಪೂರ್ವನಿಯೋಜಿತ;

	Vector3(ಸ್ಥಿರಾಂಕ Matrix31 &other) :
		ಸದಿಶ<Type, 3>(other)
	{
	}

	ಸ್ಪಷ್ಟ Vector3(ಸ್ಥಿರಾಂಕ Type ದತ್ತಾಂಶ_2[3]) :
		ಸದಿಶ<Type, 3>(ದತ್ತಾಂಶ_2)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*ಇದು);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	ಬಳಸಿ base = ಸದಿಶ<Type, 3>;
	ಬಳಸಿ base::base;

	Vector3 cross(ಸ್ಥಿರಾಂಕ Matrix31 &b) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ Vector3 &a(*ಇದು);
		ಹಿಂತಿರುಗಿ {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	ಸಾಲೊಳಗೆ Vector3 ಕ್ರಿಯಾಕಾರಕ+(Vector3 other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix31::ಕ್ರಿಯಾಕಾರಕ+(other);
	}

	ಸಾಲೊಳಗೆ Vector3 ಕ್ರಿಯಾಕಾರಕ+(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix31::ಕ್ರಿಯಾಕಾರಕ+(scalar);
	}

	ಸಾಲೊಳಗೆ Vector3 ಕ್ರಿಯಾಕಾರಕ-(Vector3 other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix31::ಕ್ರಿಯಾಕಾರಕ-(other);
	}

	ಸಾಲೊಳಗೆ Vector3 ಕ್ರಿಯಾಕಾರಕ-(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix31::ಕ್ರಿಯಾಕಾರಕ-(scalar);
	}

	ಸಾಲೊಳಗೆ Vector3 ಕ್ರಿಯಾಕಾರಕ-() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix31::ಕ್ರಿಯಾಕಾರಕ-();
	}

	ಸಾಲೊಳಗೆ Vector3 ಕ್ರಿಯಾಕಾರಕ*(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix31::ಕ್ರಿಯಾಕಾರಕ*(scalar);
	}

	ಸಾಲೊಳಗೆ Type ಕ್ರಿಯಾಕಾರಕ*(Vector3 b) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಸದಿಶ<Type, 3>::ಕ್ರಿಯಾಕಾರಕ*(b);
	}

	ಸಾಲೊಳಗೆ Vector3 ಕ್ರಿಯಾಕಾರಕ%(ಸ್ಥಿರಾಂಕ Matrix31 &b) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ (*ಇದು).cross(b);
	}

	Const_ತುಂಡು<Type, 2, 1, 3, 1> xy() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ {0, 0, ಇದು};
	}

	ತುಂಡು_2<Type, 2, 1, 3, 1> xy()
	{
		ಹಿಂತಿರುಗಿ {0, 0, ಇದು};
	}

	Dcm<Type> hat() ಸ್ಥಿರಾಂಕ      // inverse to Dcm.vee() operation
	{
		ಸ್ಥಿರಾಂಕ Vector3 &v(*ಇದು);
		Dcm<Type> A;
		A(0, 0) = 0;
		A(0, 1) = -v(2);
		A(0, 2) = v(1);
		A(1, 0) = v(2);
		A(1, 1) = 0;
		A(1, 2) = -v(0);
		A(2, 0) = -v(1);
		A(2, 1) = v(0);
		A(2, 2) = 0;
		ಹಿಂತಿರುಗಿ A;
	}

};

ಬಳಸಿ Vector3f = Vector3<ದಶಮಾಂಶ>;
ಬಳಸಿ Vector3d = Vector3<ದ್ವಿಗುಣ>;

} // namespace matrix
