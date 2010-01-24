// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRControlStmt.h"
#include "ir/IRCatchStmt.h"
#include "ir/IRForLoop.h"
#include "ir/IRGatherLoop.h"
#include "util/UtCast.h"

IRControlStmt::IRControlStmt(Opcode opcode, IRStmt* enclosingStmt,
                             const IRPos& pos) :
    IRStmt(kIRControlStmt, pos),
    mOpcode(opcode),
    mEnclosingStmt(enclosingStmt)
{
#ifndef NDEBUG
    assert(mEnclosingStmt && "Enclosing statement cannot be NULL");
    switch (mOpcode) {
      case kOpcode_Break:
      case kOpcode_Continue:
          assert((UtIsInstance<IRForLoop*>(mEnclosingStmt) ||
                  UtIsInstance<IRGatherLoop*>(mEnclosingStmt)) &&
                 "Invalid enclosing statement for break/continue");
          break;
      case kOpcode_Return:
          assert(UtIsInstance<IRCatchStmt*>(mEnclosingStmt) &&
                 "Expected catch statement enclosing return");
          break;
      default:
          assert(false && "Invalid opcode for control-flow statement");
          break;
    }
#endif
}


IRControlStmt::~IRControlStmt()
{
}
