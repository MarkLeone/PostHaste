// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRTypes.h"
#include "ir/IRArrayType.h"
#include "ir/IRStructType.h"

// Statically initialized vector of simple type instances.
IRStaticTypes IRTypes::sStaticTypes;

// Construct static vector of simple type instances.
IRStaticTypes::IRStaticTypes()
{
    reserve(kIRNumTypeKinds);
    for (unsigned int kind = 0; kind < kIRNumTypeKinds; ++kind)
        push_back(IRType(static_cast<IRTypeKind>(kind)));
}

// Get an array type instance.
const IRArrayType* 
IRTypes::GetArrayType(const IRType* elementType, int length)
{
    // Construct a candidate type and insert it into the set of canonical type
    // instances, yielding an iterator for the canonical instance.
    IRArrayType ty(elementType, length);
    typedef std::set<IRArrayType>::iterator Iter;
    std::pair<Iter, bool> p = mArrayTypes.insert(ty);
    return &(*p.first);
}

// Get a struct type instance.
const IRStructType* 
IRTypes::GetStructType(const std::string& name,
                       const std::vector<const IRType*>& types,
                       const std::vector<IRDetail>& details,
                       const std::vector<std::string>& names)
{
    // Construct a candidate type and insert it into the set of canonical type
    // instances, yielding an iterator for the canonical instance.
    IRStructType ty(name, types, details, names);
    typedef std::set<IRStructType>::iterator Iter;
    std::pair<Iter, bool> p = mStructTypes.insert(ty);
    return &(*p.first);
}
