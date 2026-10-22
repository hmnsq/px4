/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#निर्देशः once

#अन्तर्भाव <cmath>
#अन्तर्भाव <cstdio>
#अन्तर्भाव <cstring>

#अन्तर्भाव "helper_functions.hpp"
#अन्तर्भाव "Slice.hpp"

नामाकाश matrix
{

प्रतिरूपम्<प्रकारनाम Type, size_t M, size_t N>
वर्ग Matrix
{
	Type दत्त_3[M][N] {};

सार्वजनिक:

	// Constructors
	Matrix() = स्वभाव;

	स्पष्ट Matrix(ध्रुव Type दत्त_2[M * N])
	{
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				दत्त_3[i][j] = दत्त_2[N * i + j];
			}
		}
	}

	स्पष्ट Matrix(ध्रुव Type दत्त_2[M][N])
	{
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				दत्त_3[i][j] = दत्त_2[i][j];
			}
		}
	}

	Matrix(ध्रुव Matrix &other)
	{
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				दत्त_3[i][j] = other(i, j);
			}
		}
	}

	प्रतिरूपम्<प्रकारनाम S>
	Matrix(ध्रुव Matrix<S, M, N> &aa)
	{
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				दत्त_3[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	प्रतिरूपम्<size_t P, size_t Q>
	Matrix(ध्रुव खण्डिका_2<Type, M, N, P, Q> &अन्तर्_खण्डिका)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) = अन्तर्_खण्डिका(i, j);
			}
		}
	}

	प्रतिरूपम्<size_t P, size_t Q>
	Matrix(ध्रुव Const_खण्डिका<Type, M, N, P, Q> &अन्तर्_खण्डिका)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) = अन्तर्_खण्डिका(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	अन्तःस्थ ध्रुव Type &संक्रियकः()(size_t i, size_t j) ध्रुव
	{
		प्रतिज्ञा(i < M);
		प्रतिज्ञा(j < N);

		प्रत्यागम दत्त_3[i][j];
	}

	अन्तःस्थ Type &संक्रियकः()(size_t i, size_t j)
	{
		प्रतिज्ञा(i < M);
		प्रतिज्ञा(j < N);

		प्रत्यागम दत्त_3[i][j];
	}

	Matrix<Type, M, N> &संक्रियकः=(ध्रुव Matrix<Type, M, N> &other)
	{
		यदि (एतत् != &other) {
			Matrix<Type, M, N> &self = *एतत्;

			प्रति (size_t i = 0; i < M; i++) {
				प्रति (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		प्रत्यागम (*एतत्);
	}

	शून्य copy_गन्तव्यम्(Type dst[M * N]) ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	शून्य copy_गन्तव्यम्_Column_Major(Type dst[M * N]) ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	/**
	 * Matrix Operations
	 */

	// this might use a lot of programming memory
	// since it instantiates a class for every
	// required mult pair, but it provides
	// compile time size_t checking
	प्रतिरूपम्<size_t P>
	Matrix<Type, M, P> संक्रियकः*(ध्रुव Matrix<Type, N, P> &other) ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;
		Matrix<Type, M, P> res{};

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t k = 0; k < P; k++) {
				प्रति (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		प्रत्यागम res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T
	प्रतिरूपम्<size_t P>
	Matrix<Type, M, M> multiplyByTranspose(ध्रुव Matrix<Type, P, N> &other) ध्रुव
	{
		Matrix<Type, M, P> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t k = 0; k < P; k++) {
				प्रति (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		प्रत्यागम res;
	}

	// Element-wise multiplication
	Matrix<Type, M, N> emult(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		प्रत्यागम res;
	}

	// Element-wise division
	Matrix<Type, M, N> edivide(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		प्रत्यागम res;
	}

	Matrix<Type, M, N> संक्रियकः+(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		प्रत्यागम res;
	}

	Matrix<Type, M, N> संक्रियकः-(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		प्रत्यागम res;
	}

	// unary minus
	Matrix<Type, M, N> संक्रियकः-() ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		प्रत्यागम res;
	}

	शून्य संक्रियकः+=(ध्रुव Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	शून्य संक्रियकः-=(ध्रुव Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	प्रतिरूपम्<size_t P>
	शून्य संक्रियकः*=(ध्रुव Matrix<Type, N, P> &other)
	{
		Matrix<Type, M, N> &self = *एतत्;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	Matrix<Type, M, N> संक्रियकः*(Type scalar) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		प्रत्यागम res;
	}

	अन्तःस्थ Matrix<Type, M, N> संक्रियकः/(Type scalar) ध्रुव
	{
		प्रत्यागम (*एतत्) * (1 / scalar);
	}

	Matrix<Type, M, N> संक्रियकः+(Type scalar) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		प्रत्यागम res;
	}

	अन्तःस्थ Matrix<Type, M, N> संक्रियकः-(Type scalar) ध्रुव
	{
		प्रत्यागम (*एतत्) + (-1 * scalar);
	}

	शून्य संक्रियकः*=(Type scalar)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	शून्य संक्रियकः/=(Type scalar)
	{
		Matrix<Type, M, N> &self = *एतत्;
		self *= (Type(1) / scalar);
	}

	अन्तःस्थ शून्य संक्रियकः+=(Type scalar)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	अन्तःस्थ शून्य संक्रियकः-=(Type scalar)
	{
		Matrix<Type, M, N> &self = *एतत्;
		self += (-scalar);
	}

	तार्किक संक्रियकः==(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		प्रत्यागम isEqual(*एतत्, other);
	}

	तार्किक संक्रियकः!=(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;
		प्रत्यागम !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	शून्य write_string(वर्ण *buf, size_t n) ध्रुव
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				snprintf(buf + वाक्यदैर्घ्यम्(buf), n - वाक्यदैर्घ्यम्(buf), "\t%8.8g", द्विगुण(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + वाक्यदैर्घ्यम्(buf), n - वाक्यदैर्घ्यम्(buf), "\n");
		}
	}

	शून्य print(भिन्न eps = 1e-9) ध्रुव
	{
		// print column numbering
		यदि (N > 1) {
			प्रारूप_लिख("  ");

			प्रति (अचिह्नित i = 0; i < N; i++) {
				प्रारूप_लिख("|%2u      ", i);

			}

			प्रारूप_लिख("\n");
		}

		ध्रुव Matrix<Type, M, N> &self = *एतत्;
		तार्किक is_पूर्व_सममितम् = सत्य; // assume symmetric until one element is not

		प्रति (अचिह्नित i = 0; i < M; i++) {
			प्रारूप_लिख("%2u|", i); // print row numbering

			प्रति (अचिह्नित j = 0; j < N; j++) {
				द्विगुण d = static_cast<द्विगुण>(self(i, j));

				// if symmetric don't print upper triangular elements
				यदि (is_पूर्व_सममितम् && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<द्विगुण>(self(j, i))) < (द्विगुण)eps)
				   ) {
					// print empty space
					प्रारूप_लिख("         ");

				} अन्यथा {
					// avoid -0.0 for display
					यदि (fabs(d - 0.0) < (द्विगुण)eps) {
						// print fixed width zero
						प्रारूप_लिख(" 0       ");

					} अन्यथा यदि ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						प्रारूप_लिख("% .1e ", d);

					} अन्यथा {
						प्रारूप_लिख("% 6.5f ", d);
					}

					is_पूर्व_सममितम् = असत्य; // not symmetric if once inside here
				}
			}

			प्रारूप_लिख("\n");
		}
	}

	Matrix<Type, N, M> transpose() ध्रुव
	{
		Matrix<Type, N, M> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		प्रत्यागम res;
	}

	// tranpose alias
	अन्तःस्थ Matrix<Type, N, M> T() ध्रुव
	{
		प्रत्यागम transpose();
	}

	प्रतिरूपम्<size_t P, size_t Q>
	Const_खण्डिका<Type, P, Q, M, N> खण्डिका(size_t x0, size_t y0) ध्रुव
	{
		प्रत्यागम {x0, y0, एतत्};
	}

	प्रतिरूपम्<size_t P, size_t Q>
	खण्डिका_2<Type, P, Q, M, N> खण्डिका(size_t x0, size_t y0)
	{
		प्रत्यागम {x0, y0, एतत्};
	}

	Const_खण्डिका<Type, 1, N, M, N> row(size_t i) ध्रुव
	{
		प्रत्यागम खण्डिका<1, N>(i, 0);
	}

	खण्डिका_2<Type, 1, N, M, N> row(size_t i)
	{
		प्रत्यागम खण्डिका<1, N>(i, 0);
	}

	Const_खण्डिका<Type, M, 1, M, N> col(size_t j) ध्रुव
	{
		प्रत्यागम खण्डिका<M, 1>(0, j);
	}

	खण्डिका_2<Type, M, 1, M, N> col(size_t j)
	{
		प्रत्यागम खण्डिका<M, 1>(0, j);
	}

	शून्य समुच्चयः_Row(size_t i, ध्रुव Matrix<Type, N, 1> &row_अन्तर्)
	{
		खण्डिका<1, N>(i, 0) = row_अन्तर्.transpose();
	}

	शून्य समुच्चयः_Row(size_t i, Type val)
	{
		खण्डिका<1, N>(i, 0) = val;
	}

	शून्य समुच्चयः_Col(size_t j, ध्रुव Matrix<Type, M, 1> &column)
	{
		खण्डिका<M, 1>(0, j) = column;
	}

	शून्य समुच्चयः_Col(size_t j, Type val)
	{
		खण्डिका<M, 1>(0, j) = val;
	}

	शून्य समुच्चयः_Zero()
	{
		स्मृतिं_पूरय(दत्त_3, 0, परिमाण(दत्त_3));
	}

	अन्तःस्थ शून्य zero()
	{
		समुच्चयः_Zero();
	}

	शून्य समुच्चयः_All(Type val)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	अन्तःस्थ शून्य समुच्चयः_One()
	{
		समुच्चयः_All(1);
	}

	अन्तःस्थ शून्य समुच्चयः_Na_N()
	{
		समुच्चयः_All(NAN);
	}

	शून्य समुच्चयः_तटस्थः()
	{
		समुच्चयः_Zero();
		Matrix<Type, M, N> &self = *एतत्;

		ध्रुव size_t min_i = M > N ? N : M;

		प्रति (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	अन्तःस्थ शून्य तटस्थः()
	{
		समुच्चयः_तटस्थः();
	}

	अन्तःस्थ शून्य swap_पङ्क्तिकाः(size_t a, size_t b)
	{
		प्रतिज्ञा(a < M);
		प्रतिज्ञा(b < M);

		यदि (a == b) {
			प्रत्यागम;
		}

		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	अन्तःस्थ शून्य swap_स्तम्भसङ्ख्या(size_t a, size_t b)
	{
		प्रतिज्ञा(a < N);
		प्रतिज्ञा(b < N);

		यदि (a == b) {
			प्रत्यागम;
		}

		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	अन्तःस्थ शून्य nan_गन्तव्यम्_Zero()
	{

		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				यदि (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	Matrix<Type, M, N> निरपेक्षम्() ध्रुव
	{
		Matrix<Type, M, N> r;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*एतत्)(i, j)));
			}
		}

		प्रत्यागम r;
	}

	Type max() ध्रुव
	{
		Type max_val = (*एतत्)(0, 0);

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
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

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				Type val = (*एतत्)(i, j);

				यदि (val < min_val) {
					min_val = val;
				}
			}
		}

		प्रत्यागम min_val;
	}

	तार्किक isAllNan() ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;
		तार्किक परिणाम = सत्य;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				परिणाम = परिणाम && std::isnan(self(i, j));
			}
		}

		प्रत्यागम परिणाम;
	}

	तार्किक isAllFinite() ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				यदि (!std::isfinite(self(i, j))) {
					प्रत्यागम असत्य;
				}
			}
		}

		प्रत्यागम सत्य;
	}
};

प्रतिरूपम्<प्रकारनाम Type, size_t M, size_t N>
Matrix<Type, M, N> zeros()
{
	Matrix<Type, M, N> m;
	m.समुच्चयः_Zero();
	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t M, size_t N>
Matrix<Type, M, N> ones()
{
	Matrix<Type, M, N> m;
	m.समुच्चयः_One();
	प्रत्यागम m;
}

प्रतिरूपम्<size_t M, size_t N>
Matrix<भिन्न, M, N> nans()
{
	Matrix<भिन्न, M, N> m;
	m.समुच्चयः_Na_N();
	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> संक्रियकः*(Type scalar, ध्रुव Matrix<Type, M, N> &other)
{
	प्रत्यागम other * scalar;
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
तार्किक isEqual(ध्रुव Matrix<Type, M, N> &x,
	     ध्रुव Matrix<Type, M, N> &y, ध्रुव Type eps = Type(1e-4f))
{
	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			यदि (!isEqualF(x(i, j), y(i, j), eps)) {
				प्रत्यागम असत्य;
			}
		}
	}

	प्रत्यागम सत्य;
}

नामाकाश typeFunction
{
प्रतिरूपम्<प्रकारनाम Type>
Type min(ध्रुव Type x, ध्रुव Type y)
{
	तार्किक x_is_nan = std::isnan(x);
	तार्किक y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	यदि (x_is_nan || y_is_nan) {
		यदि (x_is_nan && !y_is_nan) {
			प्रत्यागम y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		प्रत्यागम x;
	}

	प्रत्यागम (x < y) ? x : y;
}

प्रतिरूपम्<प्रकारनाम Type>
Type max(ध्रुव Type x, ध्रुव Type y)
{
	तार्किक x_is_nan = std::isnan(x);
	तार्किक y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	यदि (x_is_nan || y_is_nan) {
		यदि (x_is_nan && !y_is_nan) {
			प्रत्यागम y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		प्रत्यागम x;
	}

	प्रत्यागम (x > y) ? x : y;
}

प्रतिरूपम्<प्रकारनाम Type>
Type constrain(ध्रुव Type x, ध्रुव Type अधस्तनम्_bound, ध्रुव Type ऊर्ध्वस्थम्_bound)
{
	यदि (अधस्तनम्_bound > ऊर्ध्वस्थम्_bound) {
		प्रत्यागम NAN;

	} अन्यथा यदि (std::isnan(x)) {
		प्रत्यागम NAN;

	} अन्यथा {
		प्रत्यागम typeFunction::max(अधस्तनम्_bound, typeFunction::min(ऊर्ध्वस्थम्_bound, x));
	}
}
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ध्रुव Matrix<Type, M, N> &x, ध्रुव Type scalar_ऊर्ध्वस्थम्_bound)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_ऊर्ध्वस्थम्_bound);
		}
	}

	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ध्रुव Type scalar_ऊर्ध्वस्थम्_bound, ध्रुव Matrix<Type, M, N> &x)
{
	प्रत्यागम min(x, scalar_ऊर्ध्वस्थम्_bound);
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ध्रुव Matrix<Type, M, N> &x1, ध्रुव Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ध्रुव Matrix<Type, M, N> &x, ध्रुव Type scalar_अधस्तनम्_bound)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_अधस्तनम्_bound);
		}
	}

	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ध्रुव Type scalar_अधस्तनम्_bound, ध्रुव Matrix<Type, M, N> &x)
{
	प्रत्यागम max(x, scalar_अधस्तनम्_bound);
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ध्रुव Matrix<Type, M, N> &x1, ध्रुव Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(ध्रुव Matrix<Type, M, N> &x,
			     ध्रुव Type scalar_अधस्तनम्_bound,
			     ध्रुव Type scalar_ऊर्ध्वस्थम्_bound)
{
	Matrix<Type, M, N> m;

	यदि (scalar_अधस्तनम्_bound > scalar_ऊर्ध्वस्थम्_bound) {
		m.समुच्चयः_Na_N();

	} अन्यथा {
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_अधस्तनम्_bound, scalar_ऊर्ध्वस्थम्_bound);
			}
		}
	}

	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(ध्रुव Matrix<Type, M, N> &x,
			     ध्रुव Matrix<Type, M, N> &x_अधस्तनम्_bound,
			     ध्रुव Matrix<Type, M, N> &x_ऊर्ध्वस्थम्_bound)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_अधस्तनम्_bound(i, j), x_ऊर्ध्वस्थम्_bound(i, j));
		}
	}

	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम OStream, प्रकारनाम Type, size_t M, size_t N>
OStream &संक्रियकः<<(OStream &os, ध्रुव matrix::Matrix<Type, M, N> &matrix)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	वर्ण string[n];
	matrix.write_string(string, n);
	os << string;
	प्रत्यागम os;
}

} // namespace matrix
