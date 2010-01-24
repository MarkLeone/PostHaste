// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgDeserialize.h"
#include "cg/CgOptimize.h"
#include <llvm/Analysis/Verifier.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Support/MemoryBuffer.h>

static llvm::Module*
CgDeserialize(const unsigned char* bitcode, size_t size, 
              llvm::LLVMContext* context)
{
    const char *data = reinterpret_cast<const char *>(bitcode);
    llvm::MemoryBuffer* buffer =
        llvm::MemoryBuffer::getMemBuffer(data, data + size);

    // Parse the bitcode into a Module
    std::string errInfo;
    llvm::Module* module = llvm::ParseBitcodeFile(buffer, *context, &errInfo);
    delete buffer;
    assert(module != NULL && "Failed to deserialize module");

#ifndef NDEBUG
    bool bad = llvm::verifyModule(*module);
    assert(!bad && "Deserialized module verification failed");
#endif
    return module;
}

llvm::Module*
CgDeserializeSkeleton(llvm::LLVMContext* context)
{
    // Plugin skeleton is defined in lib/cg/CgSkeleton.cpp, compiled to LLVM
    // bitcode, and serialized into static data.
    extern unsigned char gCgSkeleton[];
    extern size_t gCgSkeletonSize;
    llvm::Module* module = CgDeserialize(gCgSkeleton, gCgSkeletonSize, context);

    // Internalize everything except the skeletal entry point, the function
    // table, and the static initializer.
    std::vector<const char*> exports;
    exports.push_back("CgEntryFunc");
    exports.push_back("RslPublicFunctions");
    exports.push_back("llvm.global_ctors");
    CgInternalize(module, exports);
    return module;
}

llvm::Module*
CgDeserializeShadeops(llvm::LLVMContext* context)
{
    // Ops are defined in lib/ops/Ops.cpp, compiled to LLVM bitcode,
    // and serialized into static data.  
    extern unsigned char gShadeops[];
    extern size_t gShadeopsSize;
    llvm::Module* module = CgDeserialize(gShadeops, gShadeopsSize, context);

    // Internalize all the shadeops, constants, etc.
    std::vector<const char*> noExports;
    noExports.push_back("OpNameThatDoesNotExist");
    CgInternalize(module, noExports);
    return module;
}

