#include "matrix.h"

using namespace MatrixExplorer;

void matrix::swap_rows(size_t a, size_t b) {
	for (size_t i = 0; i < cols; i++) {
		double a_elem = elems[a * cols + i];
		elems[a * cols + i] = elems[b * cols + i];
		elems[b * cols + i] = a_elem;
	}
}

void matrix::scale_row(size_t k, double scalar) {
	for (size_t i = 0; i < cols; i++) {
		elems[k * cols + i] *= scalar;
	}
}

void matrix::subtract_rows(size_t subtract_from, size_t how_much, double scale) {
	for (size_t i = 0; i < cols; i++) {
		elems[subtract_from * cols + i] -= elems[how_much * cols + i] * scale;
	}
}

matrix matrix::reduce() {
	std::vector<double> new_elems(elems.get(), elems.get() + (rows * cols));
	matrix mat(rows, cols, new_elems);

	for (size_t i = 0; i < cols; i++) {
		bool found_nonzero = false;
		for (size_t j = i; j < rows; j++) {
			if (mat.elems[j * cols + i] != 0) {
				mat.swap_rows(i, j);
				found_nonzero = true;
				break;
			}
		}

		if (found_nonzero) {
			double non_zero_elem = mat.elems[i * cols + i];
			for (size_t j = i + 1; j < rows; j++) {
				double leading = mat.elems[j * cols + i];
				if (leading != 0) {
					mat.subtract_rows(j, i, leading / non_zero_elem);
				}
			}
		}
	}

	return mat;
}

matrix matrix::row_reduce() {
	matrix reduced = reduce();

	for (size_t i = 0; i < reduced.cols; i++) {
		double elem = reduced.elems[i * cols + i];
		if (elem != 0) {
			reduced.scale_row(i, 1 / elem);
		}
	}

	for (size_t i = 0; i < std::min(reduced.rows, reduced.cols); i++) {
		for (size_t j = i + 1; j < std::min(reduced.rows, reduced.cols); j++) {
			double elem = reduced.elems[i * cols + j];
			reduced.subtract_rows(i, j, elem);
		}
	}

	return reduced;
}