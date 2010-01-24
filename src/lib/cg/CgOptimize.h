// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_OPTIMIZE_H
#define CG_OPTIMIZE_H

#include "cg/CgFwd.h"
#include <vector>

/// Optimize the given LLVM module based on the specified optimization level.
void CgOptimize(llvm::Module* module, 
                int optimizationLevel);

/// Internalize all the symbols in the given module except the specified
/// exports.
void CgInternalize(llvm::Module* module, 
                   const std::vector<const char*>& exports);

#endif // ndef CG_OPTIMIZE_H
