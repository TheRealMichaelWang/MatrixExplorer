#pragma once

#include <cstdint>
#include <string>

namespace MatrixExplorer {
	class rational {
	private:
		uint64_t numerator;
		uint32_t denominator;

		bool is_negate;

		//euclids method gcd
		static const uint64_t gcd(uint64_t a, uint64_t b) noexcept {
			return b == 0 ? a : gcd(b, a % b);
		}

		static const uint64_t lcm(uint64_t a, uint64_t b) noexcept {
			return (a / gcd(a, b)) * b; //not (a * b) / gcd to avoid overflow
		}

		static void write_int(std::string& dest, uint64_t a) {
			uint64_t to_print = a;
			size_t pos = dest.size();
			while (to_print > 0) {
				dest.push_back('0' + (to_print % 10));
				to_print /= 10;
			}
			std::reverse(dest.begin() + pos, dest.end());
		}

		rational(uint64_t numerator, uint32_t denominator, bool is_negate=false) noexcept(false) : numerator(numerator), denominator(denominator), is_negate(is_negate&& numerator != 0) {
			if (denominator == 0) {
				throw std::invalid_argument("Cannot divide by zero.");
			}
			
			uint64_t common = gcd(numerator, denominator);
			this->numerator /= common;
			this->denominator /= common;
		}

	public:
		rational(uint64_t integer) : numerator(integer), denominator(1), is_negate(false) { }
		rational() : rational(0) { }

		static rational parse(std::string str);
		std::string to_string(bool print_as_frac = false);

		const bool is_zero() const noexcept {
			return numerator == 0;
		}

		bool operator==(rational const& rat) {
			return numerator == rat.numerator && denominator == rat.denominator && is_negate == rat.is_negate;
		}

		bool operator!=(rational const& rat) {
			return numerator != rat.numerator || denominator != rat.denominator || is_negate != rat.is_negate;
		}

		rational operator+(rational const& rat) {
			if (rat.is_zero()) {
				return *this;
			}

			uint32_t new_denom = lcm(denominator, rat.denominator);
			uint64_t a_num = numerator * (new_denom / denominator);
			uint64_t b_num = rat.numerator * (new_denom / rat.denominator);

			if (is_negate == rat.is_negate) {
				return rational(a_num + b_num, new_denom, is_negate);
			}
			
			if (a_num > b_num) {
				return rational(a_num - b_num, new_denom, is_negate);
			}
			else {
				return rational(b_num - a_num, new_denom, rat.is_negate);
			}
		}

		rational operator-(rational const& rat) {
			if (rat.is_zero()) {
				return *this;
			}

			uint32_t new_denom = lcm(denominator, rat.denominator);
			uint64_t a_num = numerator * (new_denom / denominator);
			uint64_t b_num = rat.numerator * (new_denom / rat.denominator);

			if (is_negate != rat.is_negate) {
				return rational(a_num + b_num, new_denom, is_negate);
			}
			
			if (a_num > b_num) {
				return rational(a_num - b_num, new_denom, is_negate);
			}
			else {
				return rational(b_num - a_num, new_denom, !is_negate);
			}
		}

		rational operator-() {
			return rational(numerator, denominator, !is_negate);
		}

		rational operator*(rational const& rat) {
			return rational(numerator * rat.numerator, denominator * rat.denominator, is_negate != rat.is_negate);
		}

		rational operator/(rational const& rat) {
			return rational(numerator * rat.denominator, denominator * rat.numerator, is_negate != rat.is_negate);
		}

		rational inverse() {
			return rational(denominator, numerator, is_negate);
		}

		double to_double() {
			return static_cast<double>(numerator) / static_cast<double>(denominator);
		}

		size_t compute_hash();
	};
}