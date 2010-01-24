// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef UT_VECTOR_H
#define UT_VECTOR_H

#include <assert.h>
#include <iostream>
#include <stdarg.h>
#include <vector>

/// UtVector wraps std::vector<T> with bounds checking for operator[].
template<typename T>
class UtVector : public std::vector<T>
{
private:
    void CheckRange(size_t i) const
    {
        assert(i < std::vector<T>::size() && "Vector index out of range");
    }

    void CheckRange(unsigned int i) const
    {
        assert(i < std::vector<T>::size() && "Vector index out of range");
    }

    void CheckRange(int i) const
    {
        assert(0 <= i && (size_t)i < std::vector<T>::size() && 
               "Vector index out of range");
    }

public:
    /// Non-const index operator with range checking.
    template<typename Index>
    T& operator[](Index i)
    {
        CheckRange(i);
        return std::vector<T>::operator[](i);
    }

    /// Const index operator with range checking.
    template<typename Index>
    const T& operator[](Index i) const
    {
        CheckRange(i);
        return std::vector<T>::operator[](i);
    }

    /// Construct an empty vector.
    UtVector() { }

    /// Construct a vector with n copies of the given (optional) value.
    explicit UtVector(size_t n, const T& value = T()) :
        std::vector<T>(n, value)
    {
    }

    /// Construct vector from iterator range.
    template <class Iter>
    UtVector(Iter first, Iter last) :
        std::vector<T>(first, last)
    {
    }

    /// Copy constructor
    UtVector(const UtVector<T>& v) :
        std::vector<T>(v)
    {
    }

    /// Construct from varargs.
    UtVector(size_t numArgs, ...)
    {
        this->reserve(numArgs);
        va_list ap;
        va_start(ap, numArgs);
        for (size_t i = 0; i < numArgs; ++i)
            push_back(va_arg(ap, T));
        va_end(ap);
    }

};

/// Print a vector, assuming operator<< is defined on the element type.
template<typename T>
std::ostream& operator<<(std::ostream& out, const UtVector<T>& vec)
{
    out << "{";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0)
            out << ", ";
        out << vec[i];
    }
    out << "}";
    return out;
}

#endif // ndef UT_VECTOR_H
