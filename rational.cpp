#include <stdexcept>
#include <cassert>
#include "rational.h"

using namespace MatrixExplorer;

rational rational::parse(std::string str) {
	if (str.size() == 0) {
		throw std::invalid_argument("Str must have a length greater than zero.");
	}

	size_t pos = 0;

	bool is_negative = (str.at(pos) == '-');
	if (is_negative) {
		pos++;
		if (pos == str.size()) {
			throw std::invalid_argument("Negate cannot be followed by EOF.");
		}
	}

	while (pos < str.size() && str.at(pos) == '0') {
		pos++;
	}

	if (pos == str.size()) {
		return rational(0, 0, 0, flags::IS_ZERO);
	}

	uint64_t int_part = 0;
	uint16_t significant_int_digits = 1;
	while (pos < str.size() && str.at(pos) != '.') {
		char digit = str.at(pos);
		if (digit == '0') {
			if (significant_int_digits == UINT16_MAX) {
				throw std::invalid_argument("Overflow: Cannot have more that 2^16-1 significant digits.");
			}

			significant_int_digits++;
		}
		else if (digit >= '1' && digit <= '9') {
			while (significant_int_digits > 0) {
				if (int_part >= UINT16_MAX / 10) {
					throw std::invalid_argument("Overflow: Int part too big.");
				}

				int_part *= 10;
				significant_int_digits--;
			}
			significant_int_digits = 1;
			int_part += (digit - '0');
		}
		else {
			throw std::invalid_argument("Only digit characters (0-9) are valid.");
		}

		pos++;
	}
	significant_int_digits--;

	if (pos == str.size()) { //return int 
		return rational(int_part, 1, significant_int_digits);
	}
	
	assert(str.at(pos) == '.');
	pos++;

	uint16_t significant_frac_digits = 1;
	while (pos < str.size() && str.at(pos) == '0') {
		pos++;

		if (significant_frac_digits == UINT16_MAX) {
			throw std::invalid_argument("Overflow: To many zeros behind decimal.");
		}

		significant_frac_digits++;
	}

	if (pos == str.size()) { //return int.00 
		if (int_part == 0) {
			return rational(0, 0, 0, 0, 0, flags::IS_ZERO);
		}
		return rational(int_part, 1, significant_int_digits);
	}

	uint16_t temp_frac_digits = 1;
	uint64_t frac_part = 0;
	while (pos < str.size()) {
		char digit = str.at(pos);

		if (digit == '0') {
			if (temp_frac_digits == UINT16_MAX) {
				throw std::invalid_argument("Overflow: Fraction part too small.");
			}

			temp_frac_digits++;
		}
		else if (digit >= '1' && digit <= '9') {
			if (significant_frac_digits >= UINT16_MAX - temp_frac_digits) {
				throw std::invalid_argument("Overflow: To many zeros behind decimal.");
			}

			significant_frac_digits += temp_frac_digits;
			while (temp_frac_digits > 0) {
				frac_part *= 10;
				temp_frac_digits--;
			}
			temp_frac_digits = 1;
			frac_part += (digit -= '0');
		}
		else {
			throw std::invalid_argument("Only digit characters (0-9) are valid.");
		}
		 
		pos++;
	}

	if (int_part == 0) {
		return rational(frac_part, 1, significant_frac_digits, 1, 1, flags::SIGNIFICANT_DIGITS_IS_NEGATIVE);
	}
	else {
		uint64_t numerator = int_part;
		for (size_t i = 0; i < static_cast<size_t>(significant_int_digits) + static_cast<size_t>(significant_frac_digits); i++) {
			if (numerator >= UINT64_MAX / 10) {
				throw std::invalid_argument("Overflow: Numerator too large.");
			}
			numerator *= 10;
		}
		numerator = numerator + frac_part;
		return rational(numerator, 1, significant_frac_digits, 1, 1, flags::SIGNIFICANT_DIGITS_IS_NEGATIVE);
	}
}

rational::rational(uint64_t numerator_, uint64_t denominator_, uint16_t significant_digits_, uint16_t exponent, uint16_t root, flags my_flags) : numerator(numerator_), denominator(denominator_), significant_digits(significant_digits_), exponent(exponent), root(root), my_flags(my_flags) {
	if (!(my_flags & flags::IS_ZERO)) {
		if (denominator == 0) {
			throw std::invalid_argument("Denominator cannot be zero.");
		}
		if (root == 0) {
			throw std::invalid_argument("Numerator cannot be zero.");
		}
	}

	uint64_t common = gcd(numerator, denominator);
	numerator /= common;
	denominator /= common;

	if (significant_digits != 0) {
		if (my_flags & flags::SIGNIFICANT_DIGITS_IS_NEGATIVE) {
			uint16_t old_sig_digits = significant_digits;
			for (uint_fast16_t i = 0; i < old_sig_digits; i++) {
				uint64_t common = gcd(numerator, 10);

				if (common == 1) {
					break;
				}

				numerator /= common;
				denominator *= (10 / common);
				significant_digits--;
			}
		}
		else {
			uint16_t old_sig_digits = significant_digits;
			for (uint_fast16_t i = 0; i < old_sig_digits; i++) {
				uint64_t common = gcd(denominator, 10);
				
				numerator *= (10 / common);
				denominator /= common;
				significant_digits--;
			}
		}
	}

	uint64_t exp_common = gcd(exponent, root);
	this->exponent /= exp_common;
	this->root /= exp_common;
}

std::string MatrixExplorer::rational::to_string()
{
	if (my_flags & flags::IS_ZERO) {
		return "0";
	}
	else {
		std::string s;
		if (my_flags & flags::NUMERATOR_IS_NEGATIVE) {
			s.push_back('-');
		}

		bool encapsulate_paren = false;
		if (exponent != 1 || root != 1) {
			encapsulate_paren = (denominator != 1);
			s.push_back('(');
		}

		if (my_flags & flags::SIGNIFICANT_DIGITS_IS_NEGATIVE) {
			std::string s2;
			write_int(s2, numerator);

			if (significant_digits >= s2.length()) {
				s.push_back('0');
				s.push_back('.');
				for (size_t i = s2.length(); i < significant_digits; i++) {
					s.push_back('0');
				}
				s.append(s2);
			}
			else {
				size_t pos = s.length() - significant_digits;
				for (size_t i = 0; i < s2.length(); i++) {
					if (i == pos) {
						s.push_back('.');
					}
					s.push_back(s2.at(i));
				}
			}
		}
		else {
			write_int(s, numerator);
			for (size_t i = 0; i < significant_digits; i++) {
				s.push_back('0');
			}
		}

		if (denominator != 1) {
			s.push_back('/');
			write_int(s, denominator);
		}

		if (encapsulate_paren) {
			s.push_back(')');
		}

		if (exponent != 1 || root != 1) {
			s.push_back('^');

			if (root == 1) {
				write_int(s, exponent);
			}
			else {
				write_int(s, exponent);
			}
		}
	}
}