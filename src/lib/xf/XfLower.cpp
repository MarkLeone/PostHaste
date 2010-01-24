// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.


#include "xf/XfLower.h"
#include "ir/IRShader.h"
#include "ir/IRTypes.h"
#include "ir/IRValues.h"
#include "slo/SloShader.h"
#include "util/UtCast.h"
#include "util/UtLog.h"
#include <algorithm>            // for std::sort
#include <sstream>
#include <stdarg.h>

// Transform IR shader to an SLO shader.
SloShader* 
XfLower(const IRShader& ir, UtLog* log)
{
    return XfLowerImpl(log).LowerShader(ir);
}

// Constructor
XfLowerImpl::XfLowerImpl(UtLog* log) :
    mLog(log),
    mSlo(new SloShader),
    mNumAnonNumConsts(0),
    mNumAnonStringConsts(0),
    mNumAnonTemps(0),
    mNumLabels(0),
    mCondTempIndex(-1),
    mEmptyStringIndex(-1),
    mZeroIndex(-1)
{
    // TODO: could reserve storage for strings and symbols?
}

XfLowerImpl::~XfLowerImpl()
{
    // When testing we don't always construct an entire shader, so the
    // destructor deletes it if it's unclaimed.
    delete mSlo;
}    

SloShader* 
XfLowerImpl::LowerShader(const IRShader& ir)
{
    // TODO: debug info strings.
    // Lower constants, parameters, locals, and globals.
    PC beginPC = LowerSymbols(ir);

    // Lower shader body.
    LowerStmt(ir.GetBody(), beginPC);
    
    // Set SLO header and info.
    SetSloInfo(ir, beginPC);

    // The caller owns the shader.  Clear our pointer to avoid deletion.
    SloShader* slo = mSlo;
    mSlo = NULL;
    return slo;
}

void
XfLowerImpl::SetSloInfo(const IRShader& ir, int beginPC)
{
    mSlo->mHeader = SloHeader(4.2f, 0, 0);
    mSlo->mInfo = 
        SloInfo(ir.GetName(),                   // name
                (int) mSlo->mSymbols.size(),    // numSyms
                (int) mSlo->mConstants.size(),  // numConstants
                (int) mSlo->mStrings.size(),    // numStrings
                0,                              // unknown3
                (int) mSlo->mInsts.size(),      // numInsts
                0,                              // unknown1
                beginPC,                        // numInits
                beginPC,                        // numInits2
                0,                              // unknown2
                beginPC,                        // beginPC
                ir.GetType(),                   // type
                1                               // optUnknown
                );
}


// ---------- Types and detail ----------

// Lower IR detail.
static SloDetail
LowerDetail(IRDetail detail)
{
    switch (detail) {
      case kIRDetailNone: return kSloDetailNone;
      case kIRVarying: return kSloVarying;
      case kIRUniform: return kSloUniform;
    }
    assert(false && "Unimplemented IR detail");
    return kSloDetailNone;
}

// Lower an IR type, storing the SloType enum and array length (if any) in the
// given symbol.
void
XfLowerImpl::LowerType(const IRType* ty, SloSymbol* sym) const
{
    // Get array length, if any.
    if (const IRArrayType* arrayTy = UtCast<const IRArrayType*>(ty)) {
        sym->mLength = arrayTy->GetLength();
        ty = arrayTy->GetElementType();
    }
    else
        sym->mLength = SloSymbol::kSloSymbolNoLength;

    // TODO: handle struct types here.

    sym->mType = LowerType(ty->GetKind());
}

SloType
XfLowerImpl::LowerType(IRTypeKind irType) const
{
    switch (irType) {
      case kIRVoidTy: return kSloVoid;
      case kIRBoolTy: return kSloBool;
      case kIRFloatTy: return kSloFloat;
      case kIRPointTy: return kSloPoint;
      case kIRVectorTy: return kSloVector;
      case kIRNormalTy: return kSloNormal;
      case kIRColorTy: return kSloColor;
      case kIRMatrixTy: return kSloMatrix;
      case kIRStringTy: return kSloString;
      case kIRShaderTy: return kSloShader;

      case kIRArrayTy:
          assert(false && "Cannot lower array type kind");
          return kSloTypeNone;
      case kIRStructTy:
          assert(false && "Struct types are not yet supported");
          return kSloTypeNone;
      case kIRNumTypeKinds:
          assert(false && "Invalid IR type kind");
          return kSloTypeNone;
    }    
    assert(false && "Unimplemented IR type kind");
    return kSloTypeNone;
}

// ---------- Constants ----------

// Lower a constant.  Creates a symbol and returns the symbol index.
// Sorting the constant data is performed in a separate pass.
int
XfLowerImpl::LowerConstant(const IRConst* constant)
{
    assert(constant->GetDetail() == kIRUniform && "Constant must be uniform");

    // Note that the name might be empty.  That's fixed below.
    int index = NewSymbol(constant->GetName(), kSloConstant,
                          constant->GetType(), kSloUniform);
    SloSymbol& sym = mSlo->mSymbols[index];

    // Fix empty name if necessary.
    if (sym.mName.empty())
        sym.mName = NewConstName(sym.mType);

    // Special case: keep track of the empty string constant if we find one,
    // so we can use it as an 'enterproc' argument.
    if (const IRStringConst* string = UtCast<const IRStringConst*>(constant)) {
        if (mEmptyStringIndex == -1 && string->Get()[0] == '\0')
            mEmptyStringIndex = index;
    }

    // Special case: keep track of the zero constant if we find one,
    // so we can use it as an 'illuminance' category placeholder.
    if (const IRNumConst* num = UtCast<const IRNumConst*>(constant)) {
        if (mZeroIndex == -1 && 
            num->GetType()->IsFloat() && num->GetFloat() == 0.0f)
            mZeroIndex = index;
    }
    return index;
}

// Test routine for LowerConstant().  Creates a symbol and stores the constant
// data.  Returns the symbol index.
int
XfLowerImpl::TestLowerConstant(const IRConst* constant)
{
    int index = LowerConstant(constant);
    SloSymbol& sym = mSlo->mSymbols[index];
    sym.mOffset = StoreConstant(constant);
    return index;
}

// Create a symbol and returns its index.  The caller is responsible for
// setting the array length and other fields (e.g. space, offset) if
// necessary.
XfLowerImpl::SymIndex
XfLowerImpl::NewSymbol(const char* name, SloStorage storage,
                       SloType type, SloDetail detail)
{
    int noLength = SloSymbol::kSloSymbolNoLength;
    SloSymbol sym(name,                 // name
                  type,                 // type
                  storage,              // storage
                  -1,                   // space
                  0,                    // offset
                  detail,               // detail
                  0, 0,                 // unknown1,2
                  0, 0,                 // write1,2
                  0, 0,                 // pc1,2
                  noLength              // length
                  );

    // Push the symbol and return its index.
    SymIndex index = (SymIndex) mSlo->mSymbols.size();
    mSlo->mSymbols.push_back(sym);
    return index;
}

// Create a symbol and returns its index.  The caller is responsible for
// setting the space and symbol offset if necessary.
XfLowerImpl::SymIndex
XfLowerImpl::NewSymbol(const char* name, SloStorage storage, 
                       const IRType* type, SloDetail detail)
{
    // Create symbol with placeholders for the tpe and length.
    SymIndex index = NewSymbol(name, storage, kSloTypeNone, detail);
    
    // Set the symbol type and length.
    LowerType(type, &mSlo->mSymbols[index]);
    return index;
}

// Get the canonical condition temporary.  Condition temporaries are obsolete,
// so we generate a canonical one on demand.
XfLowerImpl::SymIndex
XfLowerImpl::GetCondTemp()
{
    if (mCondTempIndex == -1)
        mCondTempIndex =
            NewSymbol("$Cond0", kSloCondTemp, kSloCond, kSloVarying);
    return mCondTempIndex;
}

// Get the index of the empty string constant.  A new string constant
// is created if necessary.
XfLowerImpl::SymIndex
XfLowerImpl::GetEmptyString()
{
    if (mEmptyStringIndex == -1) {
        int constIndex = (int) mSlo->mStrings.size();
        mSlo->mStrings.push_back("");
        std::string name = NewConstName(kSloString);
        mEmptyStringIndex =
            NewSymbol(name.c_str(), kSloConstant, kSloString, kSloUniform);
        SloSymbol* sym = &mSlo->mSymbols[mEmptyStringIndex];
        sym->mOffset = constIndex;
    }
    return mEmptyStringIndex;
}

// Get the index of the zero constant.  A new float constant
// is created if necessary.
XfLowerImpl::SymIndex
XfLowerImpl::GetZero()
{
    if (mZeroIndex == -1) {
        int constIndex = (int) mSlo->mStrings.size();
        mSlo->mConstants.push_back(0);
        std::string name = NewConstName(kSloFloat);
        mZeroIndex =
            NewSymbol(name.c_str(), kSloConstant, kSloFloat, kSloUniform);
        SloSymbol* sym = &mSlo->mSymbols[mZeroIndex];
        sym->mOffset = constIndex;
    }
    return mZeroIndex;
}

// Copy constant data from IR constants to SLO shader constants.
void
XfLowerImpl::StoreConstants(IRConsts constants) const
{
    // Sort the constants based on their original data offset so the data
    // layout will be comparable to the original SLO.
    std::sort(constants.begin(), constants.end(), DataOffsetIsLess);

    // Store the constant data using the sorted order.
    std::vector<IRConst*>::const_iterator it;
    for (it = constants.begin(); it != constants.end(); ++it) {
        IRConst* constant = *it;
        SymIndex index = GetSymIndex(constant);
        mSlo->mSymbols[index].mOffset = StoreConstant(constant);
    }
}

// Store constant data in the appopriate SLO constant block, returning its
// offset.
int
XfLowerImpl::StoreConstant(const IRConst* constant) const
{
    // TODO: reseve constant storage in advance to reduce cost of push_back.
    if (const IRNumConst* c = UtCast<const IRNumConst*>(constant))
        return StoreNumConstant(c->GetData(), c->GetType());
    else if (const IRNumArrayConst* c =
             UtCast<const IRNumArrayConst*>(constant))
        return StoreNumConstant(c->GetData(), c->GetType());
    else if (const IRStringConst* c = UtCast<const IRStringConst*>(constant)) {
        mSlo->mStrings.push_back(c->Get());
        return static_cast<int>(mSlo->mStrings.size()-1);
    }
    else if (const IRStringArrayConst* c = 
             UtCast<const IRStringArrayConst*>(constant)) {
        int index = static_cast<int>(mSlo->mStrings.size());
        for (unsigned int i = 0; i < c->GetLength(); ++i)
            mSlo->mStrings.push_back(c->GetElement(i));
        return index;
    }
    assert(false && "Unimplemented kind of IR constant");
    return 0;
}

// Store numeric constant data, returning its offset.
int 
XfLowerImpl::StoreNumConstant(const float* data, const IRType* ty) const
{
    UtVector<float>& pool = mSlo->mConstants;
    int index = static_cast<int>(pool.size());
    unsigned int size = ty->GetSize();
    mSlo->mConstants.insert(pool.end(), data, data+size);
    return index;
}

std::string 
XfLowerImpl::NewConstName(SloType type)
{
    std::stringstream name;
    if (type == kSloString)
        name << "$S_" << mNumAnonStringConsts++;
    else
        name << "$C_" << mNumAnonNumConsts++;
    return name.str();
}

std::string 
XfLowerImpl::NewTempName()
{
    std::stringstream name;
    name << "$T_" << mNumAnonTemps++;
    return name.str();
}

// ---------- Variables ----------

XfLowerImpl::SymIndex
XfLowerImpl::GetSymIndex(const IRValue* value) const
{
    ValueSymbolMap::const_iterator it = mValueSymbols.find(value);
    assert(it != mValueSymbols.end() && "IR value has no symbol");
    return it->second;
}

SloSymbol*
XfLowerImpl::GetSymbol(const IRValue* value) const
{
    return &mSlo->mSymbols[GetSymIndex(value)];
}

// Lower the symbols (parameters, constants, locals, and globals).  Populates
// mValueSymbols, which maps values to symbols. Returns the end PC from the
// last initializer, which will be the start PC of the shader.
XfLowerImpl::PC
XfLowerImpl::LowerSymbols(const IRShader& ir)
{
    // Lower the symbols, keeping track of their symbol indices.
    std::vector<IRValue*>::const_iterator it;
    for (it = ir.mSymbols->begin(); it != ir.mSymbols->end(); ++it) {
        const IRValue* val = *it;
        mValueSymbols[val] = LowerSymbol(val);
    }

    // Copy constant data from IR constants to SLO shader constants.
    StoreConstants(*ir.mConstants);

    // Lower shader parameter initializers, returning the end PC.
    return LowerShaderParamInits(*ir.mParams);
}

// Lower shader parameter initializers, returning the end PC.
XfLowerImpl::PC
XfLowerImpl::LowerShaderParamInits(const IRShaderParams& params)
{
    PC pc = 0;
    IRShaderParams::const_iterator it;
    for (it = params.begin(); it != params.end(); ++it) {
        IRShaderParam* param = *it;
        if (param->GetInitStmt()) {
            // Lower the parameter initializer, which might be empty.
            PC beginPC = pc;
            pc = LowerStmt(param->GetInitStmt(), pc);
            if (pc > beginPC) {
                // Update the parameter symbol's initializer info.  Note that
                // the initPC is off by one (0 denotes no initializer).
                SloSymbol* sym = GetSymbol(param);
                sym->mInitPC = beginPC + 1;
                sym->mNumInsts = pc - beginPC;
            }
        }
    }
    return pc;
}

// Order constants based on their original offset for sorting, making the
// output SLO easier to compare to the input.
bool 
XfLowerImpl::DataOffsetIsLess(IRConst* c1, IRConst* c2)
{
    return c1->mOrigDataOffset < c2->mOrigDataOffset;
}

// Lower a symbol, which must be an IRVar or IRConst.  Returns the symbol index.
// XXX use an IRVisitor here.
XfLowerImpl::SymIndex
XfLowerImpl::LowerSymbol(const IRValue* sym)
{
    // XXX symbol space is not lowered properly.
    if (const IRShaderParam* var = UtCast<const IRShaderParam*>(sym))
        return LowerShaderParam(var);
    if (const IRLocalVar* var = UtCast<const IRLocalVar*>(sym))
        return LowerLocal(var);
    if (const IRGlobalVar* var = UtCast<const IRGlobalVar*>(sym))
        return LowerGlobal(var);
    else if (const IRConst* c = UtCast<const IRConst*>(sym))
        return LowerConstant(c);
    else {
        assert(false && "Expected a variable or constant");
        return 0;
    }
}

// Lower a shader parameter definition, returning the index of its SLO symbol.
XfLowerImpl::SymIndex 
XfLowerImpl::LowerShaderParam(const IRShaderParam* var)
{
    SloStorage storage = var->IsOutput() ? kSloOutput : kSloInput;
    SymIndex index = NewSymbol(var->GetFullName(), storage, 
                              var->GetType(),
                              LowerDetail(var->GetDetail()));
    SloSymbol& sym = mSlo->mSymbols[index];    
    sym.mOffset = index;
    return index;
}

// Lower a global variable definition, returning the index of its SLO symbol.
XfLowerImpl::SymIndex
XfLowerImpl::LowerGlobal(const IRGlobalVar* var)
{
    SymIndex index = NewSymbol(var->GetFullName(), kSloGlobal, var->GetType(),
                               LowerDetail(var->GetDetail()));
    if (var->IsWritten()) {
        SloSymbol* sym = &mSlo->mSymbols[index];
        sym->mWrite1 = 1;
        sym->mWrite2 = 1;
    }
    return index;
}

// Lower a local variable definition, returning the index of its SLO symbol.
XfLowerImpl::SymIndex
XfLowerImpl::LowerLocal(const IRLocalVar* var)
{
    return NewSymbol(var->GetFullName(), kSloLocal, var->GetType(),
                     LowerDetail(var->GetDetail()));
}

// ---------- Instructions ----------

XfLowerImpl::PC
XfLowerImpl::LowerInst(IRInst* inst, PC pc)
{
    // Lower the arguments and result.
    const IRValues& args = inst->GetArgs();
    int numArgs = (int) args.size();
    ArgNum firstArg = -1;
    if (inst->GetResult() == NULL)
        firstArg = LowerArgs(args);
    else {
        ++numArgs;
        firstArg = LowerArg(inst->GetResult());
        LowerArgs(args);
    }

    // Construct the instruction.
    NewInst(pc, inst->GetOpcode(), inst->GetName(),
            numArgs, firstArg, inst->GetPos());
    return pc+1;
}

// Lower an instruction argument, pushing its symbol index on the SLO
// argument table and returning its index.
XfLowerImpl::ArgNum
XfLowerImpl::LowerArg(const IRValue* arg) const
{
    ArgNum index = (ArgNum) mSlo->mArgs.size();
    mSlo->mArgs.push_back(GetSymIndex(arg));
    return index;
}

// Lower a sequence of arguments, pushing their symbol indices on the SLO
// argument table.  Returns the index of the first argument, or -1 if there
// are no arguments.
XfLowerImpl::ArgNum
XfLowerImpl::LowerArgs(const IRValues& args) const
{
    if (args.empty())
        return -1;
    ArgNum firstArg = (ArgNum) mSlo->mArgs.size();
    IRValues::const_iterator it;
    for (it = args.begin(); it != args.end(); ++it)
        LowerArg(*it);
    return firstArg;
}

// ---------- Statements ----------

XfLowerImpl::SymIndex
XfLowerImpl::NewLabel(PC begin, PC end)
{
    std::stringstream name;
    name << "$Code" << mNumLabels++;
    
    // Not sure why the detail is varying.
    SymIndex index = NewSymbol(name.str().c_str(), 
                               kSloCode, kSloLabel, kSloVarying);
    SloSymbol* sym = &mSlo->mSymbols[index];
    sym->mPc1 = begin;
    sym->mPc2 = end;
    return index;
}

// Lower the given statement.  If a label was provided (not -1), it is updated
// with the resulting begin and end PCs.
XfLowerImpl::PC
XfLowerImpl::LowerStmt(IRStmt* stmt, PC beginPC, SymIndex labelIndex)
{
    PC endPC = Dispatch<PC>(stmt, beginPC);
    if (labelIndex != -1) {
        SloSymbol* sym = &mSlo->mSymbols[labelIndex];
        sym->mPc1 = beginPC;
        sym->mPc2 = endPC;
    }
    return endPC;
}

XfLowerImpl::PC
XfLowerImpl::Visit(IRBlock* block, PC pc)
{
    const IRInsts& insts = block->GetInsts();
    IRInsts::const_iterator it;
    for (it = insts.begin(); it != insts.end(); ++it)
        pc = LowerInst(*it, pc);
    return pc;
}

void
XfLowerImpl::NewEnterProcInst(PC pc, SymIndex funcNameSymIndex,
                              const IRPos& pos)
{
    int numArgs = 1;
    ArgNum argNum = PushArgs(numArgs, funcNameSymIndex);
    NewInst(pc, kOpcode_EnterProc, OpcodeName(kOpcode_EnterProc),
            numArgs, argNum, pos);
}

XfLowerImpl::PC
XfLowerImpl::Visit(IRSeq* seq, PC pc)
{
    // Generate an 'enterproc' instruction if the sequence represents an
    // inlined function call.
    if (seq->GetFuncName())
        NewEnterProcInst(pc++, GetSymIndex(seq->GetFuncName()), seq->GetPos());

    const IRStmts& stmts = seq->GetStmts();
    IRStmts::const_iterator it;
    for (it = stmts.begin(); it != stmts.end(); ++it)
        pc = LowerStmt(*it, pc);

    // Generate an 'exitproc' if necessary, which is an 'enterproc' with an
    // empty filename.  TODO: record end pos in IRSeq? (case 166)
    if (seq->GetFuncName())
        NewEnterProcInst(pc++, GetEmptyString(), seq->GetPos());

    return pc;
}

// Lower a "catch" statement, which represents an inlined function call with a
// nested return.
XfLowerImpl::PC
XfLowerImpl::Visit(IRCatchStmt* stmt, PC pc)
{
    // A label records the extent of the body.
    SymIndex bodyLabel = NewLabel(0,0); // placeholder

    // SLO uses marker variables to associate "return" instructions with their
    // enclosing _funccall instructions.  We kept the original marker variable
    // (if not, we could easily create a new one).
    assert(stmt->mMarker && "No marker variable for catch statement");
    SymIndex marker = GetSymIndex(stmt->mMarker);

    // Generate a "funccall" instruction, which takes the label and marker.
    int numArgs = 2;
    ArgNum firstArg = PushArgs(numArgs, bodyLabel, marker);
    NewInst(pc++, kOpcode_FuncCall, OpcodeName(kOpcode_FuncCall),
            numArgs, firstArg, stmt->GetPos());

    // Lower the body of the catch statement, updating the label with its
    // begin and end PCs.
    return LowerStmt(stmt->GetBody(), pc, bodyLabel);
}

// Get the marker variable
IRLocalVar*
XfLowerImpl::GetMarker(IRStmt* stmt) const
{
    IRLocalVar* marker = NULL;
    if (IRForLoop* forLoop = UtCast<IRForLoop*>(stmt))
        marker = forLoop->mMarker;
    else if (IRGatherLoop* gatherLoop = UtCast<IRGatherLoop*>(stmt))
        marker = gatherLoop->mMarker;
    else if (IRCatchStmt* catchStmt = UtCast<IRCatchStmt*>(stmt))
        marker = catchStmt->mMarker;
    else 
        assert(false && "Unimplemented kind of control statement");
    assert(marker != NULL && "Missing marker for control statement");
    return marker;
}

// Lower a control-flow statement, i.e. break/continue/return.
XfLowerImpl::PC
XfLowerImpl::Visit(IRControlStmt* stmt, PC pc)
{
    // SLO uses marker variables to associate control instructions with
    // enclosing loop or _funcall instructions.  We kept the original marker
    // variable (if not, we could easily create a new one).
    IRStmt* enclosingStmt = stmt->GetEnclosingStmt();
    SymIndex marker = GetSymIndex(GetMarker(enclosingStmt));
    int numArgs = 1;
    ArgNum firstArg = PushArgs(numArgs, marker);

    // Break instructions in "for" loops take the loop condition variable
    // as an additional argument.  
    Opcode opcode = stmt->GetOpcode();
    if (opcode == kOpcode_Break) {
        if (IRForLoop* forLoop = UtCast<IRForLoop*>(enclosingStmt)) {
            ++numArgs;
            PushArgs(1, GetSymIndex(forLoop->GetCond()));
        }
    }        
    NewInst(pc++, opcode, OpcodeName(opcode), numArgs, firstArg,
            stmt->GetPos());
    return pc;
}

// Push SLO instruction arguments, returning the index of the first
// (or -1 if there are none).
XfLowerImpl::ArgNum
XfLowerImpl::PushArgs(unsigned int numArgs, ...) const
{
    if (numArgs == 0)
        return -1;
    ArgNum firstArg = (ArgNum) mSlo->mArgs.size();
    va_list ap;
    va_start(ap, numArgs);
    for (unsigned int i = 0; i < numArgs; ++i) {
        SymIndex index = va_arg(ap, SymIndex);
        mSlo->mArgs.push_back(index);
    }
    va_end(ap);
    return firstArg;
}

// Create a new SLO instruction.
void
XfLowerImpl::NewInst(PC pc, Opcode opcode, const char* name,
                     int numArgs, ArgNum firstArg, const IRPos& pos)
{
    assert(pc == (PC) mSlo->mInsts.size() && "Invalid PC while lowering");

    // Get a persistent name for the opcode.  If it's an unknown instruction,
    // the SloShader's OpcodeNames object will own the string.
    name = mSlo->mOpNames.GetName(opcode, name);

    SloInst inst;
    if (pos.IsEmpty())
        inst = SloInst(opcode, name, numArgs, firstArg);
    else {
        // Additional fields: filename string index, line number, and an
        // obsolete debug info argument.
        const IRStringConst* fileStr = pos.GetFileString();
        int fileIndex = GetSymbol(fileStr)->mOffset;
        inst = SloInst(opcode, name, numArgs, firstArg, fileIndex, 
                       pos.GetLine(), 0);
    }
    mSlo->mInsts.push_back(inst);
}

XfLowerImpl::PC
XfLowerImpl::Visit(IRIfStmt* stmt, PC pc)
{
    // Arguments: condSym, condLabel, thenLabel, elseLabel, condTemp The
    // condition temporary is unused.  The condition label is empty because
    // any condition code precedes the IRIfStmt in an IRSeq.
    SymIndex condSym = GetSymIndex(stmt->GetCond());
    SymIndex condLabel = NewLabel(0,0);         // empty
    SymIndex thenLabel = NewLabel(0,0);         // placeholder
    SymIndex elseLabel = NewLabel(0,0);         // placeholder
    SymIndex condTemp = GetCondTemp();
    int numArgs = 5;
    ArgNum firstArg = PushArgs(numArgs, condSym, condLabel,
                               thenLabel, elseLabel, condTemp);

    NewInst(pc++, kOpcode_If, OpcodeName(kOpcode_If), 
            numArgs, firstArg, stmt->GetPos());
    pc = LowerStmt(stmt->GetThen(), pc, thenLabel);
    pc = LowerStmt(stmt->GetElse(), pc, elseLabel);
    return pc;
}

XfLowerImpl::PC
XfLowerImpl::Visit(IRForLoop* loop, PC pc)
{
    // Arguments: condSym, condLabel, bodyLabel, iterateLabel, condTemp
    // followed by an optional marker if the body has break/continue.
    SymIndex condSym = GetSymIndex(loop->GetCond());
    SymIndex condLabel = NewLabel(0,0);         // placeholder
    SymIndex bodyLabel = NewLabel(0,0);         // placeholder
    SymIndex iterLabel = NewLabel(0,0);         // placeholder
    SymIndex condTemp = GetCondTemp();

    // SLO uses marker variables to associate break/continue instructions with
    // the corresponding loop.  We kept the original marker variable (if any).
    // in XfRaise.
    int numArgs;
    ArgNum firstArg;
    if (loop->mMarker == NULL) {
        numArgs = 5;
        firstArg = PushArgs(numArgs, condSym, condLabel,
                            bodyLabel, iterLabel, condTemp);
    }
    else {
        numArgs = 6;
        SymIndex markerSym = GetSymIndex(loop->mMarker);
        firstArg = PushArgs(numArgs, condSym, condLabel,
                            bodyLabel, iterLabel, condTemp, markerSym);
    }

    NewInst(pc++, kOpcode_For, OpcodeName(kOpcode_For), 
            numArgs, firstArg, loop->GetPos());
    pc = LowerStmt(loop->GetCondStmt(), pc, condLabel);
    pc = LowerStmt(loop->GetBody(), pc, bodyLabel);
    pc = LowerStmt(loop->GetIterateStmt(), pc, iterLabel);
    return pc;
}

XfLowerImpl::PC
XfLowerImpl::Visit(IRIlluminanceLoop* loop, PC pc)
{
    return LowerIllumStmt(loop, loop->GetCategory(), pc);
}

XfLowerImpl::PC
XfLowerImpl::Visit(IRIlluminateStmt* stmt, PC pc)
{
    return LowerIllumStmt(stmt, NULL, pc);
}

XfLowerImpl::PC
XfLowerImpl::LowerIllumStmt(IRSpecialForm* loop, IRValue* category, PC pc)
{
    // Arguments: boolTemp, condTemp, category, bodyLabel, userArgs...
    SymIndex boolTemp =
        NewSymbol(NewTempName().c_str(), kSloLocal, kSloBool, kSloVarying);    
    SymIndex condTemp = GetCondTemp();
    SymIndex categorySym = category ? GetSymIndex(category) : GetZero();
    SymIndex bodyLabel = NewLabel(0,0); // placeholder

    // Push these arguments on the SLO shader's argument block, followed by
    // the remaining user args.
    int numArgs = 4;
    ArgNum firstArg =
        PushArgs(numArgs, boolTemp, condTemp, categorySym, bodyLabel);
    numArgs += (int) loop->GetArgs().size();
    LowerArgs(loop->GetArgs());

    // Generate an illuminance/illuminate/solar instruction.
    Opcode opcode = kOpcode_Unknown;
    if (UtCast<IRIlluminanceLoop*>(loop))
        opcode = kOpcode_Illuminance;
    else if (IRIlluminateStmt* illum = UtCast<IRIlluminateStmt*>(loop))
        opcode = illum->IsSolar() ? kOpcode_Solar : kOpcode_Illuminate;
    assert(opcode != kOpcode_Unknown && "Unhandled illum statement");
    NewInst(pc++, opcode, OpcodeName(opcode), numArgs, firstArg,
            loop->GetPos());

    // Lower the body of the loop, updating its label with begin/end PCs.
    return LowerStmt(loop->GetBody(), pc, bodyLabel);
}

XfLowerImpl::PC
XfLowerImpl::Visit(IRGatherLoop* loop, PC pc)
{
    // Arguments: bodyLabel, elseLabel, markerTemp, category, args...
    SymIndex bodyLabel = NewLabel(0,0); // placeholder
    SymIndex elseLabel = NewLabel(0,0); // placeholder
    assert(loop->mMarker && "No marker variable for catch statement");
    SymIndex markerSym = GetSymIndex(loop->mMarker);
    SymIndex categorySym = GetSymIndex(loop->GetCategory());

    // Push these arguments on the SLO shader's argument block, followed by
    // the remaining user args.
    int numArgs = 4;
    ArgNum firstArg =
        PushArgs(numArgs, bodyLabel, elseLabel, markerSym, categorySym);
    numArgs += (int) loop->GetArgs().size();
    LowerArgs(loop->GetArgs());

    // Generate a gather instruction
    NewInst(pc++, kOpcode_Gather, OpcodeName(kOpcode_Gather), 
            numArgs, firstArg, loop->GetPos());

    // Lower the loop body and "else" statement, updating labels with
    // begin/end PCs.
    pc = LowerStmt(loop->GetBody(), pc, bodyLabel);
    pc = LowerStmt(loop->GetElseStmt(), pc, elseLabel);
    return pc;
}

XfLowerImpl::PC
XfLowerImpl::Visit(IRPluginCall* call, PC pc)
{
    // Arguments: result, funcName, funcName, pluginName, prototype, args...
    // Not sure why the function name is repeated.
    SymIndex result = GetSymIndex(call->GetResult());
    SymIndex funcName = GetSymIndex(call->GetFuncName());
    SymIndex pluginName = GetSymIndex(call->GetPluginName());
    SymIndex prototype = GetSymIndex(call->GetPrototype());

    // Push these arguments on the SLO shader's argument block, followed by
    // the remaining user args.
    int numArgs = 5;
    ArgNum firstArg = PushArgs(numArgs, result, funcName, funcName,
                               pluginName, prototype);
    numArgs += (int) call->GetArgs().size();
    LowerArgs(call->GetArgs());

    // Generate "calldso" instruction.
    NewInst(pc++, kOpcode_CallDSO, OpcodeName(kOpcode_CallDSO), 
            numArgs, firstArg, call->GetPos());
    return pc;
}
