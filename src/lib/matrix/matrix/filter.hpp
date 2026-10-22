#رهنمود once

#ضمن "Scalar.hpp"
#ضمن "SquareMatrix.hpp"

نطاق ماتریس
{

قالب<نام_نوع Type, size_t M, size_t N>
صحيح kalman_correct(
	ثابت ماتریس_2<Type, M, M> &P,
	ثابت ماتریس_2<Type, N, M> &C,
	ثابت ماتریس_2<Type, N, N> &R,
	ثابت ماتریس_2<Type, N, 1> &r,
	ماتریس_2<Type, M, 1> &dx,
	ماتریس_2<Type, M, M> &dP,
	Type &beta
)
{
	مربع_ماتریس<Type, N> S_I = مربع_ماتریس<Type, N>(C * P * C.T() + R).I();
	ماتریس_2<Type, M, N> K = P * C.T() * S_I;
	dx = K * r;
	beta = Scalar<Type>(r.T() * S_I * r);
	dP = K * C * P * (-1);
	أرجع 0;
}

} // namespace matrix
