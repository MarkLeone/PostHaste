// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.


#include "slo/SloInputFile.h"
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

SloInputFile::SloInputFile(const char* filename, UtLog* log) :
    SloFile(filename, log),
    mLineNum(0),
    mBuffer(static_cast<char*>(malloc(128))),
    mBufferSize(128)
{
}

SloInputFile::SloInputFile(FILE* file, UtLog* log) :
    SloFile(file, log),
    mLineNum(0),
    mBuffer(static_cast<char*>(malloc(128))),
    mBufferSize(128)
{
}

SloInputFile::~SloInputFile()
{
    free(mBuffer);
}

// Report a warning/error for this file at the current line number.
void 
SloInputFile::Report(UtSeverity severity, const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    mLog->WriteWhereV(severity, mFilename.c_str(), mLineNum, msg, ap);
    va_end(ap);
}

// Read a line (stripping any newline).  Returns a pointer to a buffer
// that is owned by this SloInputFile.  NULL is returned if the end of
// the file was encountered before any characters were read.  If an error
// occurrs, it is reported and NULL is returned.
const char*
SloInputFile::ReadLine()
{
    assert(mFile != NULL && "Cannot read from unopened SloFile");
    mBuffer[0] = '\0';
    char* buffer = mBuffer;
    size_t bytesRead = 0;
    while (true) {
        // Read into buffer.  
      unsigned int numBytes = 
	static_cast<unsigned int>(mBufferSize - bytesRead);
      char* s = fgets(buffer, numBytes, mFile);
        // Check for error
        if (ferror(mFile)) {
            mLog->Write(kUtError, "Error reading from file: %s",
                        mFilename.c_str());
            return NULL;
        }
        // Break on EOF.
        if (s == NULL || s[0] == '\0') 
            break;
        // If we reached a newline, strip it and break.
        size_t len = strlen(s);
        if (s[len-1] == '\n') {
            s[len-1] = '\0';
            --len;
            break;
        }
        // Otherwise, resize the buffer and keep reading.
        bytesRead += len;
        mBufferSize *= 2;
        mBuffer = static_cast<char*>(realloc(mBuffer, mBufferSize));
        buffer = mBuffer + bytesRead;
    }
    // Return NULL if we reached EOF without reading anything.
    if (feof(mFile) && mBuffer[0] == '\0')
        return NULL;
    ++mLineNum;
    return mBuffer;
}

