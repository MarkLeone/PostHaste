// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Module.h>
#include <llvm/ModuleProvider.h>
#include <llvm/PassManager.h>
#include <llvm/Support/CallSite.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/InlinerPass.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/UnifyFunctionExitNodes.h>

#include <limits>

using namespace llvm;

static void 
AddOptimizationPasses(PassManager &MPM, FunctionPassManager &FPM,
                      unsigned OptLevel) 
{
  MPM.add(createGlobalDCEPass()); // Remove unused fns and globs

  if (OptLevel == 0)
      return;

  FPM.add(createCFGSimplificationPass());
  if (OptLevel == 1)
      FPM.add(createPromoteMemoryToRegisterPass());
  else
      FPM.add(createScalarReplAggregatesPass(std::numeric_limits<signed int>::max()));
  FPM.add(createInstructionCombiningPass());
  FPM.add(createUnifyFunctionExitNodesPass());  // eliminate early returns

  MPM.add(createFunctionInliningPass());      // Inline small functions
  MPM.add(createRaiseAllocationsPass());      // call %malloc -> malloc inst
  MPM.add(createCFGSimplificationPass());       // Clean up disgusting code
  MPM.add(createPromoteMemoryToRegisterPass()); // Kill useless allocas
  MPM.add(createInstructionCombiningPass());    // Clean up after IPCP & DAE
  MPM.add(createCFGSimplificationPass());       // Clean up after IPCP & DAE
  if (OptLevel > 2)
    MPM.add(createArgumentPromotionPass());   // Scalarize uninlined fn args
  MPM.add(createSimplifyLibCallsPass());      // Library Call Optimizations
  MPM.add(createInstructionCombiningPass());  // Cleanup for scalarrepl.
  MPM.add(createJumpThreadingPass());         // Thread jumps.
  MPM.add(createCFGSimplificationPass());     // Merge & remove BBs
  MPM.add(createScalarReplAggregatesPass(std::numeric_limits<signed int>::max()));  // Break up aggregate allocas
  MPM.add(createInstructionCombiningPass());  // Combine silly seq's
  MPM.add(createCondPropagationPass());       // Propagate conditionals
  MPM.add(createTailCallEliminationPass());   // Eliminate tail calls
  MPM.add(createCFGSimplificationPass());     // Merge & remove BBs
  MPM.add(createReassociatePass());           // Reassociate expressions
  
  MPM.add(createInstructionCombiningPass());  // Clean up after the unroller
  MPM.add(createGVNPass());                   // Remove redundancies
  MPM.add(createSCCPPass());                  // Constant prop with SCCP
  
  // Run instcombine after redundancy elimination to exploit opportunities
  // opened up by them.
  MPM.add(createInstructionCombiningPass());
  MPM.add(createCondPropagationPass());       // Propagate conditionals
  MPM.add(createDeadStoreEliminationPass());  // Delete dead stores
  MPM.add(createAggressiveDCEPass());   // Delete dead instructions

  MPM.add(createCFGSimplificationPass());     // Merge & remove BBs
  
  MPM.add(createGlobalDCEPass());             // Remove unused fns and globs
  MPM.add(createStripDeadPrototypesPass());   // Get rid of dead prototypes
  MPM.add(createDeadTypeEliminationPass());   // Eliminate dead types

  MPM.add(createConstantMergePass()); // Merge dup global constants 

  return;
}

void CgOptimize(llvm::Module* module, int optimizationLevel)
{
    // Construct module and function pass managers.
    PassManager passes;
    passes.add(new llvm::TargetData(module));
    ExistingModuleProvider provider(module);
    FunctionPassManager funcPasses(&provider);
    funcPasses.add(new TargetData(module));

    // Add optimization passes based on optimization level.
    AddOptimizationPasses(passes, funcPasses, optimizationLevel);

    // Run function passes.
    for (Module::iterator I = module->begin(), E = module->end();
         I != E; ++I)
        funcPasses.run(*I);

    // Take ownership of the module back from the module provider.
    llvm::Module* sameModule = provider.releaseModule();
    if (sameModule != module)
        assert(false && "ExistingModuleProvider invariant failure");

    // Run module passes
    passes.add(createVerifierPass());
    passes.run(*module);
}


// Internalize all the symbols in the given module except the specified
// exports.
void CgInternalize(llvm::Module* module,
                   const std::vector<const char*>& exports)
{
    // Construct module and function pass managers.
    PassManager passes;
    passes.add(createInternalizePass(exports));
    passes.run(*module);
}


