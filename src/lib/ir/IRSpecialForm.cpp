// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRSpecialForm.h"

IRSpecialForm::~IRSpecialForm()
{
    delete mBody;
}
