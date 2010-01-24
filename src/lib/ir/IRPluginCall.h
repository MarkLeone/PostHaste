// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_PLUGIN_CALL_H
#define IR_PLUGIN_CALL_H

#include "ir/IRStmt.h"

// IR for a call to a plugin function.
class IRPluginCall : public IRStmt {
public:
    /// Get the result, which is never NULL, even if the function is void.
    IRVar* GetResult() const { return mResult; }

    /// Get the function call arguments.
    IRValues GetArgs() const { return mArgs; }

    /// Get the plugin function name.
    const IRStringConst* GetFuncName() const { return mFuncName; }

    /// Get the plugin (DSO) name.
    const IRStringConst* GetPluginName() const { return mPluginName; }

    /// Get the function prototype (from the plugin function table).
    const IRStringConst* GetPrototype() const { return mPrototype; }

    // Construct IR for a plugin function call.  The result cannot be NULL,
    // even if the function return type is void.
    IRPluginCall(IRVar* result,
                 const IRValues& args,
                 const IRStringConst* funcName,
                 const IRStringConst* pluginName,
                 const IRStringConst* prototype,
                 const IRPos& pos) :
        IRStmt(kIRPluginCall, pos),
        mResult(result),
        mArgs(args),
        mFuncName(funcName),
        mPluginName(pluginName),
        mPrototype(prototype)
    {
        assert(mResult != NULL && "Plugin call result can't be NULL, "
               "even if the function is void");
    }

    /// The destructor is a no-op.
    virtual ~IRPluginCall();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRPluginCall;
    }

private:
    IRVar* mResult;             // possibly NULL
    IRValues mArgs;
    const IRStringConst* mFuncName;
    const IRStringConst* mPluginName;
    const IRStringConst* mPrototype;
};

#endif // ndef IR_PLUGIN_CALL_H
