#include "ir/IRInst.h"
#include "ir/IRShader.h"
#include "ir/IRValues.h"
#include "ir/IRVisitor.h"
#include "slo/SloInputFile.h"
#include "slo/SloShader.h"
#include "util/UtLog.h"
#include "xf/XfLiveVars.h"
#include "xf/XfRaise.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>

class TestXfLiveVarsWriter : public testing::Test {
public:
    UtLog mLog;
    TestXfLiveVarsWriter() :
        mLog(stderr)
    {
    }

    IRShader* LoadShader(const char* filename) 
    {
        SloInputFile in(filename, &mLog);
        int status = in.Open();
        assert(status == 0 && "SLO open failed");
        SloShader slo;
        status = slo.Read(&in);
        assert(status == 0 && "SLO read failed");
        IRShader* shader = XfRaise(slo, &mLog);
        assert(shader != NULL && "Raising to IR failed");
        return shader;
    }

    void WriteLiveVars(const char* filename);
};


class LiveVarWriter : public IRVisitor<LiveVarWriter> {
private:
    std::ostream& mOut;
    XfLiveVarsImpl* mLiveVars;

    std::ostream& Indent(unsigned int indent) const
    {
        return mOut << std::setw(indent) << "";
    }

public:
    LiveVarWriter(std::ostream& out,
                  XfLiveVarsImpl* liveVars) : 
        mOut(out),
        mLiveVars(liveVars)
    { }

    void Visit(IRBlock* block, unsigned int indent)
    {
        const IRInsts& insts = block->GetInsts();
        IRInsts::const_iterator it;
        for (it = insts.begin(); it != insts.end(); ++it) {
            IRInst* inst = *it;
            Indent(indent) << *inst << ";\n";
        }
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
        Indent(indent) << "for (;\n";
        IRVar* loopVar = UtCast<IRVar*>(loop->GetCond());
        Write(loop->GetCondStmt(), indent+4);
        Write(loop->GetIterateStmt(), indent+4);
        Indent(indent+4) << ") {\n";
        Write(loop->GetBody(), indent+4);
        Indent(indent) << "}\n";
    }

    void Visit(IRControlStmt* stmt, unsigned int indent)
    {
        const char* name = OpcodeName(stmt->GetOpcode());
        assert(name[0] == '_' && "Expected underscore in control opcode name");
        Indent(indent) << ++name << ";\n";
    }

    void Visit(IRSpecialForm* stmt, unsigned int indent)
    {
        Indent(indent) << "special (";
        IRWriteValues(mOut, stmt->GetArgs());
        mOut << ") {\n";
        Write(stmt->GetBody(), indent+4);
        Indent(indent) << "}\n";
    }

    void Visit(IRSeq* seq, unsigned int indent)
    {
        Indent(indent) << "{\n";
        const IRStmts& stmts = seq->GetStmts();
        IRStmts::const_iterator it;
        for (it = stmts.begin(); it != stmts.end(); ++it)
            Write(*it, indent+4);
        Indent(indent) << "}\n";
    }

    void Visit(IRCatchStmt* stmt, unsigned int indent)
    {
        Indent(indent) << "catch {\n";
        Write(stmt->GetBody(), indent+4);
        Indent(indent) << "}\n";
    }

    void Visit(IRPluginCall* stmt, unsigned int indent)
    {
        Indent(indent) << *stmt->GetResult()  << " = " 
                       << stmt->GetFuncName()->Get() << "(";
        IRWriteValues(mOut, stmt->GetArgs());
        mOut << ");\n";
    }

    void Write(IRStmt* stmt, unsigned int indent)
    {
        IRVarSet live;
        mLiveVars->GetLive(stmt, &live);
        Indent(indent) << "// Free: " << live << "\n";
        return Dispatch<void>(stmt, indent);
    }
};

void
TestXfLiveVarsWriter::WriteLiveVars(const char* filename)
{
    std::cout << "---------- " << filename << " ----------\n";
    IRShader* shader = LoadShader(filename);
    XfLiveVarsImpl liveVars(shader->GetGlobals());
    LiveVarWriter writer(std::cout, &liveVars);
    writer.Write(shader->GetBody(), 0);
}

TEST_F(TestXfLiveVarsWriter, TestWriteLiveVars) {
    WriteLiveVars("areacam.slo");
    WriteLiveVars("lumpy.slo");
    WriteLiveVars("oak.slo");
}


int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
