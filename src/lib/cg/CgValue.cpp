// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgValue.h"
#include "cg/CgConst.h"
#include "cg/CgTypes.h"
#include "cg/CgVars.h"
#include "ir/IRConst.h"
#include "ir/IRType.h"
#include "ir/IRValue.h"
#include "ir/IRVar.h"
#include "util/UtCast.h"
#include <llvm/Constant.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Support/IRBuilder.h>

/// Convert an IR value to an LLVM value (possibly a first-class
/// aggregate).  If the value is a variable, it value is loaded.
llvm::Value* 
CgValue::ConvertVal(const IRValue* value) const
{
    if (const IRConst* constant = UtCast<const IRConst*>(value))
        return mConsts->Convert(constant);

    // Otherwise it's a variable.  Get its value (generating a load
    // instruction if necessary).
    const IRVar* var = UtStaticCast<const IRVar*>(value);
    return mVars->GetValue(var);
}

// Convert an IR value to an LLVM value that is suitable as a shadeop arg
// (i.e. a non-scalar value is converted to a location).
llvm::Value* 
CgValue::ConvertArg(const IRValue* value) const
{
    assert(value != NULL && "Cannot convert NULL argument");

    // If it's a constant, we might have to "spill" it to a global if it's
    // passed by reference.
    if (const IRConst* constant = UtCast<const IRConst*>(value))
        return ConvertConstArg(constant);

    // Otherwise it's a variable.  Get its location if it's passed by
    // reference; otherwise get its value (generating a load instruction if
    // necessary).
    const IRVar* var = UtStaticCast<const IRVar*>(value);
    return mTypes->IsPassByRef(var->GetType()) ? 
        ConvertArrayPtr(mVars->GetLocation(var)) : mVars->GetValue(var);
}

// Convert an IR constant to a function argument.  Floats and strings convert
// as usual, but triples, matrices, and arrays are converted into pointers to
// global constants.
llvm::Value* 
CgValue::ConvertConstArg(const IRConst* constant) const
{
    // Convert the value.
    llvm::Constant* value = mConsts->Convert(constant);

    // If it's pass-by-value, return it.
    if (!mTypes->IsPassByRef(constant->GetType()))
        return value;

    // Any other constant will be passed by reference, so we store it in a
    // global constant and return a pointer.
    llvm::Value* arg =
        new llvm::GlobalVariable(*mModule, value->getType(), true,
                                 llvm::GlobalValue::InternalLinkage,
                                 value, "");
    // If it's an array, convert the type from pointer-to-array to
    // pointer-to-element, which is the shadeop calling convention.
    return ConvertArrayPtr(arg);
}

// If the given value is a pointer to an array, convert it to a pointer
// to the first array element, which is the shadeop calling convention.
llvm::Value* 
CgValue::ConvertArrayPtr(llvm::Value* value) const
{
    // If it's not a pointer to an array, return it unmodified.
    const llvm::PointerType* ptrTy = 
        llvm::dyn_cast<llvm::PointerType>(value->getType());
    if (ptrTy == NULL)
        return value;
    const llvm::ArrayType* arrayTy = 
        llvm::dyn_cast<llvm::ArrayType>(ptrTy->getElementType());
    if (arrayTy == NULL)
        return value;

    // Generate a "getelementptr" instruction that references the first element.
    llvm::Constant* zero = GetInt(0);
    llvm::Value* indices[] = { zero, zero };
    return mBuilder->CreateInBoundsGEP(value, indices, indices+2);
}


// Convert bool (i32) to bit for LLVM branch/select instruction.
llvm::Value* 
CgValue::BoolToBit(llvm::Value* condVal) const
{
    const llvm::Type* boolTy = llvm::Type::getInt32Ty(*mContext);
    assert(condVal->getType() == boolTy && "Expected bool condition value");
    llvm::Value* zero = llvm::ConstantInt::get(boolTy, 0);
    return mBuilder->CreateICmpNE(condVal, zero, "tobool");
}
