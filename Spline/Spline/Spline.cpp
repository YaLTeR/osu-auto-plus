// Spline.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Spline.h"

#include "matrix.hpp"

Matrix gauss_with_main(Matrix A, Matrix f)
{
	if (f.GetWidth() != 1)
		throw std::logic_error("gauss(): f width != 1");
	if (A.GetHeight() != f.GetHeight())
		throw std::logic_error("gauss(): A height != f height");
	if (A.GetWidth() != A.GetHeight())
		throw std::logic_error("gauss(): A width != A height");

	Matrix x(1, f.GetHeight());

	for (size_t s = 0; s < A.GetWidth() - 1; ++s) {
		double m = 0;
		size_t m_i = 0;
		for (size_t i = s; i < A.GetHeight(); ++i) {
			auto t = std::abs(A(i, s));
			if (t > m) {
				m = t;
				m_i = i;
			}
		}
		if (m == 0) {
			throw std::logic_error("gauss(): |A| = 0");
		}
		A.SwapRows(m_i, s);
		f.SwapRows(m_i, s);

		for (size_t i = s + 1; i < A.GetHeight(); ++i) {
			double k = A(i, s) / A(s, s);
			for (size_t j = s; j < A.GetWidth(); ++j) {
				A(i, j) -= A(s, j) * k;
			}
			f(i, 0) -= f(s, 0) * k;
		}
		double k = A(s, s);
		A(s, s) = 1;
		for (size_t i = s + 1; i < A.GetWidth(); ++i) {
			A(s, i) /= k;
		}
		f(s, 0) /= k;
	}

	for (size_t s = A.GetHeight(); s > 0; --s) {
		auto temp = f(s - 1, 0);
		for (size_t i = A.GetWidth() - 1; i > s - 1; --i) {
			temp -= A(s - 1, i) * x(i, 0);
		}
		x(s - 1, 0) = temp / A(s - 1, s - 1);
	}

	return x;
}

struct Spline {
	std::vector<double> ts;
	std::vector<double> xs;
	std::vector<double> ys;
};

SPLINE_API void spline_interp(double step, double d1_x, double d1_y, double dn_x, double dn_y, double* ts, double* xs, double* ys, uint32_t n, struct spline_points* result)
{
	assert(n >= 2);

	// In our calculations t0 == 0. Determine the offset and subtract it.
	const auto t_offset = ts[0];

	for (size_t i = n - 1; i > 0; --i)
		ts[i] -= ts[i - 1];

	ts[0] = 0.0;

	// Calculate the derivatives in intermediate points.
	Matrix m(n, n);
	m(0, 0) = 1.0;
	for (size_t i = 1; i < n - 1; ++i) {
		m(i, i - 1) = ts[i + 1];
		m(i, i) = 2 * (ts[i] + ts[i + 1]);
		m(i, i + 1) = ts[i];
	}
	m(n - 1, n - 1) = 1;

	Matrix R_x(1, n), R_y(1, n);
	R_x(0, 0) = d1_x;
	R_y(0, 0) = d1_y;
	for (size_t i = 1; i < n - 1; ++i) {
		R_x(i, 0) = (3 / (ts[i] * ts[i + 1])) * (ts[i] * ts[i] * (xs[i + 1] - xs[i]) + ts[i + 1] * ts[i + 1] * (xs[i] - xs[i - 1]));
		R_y(i, 0) = (3 / (ts[i] * ts[i + 1])) * (ts[i] * ts[i] * (ys[i + 1] - ys[i]) + ts[i + 1] * ts[i + 1] * (ys[i] - ys[i - 1]));
	}
	R_x(n - 1, 0) = dn_x;
	R_y(n - 1, 0) = dn_y;

	const auto D_x = gauss_with_main(m, R_x);
	const auto D_y = gauss_with_main(m, R_y);

	// Print the derivatives.

	// cout << "Derivatives:";
	// for (size_t i = 0; i < n; ++i) {
	// 	cout << '\n' << i << ": (" << D_x(i, 0) << ", " << D_y(i, 0) << ')';
	// }

	// Calculate the spline coefficients.
	std::vector<Matrix> Bs_x; // vector of Matrix(1, 4).
	Bs_x.reserve(n - 1);
	std::vector<Matrix> Bs_y; // vector of Matrix(1, 4).
	Bs_y.reserve(n - 1);

	for (size_t i = 0; i < n - 1; ++i) {
		const auto t = ts[i + 1];
		const auto tt = t * t;
		const auto ttt = tt * t;

		Matrix mat(4, 4);
		mat(0, 0) = 1.0;
		mat(1, 1) = 1.0;
		mat(2, 0) = -3 / tt;
		mat(2, 1) = -2 / t;
		mat(2, 2) = 3 / tt;
		mat(2, 3) = -1 / t;
		mat(3, 0) = 2 / ttt;
		mat(3, 1) = 1 / tt;
		mat(3, 2) = -2 / ttt;
		mat(3, 3) = 1 / tt;

		Matrix P_x(1, 4);
		P_x(0, 0) = xs[i];
		P_x(1, 0) = D_x(i, 0);
		P_x(2, 0) = xs[i + 1];
		P_x(3, 0) = D_x(i + 1, 0);

		Bs_x.emplace_back(mat * P_x);

		Matrix P_y(1, 4);
		P_y(0, 0) = ys[i];
		P_y(1, 0) = D_y(i, 0);
		P_y(2, 0) = ys[i + 1];
		P_y(3, 0) = D_y(i + 1, 0);

		Bs_y.emplace_back(mat * P_y);
	}

	// Return ts to their original state.
	ts[0] = t_offset;

	for (size_t i = 1; i < n; ++i)
		ts[i] += ts[i - 1];

	// Output the result.
	auto spline = new Spline();

	spline->ts.push_back(ts[0]);
	spline->xs.push_back(xs[0]);
	spline->ys.push_back(ys[0]);

	size_t current_i = 0;
	for (double t = ts[0] + step; t < ts[n - 1] - step / 2; t += step) {
		while (ts[current_i] <= t)
			++current_i;

		const auto t_ = t - ts[current_i - 1];

		const auto& B_x = Bs_x[current_i - 1];
		const auto x = B_x(0, 0) + B_x(1, 0) * t_ + B_x(2, 0) * t_ * t_ + B_x(3, 0) * t_ * t_ * t_;
		const auto& B_y = Bs_y[current_i - 1];
		const auto y = B_y(0, 0) + B_y(1, 0) * t_ + B_y(2, 0) * t_ * t_ + B_y(3, 0) * t_ * t_ * t_;

		spline->ts.push_back(t);
		spline->xs.push_back(x);
		spline->ys.push_back(y);
	}

	spline->ts.push_back(ts[n - 1]);
	spline->xs.push_back(xs[n - 1]);
	spline->ys.push_back(ys[n - 1]);

	result->ts = spline->ts.data();
	result->xs = spline->xs.data();
	result->ys = spline->ys.data();
	result->n  = spline->ts.size();
	result->private_data = spline;
}

SPLINE_API void spline_free(struct spline_points* points)
{
	delete static_cast<Spline*>(points->private_data);
}
