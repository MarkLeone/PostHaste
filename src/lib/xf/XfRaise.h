// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef XF_RAISE_H
#define XF_RAISE_H

#include "ir/IRDetail.h"
#include "ir/IRPos.h"
#include "ir/IRType.h"         // for IRTypeKind
#include "ir/IRTypedefs.h"
#include "slo/SloEnums.h"      // for SloType
#include "util/UtVector.h"
#include <list>
#include <map>

class IRBlock;
class IRConst;
class IRGlobalVar;
class IRLocalVar;
class IRShader;
class IRShaderParam;
class IRStmt;
class IRTypes;
class IRValue;
class SloInst;
class SloShader;
class SloSymbol;
class UtLog;

/// Transform SLO shader to an IR shader.
IRShader* XfRaise(const SloShader& shader, UtLog* log);

/// Implementation of SLO to IR raising.  Methods are all public for testing.
class XfRaiseImpl {
public:
    XfRaiseImpl(const SloShader& slo, UtLog* log);
    ~XfRaiseImpl();

    IRShader* RaiseShader();
    IRShader* ConstructShader(IRStmt* body);

    IRDetail RaiseDetail(SloDetail detail) const;
    IRTypeKind RaiseType(SloType sloType) const;
    const IRType* RaiseType(const SloSymbol& sym) const;
    const char* GetSpace(const SloSymbol& sym) const;

    void RaiseSym(const SloSymbol& sym, size_t symNum);
    IRConst* RaiseConst(const SloSymbol& sym);
    IRLocalVar* RaiseLocal(const SloSymbol& sym) const;
    IRGlobalVar* RaiseGlobal(const SloSymbol& sym) const;
    IRShaderParam* RaiseShaderParam(const SloSymbol& sym, size_t symNum);
    IRStmt* RaiseInitializer(const SloSymbol& sym);

    typedef int SymIndex;
    typedef int StringIndex;
    const IRStringConst* LookupString(StringIndex stringIndex);
    IRPos GetPos(const SloInst& inst);
    const SloSymbol& GetArgSym(SymIndex i, const SloInst& inst) const;
    const SloSymbol& GetLabelArg(SymIndex i, const SloInst& inst) const;
    IRValue* GetArgVal(SymIndex i, const SloInst& inst) const;
    bool StartsBlock(const SloInst& inst) const;
    bool EndsSeq(const SloInst& inst) const;

    typedef unsigned int PC;
    IRStmt* RaiseSeq(PC begin, PC end);
    IRStmt* RaiseSeq(PC& begin, PC end, IRStringConst* funcName);
    IRBlock* RaiseBlock(PC& begin, PC end);
    IRInst* RaiseInst(const SloInst& inst, PC pc);
    IRStmt* RaiseIf(const SloInst& inst, PC& pc);
    IRStmt* RaiseFor(const SloInst& inst, PC& pc);
    IRStmt* RaiseIllumLoop(const SloInst& inst, PC& pc);
    IRStmt* RaiseGather(const SloInst& inst, PC& pc);
    IRStmt* RaiseFuncCall(const SloInst& inst, PC& pc);
    IRStmt* RaiseControlStmt(const SloInst& inst, PC& pc);
    IRStmt* RaisePluginCall(const SloInst& inst, PC& pc);
    IRStmt* FindMarkedStmt(const IRLocalVar* var) const;

private:
    const SloShader* mSlo;
    UtLog* mLog;
    IRTypes* mTypes;
    IRValues* mSymbols;
    IRShaderParams* mParams;
    IRConsts* mConstants;
    IRLocalVars* mLocals;
    IRGlobalVars* mGlobals;
    UtVector<IRValue*> mSymValues;
    IRGlobalVar* mGlobalNg;

    // We use a map from string index to IR string constant for filename
    // strings in instruction debug info, which refer to the string index,
    // not the symbol index.
    typedef std::map<StringIndex, const IRStringConst*> StringIndexMap;
    StringIndexMap mStringIndexMap;

    // "Marker" variables are associated with loops and "catch" statements.  
    // A break/continue/return instruction that references a marker is raised
    // to a statement that points to the corresponding loop or catch
    // statement.
    struct MarkedStmt {
        IRLocalVar* mMarker;
        IRStmt* mStmt;
        MarkedStmt(IRLocalVar* marker, IRStmt* stmt) : 
            mMarker(marker), mStmt(stmt) { }
    };
    std::list<MarkedStmt> mMarkedStmts;
};

#endif // ndef XF_RAISE_H

