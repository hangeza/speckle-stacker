#pragma once

#include <typeinfo>
#include <vector>
//#include <climits>
//#include <algorithm>
//#include <functional>
#include <cassert>
#include <cstdarg>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>

#include "types.h"

namespace smip {

//! Container class for general (1-dim) Arrays
/*!
 * ...
*/
template <typename T>
class Array_base {
public:
    typedef T value_type;
    typedef T* ptr_type;
    typedef T* iterator;
    typedef const T* const_iterator;
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
    ~Array_base();

    Array_base<T>& operator=(Array_base<T>&& other);

    iterator begin() { return _mem.get(); }
    const_iterator begin() const { return data().get(); }
    const_iterator cbegin() const { return data().get(); }
    iterator end() { return _mem.get() + _size; }
    const_iterator end() const { return data().get() + _size; }
    const_iterator cend() const { return data().get() + _size; }

    std::shared_ptr<T[]>& data() { return _mem; }
    std::shared_ptr<const T[]> data() const { return std::const_pointer_cast<const T[]>(_mem); }

    reference operator[](std::size_t i) { return _mem.get()[i]; }
    const_reference operator[](std::size_t i) const
    {
        return std::const_pointer_cast<const T[]>(_mem).get()[i];
    }
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

    bool resize(std::size_t new_size)
    {
        if (_isReference)
            return false;
        auto temp = std::make_unique<T[]>(new_size);
        _mem.reset(temp.release());
        _size = new_size;
        return (_mem != nullptr);
    }

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
    //     std::cout<<"Array_base<T>::Array_base(const Array_base&)\n";
    if (_size == 0) {
        _mem.reset();
        return;
    }

    auto temp = std::make_unique<T[]>(_size);
    //std::shared_ptr<T> buf(nullptr, [](T* p) { delete[] p; });
    _mem.reset(temp.release());
    //_mem=new T[_size];
    std::copy(src.begin(), src.end(), this->begin());
}

template <typename T>
template <concept_arithmetic U>
Array_base<T>::Array_base(const Array_base<U>& src)
    : _size(src._size)
{
    //     std::cout<<"Array_base<T>::Array_base(const Array_base&)\n";
    if (_size == 0) {
        _mem.reset();
        return;
    }
    auto temp = std::make_unique<T[]>(_size);
    //std::shared_ptr<T> buf(nullptr, [](T* p) { delete[] p; });
    _mem.reset(temp.release());
    //_mem=new T[_size];
    std::copy(src.begin(), src.end(), this->begin());
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
Array_base<T>::~Array_base()
{
    if (!_isReference && _size) {
        _mem.reset();
    }
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
void Array_base<T>::set_at(std::shared_ptr<T[]> data, size_t a_size)
{
    _size = a_size;
    _mem = data;
    _isReference = true;
}

} // namespace smip
