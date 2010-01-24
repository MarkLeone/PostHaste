// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_DETAIL_H
#define IR_DETAIL_H

/// Detail.
enum IRDetail {
    kIRDetailNone,
    kIRUniform,
    kIRVarying,
};

/// Convert detail to string.
const char* IRDetailToString(IRDetail det);

#endif // ndef IR_DETAIL_H
