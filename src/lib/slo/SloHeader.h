// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_HEADER_H
#define SLO_HEADER_H

#include <stdio.h>              // for FILE*
class SloInputFile;
class SloOutputFile;

/// SLO header record.
class SloHeader {
public:
    float mVersion;             ///< SLO version number.
    int mUnknown1;              ///< Unknown header field.
    int mUnknown2;              ///< Unknown header field.

    /// Default constructor
    SloHeader() { }

    /// Construct SLO header record.
    SloHeader(float version, int unknown1, int unknown2) :
        mVersion(version), mUnknown1(unknown1), mUnknown2(unknown2)
    {
    }

    /// Read from a file.  Returns 0 for success.
    int Read(SloInputFile* in);

    /// Write to a file.
    void Write(SloOutputFile* out) const;
};

#endif // ndef SLO_HEADER_H
