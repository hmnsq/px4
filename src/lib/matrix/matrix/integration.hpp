#pragma once

#ضمن "Vector.hpp"

نطاق matrix
{

template<typename Type, size_t M, size_t N>
صحيح integrate_rk4(
	Vector<Type, M> (*f)(Type, ثابت Matrix<Type, M, 1> &x, ثابت Matrix<Type, N, 1> &u),
	ثابت Matrix<Type, M, 1> &y0,
	ثابت Matrix<Type, N, 1> &u,
	Type t0,
	Type tf,
	Type h0,
	Matrix<Type, M, 1> &y1
)
{
	// https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
	Type t1 = t0;
	y1 = y0;
	Type h = h0;
	Vector<Type, M> k1, k2, k3, k4;

	אם (tf < t0) { החזר -1; } // make sure t1 > t0

	כלעוד (t1 < tf) {
		אם (t1 + h0 < tf) {
			h = h0;

		} אחרת {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	החזר 0;
}

} // namespace matrix
