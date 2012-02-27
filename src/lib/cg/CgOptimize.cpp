// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include <limits>

using namespace llvm;

static void 
AddOptimizationPasses(PassManagerBase &MPM, FunctionPassManager &FPM,
                      unsigned OptLevel) 
{
    PassManagerBuilder Builder;
    Builder.OptLevel = OptLevel;

    if (OptLevel == 0) {
        Builder.DisableUnrollLoops = true;
        // No inlining pass
    }
    else {
        Builder.Inliner = createFunctionInliningPass(1000);
    }

    Builder.populateFunctionPassManager(FPM); 
    Builder.populateModulePassManager(MPM);

    if (OptLevel == 0) {
	MPM.add(createGlobalDCEPass()); // Remove unused fns and globs
    }
}

void CgOptimize(llvm::Module* module, int optimizationLevel)
{
    // Construct module and function pass managers.
    PassManager passes;
    passes.add(new llvm::TargetData(module));
    FunctionPassManager funcPasses(module);
    funcPasses.add(new TargetData(module));

    // Add optimization passes based on optimization level.
    AddOptimizationPasses(passes, funcPasses, optimizationLevel);

    // Run function passes.
    if (optimizationLevel > 0)
    {
        funcPasses.doInitialization();
        for (Module::iterator F = module->begin(), E = module->end(); F != E; ++F)
            funcPasses.run(*F);
        funcPasses.doFinalization();
    }

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


