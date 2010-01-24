// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_FILE_H
#define SLO_FILE_H

#include "util/UtLog.h"
#include <stdio.h>
#include <string>

/// SLO file handle.
class SloFile {
public:
    /// Construct SloFile from filename.  Any errors opening, reading, or
    /// writing the file are reported to the given log.
    SloFile(const char* filename, UtLog* log);

    /// Construct SloFile from an existing file handle (e.g. stdin or stdout).
    SloFile(FILE* file, UtLog* log);

    /// The destructor closes the file if necessary.
    virtual ~SloFile();

    /// Close the file.  Returns 0 for success; otherwise reports an error.
    int Close();

    /// Report a warning/error for this file.
    virtual void Report(UtSeverity severity, const char* msg, ...);

protected:
    /// Open file with the specified mode ("r" or "w").
    int Open(const char* mode);

    /// The filename is retained for error reporting, and also for separate
    /// construction and opening.
    std::string mFilename;

    /// Error log.
    UtLog* mLog;

    /// The file handle.
    FILE* mFile;

    /// Keep track of whether the file should be closed (it shouldn't 
    /// unless we opened it).
    bool mNeedsClose;
};

#endif // ndef SLO_FILE_H
