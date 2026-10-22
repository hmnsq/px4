#pragma once

#ଅନ୍ତର୍ଭାଵ "Vector.hpp"

ନାମାକାଶ matrix
{

template<typename Type, size_t M, size_t N>
ପୂର୍ଣ୍ଣସଂଖ୍ୟା integrate_rk4(
	Vector<Type, M> (*f)(Type, ଅଚଳ Matrix<Type, M, 1> &x, ଅଚଳ Matrix<Type, N, 1> &u),
	ଅଚଳ Matrix<Type, M, 1> &y0,
	ଅଚଳ Matrix<Type, N, 1> &u,
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

	ଯଦି (tf < t0) { ଫେରନ୍ତୁ -1; } // make sure t1 > t0

	ଯେତେବେଳେ (t1 < tf) {
		ଯଦି (t1 + h0 < tf) {
			h = h0;

		} ଅନ୍ୟଥା {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	ଫେରନ୍ତୁ 0;
}

} // namespace matrix
