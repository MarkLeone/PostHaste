// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRNumArrayConst.h"
#include "ir/IRNumConst.h"              // for IRWriteData

std::ostream& 
operator<<(std::ostream& out, const IRNumArrayConst& constant)
{
    IRWriteData(constant.GetData(), constant.GetType(), out);
    return out;
}
