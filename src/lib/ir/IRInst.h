// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_INST_H
#define IR_INST_H

#include "ir/IRPos.h"
#include "ir/IRTypedefs.h"
#include "ir/IRValue.h"
#include "ops/Opcode.h"
#include <assert.h>
#include <iosfwd>
#include <string>
class IRVar;

/// Abstract base class of IR instructions.  Derived classes include:
///    - IRBasicInst
///    - IRUnknownInst
class IRInst : public IRValue {
public:
    /// Get the instruction opcode (which might be kOpcode_Unknown).
    Opcode GetOpcode() const { return mOpcode; }

    /// Get the instruction name.
    virtual const char* GetName() const;

    /// Get the short name of the instruction.
    virtual const char* GetShortName() const;

    /// Get the result (possibly NULL).
    IRVar* GetResult() const { return mResult; }

    /// Get the arguments
    const IRValues& GetArgs() const { return mArgs; }

    /// Get the source position (if any).
    const IRPos& GetPos() const { return mPos; }

    /// Construct an IR instruction of the specified kind, given its opcode,
    /// result, arguments, and optional source position.  The result can be
    /// NULL, indicating a void instruction.
    ///
    /// Note: instructions do not own their operands.  Variables are owned by
    /// the shader (locals will eventually be owned by a method).  Constants
    /// are also owned by the shader.  Once we have SSA, arguments can also be
    /// instructions, which are owned by blocks.
    IRInst(IRValueKind kind, Opcode opcode, 
           IRVar* result, const IRValues& args,
           const IRPos& pos = IRPos());

    /// Destructor (not sure why it can't be pure virtual).
    virtual ~IRInst();

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        IRValueKind kind = val->GetKind();
        return kIRInst_Begin < kind && kind < kIRInst_End;
    }

    /// Print this instruction.
    virtual void Write(std::ostream& out) const;

    /// Write the source of the instruction, omitting the destination.
    virtual void WriteSource(std::ostream& out) const;

private:
    Opcode mOpcode;             // possibly kOpcode_Unknown
    IRVar* mResult;             // possibly NULL
    IRValues mArgs;
    IRPos mPos;
};

/// Print an instruction
std::ostream& operator<<(std::ostream& out, const IRInst& inst);


#endif // ndef IR_INST_H
