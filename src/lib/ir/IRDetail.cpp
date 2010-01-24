// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRDetail.h"
#include <assert.h>

const char* 
IRDetailToString(IRDetail det)
{
    switch (det) {
      case kIRDetailNone: return "nodetail";
      case kIRUniform: return "uniform";
      case kIRVarying: return "varying";
    }
    assert(false && "Unknown detail");
    return "unknown";
}

