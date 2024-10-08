#include <sstream>
#include <string>
#include "matrix.h"

using namespace MatrixExplorer;

std::string matrix::to_string() {
	std::stringstream ss;
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			if (j != 0) {
				ss << ", ";
			}

			matrix::elem_type elem = elems.get()[i * cols + j];
			if (elem.is_zero()) { ss << "0"; } //to handle negative zero
			else { ss << elem.to_string(); }
		}
		ss << "\n";
	}
	return ss.str();
}