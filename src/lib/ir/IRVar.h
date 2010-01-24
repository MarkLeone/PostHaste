// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_VAR_H
#define IR_VAR_H

#include "ir/IRValue.h"
#include <assert.h>
#include <string>

/**
   Abstract base class of IR variables, with the following derived classes:
      - IRGlobalVar
      - IRLocalVar
      - IRShaderParam

   Future derived classes:
      - IRMemberVar
      - IRMethodParam
*/
class IRVar : public IRValue {
public:
    /// Get the full variable name.
    const char* GetFullName() const { return mName.c_str(); }

    /// Get the concise name of the variable.
    const char* GetShortName() const
    {
        return mName.c_str() + (mName[0] == '$' ? 1 : 0);
    }

    /// Get the space name, if any (empty if none);
    const char* GetSpace() const { return mSpace.c_str(); }

    /// Construct an IR variable of the specified kind with the given
    /// name, type, and detail.  The space name can be NULL or empty.
    IRVar(IRValueKind kind, const char* name, const IRType* type,
          IRDetail detail, const char* space) :
        IRValue(kind, type, detail),
        mName(name),
        mSpace(space ? space : "")
    {
        assert(kIRVar_Begin < kind && kind < kIRVar_End &&
               "Invalid kind of variable");
    }

    /// Destructor.  Not sure why this can't be pure virtual.
    virtual ~IRVar();

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        IRValueKind kind = val->GetKind();
        return kIRVar_Begin < kind && kind < kIRVar_End;
    }

    /// Print the declaration of this variable.
    virtual void WriteDecl(std::ostream& out, unsigned int indent) const;

private:
    friend class XfRaiseImpl;
    friend class XfLowerImpl;
    std::string mName;
    std::string mSpace;
};

/// Print a variable
std::ostream& operator<<(std::ostream& out, const IRVar& var);

#endif // ndef IR_VAR_H
