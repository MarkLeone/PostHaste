// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_SHADER_H
#define CG_SHADER_H

#include "cg/CgComponent.h"
#include "ir/IRTypedefs.h"
#include "ir/IRVisitor.h"
#include <list>
#include <vector>
class IRShader;
class UtLog;

/// Generate code for a shader.  The shader is partitioned (see XfPartition)
/// and a liveness analysis is used to compute the free variables of the
/// partitions.  The shader is then modified in-place, replacing compiled
/// partitions with plugin calls.  An LLVM module for the shader plugin is
/// returned, which has an entry point for each compiled partition.
llvm::Module* CgShaderCodegen(IRShader* shader, UtLog* log, 
                              int minPartitionSize=1,
                              bool dumpIR=false);

/// Implementation of shader codegen.  The methods are all public for unit
/// testing.
class CgShader : public CgComponent, public IRVisitor<CgShader> {
private:
    IRShader* mShader;
    IRTypes* mTypeFactory;
    const char* mCurrentFuncName;
    std::list<llvm::Function*> mEntryFuncs;
    std::list<const IRStringConst*> mEntryPrototypes;
    int mMinPartitionSize;
    bool mDumpIR;

public:
    CgShader(UtLog* log, int minPartitionSize=1, bool dumpIR=false);
    ~CgShader();

    llvm::Module* Codegen(IRShader* shader);
    void CodegenSetup(IRShader* shader);
    IRStmt* CodegenPartition(IRStmt* stmt);
    llvm::Function* GenKernel(IRStmt* stmt, const IRVars& args);
    llvm::Function* GenKernelFunc(const char* nameHint, const IRVars& args);
    llvm::Function* GenEntry(llvm::Function* kernelFunc, const IRVars& args);
    llvm::Function* GenEntryStub(const std::string& name);
    void GenIterators(llvm::Function* entryFunc, const IRVars& args,
                      llvm::Value* argv,
                      std::vector<llvm::Value*>* iterators);
    void GenKernelLoop(llvm::Function* entryFunc, llvm::Function* kernelFunc,
                       const IRVars& args, llvm::Value* argv,
                       const std::vector<llvm::Value*>& iterators);
    llvm::BasicBlock* GenLoop(llvm::Value* loopBound, llvm::Value** loopIndex);
    void GenDerefIterators(const IRVars& args,
                           const std::vector<llvm::Value*>& iterators,
                           std::vector<llvm::Value*>* kernelArgs);
    std::string GenPrototype(const char* funcName, const IRVars& args);
    IRStmt* GenPluginCall(const char* name, const IRVars& argVars, 
                          const IRStringConst* prototype, const IRPos& pos);
    void GenRslFuncTable();
    llvm::Constant* GenRslFuncArray();
    llvm::Constant* GenRslFunction(llvm::Constant* function, 
                                   llvm::Constant* prototype,
                                   const llvm::Type* rslFuncTy);
    llvm::Constant* GetGlobalInitializer(const char* name);

    IRStmt* Walk(IRStmt* stmt);
    bool ShouldCompile(IRStmt* stmt);
    IRStmt* Visit(IRBlock* stmt, int ignored);
    IRStmt* Visit(IRSeq* stmt, int ignored);
    IRStmt* Visit(IRIfStmt* stmt, int ignored);
    IRStmt* Visit(IRForLoop* stmt, int ignored);
    IRStmt* Visit(IRCatchStmt* stmt, int ignored);
    IRStmt* Visit(IRControlStmt* stmt, int ignored);
    IRStmt* Visit(IRGatherLoop* stmt, int ignored);
    IRStmt* Visit(IRIlluminanceLoop* stmt, int ignored);
    IRStmt* Visit(IRIlluminateStmt* stmt, int ignored);
    IRStmt* Visit(IRPluginCall* stmt, int ignored);
};

#endif // ndef CG_SHADER_H
