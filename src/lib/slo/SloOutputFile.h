// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_OUTPUT_FILE_H
#define SLO_OUTPUT_FILE_H

#include "slo/SloFile.h"

/// Output SLO file.
class SloOutputFile : public SloFile {
public:
    /// Construct SloOutputFile from filename.  Any errors opening, reading, or
    /// writing the file are reported to the given log.
    SloOutputFile(const char* filename, UtLog* log) :
        SloFile(filename, log)
    {
    }

    /// Construct SloOutputFile an already opened file handle (e.g. stdout).
    /// Any errors opening, reading, or writing the file are reported to the
    /// given log.
    SloOutputFile(FILE* file, UtLog* log) :
        SloFile(file, log)
    {
    }

    /// Open for writing.  Returns 0 for success; otherwise reports an error.
    int Open() { return SloFile::Open("w"); }

    /// Write to the file with printf-style formatting.
    void Write(const char* msg, ...);
};


#endif // ndef SLO_OUTPUT_FILE_H
