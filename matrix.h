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
	
	public:
		matrix(size_t rows, size_t cols, std::vector<double> elems_vec) : rows(rows), cols(cols), elems(new double[elems_vec.size()]) {
			assert(elems_vec.size() == rows * cols);
			std::memcpy(elems.get(), elems_vec.data(), elems_vec.size());

			declare_method("get", get_elem);
			declare_method("set", set_elem);
		}
	};
}