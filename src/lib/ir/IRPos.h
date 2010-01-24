// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_POS_H
#define IR_POS_H

#include "ir/IRStringConst.h"
#include <iosfwd>

/// IRPos represents a source position, including a filename and line number.
class IRPos {
public:
    /// Construct empty position.
    IRPos() :
        mFile(NULL),
        mLine(0)
    {
    }

    /// Construct IRPos with a filename specified by a string constant.
    IRPos(const IRStringConst* file, unsigned int line) :
        mFile(file),
        mLine(line)
    {
    }

    /// Get filename (empty if none).
    const char* GetFile() const { return mFile ? mFile->Get() : ""; }

    /// Get the filename's string constant (NULL if none).
    const IRStringConst* GetFileString() const { return mFile; }

    /// Get line number (0 if none).
    unsigned int GetLine() const { return mLine; }

    /// Check whether the position is empty.
    bool IsEmpty() const { return mFile == NULL && GetLine() == 0; }

private:
    const IRStringConst* mFile;
    unsigned int mLine;
};

/// Print source position.
std::ostream& operator<<(std::ostream& out, const IRPos& pos);


#endif // ndef IR_POS_H
