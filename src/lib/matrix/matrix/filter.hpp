#निर्देशः once

#अन्तर्भाव "Scalar.hpp"
#अन्तर्भाव "SquareMatrix.hpp"

नामाकाश matrix
{

प्रतिरूपम्<प्रकारनाम Type, size_t M, size_t N>
पूर्णाङ्क kalman_correct(
	ध्रुव Matrix<Type, M, M> &P,
	ध्रुव Matrix<Type, N, M> &C,
	ध्रुव Matrix<Type, N, N> &R,
	ध्रुव Matrix<Type, N, 1> &r,
	Matrix<Type, M, 1> &dx,
	Matrix<Type, M, M> &dP,
	Type &beta
)
{
	वर्गफलम्_Matrix<Type, N> S_I = वर्गफलम्_Matrix<Type, N>(C * P * C.T() + R).I();
	Matrix<Type, M, N> K = P * C.T() * S_I;
	dx = K * r;
	beta = Scalar<Type>(r.T() * S_I * r);
	dP = K * C * P * (-1);
	प्रत्यागम 0;
}

} // namespace matrix
