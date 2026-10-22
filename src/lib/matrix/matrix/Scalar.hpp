/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#निर्देशः once

#अन्तर्भाव "Matrix.hpp"

नामाकाश matrix
{

प्रतिरूपम्<प्रकारनाम Type>
वर्ग Scalar
{
सार्वजनिक:
	Scalar() = विलोपय;

	Scalar(ध्रुव Matrix<Type, 1, 1> &other) :
		मानम्_2{other(0, 0)}
	{
	}

	Scalar(Type other) : मानम्_2(other)
	{
	}

	संक्रियकः ध्रुव Type &()
	{
		प्रत्यागम मानम्_2;
	}

	संक्रियकः Matrix<Type, 1, 1>() ध्रुव
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = मानम्_2;
		प्रत्यागम m;
	}

	संक्रियकः सदिशम्<Type, 1>() ध्रुव
	{
		सदिशम्<Type, 1> m;
		m(0) = मानम्_2;
		प्रत्यागम m;
	}

	तार्किक संक्रियकः==(ध्रुव भिन्न other) ध्रुव
	{
		प्रत्यागम isEqualF(मानम्_2, other);
	}

गुप्त:
	ध्रुव Type मानम्_2;

};

उपयुज्य Scalarf = Scalar<भिन्न>;
उपयुज्य Scalard = Scalar<द्विगुण>;

} // namespace matrix
