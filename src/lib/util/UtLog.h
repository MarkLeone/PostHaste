// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef UT_LOG_H
#define UT_LOG_H

#include <stdio.h>              // for FILE*

/// Message severity level.
enum UtSeverity {
    kUtDebug = 0,
    kUtInfo,
    kUtWarning,
    kUtError,
    kUtInternal,
    kUtFatal,
};

/// Convert severity level to string.
const char* UtSeverityToString(UtSeverity level);

/// Message reporting log.
class UtLog {
public:
    /// Construct message log.
    UtLog(FILE* out) : mOut(out), mNumErrors(0) { }

    /// Get the number of errors reported.
    unsigned int GetNumErrors() const { return mNumErrors; }

    /// Write a message with the specified severity.  
    void Write(UtSeverity level, const char* msg, ...);

    /// Write a message that includes a filename and line number.  The
    /// filename is omitted if it is NULL or empty.  The line and column
    /// numbers are omitted if less or equal to zero.
    void WriteWhere(UtSeverity level, const char* filename, int line,
                    const char* msg, ...);

    /// Varargs method writing a message.
    void WriteV(UtSeverity level, const char* msg, va_list ap);

    /// Varargs method for writing a message that includes a filename and line
    /// number.  The filename is omitted if it is NULL or empty.  The line and
    /// column numbers are omitted if less or equal to zero.
    void WriteWhereV(UtSeverity level, const char* filename, int line,
                     const char* msg, va_list ap);

private:
    FILE* mOut;
    unsigned int mNumErrors;
};

#endif // ndef UT_LOG_H
