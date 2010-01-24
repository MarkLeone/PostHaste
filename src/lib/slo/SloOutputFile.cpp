// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.


#include "slo/SloOutputFile.h"
#include <stdarg.h>
#include <assert.h>

// Write to the file with printf-style formatting.
void 
SloOutputFile::Write(const char* msg, ...)
{
    assert(mFile && "Can't write SloOutputFile that isn't open");
    va_list ap;
    va_start(ap, msg);
    int n = vfprintf(mFile, msg, ap);
    va_end(ap);
    if (n < 0)
        mLog->Write(kUtError, "Error writing to file: %s", mFilename.c_str());
}
