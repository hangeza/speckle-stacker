#pragma once

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <type_traits>
#include <typeinfo>
#include <vector>

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
    iterator begin() { return iterator(m_data.get()); }
    iterator end() { return iterator(m_data.get() + m_size); }
    // Const begin and end iterators
    const_iterator begin() const { return const_iterator(m_data.get()); }
    const_iterator end() const { return const_iterator(m_data.get() + m_size); }

    std::shared_ptr<T[]>& data() { return m_data; }
    std::shared_ptr<const T[]> data() const { return std::const_pointer_cast<const T[]>(m_data); }

    // Provide access to the underlying data
    reference operator[](std::size_t index) { return m_data[index]; }
    const_reference operator[](std::size_t index) const { return m_data[index]; }

    reference at(std::size_t i)
    {
        assert(i < m_size);
        return data().get()[i];
    }
    const_reference at(std::size_t i) const
    {
        assert(i < m_size);
        return data().get()[i];
    }

public:
    std::size_t size() const { return m_size; }
    std::size_t typesize() const { return sizeof(T); }
    void set_at(std::shared_ptr<T[]> data, std::size_t a_size);
    void resize(std::size_t new_size);

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
        explicit iterator(T* p)
            : ptr(p)
        {
        }
        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }
        // Prefix increment
        iterator& operator++()
        {
            ++ptr;
            return *this;
        }
        // Postfix increment
        iterator operator++(int)
        {
            iterator temp = *this;
            ++(*this);
            return temp;
        }
        // Prefix decrement
        iterator& operator--()
        {
            --ptr;
            return *this;
        }
        // Postfix decrement
        iterator operator--(int)
        {
            iterator temp = *this;
            --(*this);
            return temp;
        }
        // Random access operations (contiguous)
        iterator& operator+=(difference_type n)
        {
            ptr += n;
            return *this;
        }
        iterator operator+(difference_type n) const { return iterator(ptr + n); }
        iterator& operator-=(difference_type n)
        {
            ptr -= n;
            return *this;
        }
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
    std::size_t m_size { 0UL };
    bool m_is_reference { false };
    std::shared_ptr<T[]> m_data { nullptr, [](T* p) { delete[] p; } };
};

// *************************************************
// Member definitions / implementation part
// *************************************************

/*
 * class Array_base
 */
template <typename T>
Array_base<T>::Array_base(const Array_base<T>& src)
    : m_size(src.m_size)
{
    if (m_size == 0) {
        m_data.reset();
        return;
    }
    auto temp = std::make_unique<T[]>(m_size);
    m_data.reset(temp.release());
    std::copy(src.begin(), src.end(), this->begin());
}

template <typename T>
template <concept_arithmetic U>
Array_base<T>::Array_base(const Array_base<U>& src)
    : m_size(src.m_size)
{
    if (m_size == 0) {
        m_data.reset();
        return;
    }
    auto temp = std::make_unique<T[]>(m_size);
    m_data.reset(temp.release());
    std::transform(src.begin(), src.end(), this->begin(),
        [](const U& x) { return static_cast<T>(x); });
}

template <typename T>
Array_base<T>::Array_base(Array_base<T>&& src)
    : m_size(src.m_size)
    , m_is_reference(src.m_is_reference)
    , m_data(std::move(src.m_data))
{
    src.m_size = 0;
    src.m_is_reference = false;
}

template <typename T>
Array_base<T>::Array_base(std::size_t a_size)
    : m_size(a_size)
{
    if (m_size == 0) {
        m_data.reset();
        return;
    }
    auto temp = std::make_unique<T[]>(m_size);
    m_data.reset(temp.release());
}

template <typename T>
Array_base<T>::Array_base(std::size_t a_size, const T& def)
    : m_size(a_size)
{
    if (!m_size)
        return;
    auto temp = std::make_unique<T[]>(m_size);
    m_data.reset(temp.release());
    for (std::size_t i = 0; i < m_size; i++)
        m_data.get()[i] = T(def);
}

template <typename T>
Array_base<T>::Array_base(std::shared_ptr<T[]> data, std::size_t a_size)
    : m_size(a_size)
    , m_data(data)
{
}

template <typename T>
Array_base<T>::Array_base(std::initializer_list<T> l)
    : m_size(l.size())
{
    if (m_size == 0) {
        this->data().reset();
        return;
    }
    auto temp = std::make_unique<T[]>(m_size);
    m_data.reset(temp.release());
    std::copy(l.begin(), l.end(), this->begin());
}

template <typename T>
Array_base<T>::~Array_base()
{
    m_data.reset();
}

template <typename T>
Array_base<T>& Array_base<T>::operator=(Array_base<T>&& other)
{
    if (this != &other) {
        m_size = other.m_size;
        m_is_reference = other.m_is_reference;
        m_data = std::move(other.m_data);
        other.m_size = 0;
        other.m_is_reference = false;
    }
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array_base<T>& Array_base<T>::operator=(const Array_base<U>& other)
{
    if (this == &other)
        return *this;
    if (this->m_size != other.m_size) {
        m_data.reset();
        m_size = other.m_size;
    }
    auto temp = std::make_unique<T[]>(m_size);
    m_data.reset(temp.release());
    std::transform(other.begin(), other.end(), this->begin(),
        [](const U& x) { return static_cast<T>(x); });
    return *this;
}

template <typename T>
void Array_base<T>::set_at(std::shared_ptr<T[]> data, std::size_t a_size)
{
    if (!data && a_size != 0) {
        throw std::invalid_argument("set_at: non-zero size with null data pointer.");
    }
    m_size = a_size;
    m_data = std::move(data);
    m_is_reference = true;
}

template <typename T>
void Array_base<T>::resize(std::size_t new_size)
{
    if (m_is_reference) {
        throw std::logic_error("Cannot resize a reference-backed Array_base.");
    }

    // If the size doesn't change, do nothing
    if (new_size == m_size) return;

    if (new_size == 0) {
        m_data.reset();
        m_size = 0;
        return;
    }

    auto temp = std::make_unique<T[]>(new_size);
    m_data.reset(temp.release());
    m_size = new_size;
}

} // namespace smip
