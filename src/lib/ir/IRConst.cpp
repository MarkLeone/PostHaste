// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRConst.h"
#include "ir/IRNumConst.h"
#include "ir/IRNumArrayConst.h"
#include "ir/IRStringConst.h"
#include "ir/IRStringArrayConst.h"
#include "util/UtCast.h"
#include <iostream>

IRConst::~IRConst() { }

std::ostream& 
operator<<(std::ostream& out, const IRConst& c)
{
    if (const IRNumConst* num = UtCast<const IRNumConst*>(&c))
        return out << *num;
    if (const IRStringConst* str = UtCast<const IRStringConst*>(&c))
        return out << *str;
    if (const IRNumArrayConst* nums = UtCast<const IRNumArrayConst*>(&c))
        return out << *nums;
    if (const IRStringArrayConst* strs = UtCast<const IRStringArrayConst*>(&c))
        return out << *strs;
    assert(false && "Unimplemented kind of constant");
    return out;
}

