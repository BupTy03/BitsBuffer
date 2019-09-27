//#include "bits_array.hpp"
#include "bits_utils.hpp"
#include <iostream>

int main()
{
	std::uint8_t bits = 0;
	bits = insert_bits(bits, 2, 3, 1);
	bits = erase_bits(bits, 2, 3);

	std::cout << "bits: ";
	for (int i = 0; i < 8; ++i) {
		std::cout << get_bit(bits, i);
	}
	std::cout << std::endl;

	return 0;
}