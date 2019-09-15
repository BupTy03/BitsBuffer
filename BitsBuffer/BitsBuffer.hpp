#pragma once
#ifndef BITS_BUFFER
#define BITS_BUFFER

#include <vector>

constexpr int BITS_IN_BYTE = 8;
constexpr int MAX_BIT_INDEX = BITS_IN_BYTE - 1;

using byte_type = std::uint8_t;

class BitsBuffer
{
	using bytes = std::vector<byte_type>;
public:
	using value_type = bool;
	using allocator_type = typename bytes::allocator_type;
	using size_type = typename bytes::size_type;
	using diffetence_type = typename bytes::difference_type;
	// using reference;
	// using const_reference;
	// using pointer;
	// using const_pointer;
	// using iterator;
	// using const_iterator;
	// using reverse_iterator;
	// using const_reverse_iterator;

public:
	std::size_t size() const { return ((std::size(storage_) - 1) * BITS_IN_BYTE) + currBit_; }

	bool at(std::size_t index) const;

	void push_back(bool val);
	void pop_back();

private:
	void range_check(std::size_t index) const;

private:
	bytes storage_;
	std::uint8_t currBit_ = BITS_IN_BYTE;
};

#endif // !BITS_BUFFER
