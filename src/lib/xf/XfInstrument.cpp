// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "xf/XfInstrument.h"
#include "xf/XfPartition.h"
#include "xf/XfPartitionInfo.h"
#include "ir/IRShader.h"
#include "util/UtLog.h"

void
XfInstrument(IRShader* shader, UtLog* log, int minPartitionSize)
{
    return XfInstrumentImpl(log, minPartitionSize).Instrument(shader);
}

// Constructor. 
XfInstrumentImpl::XfInstrumentImpl(UtLog* log, int minPartitionSize) :
    mMinPartitionSize(minPartitionSize),
    mShader(NULL)
{
}

void
XfInstrumentImpl::Instrument(IRShader* shader)
{
    // Hold onto the shader, so we can construct temporary variables.
    mShader = shader;

    // Partition the shader.
    XfPartition(shader);

    // If a minimum partition size was specified, run partition info analysis
    // to determine partition sizes.
    if (mMinPartitionSize > 1)
        XfPartitionInfo(shader, false);
    
    // Walk the shader body, wrapping partitions in timer calls.
    // TODO: do the same for the shader parameter initializers.
    shader->SetBody(Walk(shader->GetBody()));
}

IRStmt*
XfInstrumentImpl::InstrumentPartition(IRStmt* stmt)
{
    // Although the timer plugin functions are void, we must nevertheless
    // provide a result variable.
    const IRType* resultTy =  mShader->GetTypeFactory()->GetFloatTy();
    IRLocalVar* result = mShader->NewTempVar(resultTy, kIRUniform);

    // Function names, plugin names, and prototypes must be IR string constants
    IRStringConst* plugin = mShader->NewStringConst("timers");
    IRStringConst* startFunc = mShader->NewStringConst("StartTimer");
    IRStringConst* stopFunc = mShader->NewStringConst("StopTimer");
    IRStringConst* startProto = mShader->NewStringConst("void StartTimer()");
    IRStringConst* stopProto = mShader->NewStringConst("void StopTimer()");

    // Construct start and stop calls.
    IRStmt* startCall = new IRPluginCall(result, IRValues(), startFunc,
                                         plugin, startProto, stmt->GetPos());
    IRStmt* stopCall = new IRPluginCall(result, IRValues(), stopFunc,
                                        plugin, stopProto, stmt->GetPos());

    // Return a new sequence with the timer calls surrounding the given
    // statement.
    IRStmts* stmts = new IRStmts;
    stmts->push_back(startCall);
    stmts->push_back(stmt);
    stmts->push_back(stopCall);
    return new IRSeq(stmts, NULL, stmt->GetPos());
}

// If the given statement is the root of a partition, instrument it with timer
// calls.  Otherwise recursively walk its children.
IRStmt*
XfInstrumentImpl::Walk(IRStmt* stmt)
{
    if (stmt->CanCompile()) {
        if (ShouldCompile(stmt))
            return InstrumentPartition(stmt);
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
XfInstrumentImpl::ShouldCompile(IRStmt* stmt)
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
XfInstrumentImpl::Visit(IRBlock* stmt, int ignored)
{
    // No sub-statements.  We're done since this statement can't be compiled.
    assert(!stmt->CanCompile() && "Expected uncompilable block");
    return stmt;
}

IRStmt*
XfInstrumentImpl::Visit(IRSeq* seq, int ignored)
{
    IRStmts::iterator it;
    for (it = seq->GetStmts().begin(); it != seq->GetStmts().end(); ++it)
        *it = Walk(*it);
    return seq;
}

IRStmt*
XfInstrumentImpl::Visit(IRIfStmt* stmt, int ignored)
{
    stmt->SetThen(Walk(stmt->GetThen()));
    stmt->SetElse(Walk(stmt->GetElse()));
    return stmt;
}

IRStmt*
XfInstrumentImpl::Visit(IRForLoop* loop, int ignored)
{
    loop->SetCondStmt(Walk(loop->GetCondStmt()));
    loop->SetIterateStmt(Walk(loop->GetIterateStmt()));
    loop->SetBody(Walk(loop->GetBody()));
    return loop;
}

IRStmt*
XfInstrumentImpl::Visit(IRCatchStmt* stmt, int ignored)
{
    stmt->SetBody(Walk(stmt->GetBody()));
    return stmt;
}

IRStmt*
XfInstrumentImpl::Visit(IRControlStmt* stmt, int ignored)
{
    // No sub-statements, so we're done.
    return stmt;
}

IRStmt*
XfInstrumentImpl::Visit(IRGatherLoop* loop, int ignored)
{
    loop->SetBody(Walk(loop->GetBody()));
    loop->SetElseStmt(Walk(loop->GetElseStmt()));
    return loop;
}

IRStmt*
XfInstrumentImpl::Visit(IRIlluminanceLoop* loop, int ignored)
{
    loop->SetBody(Walk(loop->GetBody()));
    return loop;
}

IRStmt*
XfInstrumentImpl::Visit(IRIlluminateStmt* stmt, int ignored)
{
    stmt->SetBody(Walk(stmt->GetBody()));
    return stmt;
}

IRStmt*
XfInstrumentImpl::Visit(IRPluginCall* stmt, int ignored)
{
    // No sub-statements, so we're done.
    return stmt;
}

