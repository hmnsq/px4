#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ ಮಾತೃಕೆ
{

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು Type, size_t M, size_t N>
ಪೂರ್ಣಾಂಕ integrate_rk4(
	ಸದಿಶ<Type, M> (*f)(Type, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<Type, M, 1> &x, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<Type, N, 1> &u),
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<Type, M, 1> &y0,
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<Type, N, 1> &u,
	Type t0,
	Type tf,
	Type h0,
	ಮಾತೃಕೆ_2<Type, M, 1> &y1
)
{
	// https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
	Type t1 = t0;
	y1 = y0;
	Type h = h0;
	ಸದಿಶ<Type, M> k1, k2, k3, k4;

	ಆದರೆ (tf < t0) { ಹಿಂತಿರುಗಿ -1; } // make sure t1 > t0

	ಆಗ (t1 < tf) {
		ಆದರೆ (t1 + h0 < tf) {
			h = h0;

		} ಇಲ್ಲದಿದ್ದರೆ {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	ಹಿಂತಿರುಗಿ 0;
}

} // namespace matrix
