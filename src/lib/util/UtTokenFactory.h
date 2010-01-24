// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef UT_TOKEN_FACTORY_H
#define UT_TOKEN_FACTORY_H

#include "util/UtHashMap.h"
#include "util/UtToken.h"
#include <string>

/// A token factory produces tokens (UtToken), which are canonical strings
/// that support fast equality comparisons (i.e. via pointer equality).
class UtTokenFactory {
public:
    /// Get a token for the given string.
    UtToken Get(const char* s)
    {
        Map::const_iterator it = mMap.find(s);
        if (it == mMap.end()) {
            Map::value_type pair(s, true);
            it = mMap.insert(pair).first;
        }
        return UtToken(it->first.c_str());
    }

    /// Get a token for an STL string.
    UtToken Get(const std::string& s) { return Get(s.c_str()); }

private:
    // We don't have hash_set, so we use a hash map with dummy values.
    typedef UtHashMap<std::string, bool> Map;

    // The set of canonical strings.
    Map mMap;
};

#endif // ndef UT_TOKEN_FACTORY_H
