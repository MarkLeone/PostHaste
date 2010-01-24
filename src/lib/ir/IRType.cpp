// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRType.h"
#include "ir/IRArrayType.h"
#include "ir/IRStructType.h"
#include "util/UtCast.h"
#include <assert.h>
#include <iostream>

const char* 
IRTypeKindToString(IRTypeKind kind)
{
    switch (kind) {
      case kIRVoidTy: return "void";
      case kIRBoolTy: return "bool";
      case kIRFloatTy: return "float";
      case kIRPointTy: return "point";
      case kIRVectorTy: return "vector";
      case kIRNormalTy: return "normal";
      case kIRColorTy: return "color";
      case kIRMatrixTy: return "matrix";
      case kIRStringTy: return "string";
      case kIRShaderTy: return "shader";
      case kIRArrayTy: return "array";
      case kIRStructTy: return "struct";
      case kIRNumTypeKinds:
          assert(false && "Invalid type kind");
          return "invalid";
    }
    assert(false && "Unknown type kind");
    return "unknown";
}

unsigned int
IRType::GetSize() const
{
    switch (mKind) {
      case kIRVoidTy:
          assert(false && "Should never get size of void");
          return 0;
      case kIRBoolTy:
      case kIRFloatTy:
          return 1;
      case kIRPointTy:
      case kIRVectorTy:
      case kIRNormalTy:
      case kIRColorTy:
          return 3;
      case kIRMatrixTy:
          return 16;
      case kIRStringTy:
      case kIRShaderTy:
          return static_cast<unsigned int>(sizeof(char*) / sizeof(float));
      case kIRArrayTy: {
          const IRArrayType* arrayTy = UtStaticCast<const IRArrayType*>(this);
          assert(arrayTy->GetLength() >= 0 && 
                 "Expected fixed-length array type");
          return arrayTy->GetLength() * arrayTy->GetElementType()->GetSize();
      }
      case kIRStructTy: {
          const IRStructType* structTy =
              UtStaticCast<const IRStructType*>(this);
          unsigned int sum = 0;
          for (unsigned int i = 0; i < structTy->GetNumMembers(); ++i)
              sum += structTy->GetMemberType(i)->GetSize();
          return sum;
      }
      case kIRNumTypeKinds:
          assert(false && "Invalid type kind");
          return 0;
    }
    assert(false && "Unimplemented kind of type");
    return 0;
}

// Print a type.
std::ostream& 
operator<<(std::ostream& out, const IRType& ty)
{
    switch (ty.GetKind()) {
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
          return out << IRTypeKindToString(ty.GetKind());
      case kIRArrayTy:
          return out << *UtStaticCast<const IRArrayType*>(&ty);
      case kIRStructTy:
          return out << *UtStaticCast<const IRStructType*>(&ty);
      case kIRNumTypeKinds:
          assert(false && "Invalid type kind");
          return out << "unknown";
    }
    assert(false && "Unknown type kind");
    return out << "unknown";
}
