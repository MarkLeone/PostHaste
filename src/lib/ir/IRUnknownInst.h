// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_UNKNOWN_INST_H
#define IR_UNKNOWN_INST_H

#include "ir/IRInst.h"
#include <string>

/// IR for a unknown instruction.  Most of the functionality is in the base
/// class.
class IRUnknownInst : public IRInst {
public:
    /// Get the instruction name.
    virtual const char* GetName() const;

    /// Construct an unknown IR instruction with the specified name, result,
    /// arguments, and optional source position.  The name is copied.  The
    /// result can be NULL, indicating a void instruction.
    IRUnknownInst(const char* name, IRVar* result, const IRValues& args,
                const IRPos& pos = IRPos()) :
        IRInst(kIRUnknownInst, kOpcode_Unknown, result, args, pos),
        mName(name)
    {
    }

    /// Destructor.
    virtual ~IRUnknownInst();

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRUnknownInst;
    }

private:
    std::string mName;
};

#endif // ndef IR_UNKNOWN_INST_H
