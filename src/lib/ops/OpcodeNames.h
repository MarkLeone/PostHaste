// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef OPCODE_NAMES_H
#define OPCODE_NAMES_H

#include "ops/Opcode.h"
#include "util/UtHashMap.h"
#include "util/UtTokenFactory.h"

/// Map from instruction name to opcode.  Also serves as a token factory,
/// storing names for unknown instructions.
class OpcodeNames {
public:
    /// Construct map from instruction name to opcode.
    OpcodeNames();

    /// Look up instruction opcode from its name.  kOpcode_Unknown is returned
    /// if the instruction is unknown.
    Opcode Lookup(const char* name) const;

    /// Get a persistent name for the specified opcode.  If it's a known
    /// instruction, the name is a static string constant.  Otherwise it's
    /// owned by this opcode table.
    const char* GetName(Opcode opcode, const char* name);

private:
    // Map from instruction name to opcode.
    typedef UtHashMap<const char*, Opcode> OpcodeMap;
    OpcodeMap mOpcodeMap;

    // Token factory for unknown instruction names.
    UtTokenFactory mTokens;
};

#endif // ndef OPCODE_NAMES_H
