// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "xf/XfPartitionInfo.h"
#include "ir/IRShader.h"
#include "ir/IRStmt.h"
#include "ir/IRVarSet.h"
#include "ir/IRVisitor.h"
#include <iostream>
#include <stdio.h>

class XfPartitionInfoImpl : public IRVisitor<XfPartitionInfoImpl>
{
private:
    int mNumInsts;
    std::list<int> mLengths;
    bool mPrintReport;
    
public:
    XfPartitionInfoImpl(bool printReport) :
        mNumInsts(0),
        mPrintReport(printReport)
    { 
    }

    void Analyze(const IRShader* shader)
    {
        Walk(shader->GetBody(), kWalk);
        if (mPrintReport)
            Report(shader);
    }

    void Report(const IRShader* shader) {
        // Count average and max partition length.
        int maxLength = 0;
        int sumOfLengths = 0;
        int numPartitions = 0;
        std::list<int>::const_iterator it;
        for (it = mLengths.begin(); it != mLengths.end(); ++it) {
            int length = *it;
            maxLength = length > maxLength ? length : maxLength;
            sumOfLengths += length;
            ++numPartitions;
        }
        float avgLength = (float) sumOfLengths / numPartitions;

        std::cout.flush();
        printf("Shader: %s\n", shader->GetName());
        printf("Instructions: %i\n", mNumInsts);
        printf("Num compiled partitions: %i\n", numPartitions);
        printf("Average partition length: %.2f\n", avgLength);
        printf("Max partition length: %i\n", maxLength);
        printf("Partition lengths: ");
        for (it = mLengths.begin(); it != mLengths.end(); ++it)
            printf("%i ", *it);
        printf("\n\n");
        fflush(stdout);
    }

    enum Mode { kCount, kWalk };

    int Walk(IRStmt* stmt, Mode mode) {
        int numInsts;
        bool isPartition = (mode == kWalk) && stmt->CanCompile();
        if (isPartition) {
            // Optionally print the free variable set 
            const IRVarSet* freeVars = stmt->GetFreeVars();
            if (freeVars && mPrintReport)
                std::cout << "Partition: FreeVars = " << *freeVars << "\n"
                          << *stmt << "\n";

            // Count the number of instructions in the partition.
            numInsts = Dispatch<int>(stmt, kCount);

            // Record the num of instructions in the statement and in the
            // summary statistics.
            stmt->SetNumInsts(numInsts);
            mLengths.push_back(numInsts);
        }
        else
            numInsts = Dispatch<int>(stmt, mode);
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRBlock* stmt, Mode mode) 
    {
        int numInsts = (int) stmt->GetInsts().size();
        mNumInsts += numInsts;
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRSeq* seq, Mode mode) 
    {
        int numInsts = 0;
        IRStmts::const_iterator it;
        for (it = seq->GetStmts().begin(); it != seq->GetStmts().end(); ++it)
            numInsts += Walk(*it, mode);
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRIfStmt* stmt, Mode mode) 
    {
        mNumInsts += 1;
        int numInsts = 1;
        numInsts += Walk(stmt->GetThen(), mode);
        numInsts += Walk(stmt->GetElse(), mode);
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRForLoop* loop, Mode mode) 
    {
        mNumInsts += 1;
        int numInsts = 1;
        numInsts += Walk(loop->GetCondStmt(), mode);
        numInsts += Walk(loop->GetIterateStmt(), mode);
        numInsts += Walk(loop->GetBody(), mode);
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRControlStmt* stmt, Mode mode) 
    {
        mNumInsts += 1;
        return mode == kCount ? 1 : 0;
    }

    int Visit(IRGatherLoop* loop, Mode mode) 
    {
        mNumInsts += 1;
        int numInsts = 1;
        numInsts += Walk(loop->GetBody(), mode);
        numInsts += Walk(loop->GetElseStmt(), mode);
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRIlluminanceLoop* loop, Mode mode) 
    {
        mNumInsts += 1;
        int numInsts = 1;
        numInsts += Walk(loop->GetBody(), mode);
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRIlluminateStmt* stmt, Mode mode) 
    {
        mNumInsts += 1;
        int numInsts = 1;
        numInsts += Walk(stmt->GetBody(), mode);
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRCatchStmt* stmt, Mode mode) 
    {
        int numInsts = 1;
        numInsts += Walk(stmt->GetBody(), mode);
        return mode == kCount ? numInsts : 0;
    }

    int Visit(IRPluginCall* stmt, Mode mode) 
    {
        mNumInsts += 1;
        return mode == kCount ? 1 : 0;
    }
};

void 
XfPartitionInfo(const IRShader* shader, bool printReport)
{
    XfPartitionInfoImpl(printReport).Analyze(shader);
}
