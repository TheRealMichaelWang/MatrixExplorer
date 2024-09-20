#include <sstream>
#include "matrix.h"

using namespace MatrixExplorer;

size_t HulaScript::instance::value::index(size_t min, size_t max, HulaScript::instance& instance) const {
	expect_type(HulaScript::instance::value::vtype::NUMBER, instance);
	
	if (data.number < min || data.number >= max) {
		std::stringstream ss;
		ss << data.number << " is outside the range of [" << min << ", " << max << ").";
		instance.panic(ss.str());
	}

	return static_cast<size_t>(data.number);
}

HulaScript::instance::value matrix::get_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 2) {
		std::stringstream ss;
		ss << "Matrix get expected a row, and column. Got " << arguments.size() << " instead.";
		instance.panic(ss.str());
	}

	size_t row = arguments[0].index(1, rows + 1, instance) - 1;
	size_t col = arguments[1].index(1, cols + 1, instance) - 1;

	return HulaScript::instance::value(elems[row * cols + col]);
}

HulaScript::instance::value matrix::set_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 3) {
		std::stringstream ss;
		ss << "Matrix set expected a row, column, and element. Got " << arguments.size() << " instead.";
		instance.panic(ss.str());
	}

	size_t row = arguments[0].index(1, rows + 1, instance) - 1;
	size_t col = arguments[1].index(1, cols + 1, instance) - 1;

	elems[row * cols + col] = arguments[2].number(instance);

	return HulaScript::instance::value(arguments[2]);
}