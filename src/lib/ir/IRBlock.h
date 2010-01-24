// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_BLOCK_H
#define IR_BLOCK_H

#include "ir/IRInst.h"
#include "ir/IRStmt.h"
#include "ir/IRTypedefs.h"

/// An IR block is an IRStmt that contains a sequence of instructions (IRInst).
class IRBlock : public IRStmt {
public:
    /// Get the sequence of instructions.
    const IRInsts& GetInsts() const { return *mInsts; }

    /// Get the instructions, taking ownership.
    IRInsts* TakeInsts()
    {
        IRInsts* insts = mInsts;
        mInsts = new IRInsts;
        return insts;
    }

    /// Get a non-const reference to the sequence of instructions
    /// Currently unused.
    // IRInsts& GetInsts() { return *mInsts; }

    /// Construct a block, taking ownership of the given instructions.
     IRBlock(IRInsts* insts) :
        IRStmt(kIRBlock, insts->empty() ? IRPos() : insts->front()->GetPos()),
        mInsts(insts)
    {
    }

    /// Construct an empty block.
    IRBlock() :
        IRStmt(kIRBlock, IRPos()),
        mInsts(new IRInsts)
    {
    }

    /// The destructor recursively destroys the instructions.
    virtual ~IRBlock();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRBlock;
    }

private:
    IRInsts* mInsts;
};

#endif // ndef IR_BLOCK_H
