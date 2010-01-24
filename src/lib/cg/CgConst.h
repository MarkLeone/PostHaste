// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_CONST_H
#define CG_CONST_H

#include "cg/CgComponent.h"
class IRConst;
class IRNumArrayConst;
class IRNumConst;
class IRStringArrayConst;
class IRStringConst;
class IRType;

/// Code generation for IR constants.
class CgConst : public CgComponent {
public:
    /// Constructor.
    CgConst(const CgComponent& state) :
        CgComponent(state)
    {
    }

    /// Convert an IR constant to an LLVM constant.  String constants yield
    /// pointers to global char array constants (e.g. getelemptr(@S, 0, 0)).
    llvm::Constant* Convert(const IRConst* constant) const;

protected:
    /// Convert numeric constant data
    llvm::Constant* ConvertNumData(const float* data, const IRType* ty) const;

    /// Make a float array constant.
    llvm::Constant* MakeFloatArray(const float* data,
                                   unsigned int length) const;

    /// Make a vector constant, which is a struct containing a float array.
    llvm::Constant* MakeVector(const float* data, unsigned int length) const;

    /// Make a matrix constant, which is a struct containing an array of
    /// 4-vectors.
    llvm::Constant* MakeMatrix(const float* data) const;

    /// Convert a numeric array constant.
    llvm::Constant* ConvertNumArray(const IRNumArrayConst* array) const;

    /// Convert a string constant
    llvm::Constant* ConvertStringData(const char* str) const;

    /// Convert a string array constant
    llvm::Constant* ConvertStringArray(const IRStringArrayConst* array) const;
};

#endif // ndef CG_CONST_H

