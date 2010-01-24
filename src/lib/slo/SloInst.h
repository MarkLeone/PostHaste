// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_INSTR_H
#define SLO_INSTR_H

#include "ops/Opcode.h"
#include <string>
class OpcodeNames;
class SloInputFile;
class SloOutputFile;
class UtTokenFactory;

/// SLO instruction
class SloInst {
public:
    Opcode mOpcode;             ///< Instruction opcode (possibly "unknown")
    const char* mName;          ///< Instruction name (static or token).
    int mNumArgs;               ///< Number of arguments
    int mFirstArg;              ///< Index of first argument (-1 if no args)
    int mFilenameIndex;         ///< Index of filename (-1 if none)
    int mLineNum;               ///< Line number (-1 if none)
    int mDebugInfo;             ///< Obsolete debug info (-1 if none)

    /// Default constructor
    SloInst() { }

    /// Construct SLO instruction.
    SloInst(Opcode opcode,
            const char* name,
            int numArgs,
            int firstArg,
            int filenameIndex = -1,
            int lineNum = -1,
            int debugInfo = -1) :
        mOpcode(opcode),
        mName(name),
        mNumArgs(numArgs),
        mFirstArg(firstArg),
        mFilenameIndex(filenameIndex),
        mLineNum(lineNum),
        mDebugInfo(debugInfo)
    {
    }

    /// Read SLO instruction from a file.  Returns 0 for success.  The
    /// opcode table is a map from name to opcode, which also stores
    /// names for unknown instructions.
    int Read(SloInputFile* in, OpcodeNames* opNames);

    /// Write SLO instruction to a file.
    void Write(SloOutputFile* out, int majorSloVersion = 4) const;
};

#endif // ndef SLO_INSTR_H
