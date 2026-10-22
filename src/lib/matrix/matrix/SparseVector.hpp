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
	ga_nanaa : size_t { N = parimaa_na...(Idxs) };

	static constexpr bool duplicateIndices()
	{
		constexpr size_t indicies[] {Idxs...};

		සඳහා (size_t i = 0; i < N; i++) {
			සඳහා (size_t j = 0; j < i; j++) {
				නම් (indicies[i] == indicies[j]) {
					ආපසු true;
				}
			}
		}

		ආපසු false;
	}

	static constexpr size_t findMaxIndex()
	{
		constexpr size_t indicies[] {Idxs...};
		size_t maxIndex = 0;

		සඳහා (size_t i = 0; i < N; i++) {
			නම් (maxIndex < indicies[i]) {
				maxIndex = indicies[i];
			}
		}

		ආපසු maxIndex;
	}

	static constexpr size_t indexAt(size_t i)
	{
		constexpr size_t indicies[] {Idxs...};
		ආපසු indicies[i];
	}

	static_assert(!duplicateIndices(), "Duplicate indices");
	static_assert(N < M, "More entries than elements, use a dense vector");
	static_assert(N > 0, "A sparse vector needs at least one element");
	static_assert(findMaxIndex() < M, "Largest entry doesn't fit in sparse vector");

	Type _data[N] {};

	static constexpr පූර්ණ_සංඛ්‍යා findCompressedIndex(size_t index)
	{
		constexpr size_t indicies[] {Idxs...};
		පූර්ණ_සංඛ්‍යා compressedIndex = -1;

		සඳහා (size_t i = 0; i < N; i++) {
			නම් (index == indicies[i]) {
				compressedIndex = static_cast<පූර්ණ_සංඛ්‍යා>(i);
			}
		}

		ආපසු compressedIndex;
	}

public:
	constexpr size_t non_zeros() const
	{
		ආපසු N;
	}

	constexpr size_t index(size_t i) const
	{
		ආපසු indexAt(i);
	}

	SparseVector() = default;

	SparseVector(const matrix::Vector<Type, M> &data)
	{
		සඳහා (size_t i = 0; i < N; i++) {
			_data[i] = data(indexAt(i));
		}
	}

	explicit SparseVector(const Type data[N])
	{
		memcpy(_data, data, parimaa_na(_data));
	}

	template <size_t i>
	anta_hstha Type at() const
	{
		constexpr පූර්ණ_සංඛ්‍යා compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		ආපසු _data[compressed_index];
	}

	template <size_t i>
	anta_hstha Type &at()
	{
		constexpr පූර්ණ_සංඛ්‍යා compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		ආපසු _data[compressed_index];
	}

	anta_hstha Type atCompressedIndex(size_t i) const
	{
		assert(i < N);
		ආපසු _data[i];
	}

	anta_hstha Type &atCompressedIndex(size_t i)
	{
		assert(i < N);
		ආපසු _data[i];
	}

	void setZero()
	{
		සඳහා (size_t i = 0; i < N; i++) {
			_data[i] = Type(0);
		}
	}

	Type dot(const matrix::Vector<Type, M> &other) const
	{
		Type accum(0);

		සඳහා (size_t i = 0; i < N; i++) {
			accum += _data[i] * other(indexAt(i));
		}

		ආපසු accum;
	}

	matrix::Vector<Type, M> operator+(const matrix::Vector<Type, M> &other) const
	{
		matrix::Vector<Type, M> vec = other;

		සඳහා (size_t i = 0; i < N; i++) {
			vec(indexAt(i)) +=  _data[i];
		}

		ආපසු vec;
	}

	SparseVector &operator+=(Type t)
	{
		සඳහා (size_t i = 0; i < N; i++) {
			_data[i] += t;
		}

		ආපසු *this;
	}

	Type norm_squared() const
	{
		Type accum(0);

		සඳහා (size_t i = 0; i < N; i++) {
			accum += _data[i] * _data[i];
		}

		ආපසු accum;
	}

	Type norm() const
	{
		ආපසු std::sqrt(norm_squared());
	}

	bool longerThan(Type testVal) const
	{
		ආපසු norm_squared() > testVal * testVal;
	}
};

template<typename Type, size_t Q, size_t M, size_t ... Idxs>
matrix::Vector<Type, Q> operator*(const matrix::Matrix<Type, Q, M> &mat,
				  const matrix::SparseVector<Type, M, Idxs...> &vec)
{
	matrix::Vector<Type, Q> res;

	සඳහා (size_t i = 0; i < Q; i++) {
		const Vector<Type, M> row = mat.row(i);
		res(i) = vec.dot(row);
	}

	ආපසු res;
}

// returns x.T * A * x
template<typename Type, size_t M, size_t ... Idxs>
Type quadraticForm(const matrix::SquareMatrix<Type, M> &A, const matrix::SparseVector<Type, M, Idxs...> &x)
{
	Type res = Type(0);

	සඳහා (size_t i = 0; i < x.non_zeros(); i++) {
		Type tmp = Type(0);

		සඳහා (size_t j = 0; j < x.non_zeros(); j++) {
			tmp += A(x.index(i), x.index(j)) * x.atCompressedIndex(j);
		}

		res += x.atCompressedIndex(i) * tmp;
	}

	ආපසු res;
}

template<size_t M, size_t ... Idxs>
using SparseVectorf = SparseVector<float, M, Idxs...>;

}
