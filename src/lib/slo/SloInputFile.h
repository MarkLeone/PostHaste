// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_INPUT_FILE_H
#define SLO_INPUT_FILE_H

#include "slo/SloFile.h"

/// Input SLO file.
class SloInputFile : public SloFile {
public:
    /// Construct SloInputFile from filename.  Any errors opening, reading, or
    /// writing the file are reported to the given log.
    SloInputFile(const char* filename, UtLog* log);

    /// Construct SloInputFile from an already opened file handle
    /// (e.g. stdin).  Any errors opening, reading, or writing the file are
    /// reported to the given log.
    SloInputFile(FILE* file, UtLog* log);

    /// Destroy SloInputFile.
    virtual ~SloInputFile();

    /// Open for reading.  Returns 0 for success; otherwise reports an error.
    int Open() { return SloFile::Open("r"); }

    /// Read a line (including any newline).  Returns a pointer to a buffer
    /// that is owned by this SloInputFile.  NULL is returned if the end of
    /// the file was encountered before any characters were read.  If an error
    /// occurrs, it is reported and NULL is returned.
    const char* ReadLine();

    /// Get the line number of the most recently read/written line.  The
    /// result is one-based; zero is returned if no lines have been
    /// read/written.
    unsigned int GetLineNum() const { return mLineNum; }

    /// Report a warning/error for this file at the current line number.
    virtual void Report(UtSeverity severity, const char* msg, ...);

private:
    unsigned int mLineNum;
    char* mBuffer;
    size_t mBufferSize;
};

#endif // ndef SLO_INPUT_FILE_H
