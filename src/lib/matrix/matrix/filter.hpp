#ہدایت once

#شامل "Scalar.hpp"
#شامل "SquareMatrix.hpp"

نامخلا میٹرکس
{

سانچہ<قسم_نام Type, size_t M, size_t N>
صحیح kalman_correct(
	ثابت میٹرکس_2<Type, M, M> &P,
	ثابت میٹرکس_2<Type, N, M> &C,
	ثابت میٹرکس_2<Type, N, N> &R,
	ثابت میٹرکس_2<Type, N, 1> &r,
	میٹرکس_2<Type, M, 1> &dx,
	میٹرکس_2<Type, M, M> &dP,
	Type &beta
)
{
	مربع_میٹرکس<Type, N> S_I = مربع_میٹرکس<Type, N>(C * P * C.T() + R).I();
	میٹرکس_2<Type, M, N> K = P * C.T() * S_I;
	dx = K * r;
	beta = Scalar<Type>(r.T() * S_I * r);
	dP = K * C * P * (-1);
	واپس 0;
}

} // namespace matrix
