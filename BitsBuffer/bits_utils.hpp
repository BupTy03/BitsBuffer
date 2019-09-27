#pragma once
#ifndef BITS_UTILS_HPP
#define BITS_UTILS_HPP

#include <type_traits>

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr inline bool get_bit(T bits, std::size_t index) noexcept
{
	return bits & (1 << ((8 * sizeof(T)) - index - 1));
}

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr inline T clear_bit(T bits, std::size_t index, bool value) noexcept
{
	return bits & (~(1 << ((8 * sizeof(T)) - index - 1)));
}

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr inline T set_bit(T bits, std::size_t index, bool value) noexcept
{
	constexpr auto bits_count = 8 * sizeof(T);
	bits &= (~(1 << (bits_count - index - 1))); // clear bit
	return bits | (value << (bits_count - index - 1)); // set bit
}

/*
// normal version of insert_bits function
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr T insert_bits(T bits, std::size_t index, std::size_t count, bool value)
{
	constexpr auto bits_count = 8 * sizeof(T);
	const auto last_inserted_index = index + count;

	T restoring_mask = ~0;
	T values_mask = ~0;
	if (value)
	{
		restoring_mask = bits | (restoring_mask >> index);
		values_mask <<= bits_count - last_inserted_index;

		bits >>= count;

		bits |= values_mask;
		bits &= restoring_mask;
	}
	else
	{
		restoring_mask = bits & (restoring_mask << (bits_count - index));
		values_mask >>= last_inserted_index;

		bits >>= count;

		bits &= values_mask;
		bits |= restoring_mask;
	}

	return bits;
}
*/

// optimized version of insert_bits function
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr inline T insert_bits(T bits, std::size_t index, std::size_t count, bool value) noexcept
{
	constexpr auto bits_count = 8 * sizeof(T);
	const auto last_inserted_index = index + count;

	constexpr T mask = ~0;
	return value ? (((bits >> count) | ((mask << (bits_count - last_inserted_index)))) & (bits | ((mask >> index))))
		: ((bits >> count) & (mask >> last_inserted_index)) | (bits & (mask << (bits_count - index)));
}


#endif // !BITS_UTILS_HPP