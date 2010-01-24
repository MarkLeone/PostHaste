// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_SHADER_PARAM_H
#define IR_SHADER_PARAM_H

#include "ir/IRVar.h"
#include <assert.h>
class IRStmt;

/// IR shader parameter.
class IRShaderParam : public IRVar {
public:
    /// Get the initialization statement (never NULL).
    IRStmt* GetInitStmt() const 
    { 
        assert(mInit != NULL && "Missing initializer");
        return mInit;
    }

    /// Set the initialization statement.
    void SetInitStmt(IRStmt* init)
    {
        assert(mInit == NULL && "Initialization statement leaked");
        assert(init != NULL && "Missing initialiation statement");
        mInit = init;
    }

    /// Returns true if this is an output parameter.
    bool IsOutput() const { return mIsOutput; }

    /// Construct a shader parameter.  Takes ownership of the initialization
    /// statement.  Typically the initialization statement is initially NULL,
    /// and later updated via SetInitStmt.
    IRShaderParam(const char* name, const IRType* type,
                  IRDetail detail, const char* space,
                  bool isOutput, IRStmt* init) :
        IRVar(kIRShaderParam, name, type, detail, space),
        mInit(init),
        mIsOutput(isOutput)
    {
    }

    /// Destructor is virtual.
    virtual ~IRShaderParam();

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRShaderParam;
    }

    /// Print the declaration of this variable.
    virtual void WriteDecl(std::ostream& out, unsigned int indent) const;

private:
    IRStmt* mInit;              // possibly NULL.
    bool mIsOutput;
};

#endif // ndef IR_SHADER_PARAM_H
