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

#include "Vector.hpp"

namespace matrix
{
// Vector that only store nonzero elements,
// which indices are specified as parameter pack
template<typename Type, size_t M, size_t... Idxs>
class SparseVector
{
private:
	enum : size_t { N = sizeof...(Idxs) };

	static constexpr bool duplicateIndices()
	{
		constexpr size_t indicies[] {Idxs...};

		за (size_t i = 0; i < N; i++) {
			за (size_t j = 0; j < i; j++) {
				ако (indicies[i] == indicies[j]) {
					върни true;
				}
			}
		}

		върни false;
	}

	static constexpr size_t findMaxIndex()
	{
		constexpr size_t indicies[] {Idxs...};
		size_t maxIndex = 0;

		за (size_t i = 0; i < N; i++) {
			ако (maxIndex < indicies[i]) {
				maxIndex = indicies[i];
			}
		}

		върни maxIndex;
	}

	static constexpr size_t indexAt(size_t i)
	{
		constexpr size_t indicies[] {Idxs...};
		върни indicies[i];
	}

	static_assert(!duplicateIndices(), "Duplicate indices");
	static_assert(N < M, "More entries than elements, use a dense vector");
	static_assert(N > 0, "A sparse vector needs at least one element");
	static_assert(findMaxIndex() < M, "Largest entry doesn't fit in sparse vector");

	Type _data[N] {};

	static constexpr цяло findCompressedIndex(size_t index)
	{
		constexpr size_t indicies[] {Idxs...};
		цяло compressedIndex = -1;

		за (size_t i = 0; i < N; i++) {
			ако (index == indicies[i]) {
				compressedIndex = static_cast<цяло>(i);
			}
		}

		върни compressedIndex;
	}

public:
	constexpr size_t non_zeros() const
	{
		върни N;
	}

	constexpr size_t index(size_t i) const
	{
		върни indexAt(i);
	}

	SparseVector() = default;

	SparseVector(const matrix::Vector<Type, M> &data)
	{
		за (size_t i = 0; i < N; i++) {
			_data[i] = data(indexAt(i));
		}
	}

	explicit SparseVector(const Type data[N])
	{
		memcpy(_data, data, sizeof(_data));
	}

	template <size_t i>
	inline Type at() const
	{
		constexpr цяло compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		върни _data[compressed_index];
	}

	template <size_t i>
	inline Type &at()
	{
		constexpr цяло compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		върни _data[compressed_index];
	}

	inline Type atCompressedIndex(size_t i) const
	{
		assert(i < N);
		върни _data[i];
	}

	inline Type &atCompressedIndex(size_t i)
	{
		assert(i < N);
		върни _data[i];
	}

	void setZero()
	{
		за (size_t i = 0; i < N; i++) {
			_data[i] = Type(0);
		}
	}

	Type dot(const matrix::Vector<Type, M> &other) const
	{
		Type accum(0);

		за (size_t i = 0; i < N; i++) {
			accum += _data[i] * other(indexAt(i));
		}

		върни accum;
	}

	matrix::Vector<Type, M> operator+(const matrix::Vector<Type, M> &other) const
	{
		matrix::Vector<Type, M> vec = other;

		за (size_t i = 0; i < N; i++) {
			vec(indexAt(i)) +=  _data[i];
		}

		върни vec;
	}

	SparseVector &operator+=(Type t)
	{
		за (size_t i = 0; i < N; i++) {
			_data[i] += t;
		}

		върни *this;
	}

	Type norm_squared() const
	{
		Type accum(0);

		за (size_t i = 0; i < N; i++) {
			accum += _data[i] * _data[i];
		}

		върни accum;
	}

	Type norm() const
	{
		върни std::sqrt(norm_squared());
	}

	bool longerThan(Type testVal) const
	{
		върни norm_squared() > testVal * testVal;
	}
};

template<typename Type, size_t Q, size_t M, size_t ... Idxs>
matrix::Vector<Type, Q> operator*(const matrix::Matrix<Type, Q, M> &mat,
				  const matrix::SparseVector<Type, M, Idxs...> &vec)
{
	matrix::Vector<Type, Q> res;

	за (size_t i = 0; i < Q; i++) {
		const Vector<Type, M> row = mat.row(i);
		res(i) = vec.dot(row);
	}

	върни res;
}

// returns x.T * A * x
template<typename Type, size_t M, size_t ... Idxs>
Type quadraticForm(const matrix::SquareMatrix<Type, M> &A, const matrix::SparseVector<Type, M, Idxs...> &x)
{
	Type res = Type(0);

	за (size_t i = 0; i < x.non_zeros(); i++) {
		Type tmp = Type(0);

		за (size_t j = 0; j < x.non_zeros(); j++) {
			tmp += A(x.index(i), x.index(j)) * x.atCompressedIndex(j);
		}

		res += x.atCompressedIndex(i) * tmp;
	}

	върни res;
}

template<size_t M, size_t ... Idxs>
using SparseVectorf = SparseVector<float, M, Idxs...>;

}
