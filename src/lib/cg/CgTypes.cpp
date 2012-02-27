// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgTypes.h"
#include "ir/IRTypes.h"
#include "util/UtCast.h"
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>

CgTypes::CgTypes(llvm::LLVMContext* context) :
    mContext(context),
    mBasicTypes(kIRNumTypeKinds, NULL)
{
    // These types must agree with the shadeops in lib/ops/Ops.cpp.
    llvm::Type* floatTy = llvm::Type::getFloatTy(*mContext);
    mBasicTypes[kIRFloatTy] = floatTy;
    mBasicTypes[kIRBoolTy] = llvm::Type::getInt32Ty(*mContext);
    mBasicTypes[kIRVoidTy] = llvm::Type::getVoidTy(*mContext);

    // A triple is a struct containing a float[3] array.
    llvm::Type* tripleTy = GetVecTy(3);
    mBasicTypes[kIRPointTy] = tripleTy;
    mBasicTypes[kIRVectorTy] = tripleTy;
    mBasicTypes[kIRNormalTy] = tripleTy;
    mBasicTypes[kIRColorTy] = tripleTy;

    // A matrix is a struct containing an array of four 4-vectors.
    mBasicTypes[kIRMatrixTy] = GetMatrixTy();

    mBasicTypes[kIRStringTy] = 
        llvm::PointerType::get(llvm::Type::getInt8Ty(*mContext), 
                               kDefaultAddressSpace);
    mBasicTypes[kIRShaderTy] =
        // LLVM requires the use of i8* instead of void*.
        llvm::PointerType::get(llvm::Type::getInt8Ty(*mContext),
                               kDefaultAddressSpace);
}


// Get the type of a vector of the specified length, which is a struct
// containing a float array (must agree with OpVec3, OpVec4).
llvm::Type* 
CgTypes::GetVecTy(unsigned int length) const
{
    llvm::Type* floatTy = llvm::Type::getFloatTy(*mContext);
    llvm::Type* arrayTy = llvm::ArrayType::get(floatTy, length);
    return llvm::StructType::get(*mContext, arrayTy, NULL);
}

// Get the type of a matrix, which is a struct containing an array of
// four 4-vectors (must agree with OpMatrix4).
llvm::Type* 
CgTypes::GetMatrixTy() const
{
    llvm::Type* vecTy = GetVecTy(4);
    llvm::Type* arrayTy = llvm::ArrayType::get(vecTy, 4);
    return llvm::StructType::get(*mContext, arrayTy, NULL);
}


// Get the LLVM type for an IR type.
llvm::Type* 
CgTypes::Convert(const IRType* ty) const
{
    IRTypeKind kind = ty->GetKind();
    switch (kind) {
      case kIRVoidTy:
      case kIRBoolTy:
      case kIRFloatTy:
      case kIRPointTy:
      case kIRVectorTy:
      case kIRNormalTy:
      case kIRColorTy:
      case kIRMatrixTy:
      case kIRStringTy:
      case kIRShaderTy:
          return mBasicTypes[kind];
      case kIRArrayTy: {
          // TODO: codegen support resizable arrays.
          const IRArrayType* arrayTy = UtStaticCast<const IRArrayType*>(ty);
          assert(arrayTy->GetLength() >= 0 &&
                 "No codegen support for resizable arrays");
          llvm::Type* elemTy = Convert(arrayTy->GetElementType());
          return llvm::ArrayType::get(elemTy, arrayTy->GetLength());
      }
      case kIRStructTy: {
          const IRStructType* structTy = UtStaticCast<const IRStructType*>(ty);
          unsigned int numMembers = structTy->GetNumMembers();
          std::vector<llvm::Type*> memberTypes(numMembers);
          for (unsigned int i = 0; i < numMembers; ++i)
              memberTypes[i] = Convert(structTy->GetMemberType(i));
          return llvm::StructType::get(*mContext, memberTypes);
      }
      case kIRNumTypeKinds:
          assert(false && "Invalid IR type");
    }
    assert(false && "Unimplemented kind of IR type");
    return llvm::Type::getFloatTy(*mContext);
}


// Check whether the specified IR type should be passed/returned by reference.
// Only scalars are passed/returned by value (i.e. float, bool string, and
// shader object).
bool 
CgTypes::IsPassByRef(const IRType* ty)
{
    return !(ty->IsFloat() || ty->IsBool() || ty->IsString() || ty->IsShader());
}

// Convert the type of a shader or shadeop parameter.  Input scalars
// (float, bools, strings, and shader objects) are passed by value, while
// other types and output parameters are passed by reference.
llvm::Type* 
CgTypes::ConvertParamType(const IRType* type, bool isOutput) const
{
    llvm::Type* llvmTy = Convert(type);
    if (isOutput || IsPassByRef(type))
        llvmTy = llvm::PointerType::get(llvmTy, kDefaultAddressSpace);
    return llvmTy;
}
