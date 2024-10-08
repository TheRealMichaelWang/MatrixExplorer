#include <stdexcept>
#include <cassert>
#include "rational.h"
#include "hash.h"

using namespace MatrixExplorer;

rational rational::parse(std::string str) {
	uint64_t numerator = 0;
	uint32_t denominator = 1;

	bool is_negate = false;
	bool decimal_detected = false;
	for (size_t i = 0; i < str.size(); i++) {
		char c = str.at(i);
		if (c >= '0' && c <= '9') {
			if (numerator > UINT64_MAX / 10) {
				throw std::invalid_argument("Overflow: Numerator is too large.");
			}

			numerator *= 10;
			numerator += (c - '0');

			if (decimal_detected) {
				if (denominator > UINT32_MAX / 10) {
					throw std::invalid_argument("Overflow: Denominator is too large.");
				}
				denominator *= 10;
			}
		}
		else if (c == '.') {
			if (decimal_detected) {
				throw std::invalid_argument("Format: Two decimals detected.");
			}

			decimal_detected = true;
		}
		else if (c == '-') {
			if (is_negate) {
				throw std::invalid_argument("Format: Two negates detected.");
			}
			is_negate = true;
		}
		else {
			throw std::invalid_argument("Format: Must be digit (0-9).");
		}
	}

	return rational(numerator, denominator, is_negate);
}

std::string MatrixExplorer::rational::to_string(bool print_as_frac) {
	std::string s;
	if (is_negate) {
		s.push_back('-');
	}

	if (print_as_frac) {
		write_int(s, numerator);
		if (denominator != 1) {
			s.push_back('/');
			write_int(s, denominator);
		}
	}
	else {
		uint64_t denom10 = 1;
		size_t decimal_digits = 0;
		for (;;) {
			if(denom10 % denominator == 0) {
				break;
			}

			if (denom10 > UINT64_MAX / 10) {
				return to_string(true);
			}
			denom10 *= 10;
			decimal_digits++;
		}

		uint64_t factor = denom10 / denominator;
		if (numerator > UINT64_MAX / factor) {
			return to_string(true);
		}

		std::string s2;
		write_int(s2, numerator * factor);

		if (decimal_digits == 0) {
			return s2;
		}

		if (s2.length() < decimal_digits) {
			s.push_back('0');
			s.push_back('.');

			for (size_t i = 0; i < decimal_digits - s2.length(); i++) {
				s.push_back('0');
			}

			s.append(s2);
		}
		else {
			for (size_t i = 0; i < s2.length() - decimal_digits; i++) {
				s.push_back(s2.at(i));
			}
			s.push_back('.');
			for (size_t i = s2.length() - decimal_digits; i < s2.length(); i++) {
				s.push_back(s2.at(i));
			}
		}
	}

	return s;
}

size_t MatrixExplorer::rational::compute_hash() {
	size_t lhs = denominator;
	lhs = lhs << sizeof(bool);
	lhs += static_cast<size_t>(is_negate);
	return HulaScript::Hash::combine(numerator, lhs);
}
