#pragma once

#અન્તર્ભાવ "Vector.hpp"

નામાકાશ matrix
{

template<typename Type, size_t M, size_t N>
પૂર્ણાંક integrate_rk4(
	Vector<Type, M> (*f)(Type, અચલ Matrix<Type, M, 1> &x, અચલ Matrix<Type, N, 1> &u),
	અચલ Matrix<Type, M, 1> &y0,
	અચલ Matrix<Type, N, 1> &u,
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

	જો (tf < t0) { પાછા -1; } // make sure t1 > t0

	જ્યારે (t1 < tf) {
		જો (t1 + h0 < tf) {
			h = h0;

		} નહીંતર {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	પાછા 0;
}

} // namespace matrix
