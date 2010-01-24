// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef UT_CAST_H
#define UT_CAST_H

#include <assert.h>
#include <stdlib.h>

template<typename T> struct UtNoPtr { typedef T Type; };
template<typename T> struct UtNoPtr<T*> { typedef T Type; };
template<typename T> struct UtNoPtr<const T*> { typedef T Type; };

/// Check whether a pointer is an instance of a given class.
/// The class must define a static IsInstance() method (using a tagged union).
template<typename Dest, typename Src>
inline bool UtIsInstance(Src src) 
{
    return src != NULL ? UtNoPtr<Dest>::Type::IsInstance(src) : false;
}

/// Dynamically cast a pointer to a particular pointer type.
/// Like dynamic_cast, it returns NULL if the pointer is NULL or the
/// cast fails.
template<typename Dest, typename Src>
inline Dest UtCast(Src src)
{
    return UtIsInstance<Dest>(src) ? static_cast<Dest>(src) : 0L;
}

/// Statically cast a pointer to a particular pointer type.  Assert fails if
/// the corresponding dynamic cast would fail.
template<typename Dest, typename Src>
inline Dest UtStaticCast(Src src)
{
    assert(UtIsInstance<Dest>(src) && "Static cast failed");
    return static_cast<Dest>(src);
}

#endif // ndef UT_CAST_H
