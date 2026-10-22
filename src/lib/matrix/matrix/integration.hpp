#رهنمود once

#ضمن "Vector.hpp"

نطاق ماتریس
{

قالب<نام_نوع Type, size_t M, size_t N>
صحيح integrate_rk4(
	وکتور<Type, M> (*f)(Type, ثابت ماتریس_2<Type, M, 1> &x, ثابت ماتریس_2<Type, N, 1> &u),
	ثابت ماتریس_2<Type, M, 1> &y0,
	ثابت ماتریس_2<Type, N, 1> &u,
	Type t0,
	Type tf,
	Type h0,
	ماتریس_2<Type, M, 1> &y1
)
{
	// https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
	Type t1 = t0;
	y1 = y0;
	Type h = h0;
	وکتور<Type, M> k1, k2, k3, k4;

	إذا (tf < t0) { أرجع -1; } // make sure t1 > t0

	طالما (t1 < tf) {
		إذا (t1 + h0 < tf) {
			h = h0;

		} وإلا {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	أرجع 0;
}

} // namespace matrix
