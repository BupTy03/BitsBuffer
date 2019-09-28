//#include "bits_array.hpp"
#include "bits_utils.hpp"
#include <iostream>

int main()
{
	std::uint8_t bits = 0;
	for (int i = 0; i < 4; ++i) {
		bits = set_bit(bits, i, i & 1);
	}

	bits = insert_bits(bits, 2, 3, 1);
	std::cout << "bits: ";
	for (int i = 0; i < 8; ++i) {
		std::cout << get_bit(bits, i);
	}
	std::cout << std::endl;

	bits = erase_bits(bits, 2, 3);

	std::cout << "bits: ";
	for (int i = 0; i < 8; ++i) {
		std::cout << get_bit(bits, i);
	}
	std::cout << std::endl;

	return 0;
}