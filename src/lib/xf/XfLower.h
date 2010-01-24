// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef XF_LOWER_H
#define XF_LOWER_H

#include "slo/SloEnums.h"
#include "ir/IRDetail.h"
#include "ir/IRType.h"          // for IRTypeKind
#include "ir/IRTypedefs.h"
#include "ir/IRVisitor.h"
#include "ops/Opcode.h"
#include <map>
#include <string>

class IRConst;
class IRShader;
class SloShader;
class SloSymbol;
class UtLog;

/// Transform IR shader to an SLO shader.
SloShader* XfLower(const IRShader& shader, UtLog* log);

/// Implementation of IR to SLO lowering.  Methods are all public for testing.
class XfLowerImpl : public IRVisitor<XfLowerImpl> {
public:
    XfLowerImpl(UtLog* log);
    ~XfLowerImpl();
    typedef int PC;
    typedef int SymIndex;

    SloShader* LowerShader(const IRShader& shader);
    void SetSloInfo(const IRShader& shader, PC beginPC);
    SloShader* GetShader() const { return mSlo; }
    
    void LowerType(const IRType* ty, SloSymbol* sym) const;
    SloType LowerType(IRTypeKind irType) const;

    SymIndex NewSymbol(const char* name, SloStorage storage,
                       SloType type, SloDetail detail);
    SymIndex NewSymbol(const char* name, SloStorage storage, 
                       const IRType* type, SloDetail detail);
    SymIndex LowerSymbol(const IRValue* sym);
    SymIndex GetCondTemp();
    SymIndex GetEmptyString();
    SymIndex GetZero();

    static bool DataOffsetIsLess(IRConst* c1, IRConst* c2);
    SymIndex LowerConstant(const IRConst* constant);
    SymIndex TestLowerConstant(const IRConst* constant);
    void StoreConstants(IRConsts constants) const;
    SymIndex StoreConstant(const IRConst* constant) const;
    SymIndex StoreNumConstant(const float* data, const IRType* ty) const;
    std::string NewConstName(SloType type);
    std::string NewTempName();

    SymIndex GetSymIndex(const IRValue* value) const;
    SloSymbol* GetSymbol(const IRValue* value) const;
    PC LowerSymbols(const IRShader& ir);
    SymIndex LowerShaderParam(const IRShaderParam* var);
    PC LowerShaderParamInits(const IRShaderParams& params);
    SymIndex LowerGlobal(const IRGlobalVar* var);
    SymIndex LowerLocal(const IRLocalVar* var);

    typedef int ArgNum;
    ArgNum LowerArg(const IRValue* arg) const;
    ArgNum LowerArgs(const IRValues& arg) const;
    PC LowerInst(IRInst* inst, PC pc);

    SymIndex NewLabel(PC begin, PC end);

    PC LowerStmt(IRStmt* stmt, PC pc, SymIndex labelIndex=-1);
    PC Visit(IRBlock* block, PC pc);
    PC Visit(IRSeq* seq, PC pc);
    PC Visit(IRCatchStmt* stmt, PC pc);
    IRLocalVar* GetMarker(IRStmt* stmt) const;
    PC Visit(IRControlStmt* stmt, PC pc);
    PC Visit(IRIfStmt* stmt, PC pc);
    PC Visit(IRForLoop* stmt, PC pc);
    PC Visit(IRIlluminanceLoop* stmt, PC pc);
    PC Visit(IRIlluminateStmt* stmt, PC pc);
    PC Visit(IRGatherLoop* stmt, PC pc);
    PC LowerIllumStmt(IRSpecialForm* stmt, IRValue* category, PC pc);
    PC Visit(IRPluginCall* stmt, PC pc);

    ArgNum PushArgs(unsigned int numArgs, ...) const;
    void NewInst(PC pc, Opcode opcode, const char* name,
                 int numArgs, ArgNum firstArg, const IRPos& pos);
    void NewEnterProcInst(PC pc, SymIndex funcNameSymIndex, const IRPos& pos);

private:
    UtLog* mLog;
    SloShader* mSlo;
    unsigned int mNumAnonNumConsts;
    unsigned int mNumAnonStringConsts;
    unsigned int mNumAnonTemps;
    unsigned int mNumLabels;
    typedef std::map<const IRValue*, SymIndex> ValueSymbolMap;
    ValueSymbolMap mValueSymbols;
    SymIndex mCondTempIndex;
    SymIndex mEmptyStringIndex;
    SymIndex mZeroIndex;
};

#endif // ndef XF_LOWER_H
