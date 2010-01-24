#include "cg/CgComponent.h"
#include "cg/CgConst.h"
#include "cg/CgDeserialize.h"
#include "cg/CgInst.h"
#include "cg/CgStmt.h"
#include "cg/CgTypes.h"
#include "cg/CgValue.h"
#include "cg/CgVars.h"
#include "cg/CgDeserialize.h"
#include <llvm/LLVMContext.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Value.h>

// Create the the master state that shared by all codegen components.
CgComponent
CgComponent::Create(UtLog* log)
{
    // We seem to get leaks when we use a new LLVMContext,
    // so for now we use the global one.
    CgComponent cg;
    cg.mLog = log;
    cg.mContext = &llvm::getGlobalContext();
    cg.mModule = CgDeserializeShadeops(cg.mContext);
    cg.mBuilder = new CgBuilder(*cg.mContext);
    cg.mTypes = new CgTypes(cg.mContext);
    cg.mConsts = new CgConst(cg);
    cg.mVars = new CgVars(cg);
    cg.mValues = new CgValue(cg);
    cg.mInsts = new CgInst(cg);
    cg.mStmts = new CgStmt(cg);
    return cg;
}

// Destroy the master state.
void
CgComponent::Destroy()
{
    delete mBuilder;
    delete mTypes;
    delete mModule;             // usually NULL, unless testing.
}

// Initialize for testing.  Deserializes shadeops, adds an empty function to
// the module and sets the builder insertion point.
void 
CgComponent::InitForTest()
{
    // Create LLVM function type.
    const llvm::Type* voidTy = llvm::Type::getVoidTy(*mContext);
    std::vector<const llvm::Type*> noTypes;
    const llvm::FunctionType* funcTy = 
        llvm::FunctionType::get(voidTy, noTypes, false);

    // Create function definition.
    llvm::Function* function =
        llvm::Function::Create(funcTy, llvm::Function::ExternalLinkage,
                               "test", mModule);

    // Create an entry block and use it as the IRBuilder's insertion point.
    llvm::BasicBlock* block = 
        llvm::BasicBlock::Create(*mContext, "entry", function);
    mBuilder->SetInsertPoint(block);
}

// Generate an "alloca" instruction with the specified type.
llvm::Value*
CgComponent::GenAlloca(const llvm::Type* type, const llvm::Twine& name) const
{
    // Temporarily set the builder insertion point in the function
    // entry block.
    llvm::BasicBlock* oldInsertBlock = mBuilder->GetInsertBlock();
    llvm::BasicBlock::iterator oldInsertPoint = mBuilder->GetInsertPoint();
    llvm::Function* function = mBuilder->GetInsertBlock()->getParent();
    mBuilder->SetInsertPoint(&function->getEntryBlock(), 
                             function->getEntryBlock().begin());

    // Generate the alloca instruction.  Prefix the variable name with
    // an underscore so LLVM doesn't quote it.
    llvm::Value* inst = mBuilder->CreateAlloca(type, 0, name);

    // Restore the builder insertion point and return the instruction.
    mBuilder->SetInsertPoint(oldInsertBlock, oldInsertPoint);
    return inst;
}


// Get an integer constant (32-bit, signed).
llvm::Constant* 
CgComponent::GetInt(int i) const
{
    const llvm::Type* ty = llvm::Type::getInt32Ty(*mContext);
    return llvm::ConstantInt::getSigned(ty, i);
}
