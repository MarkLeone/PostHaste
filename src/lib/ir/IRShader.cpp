// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRShader.h"
#include "ir/IRValues.h"
#include "ir/IRStmt.h"
#include "slo/SloEnums.h"
#include "util/UtDelete.h"
#include "util/UtTokenFactory.h"
#include <iostream>
#include <sstream>

IRShader::~IRShader() 
{
    UtDeleteSeq(mParams);
    UtDeleteSeq(mConstants);
    UtDeleteSeq(mLocals);
    UtDeleteSeq(mGlobals);
    // The symbol list includes the params, constants, locals, and globals.
    // It's added by XfRaise to help preserve the original symbol table order.
    delete mSymbols;
    delete mBody;
    delete mTypes;
}

std::ostream& 
operator<<(std::ostream& out, const IRShader& shader)
{
    // Shader type and name.
    out << SloShaderTypeToString(shader.GetType())
        << " " << shader.GetName() << "(\n";

    // Shader params.
    const IRShaderParams& params = shader.GetParams();
    IRShaderParams::const_iterator it;
    for (it = params.begin(); it != params.end(); ++it) {
        IRShaderParam* param = *it;
        param->WriteDecl(out, 4);
        out << ";\n";
    }
    out << "    )\n{\n";

    // Locals.
    IRLocalVars::const_iterator ii;
    for (ii = shader.mLocals->begin(); ii != shader.mLocals->end(); ++ii) {
        IRLocalVar* var = *ii;
        var->WriteDecl(out, 4);
        out << ";\n";
    }
    if (!shader.mLocals->empty()) {
        out << "\n";
    }

    // Body.
    shader.GetBody()->Write(out, 4, true);

    out << "}\n";
    return out;
}

// Create a new temporary variable.
IRLocalVar* 
IRShader::NewTempVar(const IRType* type, IRDetail detail)
{
    // It probably doesn't matter, but we're careful to avoid name clashes
    // with other temporaries.  The shader compiler uses $T0, etc.  XfLower
    // uses $T_0, etc.  We use $TT_0, etc.
    // TODO: use an empty name here and make it unique in XfLower.
    std::stringstream name;
    name << "$TT_" << mNewTempVarCount++;
    IRLocalVar* var = new IRLocalVar(name.str().c_str(), type, detail, "");
    mLocals->push_back(var);
    mSymbols->push_back(var);
    return var;
}

// Create a new string constant.
IRStringConst* 
IRShader::NewStringConst(const char* str)
{
    // Anonymous string constants are OK.  A unique name is assigned in XfLower.
    IRStringConst* constant = new IRStringConst(str);
    mConstants->push_back(constant);
    mSymbols->push_back(constant);
    return constant;
}
