// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_VALUE_H
#define CG_VALUE_H

#include "cg/CgComponent.h"
class IRValue;
class IRConst;

/// Code generation for IR values (variables and constants).
class CgValue : public CgComponent {
public:
    /// Constructor
    CgValue(const CgComponent& state) :
        CgComponent(state)
    {
    }

    /// Convert an IR value to an LLVM value (possibly a first-class
    /// aggregate).  If the value is a variable, it value is loaded.
    llvm::Value* ConvertVal(const IRValue* value) const;

    /// Convert an IR value to an LLVM value that is suitable as a shadeop arg
    /// (i.e. a non-scalar value is converted to a location).
    llvm::Value* ConvertArg(const IRValue* value) const;

    /// Convert an IR constant to a function argument.  Scalars and strings
    /// convert as usual, but triples and matrices are converted into pointer
    /// to global constants.
    llvm::Value* ConvertConstArg(const IRConst* constant) const;

    /// If the given value is a pointer to an array, convert it to a pointer
    /// to the first array element, which is the shadeop calling convention.
    llvm::Value* ConvertArrayPtr(llvm::Value* value) const;

    /// Convert bool (i32) to bit for LLVM branch/select instruction.
    llvm::Value* BoolToBit(llvm::Value* condVal) const;
};

#endif // ndef CG_VALUE_H
