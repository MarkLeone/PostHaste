// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_BASIC_INST_H
#define IR_BASIC_INST_H

#include "ir/IRInst.h"

/// IR for a basic instruction.  Most of the functionality is in the base
/// class.
class IRBasicInst : public IRInst {
public:
    /// Construct an IR instruction with the specified opcode, result,
    /// arguments, and optional source position.  The result can be NULL,
    /// indicating a void instruction.
    IRBasicInst(Opcode opcode, IRVar* result, const IRValues& args,
                const IRPos& pos = IRPos()) :
        IRInst(kIRBasicInst, opcode, result, args, pos)
    {
    }

    /// Destructor.
    virtual ~IRBasicInst();

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRBasicInst;
    }
};

#endif // ndef IR_BASIC_INST_H
