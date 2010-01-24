// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "util/UtLog.h"
#include <assert.h>
#include <iostream>
#include <stdarg.h>

const char* 
UtSeverityToString(UtSeverity level)
{
    switch (level) {
      case kUtDebug: return "Debug";
      case kUtInfo: return "Info";
      case kUtWarning: return "Warning";
      case kUtError: return "Error";
      case kUtInternal: return "Internal error";
      case kUtFatal: return "Fatal error";
      default:
          assert(false && "Invalid severity");
          return "Internal error";
    }
}

void
UtLog::Write(UtSeverity level, const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    WriteV(level, msg, ap);
    va_end(ap);
}

void
UtLog::WriteV(UtSeverity level, const char* msg, va_list ap)
{
    if (level >= kUtError)
        ++mNumErrors;
    fflush(stdout);
    fflush(stderr);
    std::cout.flush();
    std::cerr.flush();
    fprintf(mOut, "%s: ", UtSeverityToString(level));
    vfprintf(mOut, msg, ap);
    fprintf(mOut, "\n");
    fflush(mOut);
}

void
UtLog::WriteWhere(UtSeverity level, const char* filename, int line,
                  const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    WriteWhereV(level, filename, line, msg, ap);
    va_end(ap);
}

void
UtLog::WriteWhereV(UtSeverity level, const char* filename, int line,
                   const char* msg, va_list ap)
{
    if (level >= kUtError)
        ++mNumErrors;
    fflush(stdout);
    fflush(stderr);
    std::cout.flush();
    std::cerr.flush();
    if (filename != NULL && *filename != '\0')
        fprintf(mOut, "%s", filename);
    if (line > 0)
        fprintf(mOut, ":%i", line);
    fprintf(mOut, ": %s: ", UtSeverityToString(level));
    vfprintf(mOut, msg, ap);
    fprintf(mOut, "\n");
    fflush(mOut);
}
