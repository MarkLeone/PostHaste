// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_SHADER_H
#define SLO_SHADER_H

#include "ops/OpcodeNames.h"
#include "slo/SloHeader.h"
#include "slo/SloInfo.h"
#include "slo/SloInst.h"
#include "slo/SloSymbol.h"
#include "util/UtVector.h"

/// SLO shader
class SloShader {
public:
    SloHeader mHeader;                  ///< Header record.
    SloInfo mInfo;                      ///< Info record.
    UtVector<std::string> mStrings;     ///< String constants.
    UtVector<float> mConstants;         ///< Float constants.
    UtVector<SloSymbol> mSymbols;       ///< Symbols.
    UtVector<int> mArgs;                ///< Instruction arguments.
    UtVector<SloInst> mInsts;           ///< Instructions.
    OpcodeNames mOpNames;               ///< Opcode names

    /// Read from a file.  Returns 0 for success.
    int Read(SloInputFile* in);

    /// Write to a file.
    void Write(SloOutputFile* out) const;

private:
    int ReadStrings(SloInputFile* in);
    void WriteStrings(SloOutputFile* out) const;

    int ReadConstants(SloInputFile* in);
    void WriteConstants(SloOutputFile* out) const;

    int ReadEmpty(SloInputFile* in);
    void WriteEmpty(SloOutputFile* out) const;

    int ReadSymbols(SloInputFile* in);
    void WriteSymbols(SloOutputFile* out) const;

    int ReadArgs(SloInputFile* in);
    void WriteArgs(SloOutputFile* out) const;

    int ReadInsts(SloInputFile* in);
    void WriteInsts(SloOutputFile* out) const;
};

#endif // ndef SLO_SHADER_H
