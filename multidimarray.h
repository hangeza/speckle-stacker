#pragma once

//#include <complex>
#include <deque>
#include <valarray>
#include <vector>
//#include <errno.h>
//#include <stdio.h>
//#include <cstdlib>
#include <typeinfo>
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

#include "array_base.h"
#include "dimvector.h"

namespace smip {
/*
 *! class Range
 */
class Range {
public:
    Range()
    {
        _begin = 0;
        _end = 0;
    }
    Range(int begin, int end)
    {
        _begin = begin;
        _end = end;
    }
    ~Range() { }

    inline int begin() const { return _begin; }
    inline int end() const { return _end; }
    inline int length() const { return _end - _begin + 1; }

    inline bool operator==(const Range& other) const
    {
        return (_begin == other._begin && _end == other._end);
    }
    inline bool operator!=(const Range& other) const
    {
        return !(other == (*this));
    }

protected:
    int _begin;
    int _end;
};

static const Range rAll;

// Forward declarations
template <typename T, std::size_t N>
class array_iterator;

template <typename T, std::size_t N>
class const_array_iterator;

/*
 * ! Container class for N-dim Arrays
 */
template <typename T, std::size_t NrDims>
class Array : public Array_base<T> {
public:
    typedef Array_base<T> T_base;

protected:
    using T_base::_isReference;
    using T_base::_mem;
    using T_base::_size;

public:
    typedef DimVector_old<std::size_t, NrDims> extends;
    typedef DimVector_old<int, NrDims> strides;
    typedef DimVector_old<int, NrDims> s_indices;
    typedef DimVector_old<std::size_t, NrDims> u_indices;

    typedef array_iterator<T, NrDims> iterator;
    typedef const const_array_iterator<T, NrDims> const_iterator;

    /*! default constructor \n
     * creates empty Array
     */
    Array();
    Array(const extends& a_extends,
        const T& = T {});
    Array(const extends& a_extends,
        const strides& a_strides,
        T* a_mem,
        std::size_t a_size);
    /*! Copy Constructor \n
     *  make deep copy of \e src
     */
    Array(const Array& src);
    /*! default destructor
     */
    ~Array() { }

    /* private Methods */
private:
    /*! Calculate strides of all dimensions
     */
    void CalcStrides();

public:
    /* Iterator functions */

    /*! begin iterator \n
       * \return iterator to the first element
       */
    inline iterator begin() { return std::move(iterator(*this)); }
    /*! const begin iterator \n
       * \return const_iterator to the first element
       */
    inline const_iterator begin() const { return std::move(const_iterator(*this)); }
    /*! end iterator \n
       * \return iterator pointing after the last element
       */
    inline iterator end() { return std::move(iterator()); }
    /*! const end iterator \n
       * \return const_iterator pointing after the last element
       */
    inline const_iterator end() const { return std::move(const_iterator()); }

    /* Operators */

    /*! General Assignment \n
       *\return deep copy of \e x
       */
    const Array<T, NrDims>& operator=(const Array<T, NrDims>& src);

    /*! Assignment to the entire array (subarray) from \e val
       */
    const Array<T, NrDims>& operator=(const T& val);

    /*! Assignment from std::vector<T> \n
       * Assign a vector. The result is a 1-dim. Array
       */
    const Array<T, 1>& operator=(const std::vector<T>& vec);

    /*! Select a subarray \n
       * this operation returns a collapsed subarray with the last (rightmost)\n
       * dimension fixed
       *\return N-1-dim subarray
       */
    Array<T, NrDims - 1> operator[](int in) const;

    /*! Select a subarray \n
       * this operation returns a subarray with the selected Range in the last \n
       * (rightmost) dimension
       *\return N-dim subarray
       *\todo needs work!
       */
    Array<T, NrDims> operator[](Range r);

    /*! cast to generic type T */
    //      operator T(){ return _mem[0]; }

    /*! cast to std::vector<T> */
    //      operator std::vector<T>();

    /* public Methods */

    /*! get number of Elements in array (subarray) (  != size() ) */
    std::size_t NrElements() const;
    inline const strides& GetStrides() const { return _strides; }
    inline const extends& GetExtends() const { return _extends; }
    inline std::size_t offset() const { return _offset; }
    s_indices min_sindices() const;
    s_indices max_sindices() const;

    /*! returns pointer to the first element of datafield */
    inline const T* data() const { return _mem; }
    /*! returns pointer to the first element of datafield */
    inline T* data() { return _mem; }

    /*! returns element with given indices */
    const T& at(u_indices indices) const;

    /*! print some info about actual instance to stdout */
    void print() const;

    /* Specializations */

    /*
// friend fw declarations illegal since gcc 4.xx
      template <T>
      friend std::ostream& operator<<(std::ostream& o,const Array<T,1>& v);

      template <T>
      friend std::ostream& operator<<(std::ostream& o,const Array<T,2>& v);
*/

    /* protected members */

protected:
    extends _extends;
    strides _strides;
    std::size_t _offset { 0UL };
};

// fw decl of Array<T,1>
template <typename T>
class Array<T, 1>;

/*
 *! class Array<T,2> 
 * Specialization of Array-Class for 2 dimensions
 */
template <class T>
class Array<T, 2> : public Array_base<T> {
public:
    typedef Array_base<T> T_base;

protected:
    using Array_base<T>::_mem;
    using Array_base<T>::_size;
    using Array_base<T>::_isReference;

public:
    typedef DimVector_old<std::size_t, 2> extends;
    typedef DimVector_old<int, 2> strides;
    typedef DimVector_old<int, 2> s_indices;
    typedef DimVector_old<std::size_t, 2> u_indices;

    typedef array_iterator<T, 2> iterator;
    typedef const const_array_iterator<T, 2> const_iterator;

    /* constructors */

    /*! default constructor \n
    * creates empty Array
    */
    Array();
    Array(const extends& a_extends,
        const T& init = T {});
    Array(std::size_t nrows, std::size_t ncols,
        const T& init = T {});
    Array(const extends& a_extends,
        const strides& a_strides,
        std::shared_ptr<T> a_mem,
        std::size_t a_offs,
        std::size_t a_size);
    /*! copy constructor \n
    *  make deep copy of \e src
    */
    Array(const Array& src);
    /*! constructor \n
    *  construct from initializer list
    */
    Array(std::initializer_list<std::initializer_list<T>> l);
    /*! default destructor
    */
    ~Array() = default;

    /* Iterator functions */

    inline iterator begin() { return std::move(iterator(*this)); }
    inline iterator end() { return std::move(iterator()); }
    inline const_iterator begin() const { return std::move(const_iterator(*this)); }
    inline const_iterator end() const { return std::move(const_iterator()); }

    /* Operators */

    /*! operator=(const Array&)
     * general assignment \n
     *\return deep copy of \e src
     * @todo iterator must handle swapped strides (e.g.
     * a transposed matrix)
     */
    Array<T, 2>& operator=(const Array<T, 2>& src);
    // copy assignment (copy-and-swap idiom)
    //Array<T, 2>& operator=(Array<T, 2> src);
    /*! operator=(const T&)
     * Assignment to the entire array (subarray) from \e val
     */
    Array<T, 2>& operator=(const T& val);
    Array<T, 2>& operator+=(const Array<T, 2>& x);
    Array<T, 2>& operator-=(const Array<T, 2>& x);
    Array<T, 2>& operator*=(const Array<T, 2>& x);
    Array<T, 2>& operator/=(const Array<T, 2>& x);
    Array<T, 2>& operator*=(const T& x);
    Array<T, 2>& operator/=(const T& x);
    Array<T, 1> operator[](int in);
    Array<T, 1> operator[](int in) const;
    //! todo: implement selection of ROI
    Array<T, 2> operator[](Range r);

    /* public Methods */

    /*! get number of Elements in array (subarray) (  != size() ) */
    inline std::size_t NrElements() const { return _extends[0] * _extends[1]; }
    s_indices min_sindices() const { return { -static_cast<int>(cols()) / 2, -static_cast<int>(rows()) / 2 }; }
    s_indices max_sindices() const { return { -static_cast<int>(cols()) / 2 + static_cast<int>(cols()) - 1, -static_cast<int>(rows()) / 2 + static_cast<int>(rows()) - 1 }; }
    inline strides GetStrides() const { return _strides; }
    inline extends GetExtends() const { return _extends; }
    inline std::size_t offset() const { return _offset; }
    /*! returns number of rows */
    inline std::size_t rows() const { return _extends[1]; }
    /*! returns number of columns */
    inline std::size_t cols() const { return _extends[0]; }
    /*! returns pointer to the first element of datafield */
    //inline T* data() { return _mem; }
    //inline const T* data() const { return _mem; }
    /*! returns element with given signed indices */
    const T& at(s_indices indices) const;
    /*! returns element with given signed indices */
    T& at(s_indices indices);
    const Array<T, 1> row(std::size_t a_row) const;
    //Array<T,1> row(std::size_t row);
    const Array<T, 1> col(std::size_t a_col) const;
    Array<T, 1> col(std::size_t col);

    /*! print some info about instance to stdout */
    void print() const;

    /* protected members */
protected:
    extends _extends;
    strides _strides;
    std::size_t _offset { 0UL };
};

/*! Specialization of class Array which represents a vector */
template <typename T>
class Array<T, 1> : public Array_base<T> {
public:
    typedef Array_base<T> T_base;

protected:
    using Array_base<T>::_mem;
    using Array_base<T>::_size;
    using Array_base<T>::_isReference;

public:
    typedef DimVector_old<std::size_t, 1> extends;
    typedef DimVector_old<int, 1> strides;
    typedef DimVector_old<int, 1> indices;

    typedef array_iterator<T, 1> iterator;
    typedef const const_array_iterator<T, 1> const_iterator;

    /*! default constructor \n
    * creates empty Array
   */
    Array();
    Array(const extends& a_extends,
        const T& init = T {});
    Array(const extends& a_extends,
        const strides& a_strides,
        std::shared_ptr<T> a_mem,
        std::size_t a_offs,
        std::size_t a_size);
    Array(const extends& a_extends,
        const strides& a_strides,
        const std::shared_ptr<const T>& a_mem,
        std::size_t a_offs,
        std::size_t a_size);
    /*! copy constructor \n
    *  make deep copy of \e src
    */
    Array(const Array& src);
    /*! constructor \n
    *  construct from initializer list
    */
    Array(std::initializer_list<T> l);
    /*! default destructor
    */
    //   ~Array(){}

public:
    /* Iterator functions */

    inline iterator begin() { return iterator(*this); }
    inline iterator end() { return iterator(); }
    inline const_iterator begin() const { return const_iterator(*this); }
    inline const_iterator end() const { return const_iterator(); }

    /* Operators */

    //Array<T,1>& operator=(const Array<T,1> &src) const;
    /*! General Assignment \n
        *\return deep copy of \e x
        */
    Array<T, 1>& operator=(const Array<T, 1>& src);
    /*! General Assignment \n
        *\return deep copy of \e x
        */
    //       const Array<T,1>& operator=(const Array<T,1> &x);
    /*! Assignment to the entire array (subarray) from \e val
        */
    const Array<T, 1>& operator=(const T& val);
    Array<T, 1>& operator=(const std::vector<T>& vec);
    Array<T, 1>& operator+=(const Array<T, 1>& x);
    Array<T, 1>& operator-=(const Array<T, 1>& x);
    Array<T, 1>& operator*=(const Array<T, 1>& x);
    Array<T, 1>& operator/=(const Array<T, 1>& x);
    Array<T, 1>& operator*=(const T& x);
    Array<T, 1>& operator/=(const T& x);
    operator std::vector<T>()
    {
        std::vector<T> x;
        for (std::size_t i = 0; i < NrElements(); ++i)
            x.push_back(_mem[_offset + i * _strides[0]]);
        return x;
    }

    const T& operator[](std::size_t in) const;
    T& operator[](std::size_t in);
    inline Array<T, 1>& operator[](Range r) { return *this; }

    /* public Methods */

    /*! get number of Elements in array (subarray) (  != size() ) */
    std::size_t NrElements() const { return _extends[0]; }

    bool resize(std::size_t new_size)
    {
        if (!T_base::resize(new_size))
            return false;
        _extends.front() = new_size;
        _strides.front() = 1;
        _offset = 0UL;
        return true;
    }

    inline strides GetStrides() const { return _strides; }
    inline extends GetExtends() const { return _extends; }
    inline std::size_t offset() const { return _offset; }

    /*! returns pointer to the first element of datafield */
    //inline const T* data() const { return _mem; }
    /*! returns pointer to the first element of datafield */
    //inline T* data() { return _mem; }

    /*! returns element with given indices */
    T& at(std::size_t index) const { return _mem[_strides[0] * index]; }

    /*! print some info about actual instance to stdout */
    void print() const
    {
        std::cout << "object: Array<1>" << std::endl;
        std::cout << "field address: " << this << std::endl;
        std::cout << "data address: " << _mem << std::endl;
        if (_isReference)
            std::cout << "(field is a reference!)" << std::endl;
        std::cout << "datatype: " << typeid(T).name() << std::endl;
        std::cout << "size of datatype: " << sizeof(T) << " bytes" << std::endl;
        std::cout << "nr. of elements: " << NrElements() << std::endl;
        std::cout << "extend: " << _extends[0] << std::endl;
        std::cout << "stride: " << _strides[0] << std::endl;
        std::cout << "offset: " << _offset << std::endl;
        std::cout << "size: " << this->size() << std::endl;
        std::cout << "size of array: " << sizeof(T) * this->size() << " bytes" << std::endl;
    }

    /* protected members */

protected:
    extends _extends;
    strides _strides;
    std::size_t _offset { 0UL };
};

template <typename T>
inline std::ostream& operator<<(std::ostream& o, const Array<T, 1>& v)
{
    //   std::cout<<"ostream<<(Array<T,1>)"<<std::endl;
    o << "(";
    //   v.Print();
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(o, " "));
    o << "\b)";
    return o;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& o, Array<T, 2>& v)
{
    /*
   std::cout<<"ostream<<(Array<T,2>)"<<std::endl;
   std::cout<<"dims="<<v.GetExtends().size()<<std::endl;
   std::cout<<"size="<<v.size()<<std::endl;
   std::cout<<"extends[0]="<<v.GetExtends()[0]<<std::endl;
   std::cout<<"strides[0]="<<v.GetStrides()[0]<<std::endl;
   v[0].Print();
*/
    for (std::size_t i = 0; i < v.rows(); i++) {
        o << v.row(i) << std::endl;
    }
    //   o<<"\b)";
    //   std::copy(v.begin(),v.end(),std::ostream_iterator<Array<T,1> >(o," "));
    return o;
}

// *************************************************
// Member definitions / implementation part
// *************************************************

/*
 * class Array
 */
template <typename T, std::size_t NrDims>
Array<T, NrDims>::Array()
    : Array_base<T>()
{
    _extends.fill(NrDims, 0);
    _strides.fill(0);
}

template <typename T, std::size_t NrDims>
Array<T, NrDims>::Array(const extends& a_extends, const T& init)
    : Array_base<T>(_size = std::accumulate(a_extends.begin(),
                        a_extends.end(),
                        1,
                        std::multiplies<int>()),
        init)
{
    assert((a_extends.size() == NrDims));
    _extends = a_extends;
    _strides.fill(0);
    CalcStrides();
}

template <typename T, std::size_t NrDims>
Array<T, NrDims>::Array(const extends& a_extends,
    const strides& a_strides,
    T* a_mem,
    size_t a_size)
{
    assert((a_extends.size() == NrDims));
    _extends = a_extends;
    assert((a_strides.size() == NrDims));
    _strides = a_strides;
    _mem = a_mem;
    _size = a_size;
    _isReference = true;
}

template <typename T, std::size_t NrDims>
Array<T, NrDims>::Array(const Array& src)
    : Array_base<T>(src)
    , _extends { src._extends }
{
    CalcStrides();
}

template <typename T, std::size_t NrDims>
void Array<T, NrDims>::CalcStrides()
{
    _strides[0] = 1;
    for (std::size_t i = 1; i < NrDims; ++i) {
        _strides[i] = std::accumulate(_extends.begin(), _extends.begin() + i, 1, std::multiplies<std::size_t>());
        //          std::cout<<"   Dim"<<i+1<<": "<<_strides[i]<<std::endl;
    }
}

template <typename T, std::size_t NrDims>
const Array<T, NrDims>& Array<T, NrDims>::operator=(const Array<T, NrDims>& src)
{
    //   std::cout<<"Array<"<<typeid(T).name()<<","<<NrDims
    //            <<">::operator=(...&x)"<<std::endl;

    if (this == &src)
        return *this;

    if (_isReference) {
        assert(this->NrElements() == src.NrElements());
        _extends = src._extends;
    } else {
        if (_size != src.NrElements()) {
            assert(this->resize(src.NrElements()));
            _isReference = false;
        }
        _extends = src._extends;

        CalcStrides();
    }

    std::copy(src.begin(), src.end(), this->begin());
    return *this;
}

template <typename T, std::size_t NrDims>
const Array<T, NrDims>& Array<T, NrDims>::operator=(const T& val)
{
    //   std::cout<<"Array<"<<typeid(T).name()<<","<<NrDims
    //            <<">::operator=(T)(0)"<<std::endl;
    if (NrDims == 0) {
        _mem[0] = val;
        return *this;
    }

    //   for (size_t i=0; i<_size; i+=_strides[0]) _mem[i]=val;
    //   std::cout<<"val[1][2]="<<_mem[19]<<std::endl;

    std::fill(this->begin(), this->end(), val);
    return *this;
}

template <typename T, std::size_t NrDims>
Array<T, NrDims - 1> Array<T, NrDims>::operator[](int in) const
{

    extends ext = _extends;
    strides str = _strides;

    ext.pop_front();
    str.pop_front();

    int offs = _strides[0] * in;

    Array<T, NrDims - 1> _x(ext,
        DimVector_old<int, NrDims - 1>(str),
        _mem + offs,
        _size - offs);

    return _x;
}

template <typename T, std::size_t NrDims>
Array<T, NrDims> Array<T, NrDims>::operator[](Range r)
{
    extends ext = _extends;
    strides str = _strides;

    ext.push_back(ext.front());
    ext.pop_front();

    str.push_back(str.front());
    str.pop_front();

    Array<T, NrDims> _x(ext,
        DimVector_old<int, NrDims>(str),
        _mem,
        _size);
    return _x;
}

template <typename T, std::size_t NrDims>
size_t Array<T, NrDims>::NrElements() const
{
    return std::accumulate(_extends.begin(), _extends.end(), 1, std::multiplies<int>());
}

template <typename T, std::size_t NrDims>
const T& Array<T, NrDims>::at(u_indices indices) const
{
    assert(indices.size() == NrDims);
    std::size_t offset { 0 };
    u_indices index { indices };
    //   std::cout<<"index.size()="<<index.size()<<std::endl;
    //   std::cout<<"index[0]="<<index[0]<<std::endl;
    // fill rest with zeroes
    for (auto i { index.size() }; i < NrDims; ++i)
        index.push_back(0);
    for (std::size_t i { 0 }; i < NrDims; ++i) {
        offset += index[i] * _strides[i];
    }
    assert(offset < _size);
    return _mem[offset];
}

template <typename T, std::size_t NrDims>
void Array<T, NrDims>::print() const
{
    std::cout << "Object: Array" << std::endl;
    std::cout << "Field Address: " << this << std::endl;
    std::cout << "Data Address: " << _mem << std::endl;
    if (_isReference)
        std::cout << "(Field is a reference!)" << std::endl;
    std::cout << "Datatype: " << typeid(T).name() << std::endl;
    std::cout << "size of Datatype: " << sizeof(T) << " bytes" << std::endl;
    std::cout << "Dimensions: " << NrDims << std::endl;
    std::cout << "Nr. of Elements: " << NrElements() << std::endl;
    std::cout << "Extends: " << std::endl;
    for (int i = 0; i < NrDims; i++)
        std::cout << "   Dim" << i + 1 << ": " << _extends[i] << std::endl;
    std::cout << "Strides: " << std::endl;
    for (int i = 0; i < NrDims; i++)
        std::cout << "   Dim" << i + 1 << ": " << _strides[i] << std::endl;
    std::cout << "size: " << this->size() << std::endl;
    std::cout << "size of Array: " << sizeof(T) * this->size() << " bytes" << std::endl;
}

/*
 * class Array<T,2>
 */
template <typename T>
Array<T, 2>::Array()
    : Array_base<T>()
{
    _extends.fill(0);
    _strides.fill(0);
    //     std::cout<<"Array<T,2>::Array()\n";
}

template <typename T>
Array<T, 2>::Array(const extends& a_extends, const T& init)
    : Array_base<T>(a_extends[0] * a_extends[1], init)
{
    //     std::cout<<"Array<T,2>::Array(const extends&, const bases&, const T&)\n";
    assert((a_extends.size() == 2));
    _extends = a_extends;
    _strides = { 1, _extends[0] };
}

template <typename T>
Array<T, 2>::Array(std::size_t nrows, std::size_t ncols, const T& init)
    : Array_base<T>(nrows * ncols, init)
{
    //     std::cout<<"Array<T,2>::Array(size_t, size_t, const bases&, const T&)\n";

    _extends = { ncols, nrows };
    _isReference = false;
    _strides = { 1, _extends[0] };
}

template <typename T>
Array<T, 2>::Array(const extends& a_extends,
    const strides& a_strides,
    std::shared_ptr<T> a_mem,
    std::size_t a_offs,
    std::size_t a_size)
    : Array_base<T>(a_mem, a_size)
    , _extends(a_extends)
    , _strides(a_strides)
    , _offset(a_offs)
{
    //     std::cout<<"Array<T,2>::Array(const extends&, const bases&, T*, size_t)\n";
    assert((a_extends.size() == 2));
    //_extends=a_extends;
    assert((a_strides.size() == 2));
    //_strides=a_strides;
    //_mem=a_mem;
    //_size=a_size;
    _isReference = true;
}

template <typename T>
Array<T, 2>::Array(const Array& src)
    : Array_base<T>(src)
    , _extends(src._extends)
    , _offset(src._offset)
{
    //     std::cout<<"Array<T,2>::Array(const Array&)\n";
    //assert(this->resize(src.NrElements()));
    assert(src._extends.size() == 2);
    _strides = { 1, _extends.front() };
    //std::copy(src.begin(),src.end(),this->begin());
}

template <typename T>
Array<T, 2>::Array(std::initializer_list<std::initializer_list<T>> l)
{
    //     std::cout<<"Array<T,2>::Array(initializer_list<T>)\n";
    const std::size_t rows { l.size() };
    if (rows == 0)
        return;
    const std::size_t cols { l.begin()->size() };
    assert(this->resize(cols * rows));
    auto memit = T_base::begin();
    for (auto row = l.begin(); row != l.end(); ++row) {
        assert(cols == row->size());
        std::copy(row->begin(), row->end(), memit);
        memit += cols;
    }
    _extends = { cols, rows };
    _strides = { 1, _extends[0] };
}

template <typename T>
Array<T, 2>& Array<T, 2>::operator=(const Array<T, 2>& src)
{
    std::cout << "Array<T,2>& Array<T,2>::operator=(const Array&)\n";
    // assert(!src._isReference);
    if (this == &src)
        return *this;
    if (_isReference) {
        _mem = nullptr;
        _size = 0;
        _isReference = false;
    }
    if (_size != src.NrElements()) {
        //std::cout<<"resizing array...\n";
        assert(this->resize(src.NrElements()));
        //_isReference = false;
    }
    _extends = src._extends;
    _strides = { 1, _extends[0] };
    _offset = 0UL;
    //     std::cout<<"copying array...\n";
    std::copy(src.begin(), src.end(), this->begin());
    //     std::cout<<"done.\n";
    return *this;
}

/*
// copy assignment (copy-and-swap idiom)
template <typename T>
Array<T, 2>& Array<T, 2>::operator=(Array<T, 2> src)
{
    std::cout<<"Array<T,2>& Array<T,2>::operator=(Array<T,2>)\n";
    std::swap(_extends, src._extends);
    std::swap(_offset, src._offset);
    std::swap(_strides, src._strides);
    std::swap(_size, src._size);
    std::swap(_mem, src._mem);
    return *this;
}
*/

template <typename T>
Array<T, 2>& Array<T, 2>::operator=(const T& val)
{
    //   std::cout<<"Array<"<<typeid(T).name()<<","<<NrDims
    //            <<">::operator=(T)(0)"<<std::endl;
    //         for (T* i=_mem; i<_mem+_size; i++) *i=val;
    //         for (size_t i=0; i<_size; i+=_strides[0]) _mem[i]=val;
    //         for (size_t i=0; i<this->NrElements(); i++) _mem[i*_strides[0]]=val;
    std::fill(this->begin(), this->end(), val);
    return *this;
}

template <typename T>
Array<T, 2>& Array<T, 2>::operator+=(const Array<T, 2>& x)
{
    assert(x.NrElements() == this->NrElements());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::plus<T>());
    return *this;
}

template <typename T>
Array<T, 2>& Array<T, 2>::operator-=(const Array<T, 2>& x)
{
    assert(x.NrElements() == this->NrElements());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::minus<T>());
    return *this;
}

template <typename T>
Array<T, 2>& Array<T, 2>::operator*=(const Array<T, 2>& x)
{
    assert(x.NrElements() == this->NrElements());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::multiplies<T>());
    return *this;
}

template <typename T>
Array<T, 2>& Array<T, 2>::operator/=(const Array<T, 2>& x)
{
    assert(x.NrElements() == this->NrElements());
    for (std::size_t i { 0 }; i < this->NrElements(); ++i) {
        if (x[i] == T {}) {
            if (std::numeric_limits<T>::has_quiet_NaN()) {
                _mem.get()[i * _strides[0]] = std::numeric_limits<T>::quiet_NaN();
            } else {
                _mem.get()[i * _strides[0]] = T { 0 / 0. };
            }
        } else
            _mem.get()[i * _strides[0]] /= x[i];
    }
    return *this;
}

template <typename T>
Array<T, 2>& Array<T, 2>::operator*=(const T& x)
{
    for (int i = 0; i < (int)this->NrElements(); i++)
        _mem.get()[i * _strides[0]] *= x;
    return *this;
}

template <typename T>
Array<T, 2>& Array<T, 2>::operator/=(const T& x)
{
    if (x == T())
        std::fill(this->begin(), this->end(), T(INFINITY));
    else
        (*this) *= T(1) / x;
    return *this;
}

template <typename T>
Array<T, 1> Array<T, 2>::operator[](int in)
{
    extends ext = _extends;
    strides str = _strides;

    ext.pop_back();
    str.pop_back();

    if (in < 0)
        in += ext.front();
    std::size_t offs = _strides.back() * in;

    Array<T, 1> _x(ext,
        DimVector_old<int, 1>(str),
        Array_base<T>::data(),
        offs,
        _size);
    std::cout << "Array<T,1> Array<T,2>::operator[](int)\n";
    return _x;
}
template <typename T>

Array<T, 1> Array<T, 2>::operator[](int in) const
{
    extends ext = _extends;
    strides str = _strides;

    ext.pop_back();
    str.pop_back();

    if (in < 0)
        in += ext.front();
    std::size_t offs = _strides.back() * in;

    const Array<T, 1> _x(ext,
        DimVector_old<int, 1>(str),
        std::const_pointer_cast<const T>(_mem) /*Array_base<T>::data()*/,
        offs,
        _size);
    std::cout << "Array<T,1> Array<T,2>::operator[](int) const\n";
    return _x;
}

//! todo: implement selection of ROI
template <typename T>
Array<T, 2> Array<T, 2>::operator[](Range r)
{
    extends ext = _extends;
    strides str = _strides;

    std::swap(ext.front(), ext.back());
    std::swap(str.front(), str.back());

    Array<T, 2> _x(ext,
        DimVector_old<int, 2>(str),
        _mem,
        0UL,
        _size);
    //     std::cout<<"const Array<T,2> Array<T,2>::operator[](range)\n";
    return _x;
}

template <typename T>
const Array<T, 1> Array<T, 2>::row(std::size_t a_row) const
{
    //assert(_isReference==false);
    extends ext = _extends;
    strides str = _strides;

    ext.pop_back();
    str.pop_back();

    //std::size_t offs=_strides.back()*a_row;
    //const T* addr { Array_base<T>::data()+offs };
    Array<T, 1> _x(ext);
    for (std::size_t i = 0; i < ext[0]; ++i) {
        const auto val = _mem.get()[_offset + a_row * _strides[1] + i];
        _x[i] = val;
    }
    /*
    const Array<T,1> _x(ext,
                DimVector_old<int,1>(str),
                addr,
                _size-offs);
*/
    //    std::cout<<"const Array<T,1> Array<T,2>::row(std::size_t row)\n";
    return _x;
}
/*
template <typename T>
Array<T,1> Array<T,2>::row(std::size_t a_row)
{
    //assert(_isReference==false);
    extends ext=_extends;
    strides str =_strides;

    ext.pop_back();
    str.pop_back();

    std::size_t offs=_strides.back()*row;

    Array<T,1> _x(ext,
                DimVector_old<int,1>(str),
                _mem+offs,
                _size-offs);
//    std::cout<<"Array<T,1> Array<T,2>::row(std::size_t row)\n";
    return _x;
}
*/
template <typename T>
const Array<T, 1> Array<T, 2>::col(std::size_t a_col) const
{
    //assert(_isReference==false);
    extends ext = _extends;
    strides str = _strides;

    ext.pop_front();
    str.pop_front();

    std::size_t offs = _strides.front() * a_col;
    const T* addr { Array_base<T>::data() + offs };
    /*
    const Array<T,1> _x(ext,
                DimVector_old<int,1>(str),
                addr,
                _size-offs);
*/

    Array<T, 1> _x(ext);
    for (std::size_t i = 0; i < ext[0]; ++i) {
        _x.data().get()[i] = _mem.data().get()[_offset + i * str[0] + a_col];
    }

    //    const Array<T,1> _x{ const_cast<Array<T,2>*>(this)->operator[](rAll)[a_col] };
    //     std::cout<<"const Array<T,1> Array<T,2>::col(std::size_t col)\n";
    return _x;
}

template <typename T>
Array<T, 1> Array<T, 2>::col(std::size_t col)
{
    //assert(_isReference==false);
    extends ext = _extends;
    strides str = _strides;

    ext.pop_front();
    str.pop_front();

    std::size_t offs = _strides.front() * col;

    Array<T, 1> _x(ext,
        DimVector_old<int, 1>(str),
        _mem, offs,
        _size);
    //    std::cout<<"Array<T,1> Array<T,2>::col(std::size_t col)\n";
    return _x;
}

template <typename T>
const T& Array<T, 2>::at(s_indices indices) const
{
    if (indices[0] < 0)
        indices[0] += _extends[0];
    if (indices[1] < 0)
        indices[1] += _extends[1];
    std::size_t addr { _offset + static_cast<std::size_t>(indices[0]) * _strides[1] + static_cast<std::size_t>(indices[1]) * _strides[0] };
    if (addr >= _size) {
        print();
        std::cout << "indices: [" << indices[0] << "," << indices[1] << "]\n";
        assert(addr < _size);
    }
    return Array_base<T>::operator[](addr);
}

template <typename T>
T& Array<T, 2>::at(s_indices indices)
{
    if (indices[0] < 0)
        indices[0] += _extends[0];
    if (indices[1] < 0)
        indices[1] += _extends[1];
    std::size_t addr { _offset + static_cast<std::size_t>(indices[0]) * _strides[1] + static_cast<std::size_t>(indices[1]) * _strides[0] };
    assert(addr < _size);
    return Array_base<T>::operator[](addr);
}

template <typename T>
void Array<T, 2>::print() const
{
    std::cout << "object: Array<2>" << std::endl;
    std::cout << "field address: " << this << std::endl;
    std::cout << "data address: " << _mem << std::endl;
    if (_isReference)
        std::cout << "(field is a reference!)" << std::endl;
    std::cout << "datatype: " << typeid(T).name() << std::endl;
    std::cout << "size of datatype: " << sizeof(T) << " bytes" << std::endl;
    std::cout << "nr. of elements: " << NrElements() << std::endl;
    std::cout << "extends: x=" << _extends[0] << ", y=" << _extends[1] << std::endl;
    std::cout << "strides: x=" << _strides[0] << ", y=" << _strides[1] << std::endl;
    std::cout << "min indices: x=" << min_sindices()[0] << ", y=" << min_sindices()[1] << std::endl;
    std::cout << "max indices: x=" << max_sindices()[0] << ", y=" << max_sindices()[1] << std::endl;
    std::cout << "size: " << this->size() << std::endl;
    std::cout << "size of array: " << sizeof(T) * this->size() << " bytes" << std::endl;
}

/*
 *! Array<T,1>
 */
template <typename T>
Array<T, 1>::Array()
    : Array_base<T>()
{
    _extends.fill(0);
    _strides.fill(0);
}

template <typename T>
Array<T, 1>::Array(const extends& a_extends,
    const T& init)
    : Array_base<T>(a_extends[0], init)
{
    std::cout << "Array<T,1>::Array(const extends&,const T&)\n";
    assert((a_extends.size() == 1));
    _extends = a_extends;
    _strides = 1;
}

template <typename T>
Array<T, 1>::Array(const extends& a_extends,
    const strides& a_strides,
    std::shared_ptr<T> a_mem,
    std::size_t a_offs,
    std::size_t a_size)
    : Array_base<T>(a_mem, a_size)
    , _extends(a_extends)
    , _strides(a_strides)
    , _offset(a_offs)
{
    std::cout << "Array<T,1>::Array(const extends&, const strides&, std::shared_ptr<T>, std::size_t, std::size_t)\n";
    assert((a_extends.size() == 1));
    assert((a_strides.size() == 1));
    //      _mem=a_mem;
    //      _size=a_size;
    _isReference = true;
}

template <typename T>
Array<T, 1>::Array(const extends& a_extends,
    const strides& a_strides,
    const std::shared_ptr<const T>& a_mem,
    std::size_t a_offs,
    std::size_t a_size)
    : Array_base<T>(a_mem, a_size)
    , _extends(a_extends)
    , _strides(a_strides)
    , _offset(a_offs)
{
    std::cout << "Array<T,1>::Array(const extends&, const strides&, std::shared_ptr<const T>, std::size_t, std::size_t)\n";
    assert((a_extends.size() == 1));
    assert((a_strides.size() == 1));
    //      _mem=a_mem;
    //      _size=a_size;
    _isReference = true;
}

template <typename T>
Array<T, 1>::Array(const Array& src)
    : Array_base<T>(src.NrElements())
    , _extends(src._extends)
{
    std::cout << "Array<T,1>::Array(const Array<T,1>&)";
    //assert(this->resize(src.NrElements()));
    assert(src._extends.size() == 1);
    _strides = 1;
    //     _isReference = false;
    std::copy(src.begin(), src.end(), T_base::begin());
}

template <typename T>
Array<T, 1>::Array(std::initializer_list<T> l)
{
    const std::size_t elements { l.size() };
    if (elements == 0)
        return;
    assert(this->resize(elements));
    std::copy(l.begin(), l.end(), T_base::begin());
    _extends = elements;
    _strides = 1;
}

template <typename T>
Array<T, 1>& Array<T, 1>::operator=(const Array<T, 1>& src)
{
    std::cout << "Array<T,1>& Array<T,1>::operator=(const Array<T,1>&)\n";
    _extends = src.size();
    if (_isReference) {
        _mem = nullptr;
        _isReference = false;
    }
    src.print();
    assert(this->resize(src.NrElements()));
    assert(src._extends.size() == 1);
    _strides = 1;
    _offset = 0;
    std::copy(src.begin(), src.end(), Array_base<T>::begin());
    return *this;
}

/*
template <typename T>
Array<T, 1>& Array<T, 1>::operator=(const Array<T, 1>& src)
{
    std::cout<<"Array<"<<typeid(T).name()<<",1>::operator=(const Array&)"<<std::endl;
    if (this == &src)
        return *this;
    //         std::cout<<"Array<T,1>::operator=(const Array<T,1>&)\n";

    if (_isReference)
        assert(NrElements() == src.NrElements());
    else {
        if (_size != src.NrElements()) {

//         if (_size) delete[] _mem;
//         _mem=0;
//         _size=src.NrElements();
//         if (_size) _mem=new T[_size];

            _isReference = false;
            assert(this->resize(src.NrElements()));
        }
        _extends = src._extends;
        _strides[0] = 1;
    }

    if (!_isReference && !src._isReference) {
        memcpy(_mem.get(), src._mem.get(), sizeof(T) * src.NrElements());
        return *this;
    } else if (_isReference) {
        _extends = src._extends;
        //            _strides=x._strides;
    }
    std::copy(src.begin(), src.end(), begin());
    return *this;
}
*/

/*
template <typename T>
const Array<T,1>& Array<T,1>::operator=(const Array<T,1> &x)
{
    //std::cout<<"Array<"<<typeid(T).name()<<","<<NrDims
    //         <<">::operator=(...&x)"<<std::endl;

    if (this == &x) return *this;
    if (_isReference) {
        _mem = nullptr;
        _isReference = false;
    }
    assert(this->resize(x.NrElements()));
    _extends=x._extends;
    _strides[0] = 1;
    std::copy(x.begin(),x.end(),begin());
    return *this;
}
*/

template <typename T>
const Array<T, 1>& Array<T, 1>::operator=(const T& val)
{
    //   std::cout<<"Array<"<<typeid(T).name()<<","<<NrDims
    //            <<">::operator=(T)(0)"<<std::endl;
    std::fill(this->begin(), this->end(), val);
    return *this;
}

template <typename T>
Array<T, 1>& Array<T, 1>::operator=(const std::vector<T>& vec)
{
    if (_isReference) {
        assert(vec.size() == this->NrElements());
        //            for (int i=0; i<(int)vec.size(); ++i)
        //               _mem[_offset + i*_strides[0]]=vec[i];
    } else if (vec.size() != this->NrElements()) {
        this->resize(vec.size());
    }

    std::copy(vec.begin(), vec.end(), this->begin());
    return *this;
    /*
    assert(vec.size()==NrElements());
    for (int i=0; i<(int)vec.size(); ++i)
    _mem[_offset + i*_strides[0]]=vec[i];
    return *this;
*/
}

template <typename T>
Array<T, 1>& Array<T, 1>::operator+=(const Array<T, 1>& x)
{
    assert(x.NrElements() == this->NrElements());
    transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::plus<T>());
    return *this;
}

template <typename T>
Array<T, 1>& Array<T, 1>::operator-=(const Array<T, 1>& x)
{
    assert(x.NrElements() == this->NrElements());
    transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::minus<T>());
    return *this;
}

template <typename T>
Array<T, 1>& Array<T, 1>::operator*=(const Array<T, 1>& x)
{
    assert(x.NrElements() == this->NrElements());
    transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::multiplies<T>());
    return *this;
}

template <typename T>
Array<T, 1>& Array<T, 1>::operator/=(const Array<T, 1>& x)
{
    assert(x.NrElements() == this->NrElements());
    for (int i = 0; i < (int)this->NrElements(); i++) {
        if (x[i] == 0)
            _mem[_offset + i * _strides[0]] = INFINITY;
        else
            _mem[_offset + i * _strides[0]] /= x[i];
    }
    return *this;
}

template <typename T>
Array<T, 1>& Array<T, 1>::operator*=(const T& x)
{
    //         std::cout<<"Array<"<<typeid(T).name()<<",1>::operator*=(const T&)"<<std::endl;
    for (int i = 0; i < (int)this->NrElements(); i++)
        _mem[_offset + i * _strides[0]] *= x;
    return *this;
}

template <typename T>
Array<T, 1>& Array<T, 1>::operator/=(const T& x)
{
    if (x == T())
        std::fill(this->begin(), this->end(), T(INFINITY));
    else
        (*this) *= T(1) / x;
    return *this;
}

/*
template <typename T>
operator std::vector<T>()
{
    std::vector<T> x;
    for (std::size_t i=0; i<NrElements(); ++i)
    x.push_back(_offset + _mem[i*_strides[0]]);
    return x;
}
*/

template <typename T>
const T& Array<T, 1>::operator[](std::size_t in) const
{
    return _mem.get()[_offset + _strides[0] * in];
}

template <typename T>
T& Array<T, 1>::operator[](std::size_t in)
{
    return _mem.get()[_offset + _strides[0] * in];
}

/*
 *! iterators
 */
template <typename T, std::size_t NrDims>
class const_array_iterator {
public:
    const_array_iterator()
        : m_extends()
        , m_strides()
        , m_dim_pos()
        , m_offset()
        , m_pos()
    {
    }
    const_array_iterator(const const_array_iterator& x)
        : m_extends { x.m_extends }
        , m_strides { x.m_strides }
        , m_dim_pos { x.m_dim_pos }
        , m_offset { x.m_offset }
        , m_pos { x.m_pos }
        , m_data { x.m_data }
    {
        //         std::cout<<"const_array_iterator<"<<NrDims<<">::const_array_iterator()\n";
    }
    const_array_iterator(const Array<T, NrDims>& x)
        : m_extends(x.GetExtends())
        , m_strides(x.GetStrides())
        , m_dim_pos()
        , m_offset(x.offset())
        , m_pos(x.offset())
        , m_data(x.data())
    {
        //         std::cout<<"const_array_iterator<"<<NrDims<<">::const_array_iterator(const const_array_iterator&)\n";
        if (!x.GetExtends().size()) {
            m_data.reset();
            return;
        }
    }
    ~const_array_iterator() = default;

    bool operator==(const const_array_iterator& x) const
    {
        //         std::cout<<"this addr="<<m_data.get()<<" x addr="<<x.m_data.get()<<" this pos="<<m_pos<<" x pos="<<x.m_pos<<"\n";
        return (m_data == x.m_data && m_pos == x.m_pos);
    }
    bool operator!=(const const_array_iterator& x) const
    {
        //         std::cout<<"this addr="<<m_data.get()<<" x addr="<<x.m_data.get()<<" this pos="<<m_pos<<" x pos="<<x.m_pos<<" result(!=)="<< (m_data!=x.m_data || m_pos!=x.m_pos) <<"\n";
        return (m_data != x.m_data || m_pos != x.m_pos);
    }
    const T& operator*() const
    {
        //         std::cout<<"const T& const_array_iterator::operator*() const : m_pos="<<m_pos<<"\n";
        return m_data.get()[m_pos];
    }
    const T* operator->() const
    {
        return &(m_data.get()[m_pos]);
        //         std::cout<<"const T* const_array_iterator::operator->() const : m_pos="<<m_pos<<"\n";
    }
    const_array_iterator& operator++()
    {
        m_dim_pos[0]++;
        m_pos += m_strides[0];
        for (std::size_t i = 0; i < m_extends.size() - 1; ++i) {
            if (m_dim_pos[i] == m_extends[i]) {
                m_dim_pos[i] = 0;
                m_dim_pos[i + 1]++;
                m_pos += m_strides[i + 1] - m_extends[i] * m_strides[i];
            }
        }
        if (m_dim_pos.back() == m_extends.back()) {
            m_pos = 0;
            m_data.reset();
        }
        return *this;
    }
    const_array_iterator operator++(int)
    {
        const_array_iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    void advance() const { ++(*this); }

protected:
    DimVector_old<std::size_t, NrDims> m_extends;
    DimVector_old<int, NrDims> m_strides;
    DimVector_old<std::size_t, NrDims> m_dim_pos;
    std::size_t m_offset { 0 };
    std::size_t m_pos { 0 };
    std::shared_ptr<const T> m_data;
};

template <typename T, std::size_t NrDims>
class array_iterator : public const_array_iterator<T, NrDims> {
public:
    array_iterator()
        : m_extends()
        , m_strides()
        , m_dim_pos()
        , m_offset()
        , m_pos()
    {
        //         std::cout<<"array_iterator<"<<NrDims<<">::array_iterator()\n";
    }
    array_iterator(const array_iterator& x)
        : m_extends { x.m_extends }
        , m_strides { x.m_strides }
        , m_dim_pos { x.m_dim_pos }
        , m_offset { x.m_offset }
        , m_pos { x.m_pos }
        , m_data { x.m_data }
    {
        //         std::cout<<"array_iterator<"<<NrDims<<">::array_iterator(const array_iterator&)\n";
    }
    array_iterator(Array<T, NrDims>& x)
        : m_extends(x.GetExtends())
        , m_strides(x.GetStrides())
        , m_dim_pos()
        , m_offset(x.offset())
        , m_pos(x.offset())
        , m_data(x.data())
    {
        if (!x.GetExtends().size()) {
            m_data.reset();
            return;
        }
    }

    ~array_iterator() = default;

    bool operator==(const array_iterator& x) const { return (m_data == x.m_data && m_pos == x.m_pos); }
    bool operator!=(const array_iterator& x) const { return (m_data != x.m_data) || m_pos != x.m_pos; }
    T& operator*() const
    {
        //         std::cout<<"T& array_iterator::operator*() const : m_pos="<<m_pos<<"\n";
        return m_data.get()[m_pos];
    }
    T* operator->() const
    {
        //         std::cout<<"T* array_iterator::operator->() const : m_pos="<<m_pos<<"\n";
        return &(m_data.get()[m_pos]);
    }
    array_iterator& operator++()
    {
        m_dim_pos[0]++;
        m_pos += m_strides[0];
        for (std::size_t i = 0; i < m_extends.size() - 1; ++i) {
            if (m_dim_pos[i] == m_extends[i]) {
                m_dim_pos[i] = 0;
                m_dim_pos[i + 1]++;
                m_pos += m_strides[i + 1] - m_extends[i] * m_strides[i];
            }
        }
        if (m_dim_pos.back() >= m_extends.back()) {
            m_pos = 0;
            m_data.reset();
        }
        return *this;
    }
    array_iterator operator++(int)
    {
        array_iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    void advance() const { ++(*this); }

protected:
    DimVector_old<std::size_t, NrDims> m_extends;
    DimVector_old<int, NrDims> m_strides;
    DimVector_old<std::size_t, NrDims> m_dim_pos;
    std::size_t m_offset { 0 };
    std::size_t m_pos { 0 };
    std::shared_ptr<T> m_data;
};

} // namespace smip

namespace std {
using namespace smip;

template <typename T, std::size_t N>
struct iterator_traits<array_iterator<T, N>> {
    typedef forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
};

template <typename T, std::size_t N>
struct iterator_traits<const_array_iterator<T, N>> {
    typedef forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;
};

}
