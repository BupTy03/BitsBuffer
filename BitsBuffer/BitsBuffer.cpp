#include "BitsBuffer.hpp"

#include <stdexcept>

// !!! bits order: from left to right - from junior to senior
static constexpr bool get_bit(byte_type val, std::size_t index) { return val & (1 << (MAX_BIT_INDEX - index)); }
static constexpr byte_type set_bit(byte_type val, std::size_t index, bool bit_val) { return val | (bit_val << (MAX_BIT_INDEX - index)); }

bool BitsBuffer::at(std::size_t index) const
{
	range_check(index);
	return get_bit( storage_.at(index / BITS_IN_BYTE), index % BITS_IN_BYTE );
}

void BitsBuffer::push_back(bool val)
{
	if (currBit_ >= BITS_IN_BYTE) {
		storage_.push_back(0);
		currBit_ = 0;
	}

	storage_.back() = set_bit(storage_.back(), currBit_, val);
	++currBit_;
}

void BitsBuffer::pop_back()
{
	if (currBit_ == 0) {
		storage_.pop_back();
		currBit_ = MAX_BIT_INDEX;
	}
	else {
		--currBit_;
	}
}

void BitsBuffer::range_check(std::size_t index) const
{
	/*if ( !((index / BITS_IN_BYTE) < std::size(storage_) && (index % BITS_IN_BYTE) <= currBit_) ) {
		throw std::out_of_range{ "BitsBuffer: index is out of range!" };
	}*/
}
