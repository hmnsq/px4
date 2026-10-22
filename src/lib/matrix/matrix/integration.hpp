#సూచన once

#అన్తర్భావ "Vector.hpp"

నామాకాశ మాతృక
{

మూస<రకం_పేరు Type, size_t M, size_t N>
పూర్ణాంకం integrate_rk4(
	సదిశ<Type, M> (*f)(Type, స్థిరరాశి మాతృక_2<Type, M, 1> &x, స్థిరరాశి మాతృక_2<Type, N, 1> &u),
	స్థిరరాశి మాతృక_2<Type, M, 1> &y0,
	స్థిరరాశి మాతృక_2<Type, N, 1> &u,
	Type t0,
	Type tf,
	Type h0,
	మాతృక_2<Type, M, 1> &y1
)
{
	// https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
	Type t1 = t0;
	y1 = y0;
	Type h = h0;
	సదిశ<Type, M> k1, k2, k3, k4;

	ఒకవేళ (tf < t0) { తిరిగి -1; } // make sure t1 > t0

	అయితే (t1 < tf) {
		ఒకవేళ (t1 + h0 < tf) {
			h = h0;

		} లేకపోతే {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	తిరిగి 0;
}

} // namespace matrix
