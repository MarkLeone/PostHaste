// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRInst.h"
#include "ir/IRTypes.h"
#include "ir/IRValue.h"
#include "ir/IRVar.h"
#include "util/UtCast.h"
#include <iostream>

IRInst::IRInst(IRValueKind kind, Opcode opcode,
               IRVar* result, const IRValues& args, const IRPos& pos) :
    IRValue(kind, result ? result->GetType() : IRTypes::GetVoidTy(), 
            result ? result->GetDetail() : kIRUniform),
    mOpcode(opcode),
    mResult(result),
    mArgs(args),
    mPos(pos)
{
    assert(kIRInst_Begin < kind && kind < kIRInst_End &&
           "Invalid kind of instruction");
}


IRInst::~IRInst() { }

const char*
IRInst::GetName() const
{
    assert(mOpcode != kOpcode_Unknown && "Can't get name of 'unknown' opcode");
    return OpcodeName(mOpcode);
}

// TODO: concise names for infix operators.
const char*
IRInst::GetShortName() const
{
    const char* fullName = GetName();
    if (*fullName == '_' || *fullName == '$')
        return fullName+1;
    else
        return fullName;
}

void
IRInst::Write(std::ostream& out) const
{
    IRVar* result = GetResult();
    if (result)
        out << *result << " = ";
    WriteSource(out);
}

void
IRInst::WriteSource(std::ostream& out) const
{
    out << GetShortName() << "(";
    IRWriteValues(out, GetArgs());
    out << ")";
}

std::ostream& 
operator<<(std::ostream& out, const IRInst& inst)
{
    inst.Write(out);
    return out;
}
