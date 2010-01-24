// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRNumConst.h"
#include "ir/IRTypes.h"
#include "util/UtCast.h"
#include <iostream>

// Write the given numeric/array data with the specified type.
void
IRWriteData(const float* data, const IRType* type, std::ostream& out)
{
    assert((type->IsNumeric() || type->IsArray()) && 
           "Expected numeric/array data");

    switch (type->GetKind()) {
      case kIRFloatTy: {
          out << *data;
          return;
      }
      case kIRPointTy:
      case kIRVectorTy:
      case kIRNormalTy:
      case kIRColorTy: {
          out << "(" << data[0] << ", " << data[1] << ", " << data[2] << ")";
          return;
      }
      case kIRMatrixTy: {
          out << "(";
          for (int i = 0; i < 16; ++i) {
              if (i > 0)
                  out << ", ";
              out << data[i];
          }
          out << ")";
          return;
      }
      case kIRArrayTy: {
          const IRArrayType* arrayTy = UtStaticCast<const IRArrayType*>(type);
          assert(arrayTy->GetLength() >= 0 && 
                 "Expected fixed-length array type");
          int length = arrayTy->GetLength();
          const IRType* elementTy = arrayTy->GetElementType();
          size_t stride = elementTy->GetSize();
          out << "{";
          for (int i = 0; i < length; ++i) {
              if (i > 0)
                  out << ", ";
              IRWriteData(data, elementTy, out);
              data += stride;
          }
          out << "}";
          return;
      }
      case kIRVoidTy:
      case kIRBoolTy:
      case kIRStringTy:
      case kIRShaderTy:
      case kIRStructTy:
      case kIRNumTypeKinds: {
          assert((type->IsNumeric() || type->IsArray()) && 
                 "Expected numeric/array type");
          return;
      }
    }
    assert(false && "Unimplemented kind of type");
}

std::ostream& 
operator<<(std::ostream& out, const IRNumConst& constant)
{
    IRWriteData(constant.GetData(), constant.GetType(), out);
    return out;
}
