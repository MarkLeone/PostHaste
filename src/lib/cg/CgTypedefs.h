// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_TYPEDEFS_H
#define CG_TYPEDEFS_H

namespace llvm {
    class ConstantFolder;
    template <bool preserveNames, typename Folder> class IRBuilder;
}
typedef llvm::IRBuilder<true, llvm::ConstantFolder> CgBuilder;

#endif // ndef CG_TYPEDEFS_H
