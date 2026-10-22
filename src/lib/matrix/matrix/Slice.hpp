/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#अन्तर्भाव <cassert>
#अन्तर्भाव <cstdio>
#अन्तर्भाव <cmath>

नामाकाश matrix
{

ढाँचा<typename Type, माप_प्रकार M, माप_प्रकार N>
वर्ग Matrix;

ढाँचा<typename Type, माप_प्रकार M>
वर्ग Vector;

ढाँचा <typename MatrixT, typename Type, माप_प्रकार P, माप_प्रकार Q, माप_प्रकार M, माप_प्रकार N>
वर्ग SliceT
{
सार्वजनिक:
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	SliceT(माप_प्रकार x0, माप_प्रकार y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		प्रतिज्ञा(x0 + P <= M);
		प्रतिज्ञा(y0 + Q <= N);
	}

	SliceT(अचर Self &other) = स्वभाव;

	अचर Type &चालक()(माप_प्रकार i, माप_प्रकार j) अचर
	{
		प्रतिज्ञा(i < P);
		प्रतिज्ञा(j < Q);

		फर्काऊ (*_data)(_x0 + i, _y0 + j);
	}

	Type &चालक()(माप_प्रकार i, माप_प्रकार j)
	{
		प्रतिज्ञा(i < P);
		प्रतिज्ञा(j < Q);

		फर्काऊ (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &चालक=(अचर Self &other)
	{
		// Protect against self-assignment
		यदि (एतत् == &other) {
			फर्काऊ *एतत्;
		}

		फर्काऊ एतत्->चालक=<M, N>(other);
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	Self &चालक=(अचर SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		फर्काऊ self;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	SliceT<MatrixT, Type, P, Q, M, N> &चालक=(अचर SliceT<अचर Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		फर्काऊ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक=(अचर Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		फर्काऊ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक=(अचर Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		फर्काऊ self;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	Matrix<Type, P, Q> चालक-(अचर SliceT<अचर Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		फर्काऊ Matrix<Type, P, Q> {*एतत्} - other;
	}


	Matrix<Type, P, Q> चालक-(अचर Matrix<Type, P, Q> &other)
	{
		फर्काऊ Matrix<Type, P, Q> {*एतत्} - other;
	}

	Matrix<Type, P, Q> चालक-(अचर Type &other)
	{
		फर्काऊ Matrix<Type, P, Q> {*एतत्} - other;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	Matrix<Type, P, Q> चालक+(अचर SliceT<अचर Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		फर्काऊ Matrix<Type, P, Q> {*एतत्} + other;
	}

	Matrix<Type, P, Q> चालक+(अचर Matrix<Type, P, Q> &other)
	{
		फर्काऊ Matrix<Type, P, Q> {*एतत्} + other;
	}

	Matrix<Type, P, Q> चालक+(अचर Type &other)
	{
		फर्काऊ Matrix<Type, P, Q> {*एतत्} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	ढाँचा <माप_प्रकार DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &चालक=(अचर Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		फर्काऊ self;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	SliceT<MatrixT, Type, P, Q, M, N> &चालक+=(अचर SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		फर्काऊ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक+=(अचर Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		फर्काऊ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक+=(अचर Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		फर्काऊ self;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	SliceT<MatrixT, Type, P, Q, M, N> &चालक-=(अचर SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		फर्काऊ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक-=(अचर Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		फर्काऊ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक-=(अचर Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		फर्काऊ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक*=(अचर Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		फर्काऊ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक/=(अचर Type &scalar)
	{
		फर्काऊ चालक*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> चालक*(अचर Type &scalar) अचर
	{
		फर्काऊ Matrix<Type, P, Q> {*एतत्} * scalar;
	}

	Matrix<Type, P, Q> चालक/(अचर Type &scalar) अचर
	{
		फर्काऊ (*एतत्) * (1 / scalar);
	}

	ढाँचा<माप_प्रकार R, माप_प्रकार S>
	अचर SliceT<MatrixT, Type, R, S, M, N> slice(माप_प्रकार x0, माप_प्रकार y0) अचर
	{
		फर्काऊ SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	ढाँचा<माप_प्रकार R, माप_प्रकार S>
	SliceT<MatrixT, Type, R, S, M, N> slice(माप_प्रकार x0, माप_प्रकार y0)
	{
		फर्काऊ SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	शून्य copyTo(Type dst[P * Q]) अचर
	{
		अचर SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	शून्य copyToColumnMajor(Type dst[P * Q]) अचर
	{
		अचर SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() अचर
	{
		अचर SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;
		Vector < Type, P < Q ? P : Q > res;

		लागि (माप_प्रकार j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		फर्काऊ res;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	Type dot(अचर SliceT<MatrixT, Type, P, Q, MM, NN> &other) अचर
	{
		अचर SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;
		Type accum(0);

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		फर्काऊ accum;
	}

	Type norm_squared() अचर
	{
		फर्काऊ dot(*एतत्);
	}

	Type norm() अचर
	{
		फर्काऊ std::वर्ग_मूल(norm_squared());
	}

	तार्किक longerThan(Type testVal) अचर
	{
		फर्काऊ norm_squared() > testVal * testVal;
	}

	Type max() अचर
	{
		Type max_val = (*एतत्)(0, 0);

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				Type val = (*एतत्)(i, j);

				यदि (val > max_val) {
					max_val = val;
				}
			}
		}

		फर्काऊ max_val;
	}

	Type min() अचर
	{
		Type min_val = (*एतत्)(0, 0);

		लागि (माप_प्रकार i = 0; i < P; i++) {
			लागि (माप_प्रकार j = 0; j < Q; j++) {
				Type val = (*एतत्)(i, j);

				यदि (val < min_val) {
					min_val = val;
				}
			}
		}

		फर्काऊ min_val;
	}

गुप्त:
	माप_प्रकार _x0, _y0;
	MatrixT *_data;
};

ढाँचा <typename Type, माप_प्रकार P, माप_प्रकार Q, माप_प्रकार M, माप_प्रकार N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

ढाँचा <typename Type, माप_प्रकार P, माप_प्रकार Q, माप_प्रकार M, माप_प्रकार N>
using ConstSlice = SliceT<अचर Matrix<Type, M, N>, Type, P, Q, M, N>;

}
