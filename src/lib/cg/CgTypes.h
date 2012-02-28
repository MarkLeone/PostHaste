// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_TYPES_H
#define CG_TYPES_H

#include "cg/CgComponent.h"
#include "cg/CgFwd.h"
#include "util/UtVector.h"
class IRType;


/// Code generation for IR types.
class CgTypes : public CgComponent {
public:
    /// Constructor.
    CgTypes(const CgComponent& state);

    /// Convert IR type to LLVM type.
    llvm::Type* Convert(const IRType* ty) const;

    /// Convert the type of a shader or shadeop parameter.  Input scalars
    /// (float, bools, strings, and shader objects) are passed by value, while
    /// other types and output parameters are passed by reference.
    llvm::Type* ConvertParamType(const IRType* type, bool isOutput) const;

    /// Check whether the specified IR type should be passed/returned by
    /// reference.  Only scalars are passed/returned by value (i.e. float,
    /// bool string, and ashader object).
    static bool IsPassByRef(const IRType* ty);

    /// Get the type of a vector, which is a struct containing an array of three
    /// floats (see OpVec3).
    llvm::Type* GetVecTy() const;

    /// Get the type of a matrix, which is a struct containing an array of
    /// four 4-vectors (see OpMatrix4).
    llvm::Type* GetMatrixTy() const;

    /// Our LLVM pointers all use a default address space.
    static const unsigned int kDefaultAddressSpace = 0;

private:
    UtVector<llvm::Type*> mBasicTypes;
};

#endif // ndef CG_TYPES_H
