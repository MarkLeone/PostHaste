// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef UT_HASH_MAP_H
#define UT_HASH_MAP_H

#include <assert.h>
#include <string.h>
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

// Avoid leaking the following "using" declarations by wrapping everything
// in a namespace, and later pulling out only the structs/classes.
namespace _UtHashMap
{ 

#ifdef __GNUC__
using namespace __gnu_cxx;
#else
using namespace std;
#endif

/// Hash functions.
struct UtHash {
    /// Use std::hash for C strings.
    size_t operator()(const char* key) const
    {
        assert(key && "NULL string not permitted as hash key");
        return hash<const char*>()(key);
    }

    /// Use std::hash for STL strings.
    size_t operator()(const std::string& key) const
    {
        return hash<const char*>()(key.c_str());
    }

    /// Convert pointers to size_t for hashing.
    template<typename Key>
    size_t operator()(const Key *key) const 
    {
        return reinterpret_cast<size_t>(key);
    }
};

/// Hash key equality.
struct UtHashEqual {
    /// Use strcmp() for C string equality.
    bool operator()(const char* key1, const char* key2) const
    {
        assert(key1 && key2 && "NULL string not permitted as hash key");
        return !strcmp(key1, key2);
    }

    /// Use equality operator for STL strings.
    bool operator()(const std::string& key1, const std::string& key2) const
    {
        return key1 == key2;
    }

    /// Use equality operator for pointers.
    template<typename Key>
    bool operator()(const Key* key1, const Key* key2) const
    {
        return key1 == key2;
    }
};

/// Eventually UtHashMap will wrap the MSVC's crappy hash_map class.  For now
/// it's simply a wrapper for GNU's std::hash_map.  TODO: consider using
/// Boost (or TR1) unordered_map.
template<typename Key, typename Value>
class UtHashMap : public hash_map<Key, Value, UtHash, UtHashEqual>
{
};

} // namespace _UtHashMap

using _UtHashMap::UtHash;
using _UtHashMap::UtHashEqual;
using _UtHashMap::UtHashMap;

#endif // ndef UT_HASH_MAP_H
