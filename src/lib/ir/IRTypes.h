// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_TYPES_H
#define IR_TYPES_H

#include "ir/IRDetail.h"
#include "ir/IRType.h"
#include "ir/IRArrayType.h"
#include "ir/IRStructType.h"
#include "util/UtVector.h"
#include <set>
#include <vector>

/// Instances of simple types are stored in a statically initalized vector
/// (see IRTypes::Get()).
struct IRStaticTypes : public UtVector<IRType> {
    /// Construct vector of static type instances.
    IRStaticTypes();
};

/// Factory for constructing IR types.  Provides static instances of basic
/// types (e.g. float, point).  IR types support pointer equality.  For
/// example, arrays with the same element type and length will have identical
/// types (provided they came from the same instance of the IRTypes factory).
class IRTypes {
public:
    // Beware of static initialization: these static type instances shouldn't
    // be used from other static initializers.
    static const IRType* GetVoidTy() { return Get(kIRVoidTy); }
    static const IRType* GetBoolTy() { return Get(kIRBoolTy); }
    static const IRType* GetFloatTy() {return Get(kIRFloatTy); }
    static const IRType* GetPointTy() {return Get(kIRPointTy); }
    static const IRType* GetVectorTy() {return Get(kIRVectorTy); }
    static const IRType* GetNormalTy() {return Get(kIRNormalTy); }
    static const IRType* GetColorTy() {return Get(kIRColorTy); }
    static const IRType* GetMatrixTy() {return Get(kIRMatrixTy); }
    static const IRType* GetStringTy() {return Get(kIRStringTy); }
    static const IRType* GetShaderTy() {return Get(kIRShaderTy); }

    /// Get a type instance of the specified kind.
    static const IRType* Get(IRTypeKind kind) { return &sStaticTypes[kind]; }

    /// Get an array type instance.  Guaranteed to be pointer-equal to other
    /// array types with the same element type and length.
    const IRArrayType* GetArrayType(const IRType* elementType, int length);

    /// Get a struct type instance.  Guaranteed to be pointer-equal to other
    /// struct types with the same struct name and member names/types/details.
    const IRStructType* GetStructType(const std::string& name,
                                      const std::vector<const IRType*>& types,
                                      const std::vector<IRDetail>& details,
                                      const std::vector<std::string>& names);

private:
    // Vector of static type instances.
    static IRStaticTypes sStaticTypes;

    // The set of canonical array type instances.
    std::set<IRArrayType> mArrayTypes;

    // The set of canonical struct type instances.
    std::set<IRStructType> mStructTypes;
};

#endif // ndef IR_TYPES_H
