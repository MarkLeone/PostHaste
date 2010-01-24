// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "xf/XfRaise.h"
#include "ir/IRShader.h"
#include "ir/IRStmts.h"
#include "ir/IRTypes.h"
#include "ir/IRValues.h"
#include "ops/OpInfo.h"
#include "slo/SloShader.h"
#include "util/UtCast.h"
#include "util/UtDelete.h"
#include "util/UtLog.h"
#include "util/UtTokenFactory.h"
#include <stack>

IRShader* 
XfRaise(const SloShader& slo, UtLog* log)
{
    return XfRaiseImpl(slo, log).RaiseShader();
}


XfRaiseImpl::XfRaiseImpl(const SloShader& slo, UtLog* log) :
    mSlo(&slo),
    mLog(log),
    mTypes(new IRTypes),
    mSymbols(new IRValues),
    mParams(new IRShaderParams),
    mConstants(new IRConsts),
    mLocals(new IRLocalVars),
    mGlobals(new IRGlobalVars),
    mSymValues(slo.mSymbols.size(), NULL),
    mGlobalNg(NULL)
{
    // Reserve storage for the bigger containers.  Doesn't have to be
    // perfectly accurate.
    mConstants->reserve(mSlo->mStrings.size() + mSlo->mConstants.size());
    mLocals->reserve(mSlo->mSymbols.size());
}

XfRaiseImpl::~XfRaiseImpl()
{
    // When testing we don't always construct an entire shader, so the various
    // IR containers might be non-NULL.
    UtDeleteSeq(mParams);
    UtDeleteSeq(mConstants);
    UtDeleteSeq(mLocals);
    UtDeleteSeq(mGlobals);
    delete mTypes;
}

// ---------- Shaders ----------

static void
CheckSloInvariants(const SloShader& slo)
{
    if (slo.mHeader.mVersion >= 4.2) {
        assert(slo.mInfo.mNumInits == slo.mInfo.mNumInits2 &&
               slo.mInfo.mNumInits == slo.mInfo.mBeginPC &&
               "Expected number of initializers to be same as beginPC");
        assert(slo.mInfo.mOptUnknown == 1 && 
               "Unexpected value for unknown info field");
    }
}


IRShader*
XfRaiseImpl::RaiseShader()
{
    // Check suspected invariants.
    CheckSloInvariants(*mSlo);

    // Convert symbols.
    for (size_t symNum = 0; symNum < mSlo->mSymbols.size(); ++symNum) {
        RaiseSym(mSlo->mSymbols[symNum], symNum);
    }

    // Raise shader parameter initializers.  This must be done in a second
    // pass because they might refer to constants later in the symbol table.
    for (size_t symNum = 0; symNum < mSlo->mSymbols.size(); ++symNum) {
        const SloSymbol sym = mSlo->mSymbols[symNum];
        if (sym.mStorage == kSloInput || sym.mStorage == kSloOutput) {
            IRShaderParam* param = 
                UtStaticCast<IRShaderParam*>(mSymValues[symNum]);
            param->SetInitStmt(RaiseInitializer(sym));
        }
    }

    // Raise the instructions, starting at the start PC.
    PC begin = mSlo->mInfo.mBeginPC;
    PC end = static_cast<PC>(mSlo->mInsts.size());
    IRStmt* body = RaiseSeq(begin, end);

    return ConstructShader(body);
}

IRShader*
XfRaiseImpl::ConstructShader(IRStmt* body)
{
    const char* name = mSlo->mInfo.mName.c_str();
    IRShader* shader = 
        new IRShader(name, mSlo->mInfo.mType, mTypes, mParams, mConstants, 
                     mLocals, mGlobals, body);

    // The symbol list includes the params, constants, locals, and globals.
    // It helps preserve the original symbol order when lowered back to SLO.
    shader->mSymbols = mSymbols;

    // The shader takes ownership of the various IR containers, so they must
    // be deleted after construction.
    mTypes = NULL;
    mSymbols = NULL;
    mParams = NULL;
    mConstants = NULL;
    mLocals = NULL;
    mGlobals = NULL;
    return shader;
}

// ---------- Detail and Types ----------

IRDetail
XfRaiseImpl::RaiseDetail(SloDetail detail) const
{
    switch (detail) {
      case kSloDetailNone: return kIRDetailNone;
      case kSloVarying: return kIRVarying;
      case kSloUniform: return kIRUniform;
      case kSloRareUniform: return kIRUniform;
    }
    mLog->Write(kUtError, "Unsupported SLO detail (%i)", detail);
    return kIRDetailNone;
}

IRTypeKind
XfRaiseImpl::RaiseType(SloType sloType) const
{
    switch (sloType) {
      case kSloBool: return kIRBoolTy;
      case kSloFloat: return kIRFloatTy;
      case kSloPoint: return kIRPointTy;
      case kSloVector: return kIRVectorTy;
      case kSloNormal: return kIRNormalTy;
      case kSloColor: return kIRColorTy;
      case kSloMatrix: return kIRMatrixTy;
      case kSloString: return kIRStringTy;
      case kSloShader: return kIRShaderTy;
      case kSloVoid: return kIRVoidTy;

      case kSloLabel:
      case kSloCond:
      case kSloTypeNone:
          assert(false && "Invalid SLO type kind");
          return kIRVoidTy;
    }    
    mLog->Write(kUtError, "Unsupported SLO type kind (%i)", sloType);
    return kIRVoidTy;
}

const IRType*
XfRaiseImpl::RaiseType(const SloSymbol& sym) const
{
    IRTypeKind kind = RaiseType(sym.mType);
    const IRType* type = mTypes->Get(kind);
    if (sym.IsArray())
        type = mTypes->GetArrayType(type, sym.mLength);
    return type;
}

// ---------- Symbols ----------

void
XfRaiseImpl::RaiseSym(const SloSymbol& sym, size_t symNum)
{
    // It seems that space names other than "rgb" and "current" never appear
    // in newer shaders.
    assert((mSlo->mHeader.mVersion < 4.2 ||
            sym.mSpace == -1 ||
            mSlo->mStrings[sym.mSpace] == "rgb" ||
            mSlo->mStrings[sym.mSpace] == "current") &&
           "Unexpected space for SLO symbol");

    switch (sym.mStorage) {
      case kSloConstant: {
          IRConst* constant = RaiseConst(sym);
          if (constant != NULL) {
              mConstants->push_back(constant);
              mSymbols->push_back(constant);
              // Save original data offset info to improve layout when lowered
              // back to SLO.
              constant->mOrigDataOffset = sym.mOffset;
          }
          mSymValues[symNum] = constant;
          return;
      }
      case kSloLocal:
      case kSloTemp: {
          IRLocalVar* var = RaiseLocal(sym);
          mLocals->push_back(var);
          mSymbols->push_back(var);
          mSymValues[symNum] = var;
          return;
      }
      case kSloGlobal: {
          IRGlobalVar* var = RaiseGlobal(sym);
          mGlobals->push_back(var);
          mSymbols->push_back(var);
          mSymValues[symNum] = var;
          // Keep track of Ng, which is added to faceforward() calls.
          if (!strcmp(var->GetFullName(), "Ng"))
              mGlobalNg = var;
          return;
      }
      case kSloInput: 
      case kSloOutput: {
          IRShaderParam* var = RaiseShaderParam(sym, symNum);
          mParams->push_back(var);
          mSymbols->push_back(var);
          mSymValues[symNum] = var;
          return;
      }
          
      case kSloCode:
      case kSloCondTemp:
          // These are not explicitly represented in IR.
          return;

      case kSloStorageNone:
          assert(false && "Invalid SLO symbol storage class");
          return;
    }
    mLog->Write(kUtError, "Unsupported SLO symbol storage class (%i)",
                sym.mStorage);
}

// ---------- Constants ---------- 

IRConst* 
XfRaiseImpl::RaiseConst(const SloSymbol& sym)
{
    assert(sym.mSpace == -1 && "Unexpected space specifier on a constant");

    const char* name = sym.mName.c_str();
    const IRType* type = RaiseType(sym);
    switch (sym.mType) {
      case kSloFloat:
      case kSloColor:
      case kSloNormal:
      case kSloPoint:
      case kSloVector:
      case kSloMatrix: {
          // Note that UtVector handles index range checking.
          const float* data = &mSlo->mConstants[sym.mOffset];
          if (sym.IsArray()) {
              const IRArrayType* arrayTy = 
                  UtStaticCast<const IRArrayType*>(type);
              return new IRNumArrayConst(data, arrayTy, name);
          }
          else {
              return new IRNumConst(data, type, name);
          }
      }

      case kSloString: {
          if (sym.IsArray()) {
              const IRArrayType* arrayTy = 
                  UtStaticCast<const IRArrayType*>(type);
              const std::string* first = &mSlo->mStrings[sym.mOffset];
              return new IRStringArrayConst(first, arrayTy, name);
          }
          else {
              const char* str = mSlo->mStrings[sym.mOffset].c_str();
              IRStringConst* string = new IRStringConst(str, name);

              // We use a map from string index to IR string constant for
              // filename strings in instruction debug info, which refer to
              // the string index, not the symbol index.
              mStringIndexMap[sym.mOffset] = string;
              return string;
          }
      }

      case kSloBool:
          assert(false && "Unexpected boolean constant");
          return NULL;

      case kSloLabel:
      case kSloCond:
          // These are not explicitly represented in IR.
          return NULL;

      case kSloShader:
      case kSloVoid:
      case kSloTypeNone:
          assert(false && "Invalid SLO constant type");
          return NULL;
    }
    mLog->Write(kUtError, "Unsupported SLO constant type (%i)", sym.mType);
    return NULL;
}

// ---------- Variables ----------

// Get the space for a symbol, or NULL if none was specified.
const char*
XfRaiseImpl::GetSpace(const SloSymbol& sym) const
{
    if (sym.mSpace == -1)
        return NULL;
    else
        return mSlo->mStrings[sym.mSpace].c_str();
}

IRLocalVar* 
XfRaiseImpl::RaiseLocal(const SloSymbol& sym) const
{
    // We don't retain the offset, so it had better be zero.
    assert(sym.mOffset == 0 && "Local sym offset is non-zero");
    return new IRLocalVar(sym.mName.c_str(), RaiseType(sym),
                          RaiseDetail(sym.mDetail), GetSpace(sym));
}

IRGlobalVar* 
XfRaiseImpl::RaiseGlobal(const SloSymbol& sym) const
{
    // We don't retain the offset, so it had better be zero.
    assert(sym.mOffset == 0 && "Global sym offset is non-zero");
    return new IRGlobalVar(sym.mName.c_str(), RaiseType(sym),
                           RaiseDetail(sym.mDetail), GetSpace(sym),
                           sym.IsWritten());
}

IRShaderParam* 
XfRaiseImpl::RaiseShaderParam(const SloSymbol& sym, size_t symNum) 
{
    // The offset records its index in the symbol table (off by one in older
    // shaders), so it's not explicitly represented in IR.
    assert((sym.mOffset == (int) symNum || sym.mOffset == (int) symNum+1) &&
           "Unexpected offset in SLO symbol for shader parameter");
    bool isOutput = sym.mStorage == kSloOutput;
    return new IRShaderParam(sym.mName.c_str(), RaiseType(sym),
                             RaiseDetail(sym.mDetail), GetSpace(sym), 
                             isOutput, NULL);
}

IRStmt*
XfRaiseImpl::RaiseInitializer(const SloSymbol& sym)
{
    if (sym.mInitPC == 0) 
        return new IRSeq();
    else {
        // Note that the initPC is off by one (zero indicates no initializer).
        int initPC = sym.mInitPC - 1;
        assert(sym.mNumInsts != 0 && "Expected non-empty initialization block");
        return RaiseSeq(initPC, initPC + sym.mNumInsts);
    }
}

// ---------- Instructions ----------

// Raise a sequence of statements up to the specified end PC.
IRStmt*
XfRaiseImpl::RaiseSeq(PC begin, PC end)
{
    IRStmt* seq = RaiseSeq(begin, end, NULL); // sets begin
    assert(begin == end && "Mismatched enterproc instructions");
    return seq;
}

// Raise a sequence of statements, possibly up to the specified end PC.
// The sequence ends if an "exitproc" is encountered.  The next PC is returned
// as a result parameter.
IRStmt*
XfRaiseImpl::RaiseSeq(PC& begin, PC end, IRStringConst* funcName)
{
    IRStmts* stmts = new IRStmts;
    if (begin >= end)
        return new IRSeq(stmts, funcName);

    PC pc = begin;
    while (pc < end) {
        const SloInst& inst = mSlo->mInsts[pc];
        if (EndsSeq(inst)) {
            ++pc;
            break;
        }
        switch (inst.mOpcode) {
          case kOpcode_If: {
              stmts->push_back(RaiseIf(inst, pc)); // sets pc
              assert(pc <= end && "PC out of bounds after 'if'");
              break;
          }
          case kOpcode_For: 
          case kOpcode_While: {
              stmts->push_back(RaiseFor(inst, pc)); // sets pc
              assert(pc <= end && "PC out of bounds after 'for'");
              break;
          }
          case kOpcode_Illuminance:
          case kOpcode_Illuminate:
          case kOpcode_Solar: {
              stmts->push_back(RaiseIllumLoop(inst, pc)); // sets pc
              assert(pc <= end && "PC out of bounds after 'illum'");
              break;
          }
          case kOpcode_Gather: {
              stmts->push_back(RaiseGather(inst, pc)); // sets pc
              assert(pc <= end && "PC out of bounds after 'gather'");
              break;
          }

          case kOpcode_Break:
          case kOpcode_Continue:
          case kOpcode_Return: {
              stmts->push_back(RaiseControlStmt(inst, pc)); // sets pc.
              assert(pc <= end && "PC out of bounds after control stmt");
              break;
          }

          case kOpcode_EnterProc: {
              IRValue* argVal = GetArgVal(0, inst);
              IRStringConst* funcName = UtStaticCast<IRStringConst*>(argVal);
              assert(funcName->Get()[0] != '\0' && "Unexpected exitproc");

              // Raise instructions up to a matching exitproc instruction.
              ++pc;
              IRStmt* stmt = RaiseSeq(pc, end, funcName); // sets pc
              assert(pc <= end && "PC out of bounds after enterproc");

              // Store the enterproc position in the resulting sequence.
              IRSeq* seq = UtStaticCast<IRSeq*>(stmt);
              seq->SetPos(GetPos(inst));
              stmts->push_back(seq);
              break;
          }
          case kOpcode_FuncCall: {
              stmts->push_back(RaiseFuncCall(inst, pc)); // sets pc
              assert(pc <= end && "PC out of bounds after 'funccall'");
              break;
          }
          case kOpcode_CallDSO: {
              stmts->push_back(RaisePluginCall(inst, pc)); // sets pc
              assert(pc <= end && "PC out of bounds after 'calldso'");
              break;
          }
          default: {
              // Raise instructions until the end of a basic block is found.
              IRBlock* block = RaiseBlock(pc, end); // sets pc.
              assert(pc <= end && "PC out of bounds after block");
              if (block->GetInsts().empty())
                  delete block;
              else
                  stmts->push_back(block);
          }
        }
    }
    // Return the PC of the next instruction (which might be the end PC).
    begin = pc;

    // Return either a single statement or a sequence.
    if (stmts->size() == 1 && funcName == NULL) {
        IRStmt* stmt = stmts->front();
        delete stmts;
        return stmt;
    }
    else {
        return new IRSeq(stmts, funcName);
    }
}

// A basic block ends when a structured statement (e.g. if, for), a plugin
// call, or an enterproc instruction is encountered.
bool
XfRaiseImpl::StartsBlock(const SloInst& inst) const
{
    switch (inst.mOpcode) {
      case kOpcode_EnterProc:
      case kOpcode_For:
      case kOpcode_FuncCall:
      case kOpcode_Gather:
      case kOpcode_If:
      case kOpcode_Illuminance:
      case kOpcode_Illuminate:
      case kOpcode_CallDSO:
      case kOpcode_Solar:
      case kOpcode_While:
          return true;
      default:
          return false;
    }
}

// A sequence ends when an "exitproc" is encountered (which is represented as
// an "enterproc" with an empty argument).
bool
XfRaiseImpl::EndsSeq(const SloInst& inst) const
{
    if (inst.mOpcode == kOpcode_EnterProc) {
        IRValue* argVal = GetArgVal(0, inst);
        IRStringConst* funcName = UtStaticCast<IRStringConst*>(argVal);
        return *funcName->Get() == '\0';
    }
    else 
        return false;
}

// Raise a basic block, possibly up to the specified end PC.  The end PC is
// returned as a result parameter.
IRBlock*
XfRaiseImpl::RaiseBlock(PC& begin, PC end)
{
    assert(begin <= end && "Invalid PC range");
    IRInsts* insts = new IRInsts;
    PC pc = begin;
    for (; pc < end; ++pc) {
        const SloInst& inst = mSlo->mInsts[pc];
        if (StartsBlock(inst)) {
            break;
        }
        insts->push_back(RaiseInst(inst, pc));
    }
    // Return the PC of the next instruction (which might be the end PC).
    begin = pc;
    return new IRBlock(insts);
}

const IRStringConst*
XfRaiseImpl::LookupString(StringIndex stringIndex)
{
    StringIndexMap::const_iterator it = mStringIndexMap.find(stringIndex);
    assert(it != mStringIndexMap.end() && "String index not found");
    return it->second;
}

IRPos
XfRaiseImpl::GetPos(const SloInst& inst)
{
    if (inst.mFilenameIndex == -1) 
        return IRPos();
    else {
        const IRStringConst* filename = LookupString(inst.mFilenameIndex);
        return IRPos(filename, inst.mLineNum);
    }
}

const SloSymbol&
XfRaiseImpl::GetArgSym(SymIndex i, const SloInst& inst) const
{
    SymIndex symIndex = mSlo->mArgs[inst.mFirstArg + i];
    return mSlo->mSymbols[symIndex];
}

const SloSymbol&
XfRaiseImpl::GetLabelArg(SymIndex i, const SloInst& inst) const
{
    const SloSymbol& sym = GetArgSym(i, inst);
    assert(sym.mType == kSloLabel && sym.mStorage == kSloCode &&
           "Expected a label");
    return sym;
}

IRValue*
XfRaiseImpl::GetArgVal(SymIndex i, const SloInst& inst) const
{
    assert(i < static_cast<SymIndex>(inst.mNumArgs) &&
           "Argument number is out of bounds");
    SymIndex symIndex = mSlo->mArgs[inst.mFirstArg + i];
    IRValue* value = mSymValues[symIndex];
    if (value == NULL) {
        mLog->Write(kUtInternal, 
                    "Symbol was not converted: %s (in %s)", 
                    GetArgSym(i, inst).mName.c_str(), inst.mName);
        assert(false && "Unconverted symbol");
    }
    return value;
}

// If the first label denotes a non-empty block, return its end PC.
// Otherwise return the specified PC.
static XfRaiseImpl::PC
EndPC(const SloSymbol& label, XfRaiseImpl::PC pc)
{
    assert(label.mType == kSloLabel && label.mStorage == kSloCode &&
           "Invalid label");
    return (label.mPc1 < label.mPc2) ? label.mPc2 : pc;
        
}

// Raise an individual instruction.  The given PC is used only for diagnostics.
IRInst* 
XfRaiseImpl::RaiseInst(const SloInst& inst, PC pc)
{
    // Get the result, if any.
    IRVar* result = NULL;
    int firstArg = 0;
    if (inst.mNumArgs > 0 && !OpInfo::IsVoid(inst.mOpcode)) {
        IRValue* resultArg = GetArgVal(0, inst);
        result = UtStaticCast<IRVar*>(resultArg);
        ++firstArg;
    }

    // Get the arguments.
    IRValues args;
    for (int i = firstArg; i < inst.mNumArgs; ++i) {
        const SloSymbol& sym = GetArgSym(i, inst);

        // We should never see labels or conditional temporaries, since those
        // can't be arguments to ordinary instructions.
        if (sym.mType == kSloLabel || sym.mType == kSloCond) {
            mLog->Write(kUtInternal, "Unhandled argument kind in %s (pc %i)\n",
                        inst.mName, pc);
            assert(false && "Unhandled argument kind");
            continue;
        }

        args.push_back(GetArgVal(i, inst));
    }

    // Special case: if it's a faceforward() call with only two arguments, add
    // an "Ng" argument.
    if (inst.mOpcode == kOpcode_FaceForward && args.size() == 2) {
        // Create Ng if necessary.
        if (mGlobalNg == NULL) {
            mGlobalNg = new IRGlobalVar("Ng", mTypes->GetNormalTy(),
                                        kIRVarying, "", false);
            mGlobals->push_back(mGlobalNg);
            mSymbols->push_back(mGlobalNg);
        }
        args.push_back(mGlobalNg);
    }

    if (inst.mOpcode == kOpcode_Unknown)
        return new IRUnknownInst(inst.mName, result, args, GetPos(inst));
    else
        return new IRBasicInst(inst.mOpcode, result, args, GetPos(inst));
}

IRStmt* 
XfRaiseImpl::RaiseIf(const SloInst& inst, PC& pc)
{
    // Arguments: condSym, condLabel, thenLabel, elseLabel, condTemp
    assert(inst.mNumArgs == 5 && "Expected 5 arguments in 'if' instruction");
    IRValue* condVal = GetArgVal(0, inst);
    const SloSymbol& condLabel = GetLabelArg(1, inst);
    const SloSymbol& thenLabel = GetLabelArg(2, inst);
    const SloSymbol& elseLabel = GetLabelArg(3, inst);

    IRStmt* condStmt = RaiseSeq(condLabel.mPc1, condLabel.mPc2);
    IRStmt* thenStmt = RaiseSeq(thenLabel.mPc1, thenLabel.mPc2);
    IRStmt* elseStmt = RaiseSeq(elseLabel.mPc1, elseLabel.mPc2);

    // The next PC depends on whether the branches were empty.
    pc = EndPC(elseLabel, EndPC(thenLabel, EndPC(condLabel, pc+1)));

    IRStmt* ifStmt = new IRIfStmt(condVal, thenStmt, elseStmt, GetPos(inst));
    if (condStmt->IsEmpty()) {
        delete condStmt;
        return ifStmt;
    }
    else {
        IRStmts* stmts = new IRStmts();
        stmts->push_back(condStmt);
        stmts->push_back(ifStmt);
        return new IRSeq(stmts);
    }
    
}

IRStmt* 
XfRaiseImpl::RaiseFor(const SloInst& inst, PC& pc)
{
    // Arguments: condSym, condLabel, bodyLabel, iterateLabel, condTemp
    // followed by an optional marker if the body has break/continue.
    assert(inst.mNumArgs == 5 || inst.mNumArgs == 6 && "Malformed 'for' loop");
    bool hasBreak = inst.mNumArgs >= 6;
    IRValue* condVal = GetArgVal(0, inst);
    const SloSymbol& condLabel = GetLabelArg(1, inst);
    const SloSymbol& bodyLabel = GetLabelArg(2, inst);
    const SloSymbol& iterateLabel = GetLabelArg(3, inst);
    IRValue* markerVal = hasBreak ? GetArgVal(5, inst) : NULL;
    IRLocalVar* markerVar =
        markerVal ? UtStaticCast<IRLocalVar*>(markerVal) : NULL;

    // Before raising the body, create a skeletal loop and associate the marker
    // variable with it.  A break/continue instruction that references the
    // marker will be raised to a statement that points to the loop.
    IRForLoop* forLoop = new IRForLoop(NULL, condVal, NULL, NULL, GetPos(inst));
    if (markerVar) {
        mMarkedStmts.push_back(MarkedStmt(markerVar, forLoop));
        forLoop->mMarker = markerVar;
    }

    // Raise the conditional evaluation statement, loop body, and iteration
    // statement.
    forLoop->SetCondStmt(RaiseSeq(condLabel.mPc1, condLabel.mPc2));
    forLoop->SetBody(RaiseSeq(bodyLabel.mPc1, bodyLabel.mPc2));
    forLoop->SetIterateStmt(RaiseSeq(iterateLabel.mPc1, iterateLabel.mPc2));

    // Pop the marker variable association.  The marker is not referenced
    // outside the loop.
    if (markerVar)
        mMarkedStmts.pop_back();

    // The next PC depends on whether the various blocks were empty.
    pc = EndPC(iterateLabel, EndPC(bodyLabel, EndPC(condLabel, pc+1)));
    return forLoop;
}

// Raise an illuminance/illuminate loop
IRStmt* 
XfRaiseImpl::RaiseIllumLoop(const SloInst& inst, PC& pc)
{
    // Arguments: boolTemp, condTemp, category, bodyLabel, args...
    assert(inst.mNumArgs >= 4 && "Malformed 'illum' loop");

    // Unspecified category is represented a numeric constant.
    // (not sure why it's not always zero).
    IRValue* category = GetArgVal(2, inst);
    if (UtCast<IRNumConst*>(category))
        category = NULL;
    
    IRValues args;
    for (int i = 4; i < inst.mNumArgs; ++i)
        args.push_back(GetArgVal(i, inst));

    const SloSymbol& bodyLabel = GetLabelArg(3, inst);
    IRStmt* bodyStmt = RaiseSeq(bodyLabel.mPc1, bodyLabel.mPc2);

    // The next PC depends on whether the body was empty.
    pc = EndPC(bodyLabel, pc+1);

    if (inst.mOpcode == kOpcode_Illuminance)
        return new IRIlluminanceLoop(category, args, bodyStmt, GetPos(inst));
    else {
        assert((inst.mOpcode == kOpcode_Illuminate ||
                inst.mOpcode == kOpcode_Solar) && 
               "Expected illuminate/solar");
        bool isSolar = inst.mOpcode == kOpcode_Solar;
        return new IRIlluminateStmt(isSolar, args, bodyStmt, GetPos(inst));
    }        
}

IRStmt* 
XfRaiseImpl::RaiseGather(const SloInst& inst, PC& pc)
{
    // Arguments: bodyLabel, elseLabel, markerTemp, category, args...
    assert(inst.mNumArgs >= 4 && "Malformed 'gather' loop");

    IRValue* markerVal = GetArgVal(2, inst);
    IRLocalVar* markerVar = UtStaticCast<IRLocalVar*>(markerVal);
    IRValue* category = GetArgVal(3, inst);
    IRValues args;
    for (int i = 4; i < inst.mNumArgs; ++i)
        args.push_back(GetArgVal(i, inst));

    // Before raising the body, create a skeletal loop and associate the marker
    // variable with it.  A break/continue instruction that references the
    // marker will be raised to a statement that points to the loop.
    IRGatherLoop* gather = 
        new IRGatherLoop(category, args, NULL, NULL, GetPos(inst));
    gather->mMarker = markerVar;
    mMarkedStmts.push_back(MarkedStmt(markerVar, gather));

    const SloSymbol& bodyLabel = GetLabelArg(0, inst);
    const SloSymbol& elseLabel = GetLabelArg(1, inst);
    gather->SetBody(RaiseSeq(bodyLabel.mPc1, bodyLabel.mPc2));
    gather->SetElseStmt(RaiseSeq(elseLabel.mPc1, elseLabel.mPc2));

    // Pop the marker variable association.  The marker is not referenced
    // outside the loop.
    mMarkedStmts.pop_back();

    // The next PC depends on whether the 'else' or body was empty.
    pc = EndPC(elseLabel, EndPC(bodyLabel, pc+1));
    return gather;
}

// Raise a "_funccall" block, which arises when an inlined function call
// contains a nested return.  The result is a "catch" statement that wraps the
// function body, to which the return statements point.
IRStmt* 
XfRaiseImpl::RaiseFuncCall(const SloInst& inst, PC& pc)
{
    // Arguments: blockLabel, markerTemp
    assert(inst.mNumArgs == 2 &&
           "Expected 2 arguments in 'funccall' instruction");
    const SloSymbol& blockLabel = GetLabelArg(0, inst);
    IRValue* markerVal = GetArgVal(1, inst);
    IRLocalVar* markerVar = UtStaticCast<IRLocalVar*>(markerVal);

    // Before raising the body, create a skeletal "catch" statement and
    // associate the marker variable with it.  A return instruction that
    // references the marker will be raised to a statement that points to the
    // the catch statement.
    IRCatchStmt* catchStmt = new IRCatchStmt(NULL, GetPos(inst));
    catchStmt->mMarker = markerVar;
    mMarkedStmts.push_back(MarkedStmt(markerVar, catchStmt));

    // Raise the body of the funccall block.
    catchStmt->SetBody(RaiseSeq(blockLabel.mPc1, blockLabel.mPc2));

    // Pop the marker variable association.  The marker is not referenced
    // outside the loop.
    mMarkedStmts.pop_back();

    // The next PC depends on whether the block was empty.
    pc = EndPC(blockLabel, pc+1);
    return catchStmt;
}

// Raise a "calldso" instruction to an IRPluginCall statement.
IRStmt* 
XfRaiseImpl::RaisePluginCall(const SloInst& inst, PC& pc)
{
    ++pc;

    // Arguments: result, funcName, funcName, pluginName, prototype, args...
    assert(inst.mNumArgs >= 5 &&
           "Expected 2 arguments in 'funccall' instruction");

    // The call result must be a variable.  It's always present, even if the
    // function is void.
    IRVar* result = UtStaticCast<IRVar*>(GetArgVal(0, inst));

    // The function name must be a string constant.  Not sure why it appears
    // twice.  We only represent one copy, so we assert that they're the same.
    IRStringConst* funcName = UtStaticCast<IRStringConst*>(GetArgVal(1, inst));
#ifndef NDEBUG
    IRStringConst* funcName2 = UtStaticCast<IRStringConst*>(GetArgVal(2, inst));
    assert(!strcmp(funcName->Get(), funcName2->Get()) &&
           "DSO function names differ");
#endif

    // Get the plugin (DSO) name and prototype.
    IRStringConst* plugin = UtStaticCast<IRStringConst*>(GetArgVal(3, inst));
    IRStringConst* prototype = UtStaticCast<IRStringConst*>(GetArgVal(4, inst));
    
    // Get the function call arguments.
    IRValues args;
    for (int i = 5; i < inst.mNumArgs; ++i)
        args.push_back(GetArgVal(i, inst));

    return new IRPluginCall(result, args, funcName, plugin, prototype,
                            GetPos(inst));
}

IRStmt*
XfRaiseImpl::RaiseControlStmt(const SloInst& inst, PC& pc)
{
    ++pc;

    // The control instruction specifies a "marker" variable
    // that is associated with a loop or "catch" statement.
    assert(inst.mNumArgs >= 1 && "Malformed control statement");
    IRValue* markerVal = GetArgVal(0, inst);
    IRLocalVar* markerVar = UtStaticCast<IRLocalVar*>(markerVal);
    IRStmt* associatedStmt = FindMarkedStmt(markerVar);

    // The new control statement points to the associated loop/catch stmt.
    return new IRControlStmt(inst.mOpcode, associatedStmt, GetPos(inst));
}

// Find the loop or "catch" statement associated with a marker variable.
IRStmt*
XfRaiseImpl::FindMarkedStmt(const IRLocalVar* var) const
{
    std::list<MarkedStmt>::const_reverse_iterator it;
    for (it = mMarkedStmts.rbegin(); it != mMarkedStmts.rend(); ++it)
        if (it->mMarker == var)
            return it->mStmt;
    assert(false && "Marker variable not found");
    return NULL;
}
