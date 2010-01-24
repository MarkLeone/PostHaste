// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_SYMBOL_H
#define SLO_SYMBOL_H

#include "slo/SloEnums.h"
#include <string>
class SloInputFile;
class SloOutputFile;

// SLO symbol table entry.
class SloSymbol {
public:
    std::string mName;          ///< Name
    SloType mType;              ///< Type
    SloStorage mStorage;        ///< Storage class
    int mSpace;                 ///< Index of space name (-1 if none)
    int mOffset;                ///< Offset (storage specific)
    SloDetail mDetail;          ///< Variable detail
    int mNumInsts;              ///< Number of instructions in initializer
    int mInitPC;                ///< PC of initialier (plus one).
    int mWrite1;                ///< First write
    int mWrite2;                ///< Last write
    int mPc1;                   ///< First write (or block start)
    int mPc2;                   ///< Last write (or block end)
    int mLength;                ///< Array length (optional)

    /// Default constructor.
    SloSymbol() { }

    /// Construct SLO symbol table entry.  Note that the initPC is one plus
    /// the actual init PC (zero means no initializer).
    SloSymbol(const char* name,
              SloType type,
              SloStorage storage,
              int space,
              int offset,
              SloDetail detail,
              int numInsts,
              int initPC,
              int write1,
              int write2,
              int pc1,
              int pc2,
              int length = kSloSymbolNoLength) :
        mName(name),
        mType(type),
        mStorage(storage),
        mSpace(space),
        mOffset(offset),
        mDetail(detail),
        mNumInsts(numInsts),
        mInitPC(initPC),
        mWrite1(write1),
        mWrite2(write2),
        mPc1(pc1),
        mPc2(pc2),
        mLength(length)
    {
    }

    /// Check whether the symbol is an array. 
    bool IsArray() const { return mLength != kSloSymbolNoLength; }

    /// Check whether this symbol is written.
    bool IsWritten() const { return mWrite1 > 0; }
    
    /// Read from a file.  Returns 0 for success.
    int Read(SloInputFile* in);

    /// Write to a file.
    void Write(SloOutputFile* out) const;

private:
    friend class XfLowerImpl;
    static const int kSloSymbolNoLength = -2;
};


#endif // ndef SLO_SYMBOL_H
