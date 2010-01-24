// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_SHADER_H
#define IR_SHADER_H

#include "ir/IRTypedefs.h"
#include "ir/IRTypes.h"
#include "slo/SloEnums.h"       // for SloShaderType
#include <list>
#include <string>
class IRConst;
class IRGlobalVar;
class IRLocalVar;
class IRStmt;
class IRStringConst;

class IRShader {
public:
    /// Get shader name.
    const char* GetName() const { return mName.c_str(); }

    /// Get shader type.
    SloShaderType GetType() const { return mType; }

    /// Get shader parameters.
    const IRShaderParams& GetParams() const { return *mParams; }

    /// Get global variables used in this shader..
    const IRGlobalVars& GetGlobals() const { return *mGlobals; }

    /// Get shader body.  TODO: guard with version check.
    IRStmt* GetBody() const { return mBody; }

    /// Set shader body.  Called from XfPartition.
    void SetBody(IRStmt* stmt) { mBody = stmt; }

    /// Get this shader's type factory.
    IRTypes* GetTypeFactory() const { return mTypes; }

    /// Create a new temporary variable.
    IRLocalVar* NewTempVar(const IRType* type, IRDetail detail);

    /// Create a new string constant.
    IRStringConst* NewStringConst(const char* str);

    /// Construct IR shader, taking ownership of the given IR.
    IRShader(const char* name,
             SloShaderType type,
             IRTypes* types,
             IRShaderParams* params,
             IRConsts* constants,
             IRLocalVars* locals,
             IRGlobalVars* globals,
             IRStmt* body) :
        mName(name),
        mType(type),
        mTypes(types),
        mSymbols(NULL),
        mParams(params),
        mConstants(constants),
        mLocals(locals),
        mGlobals(globals),
        mBody(body),
        mNewTempVarCount(0),
        mNewStringConstCount(0)
    {
    }

    /// The destructor recursively deletes the children.
    ~IRShader();

private:
    friend class XfRaiseImpl;
    friend class XfLowerImpl;
    friend class CgShader;
    friend class CgWholeShader;
    friend std::ostream& operator<<(std::ostream& out, const IRShader& shader);
    std::string mName;
    SloShaderType mType;
    IRTypes* mTypes;
    IRValues* mSymbols;         // All params, constants, locals, globals.
    IRShaderParams* mParams;
    IRConsts* mConstants;
    IRLocalVars* mLocals;
    IRGlobalVars* mGlobals;
    IRStmt* mBody;
    unsigned int mNewTempVarCount;
    unsigned int mNewStringConstCount;
};

/// Print an IR shader.
std::ostream& operator<<(std::ostream& out, const IRShader& shader);

#endif // ndef IR_SHADER_H
