// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRStmt.h"
#include "ir/IRInst.h"
#include "ir/IRStmts.h"
#include "ir/IRStringConst.h"
#include "ir/IRVar.h"
#include "ir/IRVarSet.h"
#include "ir/IRVisitor.h"
#include "util/UtCast.h"
#include <iostream>
#include <iomanip>              // for std::setw()

IRStmt::~IRStmt() 
{ 
    delete mFreeVars;
}

bool
IRStmt::IsEmpty() const
{
    const IRSeq* seq = UtCast<const IRSeq*>(this);
    const IRBlock* block = UtCast<const IRBlock*>(this);
    return seq && seq->GetStmts().empty() ||
        block && block->GetInsts().empty();
}

class StmtWriter : public IRVisitor<StmtWriter> {
private:
    std::ostream& mOut;
    IRPos mLastPos;

    // Update current position, printing it if it's significantly different
    // from the previous position.
    void PrintPos(IRInst* inst, unsigned int indent)
    {
        const IRPos& newPos = inst->GetPos();
        if (newPos.GetFile() != mLastPos.GetFile() ||
            newPos.GetLine() > mLastPos.GetLine() + 5 ||
            newPos.GetLine() < mLastPos.GetLine()) {
            Indent(indent) << "#line " << newPos.GetLine() << " "
                           << '"' << newPos.GetFile() << '"' << "\n";
        }
        mLastPos = newPos;
    }

    std::ostream& Indent(unsigned int indent) const
    {
        return mOut << std::setw(indent) << "";
    }

public:
    StmtWriter(std::ostream& out) : mOut(out) { }

    void WriteCommaBlock(IRStmt* stmt, const IRVar* dest,
                         unsigned int indent)
    {
        if (stmt->IsEmpty()) {
            if (dest)
                mOut << *dest;
            return;
        }
        if (IRBlock* block = UtCast<IRBlock*>(stmt)) {
            const IRInsts& insts = block->GetInsts();
            assert(!insts.empty() && "Empty blocks handled above");

            // Wrap it in parentheses if there's a destination and more than
            // one instruction
            bool parens = dest && insts.size() > 1;
            if (parens)
                mOut << "(";

            // Print all but the last instruction.
            IRInsts::const_iterator it = insts.begin();
            while (it != insts.end()) {
                IRInst* inst = *it;
                ++it;
                if (it == insts.end())
                    break;
                mOut << *inst << ", ";
            }
            // If the target of the last instruction is the same as the
            // destination, omit it.
            IRInst* last = insts.back();
            if (last->GetResult() == dest) {
                last->WriteSource(mOut);
            }
            else  {
                mOut << *last;
                if (dest)
                    mOut << ", " << *dest;
            }
            if (parens)
                mOut << ")";
        }
        else {
            mOut << "{\n";
            Write(stmt, indent+4);
            Indent(indent) << "}";
            if (dest) 
                mOut << ", " << *dest;
            else {
                mOut << "\n";
                Indent(indent);
            }
        }
    }

    void Visit(IRBlock* block, unsigned int indent)
    {
        // Indent(indent) << "<block>\n";
        // if (block->CanCompile()) 
        //    Indent(indent) << "// <CanCompile>\n";
        const IRInsts& insts = block->GetInsts();
        IRInsts::const_iterator it;
        for (it = insts.begin(); it != insts.end(); ++it) {
            IRInst* inst = *it;
            PrintPos(inst, indent);
            Indent(indent) << *inst << ";\n";
        }
        // if (block->CanCompile()) 
        //     Indent(indent) << "// </CanCompile>\n";
        // Indent(indent) << "</block>\n";
    }

    void Visit(IRIfStmt* ifStmt, unsigned int indent)
    {
        Indent(indent) << "if (" << *ifStmt->GetCond() << ") {\n";
        Write(ifStmt->GetThen(), indent+4);
        Indent(indent) << "}\n";
        if (!ifStmt->GetElse()->IsEmpty()) {
            Indent(indent) << "else {\n";
            Write(ifStmt->GetElse(), indent+4);
            Indent(indent) << "}\n";
        }
    }

    void Visit(IRForLoop* loop, unsigned int indent)
    {
        Indent(indent) << "for (; ";
        IRVar* loopVar = UtCast<IRVar*>(loop->GetCond());
        if (loopVar != NULL) 
            WriteCommaBlock(loop->GetCondStmt(), loopVar, indent+7);
        else {
            WriteCommaBlock(loop->GetCondStmt(), NULL, indent+7);
            mOut << ", " << *loop->GetCond();
        }
        mOut << "; ";
        WriteCommaBlock(loop->GetIterateStmt(), NULL, indent+5);
        mOut << ") {\n";
        Write(loop->GetBody(), indent+4);
        Indent(indent) << "}\n";
    }

    void Visit(IRControlStmt* stmt, unsigned int indent)
    {
        // Note: multi-level break/continue statements don't print correctly.
        // We would have to count loop depth to do so.
        const char* name = OpcodeName(stmt->GetOpcode());
        assert(name[0] == '_' && "Expected underscore in control opcode name");
        Indent(indent) << ++name << ";\n";
    }

    void Visit(IRIlluminanceLoop* loop, unsigned int indent)
    {
        Indent(indent) << "illuminance (";
        if (loop->GetCategory())
            mOut << *loop->GetCategory() << ", ";
        IRWriteValues(mOut, loop->GetArgs());
        mOut << ") {\n";
        Write(loop->GetBody(), indent+4);
        Indent(indent) << "}\n";
    }

    void Visit(IRIlluminateStmt* loop, unsigned int indent)
    {
        if (loop->IsSolar())
            Indent(indent) << "solar (";
        else
            Indent(indent) << "illuminate (";
        IRWriteValues(mOut, loop->GetArgs());
        mOut << ") {\n";
        Write(loop->GetBody(), indent+4);
        Indent(indent) << "}\n";
    }

    void Visit(IRGatherLoop* loop, unsigned int indent)
    {
        Indent(indent) << "gather ("
                       << *loop->GetCategory() << ", ";
        IRWriteValues(mOut, loop->GetArgs());
        mOut << ") {\n";
        Write(loop->GetBody(), indent+4);
        Indent(indent) << "}\n";
        if (!loop->GetElseStmt()->IsEmpty()) {
            Indent(indent) << "else {\n";
            Write(loop->GetElseStmt(), indent+4);
            Indent(indent) << "}\n";
        }
    }

    void Visit(IRSeq* seq, unsigned int indent)
    {
        WriteSeq(seq, indent, false);
    }

    void Visit(IRCatchStmt* stmt, unsigned int indent)
    {
        Indent(indent) << "catch {\n";
        Write(stmt->GetBody(), indent+4);
        Indent(indent) << "}\n";
    }

    void Visit(IRPluginCall* stmt, unsigned int indent)
    {
        assert(stmt->GetResult() != NULL && "Plugin call result is missing");
        Indent(indent) << *stmt->GetResult()  << " = " 
                       << stmt->GetFuncName()->Get() << "(";
        IRWriteValues(mOut, stmt->GetArgs());
        mOut << ");\n";
    }

    void WriteSeq(IRSeq* seq, unsigned int indent, bool noBraces)
    {
        const IRStringConst* funcName = seq->GetFuncName();
        if (funcName && !noBraces) {
            Indent(indent) << "{";
            if (funcName)
                mOut << " // " << funcName->Get();
            mOut << "\n";
            indent += 4;
        }
        const IRStmts& stmts = seq->GetStmts();
        IRStmts::const_iterator it;
        for (it = stmts.begin(); it != stmts.end(); ++it)
            Write(*it, indent);
        if (funcName && !noBraces) {
            indent -= 4;
            Indent(indent) << "}";
            if (funcName)
                mOut << " // " << funcName->Get();
            mOut << "\n";
        }
    }

    void Write(IRStmt* stmt, unsigned int indent, bool noBraces = false)
    {
        if (IRSeq* seq = UtCast<IRSeq*>(stmt))
            WriteSeq(seq, indent, noBraces);
        else
            return Dispatch<void>(stmt, indent);
    }
};

void
IRStmt::Write(std::ostream& out, unsigned int indent, bool noBraces) const
{
    StmtWriter(out).Write(const_cast<IRStmt*>(this), indent, noBraces);
}

void
IRStmt::WriteInit(std::ostream& out, const IRVar* dest,
                  unsigned int indent) const
{
    StmtWriter(out).WriteCommaBlock(const_cast<IRStmt*>(this), dest, indent);
}

std::ostream& 
operator<<(std::ostream& out, const IRStmt& stmt)
{
    stmt.Write(out);
    return out;
}
