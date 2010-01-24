// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgOptimize.h"
#include "cg/CgShader.h"
#include "ir/IRShader.h"
#include "slo/SloInputFile.h"
#include "slo/SloOutputFile.h"
#include "slo/SloShader.h"
#include "util/UtLog.h"
#include "xf/XfInstrument.h"
#include "xf/XfLower.h"
#include "xf/XfRaise.h"
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Module.h>
#include <fcntl.h>                      // for O_RDW, O_CREAT
#include <libgen.h>                     // for basename()
#include <sys/types.h>                  // for mode_t
#include <sys/stat.h>                   // for mkdir()
#include <errno.h>
#include <fstream>
#include <getopt.h>
#include <stdio.h>
#include <string>

struct Options {
    std::string mAppName;
    std::string mInput;
    bool mInstrument;
    int mMinPartitionSize;
    unsigned int mOptimizationLevel;
    bool mShowPartitions;
    bool mQuiet;
    
    Options() :
        mAppName("sloraise"),
        mInstrument(false),
        mMinPartitionSize(30),
        mOptimizationLevel(2),
        mShowPartitions(false),
        mQuiet(false)
    {
    }
};

void 
Usage(const Options& options, UtLog* log)
{
    fprintf(stderr, "Usage: %s [options] infile.slo\n"
            "Options:\n"
            "  -h, --help       Print usage\n"
            "  --min N          Min. number of IR instructions in partition\n"
            "  -O<N>            Optimization level (0 to 2)\n"
            "  --show           Show IR for partitions\n"
            "  -q, --quiet      Silence most output messages\n",
            options.mAppName.c_str());
}

int
ParseOptions(Options& options, int argc, const char** argv, UtLog* log)
{
    options.mAppName = argv[0];

    // Note that long options start at 256, because short options are
    // represented by individual characters.
    enum LongOption {
        kOptNone = 256,
        kInstrument,
        kMinPartitionSize,
        kShowPartitions,
    };

    // Short options.
    static const char* shortOptions = "hO:q";

    static struct option longOptions[] = {
        { "help", no_argument, NULL, 'h' },
        { "instrument", no_argument, NULL, kInstrument },
        { "min", required_argument, NULL, kMinPartitionSize },
        { "show", no_argument, NULL, kShowPartitions },
        { NULL, 0, NULL, 0}
    };

    bool error = false;
    bool usage = false;
    int c;
    while ((c = getopt_long(argc, const_cast<char**>(argv), 
                            shortOptions, longOptions, NULL)) != -1)
        switch (c) {
          case 'h':
              usage = true;
              break;
          case 'O':
              options.mOptimizationLevel = atoi(optarg);
              break;
          case 'q':
              options.mQuiet = true;
              break;
          case kInstrument:
              options.mInstrument = true;
              break;
          case kMinPartitionSize:
              options.mMinPartitionSize = atoi(optarg);
              break;
          case kShowPartitions:
              options.mShowPartitions = true;
              break;
          default:
              error = true;
              break;
        }    

    // Remaining argument is the input filename.  There can be only one.
    if (optind < argc)
        options.mInput = argv[optind++];
    if (optind < argc)
        error = true;

    if (error || usage)
        Usage(options, log);

    return error;
}
int 
main(int argc, const char** argv) 
{
    UtLog log(stderr);
    Options options;
    int status = ParseOptions(options, argc, argv, &log);
    if (status != 0)
        return status;

    // Open the input SLO file.
    SloInputFile in(options.mInput.c_str(), &log);
    status = in.Open();
    if (status > 0)
        return status;

    // Read the SLO shader.
    SloShader slo;
    status = slo.Read(&in);
    if (status > 0)
        return status;

    // Raise SLO to IR.
    IRShader* ir = XfRaise(slo, &log);

    // If we're instrumenting, partition the shader and wrap partitions with
    // timer calls.
    if (options.mInstrument)
        XfInstrument(ir, &log, options.mMinPartitionSize);

    // Compile parts of shader to LLVM, updating IR with plugin calls.
    llvm::Module* module = NULL;
    if (!options.mInstrument) {
        module = CgShaderCodegen(ir, &log, options.mMinPartitionSize, 
                                 options.mShowPartitions);
        status = (module == NULL);
        if (status > 0) {
            delete ir;
            return status;
        }
    }

    // Create output directory if necessary.  NOTE: errno is not threadsafe.
    status = mkdir("posthaste", 0755); // rwxr-xr-x
    if (status != 0 && errno != EEXIST) {
        log.Write(kUtError, "Unable to create output directory 'posthaste'");
        delete ir;
        delete module;
        return status;
    }

    // Get the base of the input filename, minus any extension.  
    // NOTE: basename() is not threadsafe.
    std::string baseName = basename(const_cast<char*>(options.mInput.c_str()));
    baseName.erase(baseName.find_last_of('.'), std::string::npos);

    // Output the modified IR as SLO.
    SloShader* newSlo = XfLower(*ir, &log);
    std::string sloName = std::string("posthaste/") + baseName + ".slo";
    SloOutputFile sloOut(sloName.c_str(), &log);
    status = sloOut.Open();
    if (status == 0) {
        newSlo->Write(&sloOut);
        if (!options.mQuiet)
            log.Write(kUtInfo, "Wrote %s", sloName.c_str());
    }
    delete newSlo;

    if (module != NULL) {
        // Optimize the LLVM code.
        CgOptimize(module, options.mOptimizationLevel);

        // Output LLVM bitcode
        std::string bitcodeName = std::string("posthaste/") + baseName + ".bc";
        std::ofstream out(bitcodeName.c_str(),
                          std::ios::out | std::ios::binary);
        if (out.is_open()) {
            llvm::WriteBitcodeToFile(module, out);
            out.close();
            if (!options.mQuiet)
                log.Write(kUtInfo, "Wrote %s", bitcodeName.c_str());
        }
        else {
            log.Write(kUtError, "Unable to open output file %s",
                      bitcodeName.c_str());
            status = 1;
        }
    }

    delete ir;
    delete module;
    return status;
}
