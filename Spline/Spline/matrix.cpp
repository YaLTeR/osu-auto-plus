#include <cmath>
#include "matrix.hpp"

Matrix& Matrix::operator+=(const Matrix& rhs)
{
	if (Height != rhs.GetHeight())
		throw std::logic_error("height != rhs height");
	if (Width != rhs.GetWidth())
		throw std::logic_error("width != rhs width");

	for (size_t i = 0; i < Height; ++i) {
		for (size_t j = 0; j < Width; ++j) {
			operator()(i, j) += rhs(i, j);
		}
	}

	return *this;
}

Matrix& Matrix::operator*=(double rhs)
{
	for (size_t i = 0; i < Height; ++i) {
		for (size_t j = 0; j < Width; ++j) {
			operator()(i, j) *= rhs;
		}
	}

	return *this;
}

Matrix Matrix::operator*(const Matrix& rhs) const
{
	if (Width != rhs.GetHeight())
		throw std::logic_error("width != rhs height");

	Matrix rv(rhs.GetWidth(), Height);

	for (size_t i = 0; i < Height; ++i) {
		for (size_t j = 0; j < rhs.GetWidth(); ++j) {
			for (size_t k = 0; k < Width; ++k) {
				rv(i, j) += operator()(i, k) * rhs(k, j);
			}
		}
	}

	return rv;
}

Matrix Matrix::Row(size_t r) const
{
	if (r >= Height)
		throw std::out_of_range("r >= height");

	Matrix row(Width, 1);
	for (size_t i = 0; i < Width; ++i) {
		row(0, i) = operator()(r, i);
	}
	return row;
}

Matrix Matrix::SubMatrix(size_t x, size_t y, size_t w, size_t h) const
{
	if (x + w > Width)
		throw std::out_of_range("x + w >= width");
	if (y + h > Height)
		throw std::out_of_range("y + h >= height");

	Matrix submatrix(w, h);
	for (size_t i = 0; i < w; ++i) {
		for (size_t j = 0; j < h; ++j) {
			submatrix(i, j) = operator()(x + i, y + j);
		}
	}
	return submatrix;
}

Matrix Matrix::Transposed() const
{
	Matrix t(Height, Width);
	for (size_t i = 0; i < Width; ++i) {
		for(size_t j = 0; j < Height; ++j) {
			t(j, i) = operator()(i, j);
		}
	}
	return t;
}

Matrix Matrix::Adjugate() const
{
	Matrix m(Width, Height);

	for (size_t i = 0; i < Height; ++i) {
		for (size_t j = 0; j < Width; ++j) {
			m(i, j) = ((i + j) % 2) ? -Minor(i, j) : Minor(i, j);
		}
	}

	return m.Transposed();
}

Matrix Matrix::Inverse() const
{
	// return Adjugate() / Determinant();

	if (Width != Height)
		throw std::logic_error("width != height");

	Matrix copy(*this);
	Matrix inv(Width, Height);
	for (size_t s = 0; s < Width; ++s) {
		inv(s, s) = 1;
	}

	for (size_t s = 0; s < copy.GetWidth() - 1; ++s) {
		double m = 0;
		size_t m_i = 0;
		for (size_t i = s; i < copy.GetHeight(); ++i) {
			auto t = std::abs(copy(i, s));
			if (t > m) {
				m = t;
				m_i = i;
			}
		}
		if (m == 0) {
			throw std::logic_error("determinant = 0");
		}
		copy.SwapRows(m_i, s);
		inv.SwapRows(m_i, s);

		for (size_t i = s + 1; i < copy.GetHeight(); ++i) {
			double k = copy(i, s) / copy(s, s);
			for (size_t j = 0; j < copy.GetWidth(); ++j) {
				copy(i, j) -= copy(s, j) * k;
				inv(i, j) -= inv(s, j) * k;
			}
		}
		double k = copy(s, s);
		for (size_t i = 0; i < copy.GetWidth(); ++i) {
			copy(s, i) /= k;
			inv(s, i) /= k;
		}
	}

	for (size_t i = 0; i < inv.GetWidth(); ++i) {
		inv(inv.GetHeight() - 1, i) /= copy(copy.GetHeight() - 1, copy.GetWidth() - 1);
	}
	copy(copy.GetHeight() - 1, copy.GetWidth() - 1) = 1;

	for (size_t s = 1; s < copy.GetWidth(); ++s) {
		for (size_t i = 0; i < s; ++i) {
			double k = copy(i, s);
			for (size_t j = 0; j < copy.GetWidth(); ++j) {
				copy(i, j) -= k * copy(s, j);
				inv(i, j) -= k * inv(s, j);
			}
		}
	}

	return inv;
}

double Matrix::Trace() const
{
	if (Width != Height)
		throw std::logic_error("width != height");

	double trace = 0;
	for (size_t i = 0; i < Width; ++i) {
		trace += operator()(i, i);
	}
	return trace;
}

double Matrix::Determinant() const
{
	if (Width != Height)
		throw std::logic_error("width != height");

	if (Width == 0)
		return 0;
	if (Width == 1)
		return operator()(0, 0);
	if (Width == 2)
		return operator()(0, 0) * operator()(1, 1)
			- operator()(0, 1) * operator()(1, 0);

	Matrix m = *this;
	bool sign = false;
	for (size_t s = 0; s < m.GetWidth() - 1; ++s) {
		bool found_nonzero = false;
		for (size_t i = s; i < m.GetHeight(); ++i) {
			if (m(i, s) != 0) {
				found_nonzero = true;
				m.SwapRows(s, i);
				if (s != i)
					sign = !sign;
				break;
			}
		}
		if (!found_nonzero)
			return 0;

		for (size_t i = s + 1; i < m.GetHeight(); ++i) {
			double k = m(i, s) / m(s, s);
			for (size_t j = s; j < m.GetWidth(); ++j) {
				m(i, j) -= m(s, j) * k;
			}
		}
	}

	double determinant = 1;
	for (size_t i = 0; i < m.GetWidth(); ++i) {
		determinant *= m(i, i);
	}
	return determinant * (sign ? -1 : 1);
}

double Matrix::Minor(size_t r_excl, size_t c_excl) const
{
	if (r_excl >= Height)
		throw std::logic_error("r_excl >= height");
	if (c_excl >= Width)
		throw std::logic_error("c_excl >= width");

	Matrix m(Width - 1, Height - 1);
	size_t x = 0;
	for (size_t i = 0; i < Height; ++i) {
		if (i != r_excl) {
			size_t y = 0;
			for (size_t j = 0; j < Width; ++j) {
				if (j != c_excl) {
					m(x, y) = operator()(i, j);
					y++;
				}
			}
			x++;
		}
	}

	return m.Determinant();
}

void Matrix::SwapRows(size_t r1, size_t r2)
{
	if (r1 >= Height)
		throw std::out_of_range("r1 >= height");
	if (r2 >= Height)
		throw std::out_of_range("r2 >= height");
	if (r1 == r2)
		return;

	auto temp = Row(r1);
	for (size_t i = 0; i < Width; ++i) {
		operator()(r1, i) = operator()(r2, i);
		operator()(r2, i) = temp(0, i);
	}
}
