// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef OP_INFO_H
#define OP_INFO_H

#include "ops/Opcode.h"
#include "util/UtHashMap.h"

/// OpInfo provides info about the shadeops that implement an instruction.  
/// A shadeop is a C function compiled to LLVM IR.  An instruction opcode maps
/// to a shadeop name, which might require mangling to resolve overloading.
/// For example, the "add" instruction (kOpcode_Add) is implemented by
/// shadeops that operate on various combinations of floats and triples
/// (e.g. OpAdd_ff, OpAdd_ft).
class OpInfo {
public:
    /// Get the name of the shadeop that implements the specified instruction,
    /// if any.  Returns NULL if there is no implementation.  The name might
    /// require mangling to resove overloading (see OpIsOverloaded).
    static const char* GetOpName(Opcode opcode);

    /// An overloaded shadeop name is mangled with suffix denoting the
    /// argument types, ("f" for float, "t" for triple, "F" for a float array,
    /// etc).  For example, OpAdd_tt adds two triples.
    static bool IsOverloaded(Opcode opcode);

    /// Check whether a shadeop is overloaded by result type.
    static bool IsOverloadedByResult(Opcode opcode);

    /// Check whether a shadeop has a void result.  If not, the first argument
    /// in the SLO instruction is the result.
    static bool IsVoid(Opcode opcode);

    /// Check whether a shadeop has one or more output parameters.
    static bool HasOutput(Opcode opcode);

    /// Check whether the ith argument of the specified instruction is an
    /// output argument.
    static bool IsOutput(Opcode opcode, int i);

    /// Check whether the ith argument of the specified instruction is killed
    /// (i.e. value ignored on input; completely overwritten on output).
    static bool KillsArg(Opcode opcode, int i);
};

#endif // OP_NAMES_H
