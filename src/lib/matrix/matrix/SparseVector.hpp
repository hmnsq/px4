/**
 * @file SparseVector.hpp
 *
 * SparseVector class.
 *
 * @author Kamil Ritz <kritz@ethz.ch>
 * @author Julian Kent <julian@auterion.com>
 *
 */

#pragma once

#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ matrix
{
// Vector that only store nonzero elements,
// which indices are specified as parameter pack
template<typename Type, size_t M, size_t... Idxs>
ವರ್ಗ SparseVector
{
ಗುಪ್ತ:
	ಗಣನಾ : size_t { N = ಪರಿಮಾಣ...(Idxs) };

	ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ duplicateIndices()
	{
		constexpr size_t indicies[] {Idxs...};

		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			ಪ್ರತಿ (size_t j = 0; j < i; j++) {
				ಯದಿ (indicies[i] == indicies[j]) {
					ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
				}
			}
		}

		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಸ್ಥಿರ constexpr size_t findMaxIndex()
	{
		constexpr size_t indicies[] {Idxs...};
		size_t maxIndex = 0;

		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			ಯದಿ (maxIndex < indicies[i]) {
				maxIndex = indicies[i];
			}
		}

		ಪ್ರತ್ಯಾಗಮ maxIndex;
	}

	ಸ್ಥಿರ constexpr size_t indexAt(size_t i)
	{
		constexpr size_t indicies[] {Idxs...};
		ಪ್ರತ್ಯಾಗಮ indicies[i];
	}

	static_assert(!duplicateIndices(), "Duplicate indices");
	static_assert(N < M, "More entries than elements, use a dense vector");
	static_assert(N > 0, "A sparse vector needs at least one element");
	static_assert(findMaxIndex() < M, "Largest entry doesn't fit in sparse vector");

	Type _data[N] {};

	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ findCompressedIndex(size_t index)
	{
		constexpr size_t indicies[] {Idxs...};
		ಪೂರ್ಣಾಙ್ಕ compressedIndex = -1;

		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			ಯದಿ (index == indicies[i]) {
				compressedIndex = static_cast<ಪೂರ್ಣಾಙ್ಕ>(i);
			}
		}

		ಪ್ರತ್ಯಾಗಮ compressedIndex;
	}

ಸಾರ್ವಜನಿಕ:
	constexpr size_t non_zeros() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ N;
	}

	constexpr size_t index(size_t i) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ indexAt(i);
	}

	SparseVector() = ಸ್ವಭಾವ;

	SparseVector(ಧ್ರುವ matrix::Vector<Type, M> &data)
	{
		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			_data[i] = data(indexAt(i));
		}
	}

	explicit SparseVector(ಧ್ರುವ Type data[N])
	{
		memcpy(_data, data, ಪರಿಮಾಣ(_data));
	}

	template <size_t i>
	ಅನ್ತಃಸ್ಥ Type at() ಧ್ರುವ
	{
		constexpr ಪೂರ್ಣಾಙ್ಕ compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		ಪ್ರತ್ಯಾಗಮ _data[compressed_index];
	}

	template <size_t i>
	ಅನ್ತಃಸ್ಥ Type &at()
	{
		constexpr ಪೂರ್ಣಾಙ್ಕ compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		ಪ್ರತ್ಯಾಗಮ _data[compressed_index];
	}

	ಅನ್ತಃಸ್ಥ Type atCompressedIndex(size_t i) ಧ್ರುವ
	{
		ಪ್ರತಿಜ್ಞಾ(i < N);
		ಪ್ರತ್ಯಾಗಮ _data[i];
	}

	ಅನ್ತಃಸ್ಥ Type &atCompressedIndex(size_t i)
	{
		ಪ್ರತಿಜ್ಞಾ(i < N);
		ಪ್ರತ್ಯಾಗಮ _data[i];
	}

	ಶೂನ್ಯ setZero()
	{
		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			_data[i] = Type(0);
		}
	}

	Type dot(ಧ್ರುವ matrix::Vector<Type, M> &other) ಧ್ರುವ
	{
		Type accum(0);

		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			accum += _data[i] * other(indexAt(i));
		}

		ಪ್ರತ್ಯಾಗಮ accum;
	}

	matrix::Vector<Type, M> operator+(ಧ್ರುವ matrix::Vector<Type, M> &other) ಧ್ರುವ
	{
		matrix::Vector<Type, M> vec = other;

		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			vec(indexAt(i)) +=  _data[i];
		}

		ಪ್ರತ್ಯಾಗಮ vec;
	}

	SparseVector &operator+=(Type t)
	{
		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			_data[i] += t;
		}

		ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
	}

	Type norm_squared() ಧ್ರುವ
	{
		Type accum(0);

		ಪ್ರತಿ (size_t i = 0; i < N; i++) {
			accum += _data[i] * _data[i];
		}

		ಪ್ರತ್ಯಾಗಮ accum;
	}

	Type norm() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ std::sqrt(norm_squared());
	}

	ತಾರ್ಕಿಕ longerThan(Type testVal) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ norm_squared() > testVal * testVal;
	}
};

template<typename Type, size_t Q, size_t M, size_t ... Idxs>
matrix::Vector<Type, Q> operator*(ಧ್ರುವ matrix::Matrix<Type, Q, M> &mat,
				  ಧ್ರುವ matrix::SparseVector<Type, M, Idxs...> &vec)
{
	matrix::Vector<Type, Q> res;

	ಪ್ರತಿ (size_t i = 0; i < Q; i++) {
		ಧ್ರುವ Vector<Type, M> row = mat.row(i);
		res(i) = vec.dot(row);
	}

	ಪ್ರತ್ಯಾಗಮ res;
}

// returns x.T * A * x
template<typename Type, size_t M, size_t ... Idxs>
Type quadraticForm(ಧ್ರುವ matrix::SquareMatrix<Type, M> &A, ಧ್ರುವ matrix::SparseVector<Type, M, Idxs...> &x)
{
	Type res = Type(0);

	ಪ್ರತಿ (size_t i = 0; i < x.non_zeros(); i++) {
		Type tmp = Type(0);

		ಪ್ರತಿ (size_t j = 0; j < x.non_zeros(); j++) {
			tmp += A(x.index(i), x.index(j)) * x.atCompressedIndex(j);
		}

		res += x.atCompressedIndex(i) * tmp;
	}

	ಪ್ರತ್ಯಾಗಮ res;
}

template<size_t M, size_t ... Idxs>
using SparseVectorf = SparseVector<ಭಿನ್ನ, M, Idxs...>;

}
