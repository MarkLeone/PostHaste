// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgInst.h"
#include "cg/CgConst.h"
#include "cg/CgTypes.h"
#include "cg/CgValue.h"
#include "cg/CgVars.h"
#include "ir/IRArrayType.h"
#include "ir/IRInst.h"
#include "ir/IRTypedefs.h"
#include "ir/IRVar.h"
#include "util/UtCast.h"
#include "util/UtLog.h"
#include <llvm/Instructions.h>
#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>
#include <ctype.h>
#include <iostream>
#include <sstream>

static const char*
MangledType(const IRType* ty)
{
    const IRArrayType* arrayTy = UtCast<const IRArrayType*>(ty);
    bool isArray = (arrayTy != NULL);
    if (isArray)
        ty = arrayTy->GetElementType();
    switch (ty->GetKind()) {
      case kIRFloatTy:
          return isArray ? "F" : "f";
      case kIRPointTy:
      case kIRVectorTy:
      case kIRNormalTy:
      case kIRColorTy:
          return isArray ? "T" : "t";
      case kIRMatrixTy:
          return isArray ? "M" : "m";
      case kIRStringTy:
          return isArray ? "S" : "s";
      case kIRVoidTy:
      case kIRBoolTy:
      case kIRShaderTy:
      case kIRArrayTy:
      case kIRStructTy:
      case kIRNumTypeKinds:
          assert(false && "Invalid argument type for overloaded op");
          return "";
    };
    assert(false && "Unimplemented kind of IR type in shadeop mangling");
    return "";
}

// Given the name of an overloaded shadeop, mangle it to include argument type
// specifiers (e.g. OpAdd_ft).
void
CgInst::MangleArgTypes(Opcode opcode, std::stringstream& opName,
                       const IRVar* result, const IRValues& args) const
{
    opName << "_";
    if (OpInfo::IsOverloadedByResult(opcode)) {
        assert(result != NULL && "Expected result in shadeop mangling");
        opName << MangledType(result->GetType());
    }
    IRValues::const_iterator it;
    for (it = args.begin(); it != args.end(); ++it)
        opName << MangledType((*it)->GetType());
}

bool
CgInst::GenInst(const IRInst& inst) const
{
    // Check whether a shadeop is defined for this opcode.
    Opcode opcode = inst.GetOpcode();
    const char* opNamePtr = OpInfo::GetOpName(opcode);
    if (opNamePtr == NULL)
        return false;
    std::stringstream opName;
    opName << opNamePtr;

    // If the shadeop is overloaded, mangle the name to include argument type
    // specifiers (e.g. OpAdd_ft).  
    if (OpInfo::IsOverloaded(opcode))
        MangleArgTypes(opcode, opName, inst.GetResult(), inst.GetArgs());
    std::string opNameStr = opName.str();

    // Look up the shadeop in the LLVM module.  The caller reports an error if
    // the shadeop is unimplemented.
    llvm::Function* op = mModule->getFunction(opNameStr);
    if (op == NULL)
        return false;

    // Convert the arguments and generate a call to the shadeop.
    GenOpCall(op, inst);
    return true;
}

// Sanity check argument types.
static void
SanityCheckArgs(const llvm::Function::ArgumentListType& params,
                const std::vector<llvm::Value*>& args)
{
    assert(params.size() == args.size() && "Shadeop arg count mismatch");
    llvm::Function::ArgumentListType::const_iterator param = params.begin();
    std::vector<llvm::Value*>::const_iterator arg = args.begin();
    for (; param != params.end(); ++param, ++arg) {
        // XXX debugging
        if (param->getType() != (*arg)->getType())
            std::cout << "Shadeop arg type mismatch: expected " 
                      << *param->getType() << "\nfound "
                      << **arg << std::endl;
        assert(param->getType() == (*arg)->getType() &&
               "Shadeop arg type mismatch");
    }
}

// Generate a shadeop call after converting the given arguments to LLVM
// values.
void
CgInst::GenOpCall(llvm::Function* op, const IRInst& inst) const
{
    // Prepare to gather converted arguments.  
    const IRValues& args = inst.GetArgs();
    std::vector<llvm::Value*> argVals;

    // If there's a result, convert it to a location and pass it as the first
    // argument.  Note that shadeops can't easily be implemented as C
    // functions that return structs by value, since struct return conventions
    // differ between 32 and 64 bit platforms.
    IRVar* result = inst.GetResult();
    if (result) {
        llvm::Value* resultLoc = 
            mValues->ConvertArrayPtr(mVars->GetLocation(result));
        argVals.push_back(resultLoc);
        // Pass the array length too if necessary.
        if (const IRArrayType* arrayTy =
            UtCast<const IRArrayType*>(result->GetType()))
            argVals.push_back(GetInt(arrayTy->GetLength()));
    }

    // Convert the arguments to LLVM values.  Any non-scalar values are
    // converted to locations.
    IRValues::const_iterator it;
    for (it = args.begin(); it != args.end(); ++it) {
        IRValue* arg = *it;
        argVals.push_back(mValues->ConvertArg(arg));

        // Pass the array length too if necessary.
        if (const IRArrayType* arrayTy =
            UtCast<const IRArrayType*>(arg->GetType()))
            argVals.push_back(GetInt(arrayTy->GetLength()));
    }

    // Generate function call.
    SanityCheckArgs(op->getArgumentList(), argVals);
    mBuilder->CreateCall(op, argVals.begin(), argVals.end());
}
