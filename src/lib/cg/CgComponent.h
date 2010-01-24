// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_COMPONENT_H
#define CG_COMPONENT_H

#include "cg/CgFwd.h"
#include "cg/CgTypedefs.h"
#include <map>
class CgConst;
class CgInst;
class CgStmt;
class CgTypes;
class CgValue;
class CgVars;
class IRVar;
class UtLog;

/// Shared state for various code generation components.
class CgComponent {
public:
    /// Create the the master state that shared by all codegen components.
    static CgComponent Create(UtLog* log);

    /// Destroy the master state.
    void Destroy();

    /// Initialize for testing.  Adds an empty function to the module and sets
    /// the builder insertion point.
    void InitForTest();

    /// Get the LLVM context.
    llvm::LLVMContext* GetContext() const { return mContext; }

    /// Get the LLVM module.
    llvm::Module* GetModule() const { return mModule; }

    /// Get the LLVM IRBuilder.
    CgBuilder* GetBuilder() const { return mBuilder; }

    /// Get the codegen type converter.
    const CgTypes* GetTypes() const { return mTypes; }

    /// Get the variable codegen component (required to set up test bindings)
    CgVars* GetVars() const { return mVars; }

    /// Generate an "alloca" instruction with the specified type.
    llvm::Value* GenAlloca(const llvm::Type* type,
                           const llvm::Twine& name) const;

    /// Get an integer constant (32-bit, signed).
    llvm::Constant* GetInt(int i) const;

protected:
    /// Message log
    UtLog* mLog;

    /// LLVM context, which is often required to construct types and values.
    llvm::LLVMContext* mContext;

    /// The current LLVM module.
    llvm::Module* mModule;

    /// The IR builder keeps the current insertion point.
    CgBuilder* mBuilder;

    /// IR to LLVM type conversion.
    const CgTypes* mTypes;

    /// IR to LLVM constant conversion.
    const CgConst* mConsts;

    /// Bindings from IR variables to LLVM values.
    CgVars* mVars;

    /// Code generation for IR values.
    const CgValue* mValues;

    /// Code generation for IR instructions.
    const CgInst* mInsts;

    /// Code generation for IR statements.
    const CgStmt* mStmts;

public:
    // Copy constructor 
    CgComponent(const CgComponent& cg) :
        mLog(cg.mLog),
        mContext(cg.mContext),
        mModule(cg.mModule),
        mBuilder(cg.mBuilder),
        mTypes(cg.mTypes),
        mConsts(cg.mConsts),
        mVars(cg.mVars),
        mValues(cg.mValues),
        mInsts(cg.mInsts),
        mStmts(cg.mStmts)
    {
    }

private:
    /// Constructor is private.  Master component constructed only by Create().
    CgComponent() :
        mLog(NULL),
        mContext(NULL),
        mModule(NULL),
        mBuilder(NULL),
        mTypes(NULL),
        mConsts(NULL),
        mVars(NULL),
        mValues(NULL),
        mInsts(NULL),
        mStmts(NULL)
    {
    }
};

#endif // CG_COMPONENT_H
