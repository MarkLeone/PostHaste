// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_STRUCT_TYPE_H
#define IR_STRUCT_TYPE_H

#include "ir/IRDetail.h"
#include "ir/IRType.h"
#include <assert.h>
#include <iosfwd>
#include <string>
#include <vector>

/// IR struct type.
class IRStructType : public IRType {
public:
    /// Get the struct type name.
    const char* GetName() const { return mName.c_str(); }
    
    /// Get the number of members.
    unsigned int GetNumMembers() const 
    {
        return static_cast<unsigned int>(mTypes.size()); 
    }

    /// Get the Nth member type.
    const IRType* GetMemberType(unsigned int n) const 
    {
        assert(n < mTypes.size() && "Index out of range");
        return mTypes[n]; 
    }

    /// Get the detail of the Nth member.
    IRDetail GetMemberDetail(unsigned int n) const
    {
        assert(n < mDetails.size() && "Index out of range");
        return mDetails[n]; 
    }

    /// Get the Nth member name.
    const char* GetMemberName(unsigned int n) const 
    {
        assert(n < mNames.size() && "Index out of range");
        return mNames[n].c_str();
    }

    /// Print struct type (the entire type, not just the name).
    void Dump(std::ostream& out) const;
    
    /// Check if a type is an instance of this class. Required by IRCast().
    static bool IsInstance(const IRType* type) { 
        return type->GetKind() == kIRStructTy;
    }

    /// Ordering for a set of struct types (needed by IRTypes factory).
    bool operator<(const IRStructType& ty) const;

protected:
    /// Struct types are constructed only by the IRTypes factory.
    friend class IRTypes;

    /// Construct a struct type.
    IRStructType(const std::string& name,
                 const std::vector<const IRType*>& types,
                 const std::vector<IRDetail>& details,
                 const std::vector<std::string>& names) :
        IRType(kIRStructTy),
        mName(name),
        mTypes(types),
        mDetails(details),
        mNames(names)
    {
        assert(mTypes.size() == GetNumMembers() &&
               mDetails.size() == GetNumMembers() &&
               mNames.size() == GetNumMembers() &&
               "Inconsistent number of members");
    }

private:
    std::string mName;
    std::vector<const IRType*> mTypes;
    std::vector<IRDetail> mDetails;
    std::vector<std::string> mNames;
};

/// Print the name of a struct type.
std::ostream& operator<<(std::ostream& out, const IRStructType& ty);

#endif // ndef IR_STRUCT_TYPE_H
