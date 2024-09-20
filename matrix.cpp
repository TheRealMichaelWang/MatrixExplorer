#include <sstream>
#include "matrix.h"

using namespace MatrixExplorer;

int64_t HulaScript::instance::value::index(int64_t min, int64_t max, HulaScript::instance& instance) const {
	expect_type(HulaScript::instance::value::vtype::NUMBER, instance);
	
	if (data.number < min || data.number >= max) {
		std::stringstream ss;
		ss << data.number << " is outside the range of [" << min << ", " << max << ").";
		instance.panic(ss.str());
	}

	return static_cast<int64_t>(data.number);
}

HulaScript::instance::value matrix::get_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 2) {
		std::stringstream ss;
		ss << "MatrixEplorer: Matrix get expected a row, and column. Got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	int64_t row = arguments[0].index(1, rows + 1, instance) - 1;
	int64_t col = arguments[1].index(1, cols + 1, instance) - 1;

	return HulaScript::instance::value(elems[row * cols + col]);
}

HulaScript::instance::value matrix::set_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 3) {
		std::stringstream ss;
		ss << "MatrixEplorer: Matrix set expected a row, column, and element. Got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	int64_t row = arguments[0].index(1, rows + 1, instance) - 1;
	int64_t col = arguments[1].index(1, cols + 1, instance) - 1;

	elems[row * cols + col] = arguments[2].number(instance);

	return HulaScript::instance::value(arguments[2]);
}

HulaScript::instance::value matrix::transpose(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	std::vector<double> new_elems(rows * cols);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			new_elems[j * rows + i] = elems[i * cols + j];
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(cols, rows, new_elems));
}

HulaScript::instance::value MatrixExplorer::matrix::augment(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance)
{
	if (arguments.size() != 1) {
		std::stringstream ss;
		ss << "Matrix Explorer: Matrix augment expects a matrix, got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	matrix* mat_operand = dynamic_cast<matrix*>(arguments[0].foreign_obj(instance));
	if (mat_operand == NULL) {
		instance.panic("MatrixEplorer: You can only augment a matrix with another matrix.");
		return HulaScript::instance::value();
	}

	if (rows != mat_operand->rows) {
		std::stringstream ss;
		ss << "MatrixExplorer: Matrix augment expects a matrix with " << rows << " row(s), but got matrix with " << mat_operand->rows << " row(s) instead.";
		instance.panic(ss.str());
	}

	std::vector<double> new_elems;
	size_t new_cols = cols + mat_operand->cols;
	new_elems.reserve(rows * new_cols);

	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++) {
			new_elems.push_back(elems[i * cols + j]);
		}
		for (size_t j = 0; j < mat_operand->cols; j++) {
			new_elems.push_back(mat_operand->elems[i * mat_operand->cols + j]);
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(rows, new_cols, new_elems));
}

HulaScript::instance::value MatrixExplorer::matrix::reduced_echelon_form(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	return instance.add_foreign_object(std::make_unique<matrix>(reduce()));
}

HulaScript::instance::value MatrixExplorer::matrix::row_reduced_echelon_form(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	return instance.add_foreign_object(std::make_unique<matrix>(row_reduce()));
}

HulaScript::instance::value matrix::add_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) {
	matrix* mat_operand = dynamic_cast<matrix*>(operand.foreign_obj(instance));
	if (mat_operand == NULL) {
		instance.panic("MatrixEplorer: You can only add a matrix with another matrix.");
		return HulaScript::instance::value();
	}

	if (rows != mat_operand->rows || cols != mat_operand->cols) {
		instance.panic("MatrixEplorer: You can only add a matrix with another matrix of the same dimensions.");
		return HulaScript::instance::value();
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
		return HulaScript::instance::value();
	}

	if (rows != mat_operand->rows || cols != mat_operand->cols) {
		instance.panic("MatrixEplorer: You can only subtract a matrix with another matrix of the same dimensions.");
		return HulaScript::instance::value();
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
		return HulaScript::instance::value();
	}

	if (cols != mat_operand->rows) {
		instance.panic("MatrixEplorer: You can only multiply a matrix with another matrix where the columns and rows are equal, respectivley.");
	}

	std::vector<double> new_elems;
	new_elems.reserve(rows * mat_operand->cols);
	
	size_t common = cols;
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < mat_operand->cols; j++)
		{
			//result i,j = row i of this dot cols j of operand
			double sum = 0;
			for (size_t k = 0; k < common; k++)
			{
				sum += elems[i * cols + k] * mat_operand->elems[j + k * mat_operand->cols];
			}
			new_elems.push_back(sum);
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, mat_operand->cols, new_elems)));
}

HulaScript::instance::value MatrixExplorer::make_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance)
{
	std::vector<double> elems;
	std::optional<size_t> common_vec_dim = std::nullopt;

	for (auto& arg : arguments) {
		matrix* arg_mat = dynamic_cast<matrix*>(arg.foreign_obj(instance));
		if (arg_mat == NULL) {
			instance.panic("Matrix Explorer: Expected argument(s) to all be matricies.");
			return HulaScript::instance::value();
		}
		
		auto dim = arg_mat->dims();
		if (dim.second != 1) {
			std::stringstream ss;
			ss << "Matrix Explorer: Expected argument(s) to all be vectors/single column matricies.";
			instance.panic(ss.str());
		}

		if (common_vec_dim.has_value()) {
			if (dim.first != common_vec_dim.value()) {
				std::stringstream ss;
				ss << "Matrix Eplorer: Expected vector with " << common_vec_dim.value() << " elem(s), but got vector with " << dim.first << " elem(s) instead.";
				instance.panic(ss.str());
			}
		}
		else {
			common_vec_dim = dim.first;
		}

		elems.insert(elems.end(), arg_mat->elements(), arg_mat->elements() + (dim.second * dim.first));
	}

	size_t rows = common_vec_dim.has_value() ? common_vec_dim.value() : 0;
	
	std::vector<double> new_elems(elems.size());
	for (size_t i = 0; i < arguments.size(); i++) {
		for (size_t j = 0; j < rows; j++) {
			new_elems[j * arguments.size() + i] = elems[i * rows + j];
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(rows, arguments.size(), new_elems));
}

HulaScript::instance::value MatrixExplorer::make_vector(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance) {
	std::vector<double> elems;
	elems.reserve(arguments.size());

	for (auto& arg : arguments) {
		elems.push_back(arg.number(instance));
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(elems.size(), 1, elems)));
}

HulaScript::instance::value MatrixExplorer::make_identity_mat(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance) {
	if (arguments.size() != 1) {
		std::stringstream ss;
		ss << "MatrixEplorer: identity expects dimension. Got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	int64_t dim = arguments[0].index(0, INT64_MAX, instance);
	
	std::vector<double> elems;
	elems.reserve(dim * dim);
	for (size_t i = 0; i < dim; i++) {
		for (size_t j = 0; j < dim; j++) {
			elems.push_back(i == j ? 1 : 0);
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(dim, dim, elems)));
}