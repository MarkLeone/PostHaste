// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ops/OpcodeNames.h"

// Construct map from instruction name to opcode.
OpcodeNames::OpcodeNames()
{
    // Don't include the unknown instruction opcode in the map.
    assert(kOpcode_Unknown == 0 && "Opcode enum invariant botched");
    for (unsigned int i = 1; i < kOpcode_NumInsts; ++i) {
        Opcode opcode = static_cast<Opcode>(i);
        mOpcodeMap[OpcodeName(opcode)] = opcode;
    }
}

// Look up instruction opcode from its name.  kOpcode_Unknown is returned
// if the instruction is unknown.
Opcode 
OpcodeNames::Lookup(const char* name) const
{
    OpcodeMap::const_iterator it = mOpcodeMap.find(name);
    return it == mOpcodeMap.end() ? kOpcode_Unknown : it->second;
}

// Get a persistent name for the specified instruction.  If it's a known
// instruction, the name is a static string constant.  Otherwise it's
// owned by this object.
const char* 
OpcodeNames::GetName(Opcode opcode, const char* name)
{
    if (opcode == kOpcode_Unknown)
        return mTokens.Get(name).Get();
    else
        return OpcodeName(opcode);
}
