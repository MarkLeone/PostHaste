// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_INST_H
#define CG_INST_H

#include "cg/CgComponent.h"
#include "cg/CgFwd.h"
#include "cg/CgTypedefs.h"
#include "ir/IRTypedefs.h"
#include "ops/OpInfo.h"
#include <iosfwd>
class IRInst;
class UtLog;

/// Code generation for IR instructions.
class CgInst : public CgComponent {
public:
    /// Constructor
    CgInst(const CgComponent& state) :
        CgComponent(state)
    {
    }

    /// Generate LLVM code for an IR instruction, if possible (otherwise
    /// returns false).
    bool GenInst(const IRInst& inst) const;

    /// Generate a shadeop call after converting the given arguments to LLVM
    /// values.
    void GenOpCall(llvm::Function* op, const IRInst& inst) const;

    /// Given the name of an overloaded shadeop, mangle it to include argument
    /// type specifiers (e.g. OpAdd_FT).  The result type is also included if
    /// the instruction has a result but the shadeop is void, indicating that
    /// takes a result parameter
    void MangleArgTypes(Opcode opcode, std::stringstream& opName,
                        const IRVar* result, const IRValues& args) const;
};

#endif /// ndef CG_INST_H
