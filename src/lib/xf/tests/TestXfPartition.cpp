#include "ir/IRShader.h"
#include "slo/SloInputFile.h"
#include "slo/SloShader.h"
#include "util/UtLog.h"
#include "xf/XfPartition.h"
#include "xf/XfPartitionInfo.h"
#include "xf/XfRaise.h"
#include <gtest/gtest.h>
#include <iostream>

class TestXfPartition : public testing::Test { 
public:
    UtLog mLog;

    TestXfPartition() : mLog(stderr) { }

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
};

TEST_F(TestXfPartition, TestPartitionBlock) {
    IRShader* shader = LoadShader("TestPartBlock.slo");
    IRSeq* bodySeq = UtCast<IRSeq*>(shader->GetBody());
    ASSERT_TRUE(bodySeq != NULL);
    IRStmts* stmts = bodySeq->TakeStmts();
    delete bodySeq;

    ASSERT_TRUE(stmts->size() == 1);
    IRBlock* block = UtCast<IRBlock*>(stmts->front());
    ASSERT_TRUE(block != NULL);
    delete stmts;

    IRStmt* partitioned = XfPartitionImpl().Visit(block, 0);
    std::cout << *partitioned << std::endl;
    EXPECT_FALSE(partitioned->CanCompile());

    shader->SetBody(partitioned);
    delete shader;
}

TEST_F(TestXfPartition, TestPartitionSeq) {
    IRShader* shader = LoadShader("TestPartSeq.slo");
    IRSeq* bodySeq = UtCast<IRSeq*>(shader->GetBody());
    ASSERT_TRUE(bodySeq != NULL);
    
    IRStmt* partitioned = XfPartitionImpl().Visit(bodySeq, 0);
    std::cout << *partitioned << std::endl;
    EXPECT_FALSE(partitioned->CanCompile());

    shader->SetBody(partitioned);
    delete shader;
}

TEST_F(TestXfPartition, TestPartitionIf) {
    IRShader* shader = LoadShader("TestPartIf.slo");
    XfPartition(shader);
    std::cout << *shader << std::endl;
    EXPECT_FALSE(shader->GetBody()->CanCompile());
    
    XfPartitionInfo(shader, true);
    delete shader;
}

TEST_F(TestXfPartition, TestAllNative) {
    IRShader* shader = LoadShader("TestPartNative.slo");
    XfPartition(shader);
    std::cout << *shader << std::endl;
    EXPECT_TRUE(shader->GetBody()->CanCompile());

    XfPartitionInfo(shader, true);
    delete shader;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

