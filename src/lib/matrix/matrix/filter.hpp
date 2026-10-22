#لارښوونه once

#ضمن "Scalar.hpp"
#ضمن "SquareMatrix.hpp"

نطاق ماتریکس
{

کينډۍ<ډول_نوم Type, size_t M, size_t N>
صحيح kalman_correct(
	ثابت ماتریکس_2<Type, M, M> &P,
	ثابت ماتریکس_2<Type, N, M> &C,
	ثابت ماتریکس_2<Type, N, N> &R,
	ثابت ماتریکس_2<Type, N, 1> &r,
	ماتریکس_2<Type, M, 1> &dx,
	ماتریکس_2<Type, M, M> &dP,
	Type &beta
)
{
	مربع_ماتریکس<Type, N> S_I = مربع_ماتریکس<Type, N>(C * P * C.T() + R).I();
	ماتریکس_2<Type, M, N> K = P * C.T() * S_I;
	dx = K * r;
	beta = Scalar<Type>(r.T() * S_I * r);
	dP = K * C * P * (-1);
	أرجع 0;
}

} // namespace matrix
