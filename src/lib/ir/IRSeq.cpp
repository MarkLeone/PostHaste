// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRSeq.h"
#include "util/UtDelete.h"

IRSeq::~IRSeq()
{
    UtDeleteSeq(mStmts);
}
