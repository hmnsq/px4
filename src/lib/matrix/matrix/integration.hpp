#pragma once

#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ matrix
{

template<typename Type, size_t M, size_t N>
ಪೂರ್ಣಾಙ್ಕ integrate_rk4(
	Vector<Type, M> (*f)(Type, ಧ್ರುವ Matrix<Type, M, 1> &x, ಧ್ರುವ Matrix<Type, N, 1> &u),
	ಧ್ರುವ Matrix<Type, M, 1> &y0,
	ಧ್ರುವ Matrix<Type, N, 1> &u,
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

	ಯದಿ (tf < t0) { ಪ್ರತ್ಯಾಗಮ -1; } // make sure t1 > t0

	ಯಾವತ್ (t1 < tf) {
		ಯದಿ (t1 + h0 < tf) {
			h = h0;

		} ಅನ್ಯಥಾ {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

} // namespace matrix
