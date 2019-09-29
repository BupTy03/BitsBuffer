#include "pch.h"
#include "..//BitsBuffer/bits_array.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

// #define PRINT_VALUES

template<class Container1, class Container2>
bool check_equality(const Container1& cont1, const Container2& cont2) { return std::equal(std::cbegin(cont1), std::cend(cont1), std::cbegin(cont2)); }

template<class Container1, class Container2>
bool check_equal_sizes(const Container1& cont1, const Container2& cont2) { return std::size(cont1) == std::size(cont2); }

template<class BitsContainer, typename = std::enable_if_t<std::is_same_v<typename BitsContainer::value_type, bool>>>
std::ostream& operator<<(std::ostream& os, const BitsContainer& cont) 
{
	os << "{ ";
	std::copy(std::cbegin(cont), std::cend(cont), std::ostream_iterator<bool>(os, ", "));
	os << '}';
	return os;
}

template<class BitsContainer1, class BitsContainer2>
void check_containers_equality(const BitsContainer1& expected, const BitsContainer2& actual) 
{
#ifdef PRINT_VALUES
	std::cout << "Expected: " << expected << std::endl;
	std::cout << "Actual: " << actual << std::endl;
#endif // PRPRINT_VALUES

	EXPECT_TRUE(check_equal_sizes(expected, actual));
	EXPECT_TRUE(check_equality(expected, actual));
}

template<class BitsCont>
void check_inserting_one_val(std::vector<bool>& expected, bits_array<BitsCont>& actual, 
	typename std::vector<bool>::const_iterator expectedIt, typename bits_array<BitsCont>::const_iterator actualIt)
{
	// inserting one value
	const auto expectedInserted = expected.insert(expectedIt, 1);
	const auto actualInserted = actual.insert(actualIt, 1);

	EXPECT_EQ(std::distance(expected.begin(), expectedInserted), std::distance(actual.begin(), actualInserted));
	check_containers_equality(expected, actual);
}

template<class BitsCont>
void check_cleaning(std::vector<bool>& expected, bits_array<BitsCont>& actual)
{
	// check clean
	expected.clear();
	actual.clear();
	EXPECT_TRUE(expected.empty());
	EXPECT_EQ(expected.size(), 0);

	EXPECT_TRUE(actual.empty());
	EXPECT_EQ(actual.size(), 0);
	check_containers_equality(expected, actual);
}

template<class BitsCont>
void check_inserting_count_val(std::vector<bool>& expected, bits_array<BitsCont>& actual,
	typename std::vector<bool>::const_iterator expectedIt, typename bits_array<BitsCont>::const_iterator actualIt)
{
	// inserting count values
	constexpr auto count_values = 5;
	const auto expectedInserted = expected.insert(expectedIt, count_values, 1);
	const auto actualInserted = actual.insert(actualIt, count_values, 1);

	EXPECT_EQ(std::distance(expected.begin(), expectedInserted), std::distance(actual.begin(), actualInserted));
	check_containers_equality(expected, actual);
}

template<class BitsCont>
void check_inserting_range(std::vector<bool>& expected, bits_array<BitsCont>& actual,
	typename std::vector<bool>::const_iterator expectedIt, typename bits_array<BitsCont>::const_iterator actualIt)
{
	// inserting range
	const std::initializer_list<bool> lst = { 1, 0, 1, 1, 0, 0, 1, 0 };
	const auto expectedInserted = expected.insert(expectedIt, lst.begin(), lst.end());
	const auto actualInserted = actual.insert(actualIt, lst.begin(), lst.end());

	EXPECT_EQ(std::distance(expected.begin(), expectedInserted), std::distance(actual.begin(), actualInserted));
	check_containers_equality(expected, actual);
}

TEST(Inserting, BeforeBegin) {
	std::vector<bool> expected;
	bits_array<std::uint16_t> actual;

	check_inserting_one_val(expected, actual, expected.cbegin(), actual.cbegin());
	check_cleaning(expected, actual);
	check_inserting_count_val(expected, actual, expected.cbegin(), actual.cbegin());
	check_inserting_range(expected, actual, expected.cbegin(), actual.cbegin());
}

TEST(Inserting, BeforeEnd) {
	std::vector<bool> expected;
	bits_array<std::uint16_t> actual;

	check_inserting_one_val(expected, actual, expected.cend(), actual.cend());
	check_cleaning(expected, actual);
	check_inserting_count_val(expected, actual, expected.cend(), actual.cend());
	check_inserting_range(expected, actual, expected.cend(), actual.cend());
}

TEST(Inserting, BeforeMiddle) {
	constexpr auto defaultSize = 4;
	constexpr bool defaultValue = 0;

	std::vector<bool> expected(defaultSize, defaultValue);
	bits_array<std::uint32_t> actual(defaultSize, defaultValue);

	constexpr auto denom = 2;
	constexpr auto shift = defaultSize / denom;

	check_inserting_one_val(expected, actual, expected.cbegin() + shift, actual.cbegin() + shift);
	check_inserting_count_val(expected, actual, expected.cbegin() + shift, actual.cbegin() + shift);
	check_inserting_range(expected, actual, expected.cbegin() + shift, actual.cbegin() + shift);
}

template<class BitsCont>
void check_erasing(std::vector<bool>& expected, bits_array<BitsCont>& actual,
	typename std::vector<bool>::const_iterator expectedIt, typename bits_array<BitsCont>::const_iterator actualIt)
{
	const auto expectedErased = expected.erase(expectedIt);
	const auto actualErased = actual.erase(actualIt);

	EXPECT_EQ(std::distance(expected.begin(), expectedErased), std::distance(actual.begin(), actualErased));
	check_containers_equality(expected, actual);
}

template<class BitsCont>
void check_erasing_range(std::vector<bool>& expected, bits_array<BitsCont>& actual,
	typename std::vector<bool>::const_iterator expectedBegin, typename std::vector<bool>::const_iterator expectedEnd,
	typename bits_array<BitsCont>::const_iterator actualBegin, typename bits_array<BitsCont>::const_iterator actualEnd)
{
	const auto expectedErased = expected.erase(expectedBegin, expectedEnd);
	const auto actualErased = actual.erase(actualBegin, actualEnd);

	EXPECT_EQ(std::distance(expected.begin(), expectedErased), std::distance(actual.begin(), actualErased));
	check_containers_equality(expected, actual);
}

TEST(Erasing, Begin) {
	constexpr auto defaultSize = 10;
	constexpr auto defaultValue = 1;

	std::vector<bool> expected(defaultSize, defaultValue);
	bits_array<std::uint32_t> actual(defaultSize, defaultValue);

	check_erasing(expected, actual, expected.cbegin(), actual.cbegin());
}

TEST(Erasing, BeforeEnd) {
	constexpr auto defaultSize = 15;
	constexpr auto defaultValue = 1;

	std::vector<bool> expected(defaultSize, defaultValue);
	bits_array<std::uint32_t> actual(defaultSize, defaultValue);

	check_erasing(expected, actual, expected.cend() - 1, actual.cend() - 1);
}

TEST(Erasing, Middle) {
	constexpr auto defaultSize = 10;
	constexpr auto defaultValue = 1;

	std::vector<bool> expected(defaultSize, defaultValue);
	bits_array<std::uint32_t> actual(defaultSize, defaultValue);

	constexpr auto shift = defaultSize / 2;

	check_erasing(expected, actual, expected.cbegin() + shift, actual.cbegin() + shift);
}

TEST(Erasing, Range) {
	constexpr auto defaultSize = 30;
	constexpr auto defaultValue = 1;

	std::vector<bool> expected(defaultSize, defaultValue);
	bits_array<std::uint32_t> actual(defaultSize, defaultValue);

	constexpr auto shift = defaultSize / 2;
	check_erasing_range(expected, actual, expected.cbegin() + shift, expected.cend(), actual.cbegin() + shift, actual.cend());
	check_erasing_range(expected, actual, expected.cbegin(), expected.cend(), actual.cbegin(), actual.cend());
}

template<class BitsCont>
void check_push_back(std::vector<bool>& expected, bits_array<BitsCont>& actual, std::size_t count)
{
	for (std::size_t i = 0; i < count; ++i) {
		expected.push_back(i & 1);
		actual.push_back(i & 1);
	}
	check_containers_equality(expected, actual);
}

template<class BitsCont>
void check_push_back(std::vector<bool>& expected, bits_array<BitsCont>& actual, std::size_t count, bool value)
{
	for (std::size_t i = 0; i < count; ++i) {
		expected.push_back(value);
		actual.push_back(value);
	}
	check_containers_equality(expected, actual);
}

TEST(PushBack, Trivial) {
	constexpr auto defaultSize = 0;
	constexpr auto defaultValue = 0;
	constexpr auto count = 15;
	constexpr auto countZeros = 3;
	constexpr auto countOnes = 5;

	std::vector<bool> expected(defaultSize, defaultValue);
	bits_array<std::uint32_t> actual(defaultSize, defaultValue);

	check_push_back(expected, actual, count);
	check_push_back(expected, actual, countZeros, 0);
	check_push_back(expected, actual, countOnes, 1);
}

TEST(PopBack, Trivial) {
	const auto lst = {1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1};

	std::vector<bool> expected(lst.begin(), lst.end());
	bits_array<std::uint32_t> actual(lst.begin(), lst.end());
	check_containers_equality(expected, actual);

	const int count_to_pop = lst.size() / 2;
	for (int i = 0; i < count_to_pop; ++i) {
		expected.pop_back();
		actual.pop_back();
	}

	check_containers_equality(expected, actual);
}

template<class BitsCont>
void check_resize(std::vector<bool>& expected, bits_array<BitsCont>& actual, std::size_t count)
{
	expected.resize(count);
	actual.resize(count);
	check_containers_equality(expected, actual);
}

template<class BitsCont>
void check_resize(std::vector<bool>& expected, bits_array<BitsCont>& actual, std::size_t count, bool value)
{
	expected.resize(count, value);
	actual.resize(count, value);
	check_containers_equality(expected, actual);
}

TEST(Resize, Trivial) {
	const auto lst = { 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1 };
	std::vector<bool> expected(lst.begin(), lst.end());
	bits_array<std::uint32_t> actual(lst.begin(), lst.end());
	check_containers_equality(expected, actual);

	check_resize(expected, actual, expected.size());
	check_resize(expected, actual, expected.size() + 1);
	check_resize(expected, actual, expected.size() + 4);
	check_resize(expected, actual, expected.size() - 1);
	check_resize(expected, actual, expected.size() - 4);
	check_resize(expected, actual, expected.size() - 5);
	check_resize(expected, actual, expected.size() + 10, 1);
	check_resize(expected, actual, expected.size() + 3, 0);
}

TEST(Iterators, STLalgorithms) {
	std::vector<bool> expected;
	bits_array<std::uint32_t> actual;

	// checking copy
	const std::initializer_list<bool> lst = { 1, 0, 1, 1, 0, 0, 1, 0 };
	std::copy(lst.begin(), lst.end(), std::back_inserter(expected));
	std::copy(lst.begin(), lst.end(), std::back_inserter(actual));
	check_containers_equality(expected, actual);

	// checking reverse
	std::reverse(expected.begin(), expected.end());
	std::reverse(actual.begin(), actual.end());
	check_containers_equality(expected, actual);
}