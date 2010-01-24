// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRGatherLoop.h"
#include "ir/IRValue.h"

IRGatherLoop::~IRGatherLoop()
{
    // NB: the illuminance statement does not own the category or args.
    delete mElseStmt;
}
