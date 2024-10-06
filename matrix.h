#pragma once

#include <cassert>
#include <cstring>
#include <vector>
#include "ffi.h"
#include "hash.h"

#define TTMATH_NOASM
#include "ttmathbig.h"

namespace MatrixExplorer {
	class matrix : public HulaScript::foreign_method_object<matrix> {
	public:
		using elem_type = ttmath::Big<2, 2>;

		class mat_number_type : public HulaScript::instance::foreign_object {
		private:
			elem_type number_;

		public:
			mat_number_type(elem_type number) : number_(number) { }

			static elem_type unwrap(HulaScript::instance::value value, HulaScript::instance& instance) {
				mat_number_type* obj = dynamic_cast<mat_number_type*>(value.foreign_obj(instance));
				if (obj == NULL) {
					instance.panic("MatrixExplorer: Expected precise number, got something else.");
					return elem_type(0);
				}
				return obj->number_;
			}

		protected:
			HulaScript::instance::value add_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override{
				elem_type b = unwrap(operand, instance);
				return instance.add_foreign_object(std::make_unique<mat_number_type>(mat_number_type(number_ + b)));
			}

			HulaScript::instance::value subtract_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override {
				elem_type b = unwrap(operand, instance);
				return instance.add_foreign_object(std::make_unique<mat_number_type>(mat_number_type(number_ - b)));
			}

			HulaScript::instance::value multiply_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override {
				elem_type b = unwrap(operand, instance);
				return instance.add_foreign_object(std::make_unique<mat_number_type>(mat_number_type(number_ * b)));
			}

			HulaScript::instance::value divide_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override {
				elem_type b = unwrap(operand, instance);
				return instance.add_foreign_object(std::make_unique<mat_number_type>(mat_number_type(number_ / b)));
			}

			HulaScript::instance::value modulo_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override {
				elem_type b = unwrap(operand, instance);
				elem_type res = number_;
				res.Mod(b);
				return instance.add_foreign_object(std::make_unique<mat_number_type>(mat_number_type(res)));
			}

			HulaScript::instance::value exponentiate_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override {
				elem_type b = unwrap(operand, instance);
				elem_type res = number_;
				res.Pow(b);
				return instance.add_foreign_object(std::make_unique<mat_number_type>(mat_number_type(res)));
			}

		public:
			size_t compute_hash() override {
				//forgive me for this hash
				return HulaScript::Hash::dj2b(number_.ToString().c_str());
			}

			std::string to_string() override {
				return number_.ToString();
			}

			double to_number() override {
				return number_.ToDouble();
			}
		};

	private:
		size_t rows, cols;
		std::unique_ptr<elem_type[]> elems;

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

		HulaScript::instance::value get_row_vec(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value get_col_vec(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value get_rows(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value get_cols(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);

		HulaScript::instance::value get_coefficient_matrix(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value get_solution_column(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value get_left_square(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);

		HulaScript::instance::value get_dimensions(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);
		HulaScript::instance::value get_sub_matrix(std::vector<HulaScript::instance::value>& arguments, HulaScript::instance& instance);

		//add one to get right elementary matrix

		void swap_rows(size_t a, size_t b);
		void scale_row(size_t i, elem_type scalar);
		void add_rows(size_t add_to, size_t how_much);
		void subtract_rows(size_t subtract_from, size_t how_much, elem_type scale);
	public:

		matrix(size_t rows, size_t cols, std::vector<elem_type> elems_vec) : rows(rows), cols(cols), elems(new elem_type[elems_vec.size()]) {
			assert(elems_vec.size() == rows * cols);
			std::memcpy(elems.get(), elems_vec.data(), elems_vec.size() * sizeof(elem_type));

			declare_method("get", &matrix::get_elem);
			declare_method("set", &matrix::set_elem);
			declare_method("trans", &matrix::transpose);
			declare_method("augment", &matrix::augment);
			declare_method("subMat", &matrix::get_sub_matrix);

			declare_method("ref", &matrix::reduced_echelon_form);
			declare_method("rref", &matrix::row_reduced_echelon_form);
			declare_method("isRef", &matrix::is_reduced_echelon_form);
			declare_method("isRref", &matrix::is_row_reduced_echelon_form);
			declare_method("isRowEquiv", &matrix::is_row_equivalent);

			declare_method("rowAt", &matrix::get_row_vec);
			declare_method("colAt", &matrix::get_col_vec);
			declare_method("rows", &matrix::get_rows);
			declare_method("cols", &matrix::get_cols);

			declare_method("dim", &matrix::get_dimensions);
			declare_method("coef", &matrix::get_coefficient_matrix);
			declare_method("sol", &matrix::get_solution_column);
			declare_method("leftSq", &matrix::get_left_square);
		}

		const std::pair<size_t, size_t> dims() const noexcept {
			return std::make_pair(rows, cols);
		}

		const elem_type* elements() const noexcept {
			return elems.get();
		}

		std::string to_string() override;

		matrix row_reduce() const noexcept;
		matrix reduce() const noexcept;

		bool is_ref() const noexcept;
		bool is_rref() const noexcept;

		bool is_row_equivalent(const matrix& other) const noexcept;

		matrix get_row_vec(size_t i);
		matrix get_col_vec(size_t i);

		std::vector<matrix> get_rows();
		std::vector<matrix> get_cols();
	};

	HulaScript::instance::value make_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance);
	HulaScript::instance::value make_vector(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance);
	HulaScript::instance::value make_identity_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance);
	HulaScript::instance::value make_zero_matrix(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance);
}