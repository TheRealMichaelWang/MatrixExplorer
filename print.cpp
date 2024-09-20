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

			double elem = elems.get()[i * cols + j];
			if (elem == 0) { ss << "0"; } //to handle negative zero
			else { ss << elem; }
		}
		ss << "\n";
	}
	return ss.str();
}