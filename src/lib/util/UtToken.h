// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef UT_TOKEN_H
#define UT_TOKEN_H

#include <string>
#include <iosfwd>

/// A token is a canonical string (obtained from UtTokenFactory) that supports
/// fast equality comparisons with other tokens (i.e. a pointer comparison),
/// provided they all come from the same factory.  Eventually we might use
/// tokens as hash keys, since we can hash them by pointer rather than using
/// string hashing.
class UtToken {
public:
    /// Get an empty token.
    UtToken() : mName(NULL) { }

    /// Get the token name.  Tokens can also be implicitly converted to 
    /// std::string and const char*.
    const char* Get() const { return mName ? mName : ""; }

    /// A token can be converted (implicitly or explicitly) to a C string.
    operator const char*() const { return mName ? mName : ""; }

    /// Tokens support fast equality tests, but should only be compared with
    /// tokens from the same factory.
    bool operator==(const UtToken& token) const { return mName == token.mName; }

    /// Test tokens for inequality.
    bool operator!=(const UtToken& token) const { return mName != token.mName; }

private:
    friend class UtTokenFactory;

    // The constructor is private, since tokens are constructed only by
    // UtTokenFactory.
    UtToken(const char* name) : mName(name) { } 

    // The name is owned by the factory, not the token.
    const char* mName;
};

#endif // ndef UT_TOKEN_H
