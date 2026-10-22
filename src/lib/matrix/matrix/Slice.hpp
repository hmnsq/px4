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

	SliceT(ध्रुव Self &other) = स्वभाव;

	ध्रुव Type &चालक()(माप_प्रकार i, माप_प्रकार j) ध्रुव
	{
		प्रतिज्ञा(i < P);
		प्रतिज्ञा(j < Q);

		प्रत्यागम (*_data)(_x0 + i, _y0 + j);
	}

	Type &चालक()(माप_प्रकार i, माप_प्रकार j)
	{
		प्रतिज्ञा(i < P);
		प्रतिज्ञा(j < Q);

		प्रत्यागम (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &चालक=(ध्रुव Self &other)
	{
		// Protect against self-assignment
		यदि (एतत् == &other) {
			प्रत्यागम *एतत्;
		}

		प्रत्यागम एतत्->चालक=<M, N>(other);
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	Self &चालक=(ध्रुव SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		प्रत्यागम self;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	SliceT<MatrixT, Type, P, Q, M, N> &चालक=(ध्रुव SliceT<ध्रुव Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		प्रत्यागम self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक=(ध्रुव Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		प्रत्यागम self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक=(ध्रुव Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		प्रत्यागम self;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	Matrix<Type, P, Q> चालक-(ध्रुव SliceT<ध्रुव Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		प्रत्यागम Matrix<Type, P, Q> {*एतत्} - other;
	}


	Matrix<Type, P, Q> चालक-(ध्रुव Matrix<Type, P, Q> &other)
	{
		प्रत्यागम Matrix<Type, P, Q> {*एतत्} - other;
	}

	Matrix<Type, P, Q> चालक-(ध्रुव Type &other)
	{
		प्रत्यागम Matrix<Type, P, Q> {*एतत्} - other;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	Matrix<Type, P, Q> चालक+(ध्रुव SliceT<ध्रुव Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		प्रत्यागम Matrix<Type, P, Q> {*एतत्} + other;
	}

	Matrix<Type, P, Q> चालक+(ध्रुव Matrix<Type, P, Q> &other)
	{
		प्रत्यागम Matrix<Type, P, Q> {*एतत्} + other;
	}

	Matrix<Type, P, Q> चालक+(ध्रुव Type &other)
	{
		प्रत्यागम Matrix<Type, P, Q> {*एतत्} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	ढाँचा <माप_प्रकार DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &चालक=(ध्रुव Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		प्रत्यागम self;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	SliceT<MatrixT, Type, P, Q, M, N> &चालक+=(ध्रुव SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		प्रत्यागम self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक+=(ध्रुव Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		प्रत्यागम self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक+=(ध्रुव Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		प्रत्यागम self;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	SliceT<MatrixT, Type, P, Q, M, N> &चालक-=(ध्रुव SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		प्रत्यागम self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक-=(ध्रुव Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		प्रत्यागम self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक-=(ध्रुव Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		प्रत्यागम self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक*=(ध्रुव Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		प्रत्यागम self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &चालक/=(ध्रुव Type &scalar)
	{
		प्रत्यागम चालक*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> चालक*(ध्रुव Type &scalar) ध्रुव
	{
		प्रत्यागम Matrix<Type, P, Q> {*एतत्} * scalar;
	}

	Matrix<Type, P, Q> चालक/(ध्रुव Type &scalar) ध्रुव
	{
		प्रत्यागम (*एतत्) * (1 / scalar);
	}

	ढाँचा<माप_प्रकार R, माप_प्रकार S>
	ध्रुव SliceT<MatrixT, Type, R, S, M, N> slice(माप_प्रकार x0, माप_प्रकार y0) ध्रुव
	{
		प्रत्यागम SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	ढाँचा<माप_प्रकार R, माप_प्रकार S>
	SliceT<MatrixT, Type, R, S, M, N> slice(माप_प्रकार x0, माप_प्रकार y0)
	{
		प्रत्यागम SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	शून्य copyTo(Type dst[P * Q]) ध्रुव
	{
		ध्रुव SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	शून्य copyToColumnMajor(Type dst[P * Q]) ध्रुव
	{
		ध्रुव SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() ध्रुव
	{
		ध्रुव SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;
		Vector < Type, P < Q ? P : Q > res;

		प्रति (माप_प्रकार j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		प्रत्यागम res;
	}

	ढाँचा<माप_प्रकार MM, माप_प्रकार NN>
	Type dot(ध्रुव SliceT<MatrixT, Type, P, Q, MM, NN> &other) ध्रुव
	{
		ध्रुव SliceT<MatrixT, Type, P, Q, M, N> &self = *एतत्;
		Type accum(0);

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		प्रत्यागम accum;
	}

	Type norm_squared() ध्रुव
	{
		प्रत्यागम dot(*एतत्);
	}

	Type norm() ध्रुव
	{
		प्रत्यागम std::वर्ग_मूल(norm_squared());
	}

	तार्किक longerThan(Type testVal) ध्रुव
	{
		प्रत्यागम norm_squared() > testVal * testVal;
	}

	Type max() ध्रुव
	{
		Type max_val = (*एतत्)(0, 0);

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				Type val = (*एतत्)(i, j);

				यदि (val > max_val) {
					max_val = val;
				}
			}
		}

		प्रत्यागम max_val;
	}

	Type min() ध्रुव
	{
		Type min_val = (*एतत्)(0, 0);

		प्रति (माप_प्रकार i = 0; i < P; i++) {
			प्रति (माप_प्रकार j = 0; j < Q; j++) {
				Type val = (*एतत्)(i, j);

				यदि (val < min_val) {
					min_val = val;
				}
			}
		}

		प्रत्यागम min_val;
	}

गुप्त:
	माप_प्रकार _x0, _y0;
	MatrixT *_data;
};

ढाँचा <typename Type, माप_प्रकार P, माप_प्रकार Q, माप_प्रकार M, माप_प्रकार N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

ढाँचा <typename Type, माप_प्रकार P, माप_प्रकार Q, माप_प्रकार M, माप_प्रकार N>
using ConstSlice = SliceT<ध्रुव Matrix<Type, M, N>, Type, P, Q, M, N>;

}
