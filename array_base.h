#pragma once

#include <typeinfo>
#include <vector>
#include <cassert>
#include <cstdarg>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <algorithm>
#include <type_traits>

#include "types.h"

namespace smip {

/**
 * @brief Container class for general (1-dim) Arrays
 *
 */
template <typename T>
class Array_base {
public:
    class iterator;
    // Provide an iterator type
    using iterator = typename Array_base<T>::iterator;
    // Provide a const_iterator type as well
    using const_iterator = const typename Array_base<T>::iterator;
    typedef T value_type;
    typedef T* ptr_type;
    typedef T& reference;
    typedef const T& const_reference;

    Array_base() = default;
    Array_base(const Array_base<T>& src);
    template <concept_arithmetic U>
    Array_base(const Array_base<U>& src);
    Array_base(Array_base<T>&& src);
    Array_base(size_t a_size);
    Array_base(size_t a_size, const T& def);
    Array_base(std::shared_ptr<T[]> data, std::size_t a_size);
    Array_base(std::shared_ptr<const T[]> data, std::size_t a_size);
    Array_base(std::initializer_list<T> l);
    ~Array_base();

    template <concept_arithmetic U>
    Array_base<T>& operator=(const Array_base<U>& other);
    Array_base<T>& operator=(Array_base<T>&& other);

    // Begin and end iterators
    iterator begin() { return iterator(_mem.get()); }
    iterator end() { return iterator(_mem.get() + _size); }
    // Const begin and end iterators
    const_iterator begin() const { return const_iterator(_mem.get()); }
    const_iterator end() const { return const_iterator(_mem.get() + _size); }

    std::shared_ptr<T[]>& data() { return _mem; }
    std::shared_ptr<const T[]> data() const { return std::const_pointer_cast<const T[]>(_mem); }

    // Provide access to the underlying data
    reference operator[](std::size_t index) { return _mem[index]; }
    const_reference operator[](std::size_t index) const { return _mem[index]; }

    reference at(std::size_t i)
    {
        assert(i < _size);
        return data().get()[i];
    }
    const_reference at(std::size_t i) const
    {
        assert(i < _size);
        return data().get()[i];
    }

public:
    std::size_t size() const { return _size; }
    std::size_t typesize() const { return sizeof(T); }
    void set_at(std::shared_ptr<T[]> data, std::size_t a_size);
    bool resize(std::size_t new_size);

    // Iterator types
    class iterator {
    private:
        T* ptr;
    public:
        using iterator_category = std::contiguous_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() = default;
        explicit iterator(T* p) : ptr(p) {}
        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }
        // Prefix increment
        iterator& operator++() { ++ptr; return *this; }
        // Postfix increment
        iterator operator++(int) { iterator temp = *this; ++(*this); return temp; }
        // Prefix decrement
        iterator& operator--() { --ptr; return *this; }
        // Postfix decrement
        iterator operator--(int) { iterator temp = *this; --(*this); return temp; }
        // Random access operations (contiguous)
        iterator& operator+=(difference_type n) { ptr += n; return *this; }
        iterator operator+(difference_type n) const { return iterator(ptr + n); }
        iterator& operator-=(difference_type n) { ptr -= n; return *this; }
        iterator operator-(difference_type n) const { return iterator(ptr - n); }
        reference operator[](difference_type n) const { return ptr[n]; }
        difference_type operator-(const iterator& other) const { return ptr - other.ptr; }
        bool operator==(const iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }
        bool operator<(const iterator& other) const { return ptr < other.ptr; }
        bool operator<=(const iterator& other) const { return ptr <= other.ptr; }
        bool operator>(const iterator& other) const { return ptr > other.ptr; }
        bool operator>=(const iterator& other) const { return ptr >= other.ptr; }
    };

protected:
    std::size_t _size { 0UL };
    bool _isReference { false };
    std::shared_ptr<T[]> _mem { nullptr, [](T* p) { delete[] p; } };
};

// *************************************************
// Member definitions / implementation part
// *************************************************

/*
 * class Array_base
 */
template <typename T>
Array_base<T>::Array_base(const Array_base<T>& src)
    : _size(src._size)
{
    if (_size == 0) {
        _mem.reset();
        return;
    }
    auto temp = std::make_unique<T[]>(_size);
    _mem.reset(temp.release());
    std::copy(src.begin(), src.end(), this->begin());
}

template <typename T>
template <concept_arithmetic U>
Array_base<T>::Array_base(const Array_base<U>& src)
    : _size(src._size)
{
    if (_size == 0) {
        _mem.reset();
        return;
    }
    auto temp = std::make_unique<T[]>(_size);
    _mem.reset(temp.release());
    std::transform(src.begin(), src.end(), this->begin(),
        [](const U& x) { return static_cast<T>(x); });
}

template <typename T>
Array_base<T>::Array_base(Array_base<T>&& src)
    : _size(src._size)
    , _isReference(src._isReference)
    , _mem(std::move(src._mem))
{
    src._size = 0;
    src._isReference = false;
}

template <typename T>
Array_base<T>::Array_base(std::size_t a_size)
    : _size(a_size)
{
    if (_size == 0) {
        _mem.reset();
        return;
    }
    auto temp = std::make_unique<T[]>(_size);
    _mem.reset(temp.release());
}

template <typename T>
Array_base<T>::Array_base(std::size_t a_size, const T& def)
    : _size(a_size)
{
    if (!_size)
        return;
    auto temp = std::make_unique<T[]>(_size);
    _mem.reset(temp.release());
    for (std::size_t i = 0; i < _size; i++)
        _mem.get()[i] = T(def);
}

template <typename T>
Array_base<T>::Array_base(std::shared_ptr<T[]> data, std::size_t a_size)
    : _size(a_size)
    , _mem(data)
{
}

template <typename T>
Array_base<T>::Array_base(std::initializer_list<T> l)
    : _size(l.size())
{
    if (_size == 0) {
        this->data().reset();
        return;
    }
    auto temp = std::make_unique<T[]>(_size);
    _mem.reset(temp.release());
    std::copy(l.begin(), l.end(), this->begin());
}

template <typename T>
Array_base<T>::~Array_base()
{
    _mem.reset();
}

template <typename T>
Array_base<T>& Array_base<T>::operator=(Array_base<T>&& other)
{
    if (this != &other) {
        _size = other._size;
        _isReference = other._isReference;
        _mem = std::move(other._mem);
        other._size = 0;
        other._isReference = false;
    }
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array_base<T>& Array_base<T>::operator=(const Array_base<U>& other)
{
    if (this == &other)
        return *this;
    if (this->_size != other._size) {
        _mem.reset();
        _size = other._size;
    }
    auto temp = std::make_unique<T[]>(_size);
    _mem.reset(temp.release());
    std::transform(other.begin(), other.end(), this->begin(),
        [](const U& x) { return static_cast<T>(x); });
    return *this;
}

template <typename T>
void Array_base<T>::set_at(std::shared_ptr<T[]> data, size_t a_size)
{
    _size = a_size;
    _mem = data;
    _isReference = true;
}

template <typename T>
bool Array_base<T>::resize(std::size_t new_size)
{
    if (_isReference)
        return false;
    auto temp = std::make_unique<T[]>(new_size);
    _mem.reset(temp.release());
    _size = new_size;
    return (_mem != nullptr);
}

} // namespace smip
