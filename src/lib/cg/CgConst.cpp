// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgConst.h"
#include "cg/CgTypes.h"
#include "ir/IRValues.h"
#include "util/UtCast.h"
#include <llvm/Constants.h>
#include <llvm/DerivedTypes.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Support/IRBuilder.h>

// Convert an IR constant to an LLVM constant.
llvm::Constant* 
CgConst::Convert(const IRConst* constant) const
{
    if (const IRNumConst* c = UtCast<const IRNumConst*>(constant))
        return ConvertNumData(c->GetData(), c->GetType());
    if (const IRNumArrayConst* c = UtCast<const IRNumArrayConst*>(constant))
        return ConvertNumArray(c);
    if (const IRStringConst* c = UtCast<const IRStringConst*>(constant))
        return ConvertStringData(c->Get());
    if (const IRStringArrayConst* c =
        UtCast<const IRStringArrayConst*>(constant))
        return ConvertStringArray(c);
    assert(false && "Unimplemented kind of constant");
    return NULL;
}

/// Convert numeric constant data
llvm::Constant* 
CgConst::ConvertNumData(const float* data, const IRType* ty) const
{
    // Float constant?
    if (ty->IsFloat()) {
        llvm::Type* floatTy = llvm::Type::getFloatTy(*mContext);
        return llvm::ConstantFP::get(floatTy, data[0]);
    }

    // Triple?
    if (ty->IsTriple()) 
        return MakeVector(data, 3);

    // Otherwise it must be a matrix.
    assert(ty->IsMatrix() && "Expected matrix constant");
    return MakeMatrix(data);
}

/// Make a float constant.
llvm::Constant*
CgConst::MakeFloat(float f) const
{
    llvm::Type* floatTy = llvm::Type::getFloatTy(*mContext);
    return llvm::ConstantFP::get(floatTy, f);
}

// Make a float array constant.
llvm::Constant* 
CgConst::MakeFloatArray(const float* data, unsigned int length) const
{
    llvm::Type* floatTy = llvm::Type::getFloatTy(*mContext);
    std::vector<llvm::Constant*> elements(length);
    for (unsigned int i = 0; i < length; ++i)
        elements[i] = llvm::ConstantFP::get(floatTy, data[i]);
    llvm::ArrayType* arrayTy = llvm::ArrayType::get(floatTy, length);
    return llvm::ConstantArray::get(arrayTy, elements);
}

// Make a vector constant, which is a struct containing a float array.
// Note: this must agree with the definition of OpVec3.
llvm::Constant* 
CgConst::MakeVector(const float* data, unsigned int length) const
{
    std::vector<llvm::Constant*> members(3);
    members[0] = MakeFloat(data[0]);
    members[1] = MakeFloat(data[1]);
    members[2] = MakeFloat(data[2]);
    llvm::StructType* vecTy = llvm::dyn_cast<llvm::StructType>(mTypes->GetVecTy());
    assert(vecTy != NULL && "Expected vector type to be an llvm::StructType");
    return llvm::ConstantStruct::get(vecTy, llvm::ArrayRef<llvm::Constant*>(members));
}

// Make a matrix constant, which is a struct containing an array of 4-vectors.
// Note: this must agree with the definition of OpMatrix4.
llvm::Constant*
CgConst::MakeMatrix(const float* data) const 
{
    std::vector<llvm::Constant*> rows(4);
    for (int i = 0; i < 4; ++i) {
        rows[i] = MakeFloatArray(data, 4);
        data += 4;
    }
    llvm::Type* rowTy = rows[0]->getType();
    llvm::ArrayType* arrayTy = llvm::ArrayType::get(rowTy, 4);
    llvm::Constant* array = llvm::ConstantArray::get(arrayTy, rows);

    llvm::StructType* matrixTy = llvm::dyn_cast<llvm::StructType>(mTypes->GetMatrixTy());
    assert(matrixTy != NULL && "Expected matrix type to be an llvm::StructType");
    return llvm::ConstantStruct::get(matrixTy, llvm::ArrayRef<llvm::Constant*>(&array, 1));
}

llvm::Constant* 
CgConst::ConvertNumArray(const IRNumArrayConst* array) const
{
    // Get the element type.
    const IRArrayType* arrayTy =
        UtStaticCast<const IRArrayType*>(array->GetType());
    const IRType* elemTy = arrayTy->GetElementType();

    // Get the data, length, and stride.
    const float* data = array->GetData();
    unsigned int length = array->GetLength();
    assert((int) length == arrayTy->GetLength() &&
           "Array constant length mismatch");
    unsigned int stride = elemTy->GetSize();

    // Convert the elements
    std::vector<llvm::Constant*> elements(length);
    for (unsigned int i = 0; i < length; ++i) {
        elements[i] = ConvertNumData(data, elemTy);
        data += stride;
    }

    // Construct array type and construct an LLVM array constant.
    llvm::ArrayType* llvmTy = 
        llvm::ArrayType::get(mTypes->Convert(elemTy), length);
    return llvm::ConstantArray::get(llvmTy, elements);
}

llvm::Constant* 
CgConst::ConvertStringData(const char* str) const
{
    // Create a char array constant.
    // XXX use IRBuilder::CreateGlobalString instead.
    llvm::Constant* array = llvm::ConstantArray::get(*mContext, str, true);

    // Store the char array in a global constant.
    llvm::GlobalVariable* global = 
        new llvm::GlobalVariable(*mModule, array->getType(), true,
                                 llvm::GlobalValue::InternalLinkage, array, "");

    // Return a constant "getelementptr" expression that points to the first
    // character.
    llvm::Constant* zero = GetInt(0);
    llvm::Constant* indices[] = { zero, zero };
    return llvm::ConstantExpr::getGetElementPtr(global, indices, 2);
}

llvm::Constant* 
CgConst::ConvertStringArray(const IRStringArrayConst* array) const
{
    // Construct array type and construct an undefined array value.
    llvm::Type* charTy = llvm::Type::getInt8Ty(*mContext);
    llvm::Type* stringTy = 
        llvm::PointerType::get(charTy, CgTypes::kDefaultAddressSpace);
    llvm::Type* arrayTy = 
        llvm::ArrayType::get(stringTy, array->GetLength());
    llvm::Constant* arrayVal = llvm::UndefValue::get(arrayTy);

    // Insert the string constants into the array.
    unsigned int length = array->GetLength();
    for (unsigned int i = 0; i < length; ++i) {
        llvm::Constant* stringVal = ConvertStringData(array->GetElement(i));
        arrayVal = llvm::ConstantExpr::getInsertValue(arrayVal, stringVal,
                                                      llvm::ArrayRef<unsigned>(&i, 1));
    }
    return arrayVal;
}
