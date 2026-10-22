/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#निर्देशः once

#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

प्रतिरूपम्<प्रकारनाम Type>
वर्ग Vector2 : सार्वजनिक सदिशम्<Type, 2>
{
सार्वजनिक:

	उपयुज्य Matrix21 = Matrix<Type, 2, 1>;
	उपयुज्य Vector3 = सदिशम्<Type, 3>;

	Vector2() = स्वभाव;

	Vector2(ध्रुव Matrix21 &other) :
		सदिशम्<Type, 2>(other)
	{
	}

	स्पष्ट Vector2(ध्रुव Type दत्त_2[2]) :
		सदिशम्<Type, 2>(दत्त_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*एतत्);
		v(0) = x;
		v(1) = y;
	}

	उपयुज्य base = सदिशम्<Type, 2>;
	उपयुज्य base::base;

	स्पष्ट Vector2(ध्रुव Vector3 &other)
	{
		Vector2 &v(*एतत्);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ध्रुव Matrix21 &b) ध्रुव
	{
		ध्रुव Vector2 &a(*एतत्);
		प्रत्यागम a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 संक्रियकः+(Vector2 other) ध्रुव
	{
		प्रत्यागम Matrix21::संक्रियकः+(other);
	}

	Vector2 संक्रियकः+(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix21::संक्रियकः+(scalar);
	}

	Vector2 संक्रियकः-(Vector2 other) ध्रुव
	{
		प्रत्यागम Matrix21::संक्रियकः-(other);
	}

	Vector2 संक्रियकः-(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix21::संक्रियकः-(scalar);
	}

	Vector2 संक्रियकः-() ध्रुव
	{
		प्रत्यागम Matrix21::संक्रियकः-();
	}

	Vector2 संक्रियकः*(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix21::संक्रियकः*(scalar);
	}

	Type संक्रियकः*(Vector2 b) ध्रुव
	{
		प्रत्यागम सदिशम्<Type, 2>::संक्रियकः*(b);
	}

	Type संक्रियकः%(ध्रुव Matrix21 &b) ध्रुव
	{
		प्रत्यागम (*एतत्).cross(b);
	}

};

उपयुज्य Vector2f = Vector2<भिन्न>;
उपयुज्य Vector2d = Vector2<द्विगुण>;

} // namespace matrix
