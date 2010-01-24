// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "slo/SloSymbol.h"
#include "slo/SloInputFile.h"
#include "slo/SloOutputFile.h"
#include <assert.h>
#include <string.h>

int
SloSymbol::Read(SloInputFile* in)
{
    if (const char* line = in->ReadLine()) {
        // Extract name.
        size_t delimIndex = strcspn(line, " \t");
        mName = std::string(line, delimIndex);
        line += delimIndex;

        // Extract remaining fields.
        mLength = kSloSymbolNoLength;  // optional
        int type, storage, detail;
        int numFields = sscanf(line, " %i %i %i %i %i %i %i %i %i %i %i %i",
                               &type, &storage, &mSpace, &mOffset,
                               &detail, &mNumInsts, &mInitPC, &mWrite1,
                               &mWrite2, &mPc1, &mPc2, &mLength); 

        mType = static_cast<SloType>(type);
        if (!SloIsKnown(mType))
            in->Report(kUtWarning, "Unknown symbol type %i", mType);

        mStorage = static_cast<SloStorage>(storage);
        if (!SloIsKnown(mStorage))
            in->Report(kUtWarning, "Unknown symbol storage %i", mStorage);

        mDetail = static_cast<SloDetail>(detail);
        if (!SloIsKnown(mDetail))
            in->Report(kUtWarning, "Unknown symbol detail %i", mDetail);

        if (numFields == 11 || numFields == 12)
            return 0;
    }
    in->Report(kUtError, "Error reading SLO symbol");
    return 1;
}

void
SloSymbol::Write(SloOutputFile* file) const
{
    assert(!mName.empty() && "Can't write anonymous SLO symbol");
    assert(mType != kSloTypeNone && "Can't write SLO symbol with no type");
    file->Write("%s %i %i %i %i %i %i %i %i %i %i %i",
                mName.c_str(), mType, mStorage, mSpace, mOffset,
                mDetail, mNumInsts, mInitPC, mWrite1,
                mWrite2, mPc1, mPc2);
    if (IsArray())
        file->Write(" %i\n", mLength);
    else
        file->Write("\n");
}
