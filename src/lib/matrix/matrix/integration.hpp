#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "Vector.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t M, size_t N>
ਪੂਰਨ integrate_rk4(
	ਸਦਿਸ਼<Type, M> (*f)(Type, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1> &x, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, N, 1> &u),
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1> &y0,
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, N, 1> &u,
	Type t0,
	Type tf,
	Type h0,
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1> &y1
)
{
	// https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
	Type t1 = t0;
	y1 = y0;
	Type h = h0;
	ਸਦਿਸ਼<Type, M> k1, k2, k3, k4;

	ਜੇ (tf < t0) { ਵਾਪਸ -1; } // make sure t1 > t0

	ਜਦੋਂ (t1 < tf) {
		ਜੇ (t1 + h0 < tf) {
			h = h0;

		} ਨਹੀਂਤਾਂ {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	ਵਾਪਸ 0;
}

} // namespace matrix
