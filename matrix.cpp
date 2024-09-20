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
		ss << "MatrixEplorer: Matrix get expected a row, and column. Got " << arguments.size() << " instead.";
		instance.panic(ss.str());
	}

	size_t row = arguments[0].index(1, rows + 1, instance) - 1;
	size_t col = arguments[1].index(1, cols + 1, instance) - 1;

	return HulaScript::instance::value(elems[row * cols + col]);
}

HulaScript::instance::value matrix::set_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 3) {
		std::stringstream ss;
		ss << "MatrixEplorer: Matrix set expected a row, column, and element. Got " << arguments.size() << " instead.";
		instance.panic(ss.str());
	}

	size_t row = arguments[0].index(1, rows + 1, instance) - 1;
	size_t col = arguments[1].index(1, cols + 1, instance) - 1;

	elems[row * cols + col] = arguments[2].number(instance);

	return HulaScript::instance::value(arguments[2]);
}

HulaScript::instance::value matrix::add_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) {
	matrix* mat_operand = dynamic_cast<matrix*>(operand.foreign_obj(instance));
	if (mat_operand == NULL) {
		instance.panic("MatrixEplorer: You can only add a matrix with another matrix.");
	}

	if (rows != mat_operand->rows || cols != mat_operand->cols) {
		instance.panic("MatrixEplorer: You can only add a matrix with another matrix of the same dimensions.");
	}

	std::vector<double> new_elems;
	new_elems.reserve(rows * cols);

	for (size_t i = 0; i < rows * cols; i++) {
		new_elems.push_back(elems[i] + mat_operand->elems[i]);
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, cols, new_elems)));
}

HulaScript::instance::value matrix::subtract_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) {
	matrix* mat_operand = dynamic_cast<matrix*>(operand.foreign_obj(instance));
	if (mat_operand == NULL) {
		instance.panic("MatrixEplorer: You can only subtract a matrix with another matrix.");
	}

	if (rows != mat_operand->rows || cols != mat_operand->cols) {
		instance.panic("MatrixEplorer: You can only subtract a matrix with another matrix of the same dimensions.");
	}

	std::vector<double> new_elems;
	new_elems.reserve(rows * cols);

	for (size_t i = 0; i < rows * cols; i++) {
		new_elems.push_back(elems[i] + mat_operand->elems[i]);
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, cols, new_elems)));
}

HulaScript::instance::value matrix::multiply_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) {
	matrix* mat_operand = dynamic_cast<matrix*>(operand.foreign_obj(instance));
	if (mat_operand == NULL) {
		instance.panic("MatrixEplorer: You can only multiply a matrix with another matrix.");
	}

	if (cols != mat_operand->rows) {
		instance.panic("MatrixEplorer: You can only multiply a matrix with another matrix where the columns and rows are equal, respectivley.");
	}

	std::vector<double> new_elems;
	new_elems.reserve(rows * cols);
	
	size_t common = cols;
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++)
		{
			//result i,j = row i of this dot cols j of operand
			double sum = 0;
			for (size_t k = 0; k < common; k++)
			{
				sum += elems[i * cols + k] * elems[k + j * mat_operand->cols];
			}
			new_elems.push_back(sum);
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, cols, new_elems)));
}