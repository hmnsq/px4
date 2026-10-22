#pragma once

#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

template<typename Type, size_t M, size_t N>
पूर्णाङ्क integrate_rk4(
	Vector<Type, M> (*f)(Type, ध्रुव Matrix<Type, M, 1> &x, ध्रुव Matrix<Type, N, 1> &u),
	ध्रुव Matrix<Type, M, 1> &y0,
	ध्रुव Matrix<Type, N, 1> &u,
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

	जइ (tf < t0) { प्रत्यागम -1; } // make sure t1 > t0

	जाव (t1 < tf) {
		जइ (t1 + h0 < tf) {
			h = h0;

		} अण्णहा {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	प्रत्यागम 0;
}

} // namespace matrix
