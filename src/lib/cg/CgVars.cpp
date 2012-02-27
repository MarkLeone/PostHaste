// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgVars.h"
#include "cg/CgTypes.h"
#include "ir/IRShaderParam.h"
#include "ir/IRLocalVar.h"
#include "util/UtCast.h"
#include "util/UtLog.h"
#include <llvm/Support/IRBuilder.h>
#include <llvm/Value.h>

// Bind an IR variable to an LLVM value (typically a location, unless
// it's a scalar input shader parameter).  Must be called only once for a
// given variable.
void 
CgVars::Bind(const IRVar* var, llvm::Value* value)
{
    // Unless it's a shader parameter, the value should be a pointer.
    assert((UtCast<const IRShaderParam*>(var) ||
            llvm::isa<llvm::PointerType>(value->getType())) &&
           "Expected pointer type for location");
    assert(mBindings.find(var) == mBindings.end() &&
           "Variable already has a location");
    mBindings[var] = value;
}

// Get a variable's binding.  A new location is created if it's a
    /// previously unencountered local variable.
llvm::Value*
CgVars::GetBinding(const IRVar* var, bool shouldExist)
{
    Bindings::const_iterator it = mBindings.find(var);
    if (it != mBindings.end())
        return it->second;

    // If it's unbound, it had better be a local variable.
    if (UtCast<const IRLocalVar*>(var) == NULL) {
        mLog->Write(kUtInternal, "Codegen: unknown variable '%s'",
                    var->GetShortName());
        assert(false && "Codegen: Unknown variable");
    }

    // Generate an "alloca" instruction and create a new binding.
    llvm::Value* location = MakeAlloca(var);
    Bind(var, location);
    return location;
}

// Generate an "alloca" instruction for the specified variable
llvm::Value*
CgVars::MakeAlloca(const IRVar* var) const
{
    llvm::Twine name = llvm::Twine("_") + var->GetShortName();
    llvm::Type* ty = mTypes->Convert(var->GetType());
    return GenAlloca(ty, name);
}

// Get the location of a variable.
llvm::Value* 
CgVars::GetLocation(const IRVar* var)
{
    llvm::Value* binding = GetBinding(var);
    assert(llvm::isa<llvm::PointerType>(binding->getType()) &&
           "Expected a location in variable codegen");
    return binding;
}

// Get the value of a variable, generating a load instruction if necessary.
llvm::Value*
CgVars::GetValue(const IRVar* var)
{
    // Currently all variables are bound to locations.  Eventually we might
    // pass scalar input arguments of plugin entry functions by value, in
    // which case the parameters would be bound to the LLVM function args
    // (llvm::Arg).
    llvm::Value* binding = GetBinding(var);
    assert(llvm::isa<llvm::PointerType>(binding->getType()) &&
           "Pointer type expected for variable binding");
    return mBuilder->CreateLoad(binding);
}
