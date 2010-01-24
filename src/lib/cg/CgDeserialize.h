// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_DESERIALIZE_H
#define CG_DESERIALIZE_H

#include "cg/CgFwd.h"

llvm::Module* CgDeserializeSkeleton(llvm::LLVMContext* context);
llvm::Module* CgDeserializeShadeops(llvm::LLVMContext* context);

#endif // ndef CG_DESERIALIZE_H
