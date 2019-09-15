#include "BitsBuffer.hpp"

#include <iostream>

int main()
{
	BitsBuffer buffer;
	buffer.push_back(1);
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(1);
	buffer.push_back(1);
	buffer.push_back(1);
	buffer.push_back(1);
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(0);

	std::cout << "Size: " << buffer.size() << std::endl;

	buffer.pop_back();
	buffer.pop_back();
	buffer.pop_back();
	buffer.pop_back();
	buffer.pop_back();

	std::cout << "Size: " << buffer.size() << std::endl;
	for (std::size_t bitIndex = 0; bitIndex < buffer.size(); ++bitIndex) {
		std::cout << buffer.at(bitIndex);
	}

	std::cout << std::endl;

	return 0;
}