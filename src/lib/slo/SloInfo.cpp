// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "slo/SloInfo.h"
#include "slo/SloInputFile.h"
#include "slo/SloOutputFile.h"
#include <string.h>

int
SloInfo::Read(SloInputFile* in)
{
    if (const char* line = in->ReadLine()) {
        // Extract name.
        size_t delimIndex = strcspn(line, " \t");
        mName = std::string(line, delimIndex);
        line += delimIndex;

        // Extract remaining fields.
        mOptUnknown = -1;
        int shaderType;
        int numFields = sscanf(line, " %i %i %i %i %i %i %i %i %i %i %i %i",
                               &mNumSyms, &mNumConstants, &mNumStrings,
                               &mUnknown3, &mNumInsts, &mUnknown1, &mNumInits,
                               &mNumInits2, &mUnknown2, &mBeginPC, &shaderType,
                               &mOptUnknown);
        mType = static_cast<SloShaderType>(shaderType);
        if (!SloIsKnown(mType))
            in->Report(kUtWarning, "Unknown shader type %i", mType);
        if (numFields == 11 || numFields == 12)
            return 0;
    }
    in->Report(kUtError, "Error reading SLO info");
    return 1;
}

void
SloInfo::Write(SloOutputFile* file) const
{
    file->Write("%s %i %i %i %i %i %i %i %i %i %i %i",
                mName.c_str(), mNumSyms, mNumConstants, mNumStrings,
                mUnknown3, mNumInsts, mUnknown1, mNumInits,
                mNumInits2, mUnknown2, mBeginPC, mType,
                mOptUnknown);
    if (mOptUnknown != -1)
        file->Write(" %i\n", mOptUnknown);
    else
        file->Write("\n");
}
