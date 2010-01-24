// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "slo/SloInst.h"
#include "ops/OpcodeNames.h"
#include "slo/SloInputFile.h"
#include "slo/SloOutputFile.h"
#include "util/UtTokenFactory.h"

// Read SLO instruction from a file.  Returns 0 for success.  The opcode
// table is a map from name to opcode, which also stores names for unknown
// instructions.
int
SloInst::Read(SloInputFile* in, OpcodeNames* opNames)
{
    if (const char* line = in->ReadLine()) {
        // Extract name
        size_t delimIndex = strcspn(line, " \t");
        std::string name(line, delimIndex);
        line += delimIndex;

        // Look up the opcode and get a persistent name.
        mOpcode = opNames->Lookup(name.c_str());
        mName = opNames->GetName(mOpcode, name.c_str());

        // Extract remaining fields.  The last three are optional.
        mFilenameIndex = -1;
        mLineNum = -1;
        mDebugInfo = -1;
        int numFields = sscanf(line, " %i %i %i %i %i",
                               &mNumArgs, &mFirstArg, 
                               &mFilenameIndex, &mLineNum, &mDebugInfo);

        if (numFields == 2 || numFields == 5)
            return 0;
    }
    in->Report(kUtError, "Error reading SLO instruction");
    return 1;
}

void
SloInst::Write(SloOutputFile* file, int majorSloVersion) const
{
    // We're pedantic about the spacing to simplify diff-based testing.
    if (majorSloVersion >= 4) {
        if (strlen(mName) < 7)
            file->Write("%s\t", mName);
        else
            file->Write("%s ", mName);
        file->Write("\t%i %i", mNumArgs, mFirstArg);
    }
    else {
        file->Write("%s\t%i\t%i", mName, mNumArgs, mFirstArg);
    }
    if (mFilenameIndex != -1) 
        file->Write("  %i %i %i\n", mFilenameIndex, mLineNum, mDebugInfo);
    else 
        file->Write("\n");
}
