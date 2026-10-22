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

#অন্তর্ভাব "Vector.hpp"

নামাকাশ matrix
{
// Vector that only store nonzero elements,
// which indices are specified as parameter pack
template<typename Type, size_t M, size_t... Idxs>
বর্গ SparseVector
{
গুপ্ত:
	গণনা : size_t { N = পরিমাণ...(Idxs) };

	স্থির constexpr তার্কিক duplicateIndices()
	{
		constexpr size_t indicies[] {Idxs...};

		প্রতি (size_t i = 0; i < N; i++) {
			প্রতি (size_t j = 0; j < i; j++) {
				যদি (indicies[i] == indicies[j]) {
					প্রত্যাগম সত্য;
				}
			}
		}

		প্রত্যাগম অসত্য;
	}

	স্থির constexpr size_t findMaxIndex()
	{
		constexpr size_t indicies[] {Idxs...};
		size_t maxIndex = 0;

		প্রতি (size_t i = 0; i < N; i++) {
			যদি (maxIndex < indicies[i]) {
				maxIndex = indicies[i];
			}
		}

		প্রত্যাগম maxIndex;
	}

	স্থির constexpr size_t indexAt(size_t i)
	{
		constexpr size_t indicies[] {Idxs...};
		প্রত্যাগম indicies[i];
	}

	static_assert(!duplicateIndices(), "Duplicate indices");
	static_assert(N < M, "More entries than elements, use a dense vector");
	static_assert(N > 0, "A sparse vector needs at least one element");
	static_assert(findMaxIndex() < M, "Largest entry doesn't fit in sparse vector");

	Type _data[N] {};

	স্থির constexpr পূর্ণাঙ্ক findCompressedIndex(size_t index)
	{
		constexpr size_t indicies[] {Idxs...};
		পূর্ণাঙ্ক compressedIndex = -1;

		প্রতি (size_t i = 0; i < N; i++) {
			যদি (index == indicies[i]) {
				compressedIndex = static_cast<পূর্ণাঙ্ক>(i);
			}
		}

		প্রত্যাগম compressedIndex;
	}

সার্বজনিক:
	constexpr size_t non_zeros() ধ্রুব
	{
		প্রত্যাগম N;
	}

	constexpr size_t index(size_t i) ধ্রুব
	{
		প্রত্যাগম indexAt(i);
	}

	SparseVector() = স্বভাব;

	SparseVector(ধ্রুব matrix::Vector<Type, M> &data)
	{
		প্রতি (size_t i = 0; i < N; i++) {
			_data[i] = data(indexAt(i));
		}
	}

	explicit SparseVector(ধ্রুব Type data[N])
	{
		memcpy(_data, data, পরিমাণ(_data));
	}

	template <size_t i>
	অন্তঃস্থ Type at() ধ্রুব
	{
		constexpr পূর্ণাঙ্ক compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		প্রত্যাগম _data[compressed_index];
	}

	template <size_t i>
	অন্তঃস্থ Type &at()
	{
		constexpr পূর্ণাঙ্ক compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		প্রত্যাগম _data[compressed_index];
	}

	অন্তঃস্থ Type atCompressedIndex(size_t i) ধ্রুব
	{
		প্রতিজ্ঞা(i < N);
		প্রত্যাগম _data[i];
	}

	অন্তঃস্থ Type &atCompressedIndex(size_t i)
	{
		প্রতিজ্ঞা(i < N);
		প্রত্যাগম _data[i];
	}

	শূন্য setZero()
	{
		প্রতি (size_t i = 0; i < N; i++) {
			_data[i] = Type(0);
		}
	}

	Type dot(ধ্রুব matrix::Vector<Type, M> &other) ধ্রুব
	{
		Type accum(0);

		প্রতি (size_t i = 0; i < N; i++) {
			accum += _data[i] * other(indexAt(i));
		}

		প্রত্যাগম accum;
	}

	matrix::Vector<Type, M> operator+(ধ্রুব matrix::Vector<Type, M> &other) ধ্রুব
	{
		matrix::Vector<Type, M> vec = other;

		প্রতি (size_t i = 0; i < N; i++) {
			vec(indexAt(i)) +=  _data[i];
		}

		প্রত্যাগম vec;
	}

	SparseVector &operator+=(Type t)
	{
		প্রতি (size_t i = 0; i < N; i++) {
			_data[i] += t;
		}

		প্রত্যাগম *এতত্;
	}

	Type norm_squared() ধ্রুব
	{
		Type accum(0);

		প্রতি (size_t i = 0; i < N; i++) {
			accum += _data[i] * _data[i];
		}

		প্রত্যাগম accum;
	}

	Type norm() ধ্রুব
	{
		প্রত্যাগম std::sqrt(norm_squared());
	}

	তার্কিক longerThan(Type testVal) ধ্রুব
	{
		প্রত্যাগম norm_squared() > testVal * testVal;
	}
};

template<typename Type, size_t Q, size_t M, size_t ... Idxs>
matrix::Vector<Type, Q> operator*(ধ্রুব matrix::Matrix<Type, Q, M> &mat,
				  ধ্রুব matrix::SparseVector<Type, M, Idxs...> &vec)
{
	matrix::Vector<Type, Q> res;

	প্রতি (size_t i = 0; i < Q; i++) {
		ধ্রুব Vector<Type, M> row = mat.row(i);
		res(i) = vec.dot(row);
	}

	প্রত্যাগম res;
}

// returns x.T * A * x
template<typename Type, size_t M, size_t ... Idxs>
Type quadraticForm(ধ্রুব matrix::SquareMatrix<Type, M> &A, ধ্রুব matrix::SparseVector<Type, M, Idxs...> &x)
{
	Type res = Type(0);

	প্রতি (size_t i = 0; i < x.non_zeros(); i++) {
		Type tmp = Type(0);

		প্রতি (size_t j = 0; j < x.non_zeros(); j++) {
			tmp += A(x.index(i), x.index(j)) * x.atCompressedIndex(j);
		}

		res += x.atCompressedIndex(i) * tmp;
	}

	প্রত্যাগম res;
}

template<size_t M, size_t ... Idxs>
using SparseVectorf = SparseVector<ভিন্ন, M, Idxs...>;

}
