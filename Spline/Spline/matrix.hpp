#pragma once
#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>

class Matrix {
public:
	Matrix(size_t width, size_t height)
		: Width(width)
		, Height(height)
		, data(width * height)
	{};

	inline size_t GetWidth() const { return Width; }
	inline size_t GetHeight() const { return Height; }
	inline double& operator()(size_t i, size_t j)
	{
		if (i >= Height)
			throw std::out_of_range("i >= height");
		if (j >= Width)
			throw std::out_of_range("j >= width");

		return data[i * Width + j];
	}
	inline const double& operator()(size_t i, size_t j) const
	{
		if (i >= Height)
			throw std::out_of_range("i >= height");
		if (j >= Width)
			throw std::out_of_range("j >= width");

		return data[i * Width + j];
	}

	Matrix& operator+=(const Matrix& rhs);
	friend Matrix operator+(Matrix lhs, const Matrix& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	Matrix& operator*=(double rhs);
	friend Matrix operator*(Matrix lhs, double rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	Matrix& operator/=(double rhs)
	{
		*this *= (1 / rhs);
		return *this;
	}
	friend Matrix operator/(Matrix lhs, double rhs)
	{
		lhs /= rhs;
		return lhs;
	}

	Matrix operator*(const Matrix& rhs) const;

	Matrix Row(size_t r) const;
	Matrix SubMatrix(size_t x, size_t y, size_t w, size_t h) const;
	Matrix Transposed() const;
	Matrix Adjugate() const;
	Matrix Inverse() const;

	double Trace() const;
	double Determinant() const;
	double Minor(size_t r_excl, size_t c_excl) const;

	void SwapRows(size_t r1, size_t r2);

protected:
	size_t Width, Height;
	std::vector<double> data;
};
