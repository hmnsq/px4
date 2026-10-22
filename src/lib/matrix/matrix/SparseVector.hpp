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

#ଅନ୍ତର୍ଭାଵ "Vector.hpp"

ନାମାକାଶ matrix
{
// Vector that only store nonzero elements,
// which indices are specified as parameter pack
template<typename Type, size_t M, size_t... Idxs>
ଶ୍ରେଣୀ SparseVector
{
ବ୍ୟକ୍ତିଗତ:
	ଗଣନା : size_t { N = ଆକାର...(Idxs) };

	ସ୍ଥିର constexpr ତାର୍କିକ duplicateIndices()
	{
		constexpr size_t indicies[] {Idxs...};

		ପାଇଁ (size_t i = 0; i < N; i++) {
			ପାଇଁ (size_t j = 0; j < i; j++) {
				ଯଦି (indicies[i] == indicies[j]) {
					ଫେରନ୍ତୁ ସତ୍ୟ;
				}
			}
		}

		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ସ୍ଥିର constexpr size_t findMaxIndex()
	{
		constexpr size_t indicies[] {Idxs...};
		size_t maxIndex = 0;

		ପାଇଁ (size_t i = 0; i < N; i++) {
			ଯଦି (maxIndex < indicies[i]) {
				maxIndex = indicies[i];
			}
		}

		ଫେରନ୍ତୁ maxIndex;
	}

	ସ୍ଥିର constexpr size_t indexAt(size_t i)
	{
		constexpr size_t indicies[] {Idxs...};
		ଫେରନ୍ତୁ indicies[i];
	}

	static_assert(!duplicateIndices(), "Duplicate indices");
	static_assert(N < M, "More entries than elements, use a dense vector");
	static_assert(N > 0, "A sparse vector needs at least one element");
	static_assert(findMaxIndex() < M, "Largest entry doesn't fit in sparse vector");

	Type _data[N] {};

	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା findCompressedIndex(size_t index)
	{
		constexpr size_t indicies[] {Idxs...};
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressedIndex = -1;

		ପାଇଁ (size_t i = 0; i < N; i++) {
			ଯଦି (index == indicies[i]) {
				compressedIndex = static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(i);
			}
		}

		ଫେରନ୍ତୁ compressedIndex;
	}

ସର୍ବସାଧାରଣ:
	constexpr size_t non_zeros() ଅଚଳ
	{
		ଫେରନ୍ତୁ N;
	}

	constexpr size_t index(size_t i) ଅଚଳ
	{
		ଫେରନ୍ତୁ indexAt(i);
	}

	SparseVector() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	SparseVector(ଅଚଳ matrix::Vector<Type, M> &data)
	{
		ପାଇଁ (size_t i = 0; i < N; i++) {
			_data[i] = data(indexAt(i));
		}
	}

	explicit SparseVector(ଅଚଳ Type data[N])
	{
		memcpy(_data, data, ଆକାର(_data));
	}

	template <size_t i>
	ଧାଡ଼ିଭିତରେ Type at() ଅଚଳ
	{
		constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		ଫେରନ୍ତୁ _data[compressed_index];
	}

	template <size_t i>
	ଧାଡ଼ିଭିତରେ Type &at()
	{
		constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressed_index = findCompressedIndex(i);
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		ଫେରନ୍ତୁ _data[compressed_index];
	}

	ଧାଡ଼ିଭିତରେ Type atCompressedIndex(size_t i) ଅଚଳ
	{
		ପ୍ରତିଜ୍ଞା(i < N);
		ଫେରନ୍ତୁ _data[i];
	}

	ଧାଡ଼ିଭିତରେ Type &atCompressedIndex(size_t i)
	{
		ପ୍ରତିଜ୍ଞା(i < N);
		ଫେରନ୍ତୁ _data[i];
	}

	ଶୂନ୍ୟ setZero()
	{
		ପାଇଁ (size_t i = 0; i < N; i++) {
			_data[i] = Type(0);
		}
	}

	Type dot(ଅଚଳ matrix::Vector<Type, M> &other) ଅଚଳ
	{
		Type accum(0);

		ପାଇଁ (size_t i = 0; i < N; i++) {
			accum += _data[i] * other(indexAt(i));
		}

		ଫେରନ୍ତୁ accum;
	}

	matrix::Vector<Type, M> operator+(ଅଚଳ matrix::Vector<Type, M> &other) ଅଚଳ
	{
		matrix::Vector<Type, M> vec = other;

		ପାଇଁ (size_t i = 0; i < N; i++) {
			vec(indexAt(i)) +=  _data[i];
		}

		ଫେରନ୍ତୁ vec;
	}

	SparseVector &operator+=(Type t)
	{
		ପାଇଁ (size_t i = 0; i < N; i++) {
			_data[i] += t;
		}

		ଫେରନ୍ତୁ *ଏହି;
	}

	Type norm_squared() ଅଚଳ
	{
		Type accum(0);

		ପାଇଁ (size_t i = 0; i < N; i++) {
			accum += _data[i] * _data[i];
		}

		ଫେରନ୍ତୁ accum;
	}

	Type norm() ଅଚଳ
	{
		ଫେରନ୍ତୁ std::sqrt(norm_squared());
	}

	ତାର୍କିକ longerThan(Type testVal) ଅଚଳ
	{
		ଫେରନ୍ତୁ norm_squared() > testVal * testVal;
	}
};

template<typename Type, size_t Q, size_t M, size_t ... Idxs>
matrix::Vector<Type, Q> operator*(ଅଚଳ matrix::Matrix<Type, Q, M> &mat,
				  ଅଚଳ matrix::SparseVector<Type, M, Idxs...> &vec)
{
	matrix::Vector<Type, Q> res;

	ପାଇଁ (size_t i = 0; i < Q; i++) {
		ଅଚଳ Vector<Type, M> row = mat.row(i);
		res(i) = vec.dot(row);
	}

	ଫେରନ୍ତୁ res;
}

// returns x.T * A * x
template<typename Type, size_t M, size_t ... Idxs>
Type quadraticForm(ଅଚଳ matrix::SquareMatrix<Type, M> &A, ଅଚଳ matrix::SparseVector<Type, M, Idxs...> &x)
{
	Type res = Type(0);

	ପାଇଁ (size_t i = 0; i < x.non_zeros(); i++) {
		Type tmp = Type(0);

		ପାଇଁ (size_t j = 0; j < x.non_zeros(); j++) {
			tmp += A(x.index(i), x.index(j)) * x.atCompressedIndex(j);
		}

		res += x.atCompressedIndex(i) * tmp;
	}

	ଫେରନ୍ତୁ res;
}

template<size_t M, size_t ... Idxs>
using SparseVectorf = SparseVector<ଦଶମିକ, M, Idxs...>;

}
