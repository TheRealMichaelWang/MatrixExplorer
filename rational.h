#pragma once

#include <cstdint>
#include <string>

namespace MatrixExplorer {
	class rational {
	private:
		enum flags : uint16_t {
			NONE = 0,
			NUMERATOR_IS_NEGATIVE = 1,
			EXPONENT_IS_NEGATIVE = 2,
			SIGNIFICANT_DIGITS_IS_NEGATIVE = 4,
			IS_ZERO = 8
		};

		//mantissa (n/d)
		uint64_t numerator;
		uint64_t denominator;
		
		//represents 10^(significant_digits)
		uint16_t significant_digits;

		//represents the ^(exp/root)
		uint16_t exponent;
		uint16_t root;

		//represents negations, etc...
		flags my_flags;

		//euclids
		static const uint64_t gcd(uint64_t a, uint64_t b) noexcept {
			return b == 0 ? a : gcd(b, a % b);
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

		rational(uint64_t numerator, uint64_t denominator, uint16_t significant_digits, uint16_t exponent = 1, uint16_t root = 1, flags my_flags = flags::NONE);
	public:
		static rational parse(std::string str);
		std::string to_string();
	};
}