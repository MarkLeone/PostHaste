// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_VARS_H
#define CG_VARS_H

#include "cg/CgComponent.h"
#include "cg/CgFwd.h"
#include <map>
class IRVar;

/// Code generation for IR variables.  Variables are bound to values or
/// locations.  Local variables are bound to pointers from alloca
/// instructions.  Shader parameters are bound to function arguments, which
/// might be scalars (if pass by value) or pointers (if pass by reference).
class CgVars : public CgComponent {
public:
    /// Constructor.
    CgVars(const CgComponent& state) :
        CgComponent(state)
    {
    }

    /// Reset this component, clear the variable bindings.
    void Reset() { mBindings.clear(); }

    /// Bind an IR variable to an LLVM value (typically a location, unless
    /// it's a scalar input shader parameter).  Must be called only once for a
    /// given variable.
    void Bind(const IRVar* var, llvm::Value* value);

    /// Get the location of a variable.  A new location is created if it's a
    /// previously unencountered local variable.
    llvm::Value* GetLocation(const IRVar* var);

    /// Get the value of a variable, generating a load instruction if
    /// necessary.
    llvm::Value* GetValue(const IRVar* var);

private:
    /// Map from IR variable to LLVM value.
    typedef std::map<const IRVar*, llvm::Value*> Bindings;

    /// Map from IR variable to location.
    Bindings mBindings;

    /// Get a variable's binding.  A new location is created if it's a
    /// previously unencountered local variable.
    llvm::Value* GetBinding(const IRVar* var, bool shouldExist=false);

    /// Generate an "alloca" instruction for the specified variable
    llvm::Value* MakeAlloca(const IRVar* var) const;
};

#endif // ndef CG_VARS_H
