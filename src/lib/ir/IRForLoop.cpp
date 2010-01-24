// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRForLoop.h"
#include "ir/IRValue.h"

IRForLoop::~IRForLoop()
{
    delete mCondStmt;
    delete mIterateStmt;
    delete mBody;
}
