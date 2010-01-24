// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_INFO_H
#define SLO_INFO_H

#include "slo/SloEnums.h"
#include <string>
class SloInputFile;
class SloOutputFile;

/// SLO info record
class SloInfo {
public:
    std::string mName;           ///< Shader name
    int mNumSyms;               ///< Number of symbols
    int mNumConstants;          ///< Number of constants
    int mNumStrings;            ///< Number of string constants
    int mUnknown3;              ///< Unknown
    int mNumInsts;              ///< Number of instructions.
    int mUnknown1;              ///< Unknown
    int mNumInits;              ///< Number of parameter initialization instrs.
    int mNumInits2;            ///< Duplicate of mNumInits
    int mUnknown2;              ///< Unknown
    int mBeginPC;               ///< First instruction in shader body.
    SloShaderType mType;        ///< Shader type
    int mOptUnknown;            ///< Unknown optional field.

    /// Default constructor
    SloInfo() { }

    /// Construct SLO info record.
    SloInfo(const char* name,
            int numSyms,
            int numConstants,
            int numStrings,
            int unknown3,
            int numInsts,
            int unknown1,
            int numInits,
            int numInits2,
            int unknown2,
            int beginPC,
            SloShaderType type,
            int optUnknown = -1) :
        mName(name),
        mNumSyms(numSyms),
        mNumConstants(numConstants),
        mNumStrings(numStrings),
        mUnknown3(unknown3),
        mNumInsts(numInsts),
        mUnknown1(unknown1),
        mNumInits(numInits),
        mNumInits2(numInits2),
        mUnknown2(unknown2),
        mBeginPC(beginPC),
        mType(type),
        mOptUnknown(optUnknown)
    {
    }
    
    /// Read from a file.  Returns 0 for success.
    int Read(SloInputFile* in);

    /// Write to a file.
    void Write(SloOutputFile* out) const;
};

#endif // ndef SLO_INFO_H
