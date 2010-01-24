// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef XF_PARTITION_H
#define XF_PARTITION_H

#include "ir/IRVisitor.h"
class IRShader;

/// Partition the given shader, creating sequences and blocks that can be be
/// fully compiled.
void XfPartition(IRShader* shader);

/// Implementation of partitioning.  The methods are all public for testing.
class XfPartitionImpl : public IRVisitor<XfPartitionImpl> {
public:
    void Partition(IRShader* shader);

    IRStmt* Partition(IRStmt* stmt) { return Dispatch<IRStmt*>(stmt, 0); }
    IRStmt* Visit(IRBlock* stmt, int ignored);
    IRStmt* Visit(IRSeq* stmt, int ignored);
    IRStmt* Visit(IRIfStmt* stmt, int ignored);
    IRStmt* Visit(IRForLoop* stmt, int ignored);
    IRStmt* Visit(IRControlStmt* stmt, int ignored);
    IRStmt* Visit(IRGatherLoop* stmt, int ignored);
    IRStmt* Visit(IRIlluminanceLoop* stmt, int ignored);
    IRStmt* Visit(IRIlluminateStmt* stmt, int ignored);
    IRStmt* Visit(IRCatchStmt* stmt, int ignored);
    IRStmt* Visit(IRPluginCall* stmt, int ignored);
};

#endif // ndef XF_PARTITION_H
