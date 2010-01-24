// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "slo/SloHeader.h"
#include "slo/SloInputFile.h"
#include "slo/SloOutputFile.h"

int
SloHeader::Read(SloInputFile* in)
{
    if (const char* line = in->ReadLine()) {
        int numFields = sscanf(line, "shader_obj %f %i %i",
                               &mVersion, &mUnknown1, &mUnknown2);
        if (numFields == 3)
            return 0;
    }
    in->Report(kUtError, "Error reading SLO header");
    return 1;
}

void
SloHeader::Write(SloOutputFile* file) const
{
    file->Write("shader_obj %.1f %i %i\n", mVersion, mUnknown1, mUnknown2);
}
