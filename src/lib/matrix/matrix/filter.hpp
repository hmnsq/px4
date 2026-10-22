#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "Scalar.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "SquareMatrix.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t M, size_t N>
ਪੂਰਨ kalman_correct(
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, M> &P,
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, N, M> &C,
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, N, N> &R,
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, N, 1> &r,
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1> &dx,
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, M> &dP,
	Type &beta
)
{
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<Type, N> S_I = ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<Type, N>(C * P * C.T() + R).I();
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> K = P * C.T() * S_I;
	dx = K * r;
	beta = Scalar<Type>(r.T() * S_I * r);
	dP = K * C * P * (-1);
	ਵਾਪਸ 0;
}

} // namespace matrix
