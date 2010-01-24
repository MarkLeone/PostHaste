// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_STMT_H
#define IR_STMT_H

#include "ir/IRPos.h"
#include <iosfwd>
class IRVar;
class IRVarSet;

/// Kinds of IR statements.
enum IRStmtKind {
    kIRBlock,
    kIRCatchStmt,
    kIRControlStmt,
    kIRForLoop,
    kIRGatherLoop,
    kIRIfStmt,
    kIRIlluminanceLoop,
    kIRIlluminateStmt,
    kIRPluginCall,
    kIRSeq,
};

/**
   Abstract base class for an IR statement.  Derived classes are:
   - IRBlock              Block of instructions (IRInst).
   - IRCatchStmt          Body of inlined function with nested return.
   - IRControlStmt        Break, continue, or return statement.
   - IRForLoop            For loop, with cond stmt, iterate stmt, and body.
   - IRIfStmt             "if" statement with "then" and "else" stmts.
   - IRPluginCall         Call to plugin function.
   - IRSeq                Sequence of statements.
   - IRSpecialForm        Special form, which include arbitrary user arguments.
     - IRGatherLoop       e.g. gather("illuminance", P, R, phi, n) {...}
     - IRIlluminanceLoop  e.g. illuminance (P, N, PI/2, "var", value) {...}
     - IRIlluminateStmt   e.g. illuminate (from, axis, angle) {...}
*/
class IRStmt {
public:
    /// Get the source position.
    const IRPos& GetPos() const { return mPos; }

    /// Construct an IR statement of the specified kind, with optional
    /// debug info.
    IRStmt(IRStmtKind kind, const IRPos& pos) :
        mKind(kind),
        mPos(pos),
        mFreeVars(NULL),
        mNumInsts(-1),
        mCanCompile(false)
    {
    }

    /// Destructor (not sure why it can't be pure virtual).
    virtual ~IRStmt();

    /// Check whether this is an empty sequence or block.
    bool IsEmpty() const;

    /// What kind of statement is this?
    IRStmtKind GetKind() const { return mKind; }

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return true;  // Every IRStmt is an instance of this class.
    }

    /// Check whether this is a statement that can be completely compiled.
    /// Returns false Until the IR has been partitioned (see XfPartition).
    bool CanCompile() const { return mCanCompile; }

    /// Set the "can compile" flag of this statement.  Called from
    /// XfPartition.
    void SetCanCompile() { mCanCompile = true; }

    /// Get the set of free variables attached to this statement by
    /// XfFreeVars.  Returns NULL unless this is the root of a compilable
    /// partition.
    const IRVarSet* GetFreeVars() const { return mFreeVars; }

    /// Give ownership of any free variable set to the caller.
    IRVarSet* TakeFreeVars() {
        IRVarSet* freeVars = mFreeVars;
        mFreeVars = NULL;
        return freeVars;
    }

    /// Attach the given set of free variables to this statement, taking
    /// ownership.
    void SetFreeVars(IRVarSet* freeVars) { 
        assert(mFreeVars == NULL && "Free variables leaked");
        mFreeVars = freeVars; 
    }

    /// The partition info analysis (XfPartitionInfo) stores the number of IR
    /// instructions at the root of each compilable partition.  -1 is returned
    /// if the number is unknown.
    int GetNumInsts() const { return mNumInsts; }

    /// Set the number of IR instructions in this statement.  Called from
    /// XfPartitionInfo.
    void SetNumInsts(int numInsts) { mNumInsts = numInsts; }

    /// Print this statement with the specified indentation.  Braces around
    /// the outermost block (if any) can be optionally suppressed.
    void Write(std::ostream& out, unsigned int indent = 0, 
               bool noBraces = false) const;

    /// Print this statement as an expression (possibly comma-separated) with
    /// the specified destination.
    void WriteInit(std::ostream& out, const IRVar* dest,
                   unsigned int indent) const;

protected:
    IRStmtKind mKind;
    IRPos mPos;
    IRVarSet* mFreeVars;        // Only at root of partition.
    int mNumInsts;              // Only at root of partition (-1 if unknown)
    bool mCanCompile;
};

/// Print a statement
std::ostream& operator<<(std::ostream& out, const IRStmt& stmt);

#endif // ndef IR_STMT_H
