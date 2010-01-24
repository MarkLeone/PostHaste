// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "slo/SloFile.h"
#include "util/UtLog.h"
#include <assert.h>
#include <stdarg.h>

SloFile::SloFile(const char* filename, UtLog* log) :
    mFilename(filename),
    mLog(log),
    mFile(NULL),
    mNeedsClose(false)
{
}

SloFile::SloFile(FILE* file, UtLog* log) :
    mFilename(""),
    mLog(log),
    mFile(file),
    mNeedsClose(false)
{
}

SloFile::~SloFile()
{
    Close();
    mFile = NULL; 
}

// Open with the specified mode.  Returns 0 for success; otherwise reports an
// error.
int 
SloFile::Open(const char* mode)
{
    assert(mFile == NULL && "Duplicate file open");
    mFile = fopen(mFilename.c_str(), mode);
    if (mFile == NULL) {
        mLog->Write(kUtError, "Unable to open file: %s", mFilename.c_str());
        return 1;
    }
    mNeedsClose = true;
    return 0;
}

// Close the file.  Returns 0 for success; otherwise reports an error.
int 
SloFile::Close()
{
    if (mNeedsClose && mFile != NULL) {
        mNeedsClose = false;
        if (fclose(mFile)) {
            mLog->Write(kUtError, "Unable to close file: %s",
                        mFilename.c_str());
            mFile = NULL;
            return 1;
        }
        mFile = NULL;
    }
    return 0;
}

// Report a warning/error for this file at the current line number.
void 
SloFile::Report(UtSeverity severity, const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    mLog->WriteWhereV(severity, mFilename.c_str(), 0, msg, ap);
    va_end(ap);
}

