//#include "bits_array.hpp"
#include <iostream>


template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr bool get_bit(T bits, std::size_t index) 
{ 
	constexpr auto bits_count = 8 * sizeof(T);
	return bits & (1 << (bits_count - index - 1));
}

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr T clear_bit(T bits, std::size_t index, bool value)
{
	constexpr auto bits_count = 8 * sizeof(T);
	return bits & (~(1 << (bits_count - index - 1)));
}

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr T set_bit(T bits, std::size_t index, bool value)
{
	constexpr auto bits_count = 8 * sizeof(T);
	bits &= (~(1 << (bits_count - index - 1))); // clear bit
	return bits | (value << (bits_count - index - 1)); // set bit
}

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr T insert_bits(T bits, std::size_t index, std::size_t count, bool value)
{
	constexpr auto bits_count = 8 * sizeof(T);
	const auto last_inserted_index = index + count;

	if (value)
	{
		T restoring_mask = ~0;
		restoring_mask = bits | (restoring_mask >> index);

		T values_mask = ~0;
		values_mask <<= bits_count - last_inserted_index;

		bits >>= count;

		bits |= values_mask;
		bits &= restoring_mask;
	}
	else
	{
		T restoring_mask = ~0;
		restoring_mask = bits & (restoring_mask << (bits_count - index));

		T values_mask = ~0;
		values_mask >>= last_inserted_index;

		bits >>= count;

		bits &= values_mask;
		bits |= restoring_mask;
	}

	return bits;
}

int main()
{
	std::uint8_t bits = 0;
	bits = insert_bits(bits, 2, 3, 1);

	std::cout << "bits: ";
	for (int i = 0; i < 8; ++i) {
		std::cout << get_bit(bits, i);
	}
	std::cout << std::endl;

	return 0;
}