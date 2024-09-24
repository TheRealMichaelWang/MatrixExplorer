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

	for (size_t i = 0; i < std::min(reduced.rows, reduced.cols); i++) {
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

HulaScript::instance::value matrix::is_reduced_echelon_form(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	std::optional<size_t> last_pivot_pos = std::nullopt;
	for (size_t i = 0; i < rows; i++) {
		bool found_pivot = false;
		for (size_t j = 0; j < cols; j++) {
			if (elems[i*cols + j] != 0) { //potential pivot detected
				if (last_pivot_pos.has_value() && j <= last_pivot_pos.value()) {
					return HulaScript::instance::value(false);
				}
				last_pivot_pos = j;
				found_pivot = true;
				break;
			}
		}

		if (!found_pivot) {
			last_pivot_pos = cols;
		}
	}

	return HulaScript::instance::value(true);
}

HulaScript::instance::value matrix::is_row_reduced_echelon_form(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	std::optional<size_t> last_pivot_pos = std::nullopt;
	for (size_t i = 0; i < rows; i++) {
		bool found_pivot = false;
		for (size_t j = 0; j < cols; j++) {
			if (elems[i * cols + j] != 0) { //potential pivot detected
				if (elems[i * cols + j] != 1) {
					return HulaScript::instance::value(false);
				}

				if (last_pivot_pos.has_value() && j <= last_pivot_pos.value()) {
					return HulaScript::instance::value(false);
				}
				last_pivot_pos = j;
				found_pivot = true;

				if (i > 0) {
					for (size_t k = 0; k < i - 1; k++) {
						if (elems[k * cols + j] != 0) {
							return HulaScript::instance::value(false);
						}
					}
				}

				break;
			}
		}

		if (!found_pivot) {
			last_pivot_pos = cols;
		}
	}

	return HulaScript::instance::value(true);
}