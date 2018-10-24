#include <cmath>
#include <cstddef>
#include <cstring>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::size_t;

void print(const Matrix& m)
{
	for (size_t i = 0; i < m.GetHeight(); ++i) {
		for (size_t j = 0; j < m.GetWidth(); ++j) {
			cout << m(i, j) << " ";
		}
		cout << "\n";
	}
}

Matrix gauss(Matrix A, Matrix f)
{
	if (f.GetWidth() != 1)
		throw std::logic_error("gauss(): f width != 1");
	if (A.GetHeight() != f.GetHeight())
		throw std::logic_error("gauss(): A height != f height");
	if (A.GetWidth() != A.GetHeight())
		throw std::logic_error("gauss(): A width != A height");

	Matrix x(1, f.GetHeight());

	for (size_t s = 0; s < A.GetWidth() - 1; ++s) {
		bool found_nonzero = false;
		for (size_t i = s; i < A.GetHeight(); ++i) {
			if (A(i, s) != 0) {
				found_nonzero = true;
				A.SwapRows(s, i);
				f.SwapRows(s, i);
				break;
			}
		}
		if (!found_nonzero)
			throw std::logic_error("gauss(): |A| = 0");

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



struct point {
	double t;
	double x;
	double y;
};

int main(int argc, char* argv[])
{
	((void)argc);
	((void)argv);

	// Read derivatives in the first point.
	double d1_x, d1_y;
	cin >> d1_x >> d1_y;

	// Read derivatives in the last point.
	double dn_x, dn_y;
	cin >> dn_x >> dn_y;

	// Read intermediate points.
	std::vector<point> points;

	{
		double t, x, y;

		while (cin >> t >> x >> y) {
			points.push_back(point{ t, x, y });
		}
	}

	if (points.empty())
		return 1;

	const auto n = points.size();

	

	// cout << "\n\nplot ";
	// for (size_t i = 0; i < n - 1; ++i) {
	// 	const auto& B_x = Bs_x[i];
	// 	const auto t = points[i].t;

	// 	#define FORMAT(x) ((x)<0?"(":"") << (x) << ((x)<0?")":"")

	// 	cout << FORMAT(B_x(0, 0))
	// 	     << '+' << FORMAT(B_x(1, 0)) << "*(x-" << t << ')'
	// 	     << '+' << FORMAT(B_x(2, 0)) << "*(x-" << t << ")^2"
	// 	     << '+' << FORMAT(B_x(3, 0)) << "*(x-" << t << ")^3"
	// 	     << " and ";
	// }

	return 0;
}
