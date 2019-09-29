#include "bits_array.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

template<class BitsContainer, typename = std::enable_if_t<std::is_same_v<typename BitsContainer::value_type, bool>>>
std::ostream& operator<<(std::ostream& os, const BitsContainer& cont)
{
	os << "{ ";
	std::copy(std::cbegin(cont), std::cend(cont), std::ostream_iterator<bool>(os, ", "));
	os << '}';
	return os;
}

int main()
{
	const auto generator = [] { static bool flag = 0; flag = !flag; return flag; };

	constexpr auto defaultSize = 32;
	constexpr auto defaultValue = 1;

	auto beginTime = std::chrono::system_clock::now();

	std::vector<bool> vec(defaultSize, defaultValue);
	std::generate(std::begin(vec), std::end(vec), generator);
	std::sort(std::begin(vec), std::end(vec));
	vec.resize(defaultSize / 2);

	auto endTime = std::chrono::system_clock::now();
	std::cout << "std::vector<bool> time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - beginTime).count() << " ns." << std::endl;


	beginTime = std::chrono::system_clock::now();

	bits_array<std::uint32_t> arr(defaultSize, defaultValue);
	std::generate(std::begin(arr), std::end(arr), generator);
	std::sort(std::begin(arr), std::end(arr));
	arr.resize(defaultSize / 2);
	endTime = std::chrono::system_clock::now();
	std::cout << "bits_array<std::uint32_t> time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - beginTime).count() << " ns." << std::endl;

	std::cout << "sizeof(std::vector<bool>): " << sizeof(std::vector<bool>) << std::endl;
	std::cout << "sizeof(bits_array<std::uint32_t>): " << sizeof(bits_array<std::uint32_t>) << std::endl;

	std::cout << "std::vector<bool>: " << vec << std::endl;
	std::cout << "bits_array<std::uint32_t>: " << arr << std::endl;
	std::cout << "vec == arr: " << std::boolalpha << std::equal(std::cbegin(vec), std::cend(vec), std::cbegin(arr)) << std::endl;

	return 0;
}