#pragma once

#include <cassert>
#include <cstring>
#include "ffi.h"

namespace MatrixExplorer {
	class matrix : public HulaScript::foreign_method_object<matrix> {
	private:
		size_t rows, cols;
		std::unique_ptr<double[]> elems;

		HulaScript::instance::value add_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override;
		HulaScript::instance::value subtract_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override;
		HulaScript::instance::value multiply_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override;

		HulaScript::instance::value get_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value set_elem(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
	
		HulaScript::instance::value transpose(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value augment(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		
		HulaScript::instance::value reduced_echelon_form(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
			return instance.add_foreign_object(std::make_unique<matrix>(reduce()));
		}
		HulaScript::instance::value row_reduced_echelon_form(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
			return instance.add_foreign_object(std::make_unique<matrix>(row_reduce()));
		}

		HulaScript::instance::value is_reduced_echelon_form(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
			return HulaScript::instance::value(is_ref());
		}
		HulaScript::instance::value is_row_reduced_echelon_form(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance) {
			return HulaScript::instance::value(is_rref());
		}
		HulaScript::instance::value is_row_equivalent(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);

		HulaScript::instance::value get_coefficient_matrix(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value get_solution_column(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value get_left_square(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);

		HulaScript::instance::value get_dimensions(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);

		//add one to get right elementary matrix

		void swap_rows(size_t a, size_t b);
		void scale_row(size_t i, double scalar);
		void subtract_rows(size_t subtract_from, size_t how_much, double scale);
	public:

		matrix(size_t rows, size_t cols, std::vector<double> elems_vec) : rows(rows), cols(cols), elems(new double[elems_vec.size()]) {
			assert(elems_vec.size() == rows * cols);
			std::memcpy(elems.get(), elems_vec.data(), elems_vec.size() * sizeof(double));

			declare_method("get", &matrix::get_elem);
			declare_method("set", &matrix::set_elem);
			declare_method("trans", &matrix::transpose);
			declare_method("augment", &matrix::augment);

			declare_method("ref", &matrix::reduced_echelon_form);
			declare_method("rref", &matrix::row_reduced_echelon_form);
			declare_method("isRef", &matrix::is_reduced_echelon_form);
			declare_method("isRref", &matrix::is_row_reduced_echelon_form);
			declare_method("isRowEquiv", &matrix::is_row_equivalent);

			declare_method("dim", &matrix::get_dimensions);
			declare_method("coef", &matrix::get_coefficient_matrix);
			declare_method("sol", &matrix::get_solution_column);
			declare_method("leftSq", &matrix::get_left_square);
		}

		const std::pair<size_t, size_t> dims() const noexcept {
			return std::make_pair(rows, cols);
		}

		const double* elements() const noexcept {
			return elems.get();
		}

		std::string to_string() override;

		matrix row_reduce() const noexcept;
		matrix reduce() const noexcept;

		bool is_ref() const noexcept;
		bool is_rref() const noexcept;

		bool is_row_equivalent(const matrix& other) const noexcept;
	};

	HulaScript::instance::value make_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance);
	HulaScript::instance::value make_vector(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance);
	HulaScript::instance::value make_identity_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance);
	HulaScript::instance::value make_zero_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance);
}