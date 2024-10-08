#include <sstream>
#include "matrix.h"

using namespace MatrixExplorer;

HulaScript::instance::value matrix::get_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 2) {
		std::stringstream ss;
		ss << "MatrixEplorer: Matrix get expected a row, and column. Got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	int64_t row = arguments[0].index(1, rows + 1, instance) - 1;
	int64_t col = arguments[1].index(1, cols + 1, instance) - 1;

	return instance.add_foreign_object(std::make_unique<mat_number_type>(mat_number_type(elems[row * cols + col])));
}

HulaScript::instance::value matrix::set_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 3) {
		std::stringstream ss;
		ss << "MatrixEplorer: Matrix set expected a row, column, and element. Got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	int64_t row = arguments[0].index(1, rows + 1, instance) - 1;
	int64_t col = arguments[1].index(1, cols + 1, instance) - 1;

	elems[row * cols + col] = mat_number_type::unwrap(arguments[2], instance);

	return HulaScript::instance::value(arguments[2]);
}

HulaScript::instance::value matrix::transpose(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	std::vector<elem_type> new_elems(rows * cols);

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

	std::vector<elem_type> new_elems;
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

HulaScript::instance::value MatrixExplorer::matrix::is_row_equivalent(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 1) {
		std::stringstream ss;
		ss << "Matrix Explorer: Matrix isRowEquiv expects a matrix, got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	matrix* mat_operand = dynamic_cast<matrix*>(arguments[0].foreign_obj(instance));
	if (mat_operand == NULL) {
		instance.panic("MatrixEplorer: You can only determine row equivalence of a matrix with another matrix.");
		return HulaScript::instance::value();
	}

	return is_row_equivalent(*mat_operand);
}

HulaScript::instance::value MatrixExplorer::matrix::get_row_vec(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 1) {
		std::stringstream ss;
		ss << "Matrix Explorer: Matrix rowAt expects a row index, got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	size_t index = arguments[0].index(1, rows + 1, instance);
	return instance.add_foreign_object(std::make_unique<matrix>(get_row_vec(index - 1)));
}

HulaScript::instance::value MatrixExplorer::matrix::get_col_vec(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 1) {
		std::stringstream ss;
		ss << "Matrix Explorer: Matrix colAt expects a col index, got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	size_t index = arguments[0].index(1, cols + 1, instance);
	return instance.add_foreign_object(std::make_unique<matrix>(get_col_vec(index - 1)));
}

HulaScript::instance::value MatrixExplorer::matrix::get_rows(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	auto row_vecs = get_rows();
	
	std::vector<HulaScript::instance::value> elems;
	elems.reserve(row_vecs.size());

	for (auto& row_vec : row_vecs) {
		elems.push_back(instance.add_foreign_object(std::make_unique<matrix>(std::move(row_vec))));
	}

	return instance.make_array(elems);
}

HulaScript::instance::value MatrixExplorer::matrix::get_cols(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	auto col_vecs = get_cols();

	std::vector<HulaScript::instance::value> elems;
	elems.reserve(col_vecs.size());

	for (auto& row_vec : col_vecs) {
		elems.push_back(instance.add_foreign_object(std::make_unique<matrix>(std::move(row_vec))));
	}

	return instance.make_array(elems);
}

HulaScript::instance::value MatrixExplorer::matrix::get_coefficient_matrix(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	std::vector<elem_type> toret_elems;
	toret_elems.reserve(rows * (cols - 1));

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols - 1; j++) {
			toret_elems.push_back(elems[i * cols + j]);
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, cols - 1, toret_elems)));
}

HulaScript::instance::value MatrixExplorer::matrix::get_solution_column(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	std::vector<elem_type> toret_elems;
	toret_elems.reserve(rows);

	for (size_t i = 0; i < rows; i++) {
		toret_elems.push_back(elems[i * cols + (cols - 1)]);
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, 1, toret_elems)));
}

HulaScript::instance::value MatrixExplorer::matrix::get_left_square(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (cols < rows) {
		instance.panic("Cannot get the left square if the matrix has fewer columns than rows (left square side length is equal to row count).");
	}

	std::vector<elem_type> toret_elems;
	toret_elems.reserve(rows * rows);
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = rows; j < cols; j++) {
			toret_elems.push_back(elems[i * cols + j]);
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, rows, toret_elems)));
}

HulaScript::instance::value MatrixExplorer::matrix::get_dimensions(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	std::vector<std::pair<std::string, HulaScript::instance::value>> elems;
	elems.reserve(2);

	elems.push_back({ "rows", HulaScript::instance::value(static_cast<double>(rows)) });
	elems.push_back({ "cols", HulaScript::instance::value(static_cast<double>(cols)) });

	return instance.make_table_obj(elems, true);
}

HulaScript::instance::value MatrixExplorer::matrix::get_sub_matrix(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
	if (arguments.size() != 4) {
		std::stringstream ss;
		ss << "Matrix Explorer: Matrix subMat expects a row index, col index, row size, and col size, got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	size_t row_index = arguments[0].index(1, rows + 1, instance) - 1;
	size_t col_index = arguments[1].index(1, cols + 1, instance) - 1;
	size_t row_size = arguments[2].index(0, (rows - row_index) + 1, instance);
	size_t col_size = arguments[3].index(0, (cols - col_index) + 1, instance);

	std::vector<elem_type> elems;
	for (size_t i = 0; i < row_size; i++) {
		for (size_t j = 0; j < col_size; j++) {
			elems.push_back(this->elems[(row_index + i) * cols + (col_index + j)]);
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(row_size, col_size, elems)));
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

	std::vector<elem_type> new_elems;
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

	std::vector<elem_type> new_elems;
	new_elems.reserve(rows * cols);

	for (size_t i = 0; i < rows * cols; i++) {
		new_elems.push_back(elems[i] - mat_operand->elems[i]);
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

	std::vector<elem_type> new_elems;
	new_elems.reserve(rows * mat_operand->cols);
	
	size_t common = cols;
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < mat_operand->cols; j++)
		{
			//result i,j = row i of this dot cols j of operand
			elem_type sum = rational(0);
			for (size_t k = 0; k < common; k++)
			{
				sum = sum + elems[i * cols + k] * mat_operand->elems[j + k * mat_operand->cols];
			}
			new_elems.push_back(sum);
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, mat_operand->cols, new_elems)));
}

HulaScript::instance::value MatrixExplorer::make_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance)
{
	if (arguments.size() == 3 && !arguments[0].check_type(HulaScript::instance::value::FOREIGN_OBJECT)) {
		size_t rows = arguments[0].index(0, INT64_MAX, instance);
		size_t cols = arguments[1].index(0, INT64_MAX, instance);
		
		std::vector<matrix::elem_type> elems;
		elems.reserve(rows * cols);

		for (size_t i = 1; i <= rows; i++) {
			for (size_t j = 1; j <= cols; j++) {
				elems.push_back(matrix::mat_number_type::unwrap(instance.invoke_value(arguments[2], {
					HulaScript::instance::value(static_cast<double>(i)),
					HulaScript::instance::value(static_cast<double>(j))
				}), instance));
			}
		}

		return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, cols, elems)));
	}

	std::vector<matrix::elem_type> elems;
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
	
	std::vector<matrix::elem_type> new_elems(elems.size());
	for (size_t i = 0; i < arguments.size(); i++) {
		for (size_t j = 0; j < rows; j++) {
			new_elems[j * arguments.size() + i] = elems[i * rows + j];
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(rows, arguments.size(), new_elems));
}

HulaScript::instance::value MatrixExplorer::make_vector(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance) {
	std::vector<matrix::elem_type> elems;
	elems.reserve(arguments.size());

	for (auto& arg : arguments) {
		elems.push_back(matrix::mat_number_type::unwrap(arg, instance));
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(elems.size(), 1, elems)));
}

HulaScript::instance::value MatrixExplorer::make_identity_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance) {
	if (arguments.size() != 1) {
		std::stringstream ss;
		ss << "MatrixEplorer: identity expects dimension. Got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	int64_t dim = arguments[0].index(0, INT64_MAX, instance);
	
	std::vector<matrix::elem_type> elems;
	elems.reserve(dim * dim);
	for (size_t i = 0; i < dim; i++) {
		for (size_t j = 0; j < dim; j++) {
			elems.push_back(i == j ? rational(1) : rational(0));
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(matrix(dim, dim, elems)));
}

HulaScript::instance::value MatrixExplorer::make_zero_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance) {
	if (arguments.size() != 2) {
		std::stringstream ss;
		ss << "MatrixEplorer: Matrix get expected a row, and column. Got " << arguments.size() << " argument(s) instead.";
		instance.panic(ss.str());
	}

	size_t rows = arguments[0].index(0, INT64_MAX, instance);
	size_t cols = arguments[1].index(0, INT64_MAX, instance);

	std::vector<matrix::elem_type> elems(rows * cols, 0);
	return instance.add_foreign_object(std::make_unique<matrix>(matrix(rows, cols, elems)));
}
