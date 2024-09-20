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
			ss << elems[i * cols + j];
		}
		ss << "\n";
	}
	return ss.str();
}