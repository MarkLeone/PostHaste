// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_VISITOR_H
#define IR_VISITOR_H

#include "ir/IRBasicInst.h"
#include "ir/IRUnknownInst.h"
#include "ir/IRStmts.h"
#include "util/UtCast.h"
#include <stdexcept>

/// Base class for IR visitor.  The derived class should be passes as a
/// template parameter (it's required for downcasts).  The dispatch methods
/// are templated on their argument and result types.
template<typename SubClass>
class IRVisitor {
public:
    /// Given a statement, dispatch to a Visit() method in the derived class
    /// based on its kind.
    template<typename ResultTy, typename ArgTy>
    ResultTy Dispatch(IRStmt* stmt, ArgTy arg) {
        assert(stmt != NULL && "Visited NULL statement");
        SubClass* self = static_cast<SubClass*>(this);
        switch (stmt->GetKind()) {
          case kIRBlock:
              return self->Visit(UtStaticCast<IRBlock*>(stmt), arg);
          case kIRCatchStmt:
              return self->Visit(UtStaticCast<IRCatchStmt*>(stmt), arg);
          case kIRControlStmt:
              return self->Visit(UtStaticCast<IRControlStmt*>(stmt), arg);
          case kIRForLoop:
              return self->Visit(UtStaticCast<IRForLoop*>(stmt), arg);
          case kIRGatherLoop:
              return self->Visit(UtStaticCast<IRGatherLoop*>(stmt), arg);
          case kIRIfStmt:
              return self->Visit(UtStaticCast<IRIfStmt*>(stmt), arg);
          case kIRIlluminanceLoop:
              return self->Visit(UtStaticCast<IRIlluminanceLoop*>(stmt), arg);
          case kIRIlluminateStmt:
              return self->Visit(UtStaticCast<IRIlluminateStmt*>(stmt), arg);
          case kIRPluginCall:
              return self->Visit(UtStaticCast<IRPluginCall*>(stmt), arg);
          case kIRSeq:
              return self->Visit(UtStaticCast<IRSeq*>(stmt), arg);
        }
        assert(false && "Unimplemented statement kind in IRVisitor");
        throw std::runtime_error("Unimplemented statement kind in IRVisitor");
    }        

    /// Given an instruction, dispatch to a Visit() method in the derived
    /// class based on its kind.
    template<typename ResultTy, typename ArgTy>
    ResultTy Dispatch(IRInst* inst, ArgTy arg) {
        assert(inst != NULL && "Visited NULL instruction");
        SubClass* self = static_cast<SubClass*>(this);
        switch (inst->GetKind()) {
          case kIRBasicInst:
              return self->Visit(UtStaticCast<IRBasicInst*>(inst), arg);
          case kIRUnknownInst:
              return self->Visit(UtStaticCast<IRUnknownInst*>(inst), arg);
          default:
              assert(false && "Unimplemented instruction kind in IRVisitor");
              throw std::runtime_error("Unimplemented instruction kind "
                                       "in IRVisitor");
        }
    }
};

#endif // ndef IR_VISITOR_H
