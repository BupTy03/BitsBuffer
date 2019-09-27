#pragma once
#ifndef BITS_ARRAY_HPP
#define BITS_ARRAY_HPP

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <iterator>
#include <cassert>


template<class Container>
auto at(Container& cont, std::size_t index)
{
	if (index >= std::size(cont)) {
		throw std::out_of_range{ "index is out of range" };
	}

	return cont[index];
}

template<class Container>
auto at(const Container& cont, std::size_t index)
{
	if (index >= std::size(cont)) {
		throw std::out_of_range{ "index is out of range" };
	}

	return cont[index];
}

template<class Container>
bool empty(const Container& cont) { return std::begin(cont) == std::end(cont); }

template<class Container, class T>
void add(Container& cont, const T& val) { cont.push_back(val); }

template<class Container, class T>
void clear(Container& cont) { cont.clear(); }

template<class Container>
auto front(Container& cont)
{
	if (empty(cont)) {
		throw std::out_of_range{ "container is empty" };
	}
	return *(cont.begin());
}

template<class Container>
auto front(const Container& cont)
{
	if (empty(cont)) {
		throw std::out_of_range{ "container is empty" };
	}
	return *(cont.cbegin());
}

template<class Container>
auto back(Container& cont)
{
	if (empty(cont)) {
		throw std::out_of_range{ "container is empty" };
	}
	return *(cont.cend() - 1);
}

template<class Container>
auto back(const Container& cont)
{
	if (empty(cont)) {
		throw std::out_of_range{ "container is empty" };
	}
	return *(cont.cend() - 1);
}

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
class bits_array {
	class reference_impl;
	class pointer_impl;
	class const_pointer_impl;

	class iterator_impl;
	class const_iterator_impl;

	friend class iterator_impl;
	friend class const_iterator_impl;

public:
	using bits_container_type = T;
	using value_type = bool;
	using size_type = std::uint8_t;
	using difference_type = int;
	using reference = reference_impl;
	using const_reference = bool;
	using pointer = pointer_impl;
	using const_pointer = const_pointer_impl;

	using iterator = iterator_impl;
	using const_iterator = const_iterator_impl;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
	static constexpr auto max_size = 8 * sizeof(T);

	explicit bits_array() = default;
	explicit bits_array(size_type sz) : size_{ sz } { if (sz > max_size) throw std::out_of_range{ "size is greater than maximum allowed" }; }
	explicit bits_array(size_type sz, bool val) : size_{ sz }
	{
		if (sz > max_size) throw std::out_of_range{ "size is greater than maximum allowed" };
		if (!val) return;

		bits_ |= (~(0 << sz)) << (max_size - sz);
	}

	constexpr reference operator[](std::size_t bitIndex) { return reference{ bits_, static_cast<size_type>(max_size - bitIndex - 1) }; }
	constexpr bool operator[](std::size_t bitIndex) const { return (bits_ & (1 << (max_size - bitIndex - 1))); }

	constexpr iterator insert(const_iterator it, size_type count, bool val)
	{
		const auto index = it - cbegin();
		if (index < 0 || index > size_) {
			throw std::out_of_range{ "iterator is out of range" };
		}

		if (size_ + count > max_size) {
			throw std::out_of_range{ "size is greater than maximum allowed" };
		}

		bits_container_type mask = ~0;
		mask >>= index;
		mask = ~mask;

		const bits_container_type saved = bits_ & mask;
		bits_ >>= count;

		bits_container_type values_mask = ~0;
		values_mask <<= count;
		values_mask = val ? ~values_mask : values_mask;
		bits_ = val ? (bits_ | (values_mask << (max_size - index - count))) : (bits_ & (values_mask >> (index + count)));

		bits_ |= saved;
		size_ += count;

		return iterator{ *this, index - 1 };
	}
	constexpr iterator insert(const_iterator it, bool val) { return insert(it, 1, val); }
	template<typename InputIt, typename = std::enable_if_t<
		std::is_same_v<typename std::iterator_traits<InputIt>::iterator_category,
		typename std::iterator_traits<InputIt>::iterator_category>
		>>
		constexpr iterator insert(const_iterator it, InputIt first, InputIt last)
	{
		assert(first <= last);
		for (; first != last; ++first) {
			insert(it, 1, bool(*first));
		}
		return iterator{ *this, it - cbegin() };
	}

	constexpr iterator erase(const_iterator first, const_iterator last)
	{
		assert(first <= last);
		if (first == last) {
			return iterator{ *this, last - cbegin() };
		}

		const auto indexFirst = first - cbegin();
		check_index(indexFirst);

		const auto indexLast = last - cbegin();
		check_index(indexLast - 1);

		const auto count = indexLast - indexFirst;
		bits_container_type mask = ~0;
		mask >>= indexFirst;
		mask = ~mask;

		const bits_container_type saved = bits_ & mask;
		bits_ <<= count;

		size_ -= count;
		bits_ |= saved;
		return iterator{ *this, indexLast };
	}
	constexpr iterator erase(const_iterator it) { return erase(it, it + 1); }

	constexpr void push_back(bool value)
	{
		if (size_ == max_size) {
			throw std::out_of_range{ "size exceeded maximum allowed" };
		}

		bits_ |= (value << (max_size - size_ - 1));
		++size_;
	}
	constexpr void pop_back()
	{
		if (size_ == 0) {
			throw std::out_of_range{ "bit_array is empty" };
		}
		--size_;
	}

	constexpr size_type size() const { return size_; }
	constexpr void clear()
	{
		bits_ = 0;
		size_ = 0;
	}

	constexpr iterator begin() { return iterator{ *this, 0 }; }
	constexpr iterator end() { return iterator{ *this, size_ }; }

	constexpr const_iterator cbegin() const { return const_iterator{ const_cast<bits_array&>(*this), 0 }; }
	constexpr const_iterator cend() const { return const_iterator{ const_cast<bits_array&>(*this), size_ }; }

	constexpr const_iterator begin() const { return cbegin(); }
	constexpr const_iterator end() const { return cend(); }

private: // reference implementation
	class reference_impl {
		friend class bits_array<T>;
		friend class iterator_impl;
	private:
		explicit constexpr reference_impl(bits_container_type& bits_cont, size_type index_from_end)
			: bits_cont_{ &bits_cont }, index_from_end_{ index_from_end } {}
	public:
		constexpr reference_impl& operator=(bool value) { (*bits_cont_) |= (value << index_from_end_); return *this; }
		constexpr operator bool() { return ((*bits_cont_) & (1 << index_from_end_)); }

		constexpr friend void swap(reference_impl left, reference_impl right)
		{
			const bool tmp = bool(left);
			left = bool(right);
			right = tmp;
		}

	private:
		bits_container_type* bits_cont_ = nullptr;
		size_type index_from_end_ = 0;
	};

private: // pointers implementation
	class pointer_impl {
		friend class bits_array<T>;
		friend class iterator_impl;
	private:
		explicit constexpr pointer_impl(bits_container_type& bits_cont, size_type index_from_end)
			: bits_cont_{ &bits_cont }, index_from_end_{ index_from_end } {}

	public:
		constexpr reference_impl operator*() { return reference_impl{ *bits_cont_, index_from_end_ }; }
		constexpr reference_impl operator->() { return reference_impl{ *bits_cont_, index_from_end_ }; }

		constexpr operator bool() const { return bits_cont_ != nullptr; }

		constexpr bool operator==(decltype(nullptr)) const { return bits_cont_ == nullptr; }
		constexpr bool operator!=(decltype(nullptr)) const { return bits_cont_ != nullptr; }

	private:
		bits_container_type* bits_cont_ = nullptr;
		size_type index_from_end_ = 0;
	};

	class const_pointer_impl {
		friend class bits_array<T>;
		friend class const_iterator_impl;
	private:
		explicit constexpr const_pointer_impl(bits_container_type& bits_cont, size_type index_from_end)
			: bits_cont_{ &bits_cont }, index_from_end_{ index_from_end } {}

	public:
		constexpr bool operator*() { return ((*bits_cont_) | (1 << index_from_end_)); }
		constexpr bool operator->() { return ((*bits_cont_) | (1 << index_from_end_)); }

		constexpr operator bool() const { return bits_cont_ != nullptr; }

		constexpr bool operator==(decltype(nullptr)) const { return bits_cont_ == nullptr; }
		constexpr bool operator!=(decltype(nullptr)) const { return bits_cont_ != nullptr; }

	private:
		bits_container_type* bits_cont_ = nullptr;
		size_type index_from_end_ = 0;
	};

private: // iterators
	class iterator_impl : public std::iterator<std::random_access_iterator_tag, bool, int, pointer_impl, reference_impl> {
		friend class bits_array<T>;
		friend class const_iterator_impl;

		explicit constexpr iterator_impl(bits_array& context, int index)
			: context_{ &context }, index_{ index } {}

	public:
		explicit constexpr iterator_impl() = default;

		constexpr iterator_impl& operator++() { ++index_; return *this; }
		constexpr iterator_impl operator++(int) { auto result = *this; ++(*this); return result; }

		constexpr iterator_impl& operator--() { --index_; return *this; }
		constexpr iterator_impl operator--(int) { auto result = *this; --(*this); return result; }

		constexpr iterator_impl& operator+=(difference_type shift) { index_ += shift; return *this; }
		constexpr iterator_impl operator+(difference_type shift) const { auto result = *this; result += shift; return result; }

		constexpr iterator_impl& operator-=(difference_type shift) { index_ -= shift; return *this; }
		constexpr iterator_impl operator-(difference_type shift) const { auto result = *this; result -= shift; return result; }

		constexpr difference_type operator-(iterator_impl other) const { return index_ - other.index_; }

		constexpr reference_impl operator*() const
		{
			assert(context_ != nullptr);
			assert((index_ >= 0 && index_ < context_->size_));
			return reference_impl{ context_->bits_, static_cast<size_type>(max_size - index_ - 1) };
		}
		constexpr pointer_impl operator->() const { return pointer_impl{ context_->bits_, static_cast<size_type>(max_size - index_ - 1) }; }
		constexpr reference_impl operator[](difference_type n) const { return *(*this + n); }

		constexpr bool operator<(iterator_impl other) { return (*this - other) < 0; }
		constexpr bool operator>(iterator_impl other) { return (*this - other) > 0; }

		constexpr bool operator==(iterator_impl other) { return (*this - other) == 0; }
		constexpr bool operator!=(iterator_impl other) { return !(*this == other); }

		constexpr bool operator<=(iterator_impl other) { return !(*this > other); }
		constexpr bool operator>=(iterator_impl other) { return !(*this < other); }

	private:
		bits_array* context_ = nullptr;
		difference_type index_ = 0;
	};

	class const_iterator_impl : public std::iterator<std::random_access_iterator_tag, bool, int, const_pointer_impl, bool> {
		friend class bits_array<T>;

	private:
		explicit constexpr const_iterator_impl(bits_array& context, difference_type index)
			: context_{ &context }, index_{ index } {}

	public:
		explicit constexpr const_iterator_impl() = default;
		explicit constexpr const_iterator_impl(const iterator_impl& other)
			: context_{ other.context_ }, index_{ other.index_ } {}

		constexpr const_iterator_impl& operator=(const iterator_impl& other)
		{
			context_ = other.context_;
			index_ = other.index_;
			return *this;
		}

		constexpr const_iterator_impl(const const_iterator_impl&) = default;
		constexpr const_iterator_impl& operator=(const const_iterator_impl&) = default;

		constexpr const_iterator_impl(const_iterator_impl&&) = default;
		constexpr const_iterator_impl& operator=(const_iterator_impl&&) = default;

		constexpr const_iterator_impl& operator++() { ++index_; return *this; }
		constexpr const_iterator_impl operator++(int) { auto result = *this; ++(*this); return result; }

		constexpr const_iterator_impl& operator--() { --index_; return *this; }
		constexpr const_iterator_impl operator--(int) { auto result = *this; --(*this); return result; }

		constexpr const_iterator_impl& operator+=(difference_type shift) { index_ += shift; return *this; }
		constexpr const_iterator_impl operator+(difference_type shift) const { auto result = *this; result += shift; return result; }

		constexpr const_iterator_impl& operator-=(difference_type shift) { index_ -= shift; return *this; }
		constexpr const_iterator_impl operator-(difference_type shift) const { auto result = *this; result -= shift; return result; }

		constexpr difference_type operator-(const_iterator_impl other) const { return index_ - other.index_; }

		constexpr bool operator*() const
		{
			assert(context_ != nullptr);
			assert((index_ >= 0 && index_ < context_->size_));
			return (context_->bits_ | (1 << (max_size - index_ - 1)));
		}
		constexpr const_pointer_impl operator->() const { return const_pointer_impl{ context_->bits_, static_cast<size_type>(max_size - index_ - 1) }; }
		constexpr bool operator[](difference_type n) const { return *(*this + n); }

		constexpr bool operator<(const_iterator_impl other) { return (*this - other) < 0; }
		constexpr bool operator>(const_iterator_impl other) { return (*this - other) > 0; }

		constexpr bool operator==(const_iterator_impl other) { return (*this - other) == 0; }
		constexpr bool operator!=(const_iterator_impl other) { return !(*this == other); }

		constexpr bool operator<=(const_iterator_impl other) { return !(*this > other); }
		constexpr bool operator>=(const_iterator_impl other) { return !(*this < other); }

	private:
		bits_array* context_ = nullptr;
		difference_type index_ = 0;
	};

private:
	void check_index(difference_type index) { if (index < 0 || index >= size_) throw std::out_of_range{ "index is out of range" }; }

private:
	size_type size_{ 0 };
	bits_container_type bits_{ 0 };
};

#endif // !BITS_ARRAY_HPP
