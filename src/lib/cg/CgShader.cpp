// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgShader.h"
#include "cg/CgConst.h"
#include "cg/CgDeserialize.h"
#include "cg/CgStmt.h"
#include "cg/CgTypedefs.h"
#include "cg/CgTypes.h"
#include "cg/CgVars.h"
#include "ir/IRShader.h"
#include "ir/IRTypedefs.h"
#include "ir/IRValues.h"
#include "ir/IRVarSet.h"
#include "xf/XfFreeVars.h"
#include "xf/XfPartition.h"
#include "xf/XfPartitionInfo.h"
#include "util/UtLog.h"
#include <llvm/Analysis/Verifier.h>
#include <llvm/BasicBlock.h>
#include <llvm/CallingConv.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Linker.h>
#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <sstream>

llvm::Module* 
CgShaderCodegen(IRShader* shader, UtLog* log, int minPartitionSize, bool dumpIR)
{
    return CgShader(log, minPartitionSize, dumpIR).Codegen(shader);
}

// Constructor. 
CgShader::CgShader(UtLog* log, int minPartitionSize, bool dumpIR) :
    CgComponent(CgComponent::Create(log)),
    mCurrentFuncName(""),
    mMinPartitionSize(minPartitionSize),
    mDumpIR(dumpIR)
{
}

// Destructor
CgShader::~CgShader()
{
    // Destroy the master codegen coponent state, which we own.  Note that the
    // module will usually be NULL, since it's claimed by the caller (except
    // when testing).
    CgComponent::Destroy();
}

/// Generate code for a shader, which must be partitioned (see XfPartition).
/// The shader is modified in-place, replacing compiled partitions with plugin
/// calls.  An LLVM module for the shader plugin is returned, which has an
/// entry point for each compiled partition.
llvm::Module* 
CgShader::Codegen(IRShader* shader)
{
    // Load the serialized shadeops and prepare the shader for code generation,
    // identifying partitions and determining their free variables.
    CodegenSetup(shader);

    // Walk the shader body, compiling partitions and replacing them with
    // plugin calls.  TODO: do the same for the shader parameter initializers.
    shader->SetBody(Walk(shader->GetBody()));

    // TODO: return NULL if no partitions had the requested minimum size.
    // (The destructor will delete the LLVM module.)
    if (mEntryFuncs.empty()) {
        mLog->Write(kUtWarning, "No kernels had %i or more shadeops.", 
                    mMinPartitionSize);
        // TODO: For now we simply print a warning
        // return NULL;
    }

    // Optionally dump the resulting IR.
    if (mDumpIR)
        std::cout << *shader;

    // Generate the plugin function table.
    if (!mEntryFuncs.empty())
        GenRslFuncTable();

    // If there were no errors, transfer ownership of the module to the caller.
    // XXX what if there were no entry functions?  Shouldn't bother
    // returning LLVM module, so we need a separate status result.
    if (mLog->GetNumErrors() == 0) {
        llvm::Module* module = mModule;
        mModule = NULL;
        return module;
    }
    else {
        // Otherwise the destructor deletes the module.
        return NULL;
    }
}

// Load the serialized plugin skeleton and prepare the shader for code
// generation, identifying partitions and determining their free variables.
void 
CgShader::CodegenSetup(IRShader* shader)
{
    llvm::Module* skeleton = CgDeserializeSkeleton(mContext);
    llvm::Linker linker(mModule->getModuleIdentifier(), mModule);
    std::string msg;
    bool failed = linker.LinkInModule(skeleton, &msg);
    if (failed) {
        mLog->Write(kUtInternal, "Module linking failed: %s", msg.c_str());
        assert(false && "Module linking failed");
    }
    mModule = linker.releaseModule();

    // We hold onto the shader so we can create temporary variables, etc.
    mShader = shader;
    mTypeFactory = shader->GetTypeFactory();

    // The module is created from the serialized shadeops by CgGetShadeops.
    mModule->setModuleIdentifier(shader->GetName());
    mCurrentFuncName = shader->GetName();

    // Partition the shader and use a liveness analysis to determine the free
    // variables of each partition.
    XfPartition(shader);
    XfFreeVars(shader);

    // If a minimum partition size was specified, run partition info analysis
    // to determin partition sizes.
    if (mMinPartitionSize > 1)
        XfPartitionInfo(shader, false);
}

// Compile a partition into an LLVM function, returing a plugin call.
IRStmt* 
CgShader::CodegenPartition(IRStmt* stmt)
{
    assert(stmt->CanCompile() && "Expected a compilable partition");

    // Clear any existing variable bindings.
    mVars->Reset();

    // Get the free variables of the statement, which will be passed as
    // arguments to the entry point.
    const IRVarSet* freeVars = stmt->GetFreeVars();
    assert(freeVars && "Missing free variables on partitin");
    IRVars argVars;
    freeVars->GetSorted(&argVars);

    // Generate the kernel function.
    llvm::Function* kernelFunc = GenKernel(stmt, argVars);

    // Generate the plugin entry function.
    llvm::Function* entryFunc = GenEntry(kernelFunc, argVars);
    mEntryFuncs.push_back(entryFunc);
    const std::string& funcName = entryFunc->getNameStr();

    // Generate RSL prototype.
    std::string protoStr = GenPrototype(funcName.c_str(), argVars);
    IRStringConst* prototype = mShader->NewStringConst(protoStr.c_str());
    mEntryPrototypes.push_back(prototype);
        
    // Optionally dump the IR for the partition
    if (mDumpIR) {
        std::cout << protoStr << "\n{\n" ;
        stmt->Write(std::cout, 4, true);
        std::cout << "}\n\n";
    }

    // Delete the statement and return a plugin call to replace it.
    IRPos pos = stmt->GetPos();
    delete stmt;
    return GenPluginCall(funcName.c_str(), argVars, prototype, pos);
}

// Generate a kernel for the given partition.
llvm::Function*
CgShader::GenKernel(IRStmt* stmt, const IRVars& argVars)
{
    // Define a function that takes the free variables as its parameters.
    // Sets the insertion point of the IR builder in the function.
    // LLVM makes the function name unique if necessary.
    
    llvm::Function* function = GenKernelFunc(mCurrentFuncName, argVars);

    // Generate code for the body of the shader and append a return.
    mStmts->Codegen(stmt);
    mBuilder->CreateRetVoid();

#ifndef NDEBUG
    // Verify the function before returning it.
    bool bad = llvm::verifyFunction(*function);
    assert(!bad && "Codegen verification failed");
#endif
    return function;
}

/*
  Generate an RslPlugin entry function for the given kernel.  For example:
    RSLEXPORT int
    CgEntryFunc(RslContext* rslContext, int argc, const RslArg** argv)
    {
        CgIter f, c;
        CgGetIter(argv, 1, &f);
        CgGetIter(argv, 2, &c);
    
        int n = CgNumValues(argv, 0);
        for (int i = 0; i < n; ++i) 
        {
            KernelFunc(f.mData, (OpVec3*) c.mData);
            CgIncIter(&f);
            CgIncIter(&c);
        }
        return 0;
    }
  The helper functions are defined in CgSkeleton.cpp
*/
llvm::Function*
CgShader::GenEntry(llvm::Function* kernelFunc, const IRVars& args)
{
    // Generate an empty plugin entry function and set the builder insert
    // point in its entry block.  The name is based on the kernel function,
    // minus the "_kernel" suffix (which might be followed by a suffix added
    // by LLVM for uniqueness).
    std::string name = kernelFunc->getName();
    size_t suffixIndex = name.find("_kernel");
    name.erase(suffixIndex, 7);
    llvm::Function* entryFunc = GenEntryStub(name);

    // Get the "argv" argument.
    llvm::Function::arg_iterator it = entryFunc->arg_begin();
    ++it; ++it;
    assert(it != entryFunc->arg_end() && "Error fetching argv from entry func");
    llvm::Value* argv = &*it;

    // Generate code to allocate and initialize an iterator for each argument.
    std::vector<llvm::Value*> iterators;
    GenIterators(entryFunc, args, argv, &iterators);

    // Generate a loop that iterates over the active points, calling the
    // kernel function for each and incrementing the iterators.
    GenKernelLoop(entryFunc, kernelFunc, args, argv, iterators);
    return entryFunc;
}

// Generate an empty plugin entry function and set the builder insert point in
// its entry block.  The function is cloned from a skeletal one in the
// deserialized skeleton module, which simplifies getting the argument types
// right.
llvm::Function*
CgShader::GenEntryStub(const std::string& name)
{
    // Clone the skeletal entry function and add it to the module.
    llvm::Function* skeleton = mModule->getFunction("CgEntryFunc");
    assert(skeleton && "Entry function skeleton not found");
    llvm::Function* entryFunc = llvm::CloneFunction(skeleton);
    entryFunc->setName(name);
    mModule->getFunctionList().push_back(entryFunc);

    // The skeleton is empty, save for a return instruction, which we delete.
    assert(!entryFunc->isDeclaration() && "Ill-formed entry function skeleton");
    llvm::BasicBlock* startBlock = &entryFunc->getEntryBlock();
    startBlock->getInstList().pop_back();
    assert(startBlock->empty() && "Expected empty entry function skeleton");
    mBuilder->SetInsertPoint(startBlock);
    return entryFunc;
}

// Generate code to allocate and initialize an iterator for each argument.
void
CgShader::GenIterators(llvm::Function* entryFunc, const IRVars& args,
                       llvm::Value* argv,
                       std::vector<llvm::Value*>* iterators)
{
    // Get the iterator type.
    llvm::Type* iterTy = mModule->getTypeByName("struct.CgIter");
    assert(iterTy && "Iterator type not found in plugin skeleton");

    // Get the CgIter() helper function.
    llvm::Function* getIter = mModule->getFunction("CgGetIter");
    assert(getIter && "CgIter() function not found in plugin skeleton");

    // For each argument, allocate an iterator and call CgGetIter().
    iterators->reserve(args.size());
    int argNum = 1;             // argv[0] is the result, which is void.
    IRVars::const_iterator argIt;
    for (argIt = args.begin(); argIt != args.end(); ++argIt, ++argNum) {
        IRVar* var = *argIt;
        // Generate alloca.
        llvm::Twine iterName = llvm::Twine("_") + var->GetShortName() + "_iter";
        llvm::Value* iter = GenAlloca(iterTy, iterName);
        iterators->push_back(iter);
        // Generate "CgGetIter(argv, $i, $iter)", which returns void.
        mBuilder->CreateCall3(getIter, argv, GetInt(argNum), iter);
    }
}

// Generate a loop that iterates over the active points, calling the
// kernel function for each and incrementing the iterators.
void
CgShader::GenKernelLoop(llvm::Function* entryFunc, llvm::Function* kernelFunc,
                        const IRVars& args, llvm::Value* argv,
                        const std::vector<llvm::Value*>& iterators)
{
    // Get the number of values, "int n = CgNumValues(argv, 0)"
    llvm::Function* getNumValues = mModule->getFunction("CgNumValues");
    assert(getNumValues && "CgNumValues() function not found in skeleton");
    llvm::Value* numValues =
        mBuilder->CreateCall2(getNumValues, argv, GetInt(0));

    // Generate a loop from i = 0 to N-1.
    llvm::Value* loopIndex;
    llvm::BasicBlock* loopBody = GenLoop(numValues, &loopIndex);

    // The block after the loop will be the end of the function
    // All remaining code goes in the loop body.
    mBuilder->CreateRet(GetInt(0));
    mBuilder->SetInsertPoint(loopBody, loopBody->begin());

    // Generate code to dereference the iterators, obtaining a data pointer
    // for each.
    std::vector<llvm::Value*> kernelArgs;
    GenDerefIterators(args, iterators, &kernelArgs);

    // Generate call to kernel function, passing the data pointers.
    mBuilder->CreateCall(kernelFunc, kernelArgs);

    // Increment the iterators.
    llvm::Function* incIter = mModule->getFunction("CgIncIter");
    assert(incIter && "CgIncIter() function not found in skeleton");
    std::vector<llvm::Value*>::const_iterator iter;
    for (iter = iterators.begin(); iter != iterators.end(); ++iter)
        mBuilder->CreateCall(incIter, *iter);
}

/*
   Generate a loop from i = 0 to N-1 as follows:

        br label %loop_test
     loop_test:
        %index = phi i32 [ 0, %entry ], [ %next, %loop_body ]
        %less = icmp slt i32 %index, %N
        br i1 %less, label %loop_body, label %done
     loop_body:
        %next = add i32 %index, 1
        br label %loop_test
     done:

   The loop_body block is returned, along with the loop index (as a result
   parameter), allowing the caller to add code to the body.
*/
llvm::BasicBlock*
CgShader::GenLoop(llvm::Value* loopBound, llvm::Value** loopIndex)
{
    // Keep track of the current block for use by the phi note.
    llvm::BasicBlock* predBlock = mBuilder->GetInsertBlock();
    llvm::Function* function = predBlock->getParent();

    // Create the basic blocks.
    llvm::BasicBlock* testBlock = llvm::BasicBlock::Create(*mContext, "test");
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(*mContext, "body");
    llvm::BasicBlock* doneBlock = llvm::BasicBlock::Create(*mContext, "done");

    // Add the test block and generate a jump to it.
    function->getBasicBlockList().push_back(testBlock);
    mBuilder->CreateBr(testBlock);

    // Generate the loop test:
    //    %index = phi i32 [ 0, %entry ], [ %next, %loop_body ]
    //    %less = icmp slt i32 %index, %N
    mBuilder->SetInsertPoint(testBlock);
    llvm::PHINode* indexPhi =
        mBuilder->CreatePHI(llvm::Type::getInt32Ty(*mContext), 2, "index");
    indexPhi->addIncoming(GetInt(0), predBlock);
    llvm::Value* lessThan = 
        mBuilder->CreateICmp(llvm::CmpInst::ICMP_SLT, indexPhi, loopBound);

    // Generate conditional branch: 
    //    br i1 %less, label %loop_body, label %done
    mBuilder->CreateCondBr(lessThan, bodyBlock, doneBlock);

    // Generate the loop body:
    //    %next = add i32 %index, 1
    //    br label %loop_test
    function->getBasicBlockList().push_back(bodyBlock);
    mBuilder->SetInsertPoint(bodyBlock);
    llvm::Value* next = mBuilder->CreateAdd(indexPhi, GetInt(1));
    mBuilder->CreateBr(testBlock);

    // Update the phi node with [%next, %loop_body]
    indexPhi->addIncoming(next, bodyBlock);

    // Statements following the loop will be generated in the "done" block.
    function->getBasicBlockList().push_back(doneBlock);
    mBuilder->SetInsertPoint(doneBlock);

    // Return the loop body and the loop index.
    *loopIndex = indexPhi;
    return bodyBlock;
}

// Generate code to dereference the iterators, obtaining a data pointer
// for each.
void
CgShader::GenDerefIterators(const IRVars& args,
                            const std::vector<llvm::Value*>& iterators,
                            std::vector<llvm::Value*>* kernelArgs)
{
    // Load the data pointer for each iterator and cast it if necessary:
    //   %data = CgDerefIter(%iter)
    //   %ptr = bitcast float* %data to %struct.OpVec3*       
    llvm::Function* derefIter = mModule->getFunction("CgDerefIter");
    assert(derefIter && "CgDerefIter() function not found in skeleton");

    IRVars::const_iterator argIt = args.begin();
    std::vector<llvm::Value*>::const_iterator iter = iterators.begin();
    for (; argIt != args.end() && iter != iterators.end(); ++argIt, ++iter) {
        IRVar* argVar = *argIt;
        llvm::Twine argName = llvm::Twine("_") + argVar->GetShortName();
        // Load the data pointer.
        llvm::Value* iterVal = *iter;
        llvm::Value* data = mBuilder->CreateCall(derefIter, iterVal);
        // Cast to the argument type.  For now all parameters are passed by
        // reference.
        llvm::Type* argType = 
            mTypes->ConvertParamType(argVar->GetType(), true /*isOutput*/);
        llvm::Value* argVal =
            mBuilder->CreateBitCast(data, argType, argName+"_ptr");
        kernelArgs->push_back(argVal);
    }
}

// Get the maximum detail of the given variables.  If all are uniform, the
// result is uniform; otherwise it's varying.
static IRDetail
MaxDetail(const IRVars& vars)
{
    for (IRVars::const_iterator it = vars.begin(); it != vars.end(); ++it)
        if ((*it)->GetDetail() == kIRVarying)
            return kIRVarying;
    return kIRUniform;
}

// Generate a plugin call with the given arguments.
IRStmt*
CgShader::GenPluginCall(const char* name, const IRVars& argVars, 
                        const IRStringConst* prototype, const IRPos& pos)
{ 
    // Prepare to construct call to plugin entry function.  Although it's
    // a void function, we must nevertheless provide a result variable,
    // whose detail is based on the argument details.
    const IRType* resultTy =  mTypeFactory->GetFloatTy();
    IRLocalVar* result = mShader->NewTempVar(resultTy, MaxDetail(argVars));

    // Convert the free variable list to an IRValue vector.
    IRValues args(argVars.begin(), argVars.end());

    // LLVM made the function name unique if necessary.
    IRStringConst* funcName = mShader->NewStringConst(name);

    // The plugin name contains no filename extension,
    // since it's not required and omitting it is more portable.
    IRStringConst* pluginName = 
        mShader->NewStringConst(mModule->getModuleIdentifier().c_str());

    // Construct a DSO call.  
    return new IRPluginCall(result, args, funcName, pluginName, prototype, pos);
}

// Define an LLVM function that takes the specified IR variables as arguments.
// A unique function name based on the given hint is employed.
llvm::Function*
CgShader::GenKernelFunc(const char* nameHint, const IRVars& args)
{
    // Combine the shader parameters and globals, which will be the
    // entry point parameters.
    size_t numArgs = args.size();

    // Gather parameter types.  For now, all parameters are passed by
    // reference, since we don't know which ones are outputs.
    std::vector<llvm::Type*> argTypes;
    argTypes.reserve(numArgs);
    IRVars::const_iterator v;
    for (v = args.begin(); v != args.end(); ++v) {
        IRVar* var = *v;
        llvm::Type* ty = 
            mTypes->ConvertParamType(var->GetType(), true /*isOutput*/);
        argTypes.push_back(ty);
    }

    // Create LLVM function type.
    llvm::Type* voidTy = llvm::Type::getVoidTy(*mContext);
    llvm::FunctionType* funcTy =
        llvm::FunctionType::get(voidTy, argTypes, false);

    // Create function definition.  Note that LLVM will create a unique
    // name if necessary.
    llvm::Twine name = llvm::Twine(nameHint) + "_kernel";
    llvm::Function::LinkageTypes linkage = llvm::Function::ExternalLinkage;
    llvm::CallingConv::ID callingConv = llvm::CallingConv::C;
    llvm::Function* function =
        llvm::Function::Create(funcTy, linkage, name, mModule);
    function->setCallingConv(callingConv);

    // Add "noalias" and "nocapture" attributes to each parameter.
    // Set the parameter names and record their locations.
    llvm::Function::arg_iterator it;
    size_t i = 0;
    for (it = function->arg_begin(); it != function->arg_end(); ++it, ++i) {
        IRVar* var = args[i];
        llvm::Argument* param = &(*it);
        param->addAttr(llvm::Attribute::NoAlias | llvm::Attribute::NoCapture);
        param->setName(llvm::Twine("_") + var->GetShortName());
        mVars->Bind(var, param);
    }

    // Create an entry block and use it as the IRBuilder's insertion point.
    llvm::BasicBlock* block = 
        llvm::BasicBlock::Create(*mContext, "entry", function);
    mBuilder->SetInsertPoint(block);
    return function;
}

// Generate RSL prototype for a void plugin entry function with the specified
// name and arguments.
std::string
CgShader::GenPrototype(const char* funcName, const IRVars& args)
{
    std::stringstream proto;
    proto << "void " << funcName << "(";
    IRVars::const_iterator it;
    for (it = args.begin(); it != args.end(); ++it) {
        if (it != args.begin())
            proto << ", ";
        IRVar* var = *it;
        if (var->GetDetail() == kIRUniform)
            proto << "uniform ";
        // Note that array parameters look like "float[3] name", which works.
        proto << *var->GetType() << " " << var->GetShortName();
    }
    proto << ")";
    return proto.str();
}

// Update the skeletal RslPublicFunctions global variable, which is a struct
// that contains a pointer to an array of RslFunction structs.
void
CgShader::GenRslFuncTable()
{
    // Generte the RslFunction array constant, and get a pointer to the first
    // element.
    llvm::Constant* funcArray = GenRslFuncArray();
    llvm::Constant* zero = GetInt(0);
    llvm::Constant* indices[] = { zero, zero };
    llvm::Constant* funcArrayPtr = 
        llvm::ConstantExpr::getGetElementPtr(funcArray, indices, 2);

    // Get the RslPublicFunctions global variable.
    llvm::GlobalVariable* global =
        mModule->getGlobalVariable("RslPublicFunctions", true /*AllowLocal*/);
    assert(global && "No RslPublicFunctions in plugin skeleton");

    // Get the existing initializer, which is a struct constant.
    llvm::Constant* init = global->getInitializer();
    assert(init && "No initializer for RslPublicFunctions in plugin skeleton");
    llvm::ConstantStruct* initStruct = llvm::cast<llvm::ConstantStruct>(init);
    assert(initStruct->getType()->getNumElements() == 4 &&
           "Unexpected initializer for RslPublicFunctions");
    assert(initStruct->getOperand(0)->getType() == funcArrayPtr->getType() &&
           "RslFunction array type mismatch");

    // Construct a new struct constant containing a pointer to the RslFunction
    // array, along with the values from the original initializer (the version
    // number and null init/cleanup function pointers).
    std::vector<llvm::Constant*> elements(4);
    elements[0] = funcArrayPtr;
    elements[1] = initStruct->getOperand(1);
    elements[2] = initStruct->getOperand(2);
    elements[3] = initStruct->getOperand(3);

    // Update the RslPublicFunctions initializer.
    // XXX does this cause the old initializer to leak?
    llvm::Constant* newInit = llvm::ConstantStruct::getAnon(*mContext, elements);

    // Sanity check the function table type.
    llvm::Type* rslFuncTableTy =
        mModule->getTypeByName("struct.RslFunctionTable");
    if (rslFuncTableTy == NULL)
        assert(false && "No RslFunctionTable type in plugin skeleton");
    assert(newInit->getType() == rslFuncTableTy &&
           "Unexpected type for RslFunctionTable struct");
    global->setInitializer(newInit);
}

// Generate a global variable containing a constant array of RslFunction
// struct constants, one for each entry function, terminated with a
// zero-filled struct.
llvm::Constant*
CgShader::GenRslFuncArray()
{
    // Look up the RslFunction and RslFunctionTable types.
    llvm::Type* rslFuncTy = mModule->getTypeByName("struct.RslFunction");
    assert(rslFuncTy && "No RslFunction type in plugin skeleton");

    // Generate an array of constant RslFunction structs, one for each entry
    // point, plust a zero-filled terminator.
    std::vector<llvm::Constant*> rslFuncs;
    size_t funcTableSize = mEntryFuncs.size() + 1;
    rslFuncs.reserve(funcTableSize);
    std::list<llvm::Function*>::const_iterator func;
    std::list<const IRStringConst*>::const_iterator proto;
    for (func = mEntryFuncs.begin(), proto = mEntryPrototypes.begin();
         func != mEntryFuncs.end(), proto != mEntryPrototypes.end();
         ++func, ++proto) {
        llvm::Constant* protoConst = mConsts->Convert(*proto);
        llvm::Constant* rslFunc = GenRslFunction(*func, protoConst, rslFuncTy);
        rslFuncs.push_back(rslFunc);
    }
    assert(func == mEntryFuncs.end() && proto == mEntryPrototypes.end() &&
           "Number of entry functions and prototypes doesn't match");

    // We need a null RslEntryFunc pointer.  Function typedefs don't get
    // recorded in bitcode, so we use the initializer of a global null var.
    llvm::Constant* voidFuncPtr = GetGlobalInitializer("gCgNullEntryFunc");

    // We need a null string pointer.
    llvm::Type* charTy = llvm::Type::getInt8Ty(*mContext);
    llvm::PointerType* stringTy = 
        llvm::PointerType::get(charTy, CgTypes::kDefaultAddressSpace);
    llvm::Constant* nullStringPtr = llvm::ConstantPointerNull::get(stringTy);

    // Generate the zero-filled terminator struct.
    llvm::Constant* terminator = 
        GenRslFunction(voidFuncPtr, nullStringPtr, rslFuncTy);
    rslFuncs.push_back(terminator);

    // Generate a constant array of these RslFunction structs.
    llvm::ArrayType* arrayTy =
        llvm::ArrayType::get(terminator->getType(), rslFuncs.size());
    llvm::Constant* funcArray = llvm::ConstantArray::get(arrayTy, rslFuncs);

    // Store the array in a global constant.  We use external linkage
    // simply so it shows up in our test output, which filters out internals.
    return new llvm::GlobalVariable(*mModule, arrayTy, true /*isConstant*/,
                                    llvm::GlobalValue::ExternalLinkage, 
                                    funcArray, "gCgRslFunctions");
}

// Generate an RslFunction struct, which cntains a prototype (char*), the
// entry function pointer (RslEntryFunc), and init/cleanup function pointers
// (NULL).  Version 5 of the RslPlugin API also has renderBegin/End function
// pointers, but those are guarded by an #ifdef, and absent by default.
llvm::Constant*
CgShader::GenRslFunction(llvm::Constant* function, llvm::Constant* prototype,
                         llvm::Type* rslFuncTy)
{
    // We need a null RslVoidFunc pointer.  Function typedefs don't get
    // recorded in bitcode, so we use the initializer of a global null var.
    llvm::Constant* voidFuncPtr = GetGlobalInitializer("gCgNullVoidFunc");

    std::vector<llvm::Constant*> elements(4);
    elements[0] = prototype;
    elements[1] = function;
    elements[2] = voidFuncPtr;
    elements[3] = voidFuncPtr;

    llvm::Constant* rslFunc = llvm::ConstantStruct::getAnon(*mContext, elements);
    assert(rslFunc->getType() == rslFuncTy && "Type mismatch in GenRslFunc");
    return rslFunc;
}

// Get the initializer of the specified global variable.
llvm::Constant*
CgShader::GetGlobalInitializer(const char* name)
{
    llvm::GlobalVariable* global =
        mModule->getGlobalVariable(name, true /*AllowLocal*/);
    assert(global && "Global was not found in plugin skeleton");
    llvm::Constant* init = global->getInitializer();
    assert(init && "Global has no initializer");
    return init;
}

// If the given statement is the root of a partition, compile it and return an
// plugin call.  Otherwise recursively walk its children.
IRStmt*
CgShader::Walk(IRStmt* stmt)
{
    if (stmt->CanCompile()) {
        if (ShouldCompile(stmt))
            return CodegenPartition(stmt);
        else
            return stmt;
    }
    else
        return Dispatch<IRStmt*>(stmt, 0);
}


// Check whether we should compile the given partition.  Currently we run
// XfPartitionInfo prior to codegen to count the number of instructions per
// partition.  We could also take the number of free variables into account.
bool
CgShader::ShouldCompile(IRStmt* stmt)
{
    assert(stmt->CanCompile() && "Expected compilable statement");

    // Don't compile empty partitions.
    int numInsts = stmt->GetNumInsts();
    if (numInsts == 0)
        return false;

    // If XfPartitionInfo hasn't been run (e.g. in a unit test),
    // compile the partition unconditionally.
    if (numInsts < 0)
        return true;

    // For now use an optional minimum instruction count threshold.
    return numInsts >= mMinPartitionSize;
}

IRStmt*
CgShader::Visit(IRBlock* stmt, int ignored)
{
    // No sub-statements.  We're done since this statement can't be compiled.
    assert(!stmt->CanCompile() && "Expected uncompilable block");
    return stmt;
}

IRStmt*
CgShader::Visit(IRSeq* seq, int ignored)
{
    // If this sequence represents the body of an inlined function, keep track
    // of its name to improve naming of plugin functions.  
    const char* enclosingName = NULL;
    if (const IRStringConst* funcName = seq->GetFuncName()) {
        enclosingName = mCurrentFuncName;
        // Kludge: we ignore the outermost one (usually "shader") and use the
        // shader name instead.
        if (seq == mShader->GetBody())
            mCurrentFuncName = mShader->GetName();
        else
            mCurrentFuncName = funcName->Get();
    }

    IRStmts::iterator it;
    for (it = seq->GetStmts().begin(); it != seq->GetStmts().end(); ++it)
        *it = Walk(*it);

    // Restore the enclosing function name.
    if (enclosingName)
        mCurrentFuncName = enclosingName;
    return seq;
}

IRStmt*
CgShader::Visit(IRIfStmt* stmt, int ignored)
{
    stmt->SetThen(Walk(stmt->GetThen()));
    stmt->SetElse(Walk(stmt->GetElse()));
    return stmt;
}

IRStmt*
CgShader::Visit(IRForLoop* loop, int ignored)
{
    loop->SetCondStmt(Walk(loop->GetCondStmt()));
    loop->SetIterateStmt(Walk(loop->GetIterateStmt()));
    loop->SetBody(Walk(loop->GetBody()));
    return loop;
}

IRStmt*
CgShader::Visit(IRCatchStmt* stmt, int ignored)
{
    stmt->SetBody(Walk(stmt->GetBody()));
    return stmt;
}

IRStmt*
CgShader::Visit(IRControlStmt* stmt, int ignored)
{
    // No sub-statements, so we're done.
    return stmt;
}

IRStmt*
CgShader::Visit(IRGatherLoop* loop, int ignored)
{
    loop->SetBody(Walk(loop->GetBody()));
    loop->SetElseStmt(Walk(loop->GetElseStmt()));
    return loop;
}

IRStmt*
CgShader::Visit(IRIlluminanceLoop* loop, int ignored)
{
    loop->SetBody(Walk(loop->GetBody()));
    return loop;
}

IRStmt*
CgShader::Visit(IRIlluminateStmt* stmt, int ignored)
{
    stmt->SetBody(Walk(stmt->GetBody()));
    return stmt;
}

IRStmt*
CgShader::Visit(IRPluginCall* stmt, int ignored)
{
    // No sub-statements, so we're done.
    return stmt;
}

