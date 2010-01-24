// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "slo/SloShader.h"
#include "slo/SloInputFile.h"
#include "slo/SloOutputFile.h"

int
SloShader::Read(SloInputFile* in)
{
    int status = (mHeader.Read(in) ||
                  mInfo.Read(in) ||
                  ReadStrings(in) ||
                  ReadConstants(in) ||
                  ReadEmpty(in) ||
                  ReadSymbols(in) ||
                  ReadArgs(in) ||
                  ReadInsts(in));
    return status;
}

void 
SloShader::Write(SloOutputFile* out) const
{
    mHeader.Write(out);
    mInfo.Write(out);
    WriteStrings(out);
    WriteConstants(out);
    WriteEmpty(out);
    WriteSymbols(out);
    WriteArgs(out);
    WriteInsts(out);
}

int
SloShader::ReadStrings(SloInputFile* in)
{
    if (const char* line = in->ReadLine()) {
        int numStrings, numChars;
        int numFields = sscanf(line, "%i %i", &numStrings, &numChars);
        if (numFields == 2) {
            mStrings.resize(numStrings);
            for (int i = 0; i < numStrings; ++i) {
                if (const char* line = in->ReadLine()) {
                    mStrings[i] = line;
                }
                else {
                    in->Report(kUtError, "Error reading SLO string constant");
                    return 1;
                }
            }
            return 0;
        }
    }
    in->Report(kUtError, "Error reading SLO strings");
    return 1;
}

void
SloShader::WriteStrings(SloOutputFile* out) const
{
    size_t numStrings = mStrings.size();
    size_t numChars = 0;
    for (unsigned int i = 0; i < numStrings; ++i)
        numChars += mStrings[i].size() + 1;
    out->Write("%u %u\n", static_cast<unsigned int>(numStrings), 
               static_cast<unsigned int>(numChars));
    for (size_t i = 0; i < numStrings; ++i)
        out->Write("%s\n", mStrings[i].c_str());
}

int
SloShader::ReadConstants(SloInputFile* in)
{
    if (const char* line = in->ReadLine()) {
        int numConstants;
        int numFields = sscanf(line, "%i", &numConstants);
        if (numFields == 1) {
            mConstants.resize(numConstants);
            for (int i = 0; i < numConstants; ++i) {
                if (const char* line = in->ReadLine()) {
                    int numFields = sscanf(line, "%g", &mConstants[i]);
                    if (numFields != 1) {
                        in->Report(kUtError, "Error reading SLO constant");
                        return 1;
                    }
                }
                else {
                    in->Report(kUtError, "Error reading SLO constant");
                    return 1;
                }
            }
            return 0;
        }
    }
    in->Report(kUtError, "Error reading SLO constants");
    return 1;
}

void
SloShader::WriteConstants(SloOutputFile* out) const
{
    size_t numConstants = mConstants.size();
    out->Write("%u\n", static_cast<unsigned int>(numConstants));
    for (size_t i = 0; i < numConstants; ++i)
        out->Write("%.10g\n", mConstants[i]);
}

int
SloShader::ReadEmpty(SloInputFile* in)
{
    // Read empty SLO section.
    if (const char* line = in->ReadLine()) {
        int size;
        int numFields = sscanf(line, "%i", &size);
        if (numFields == 1 && size == 0) {
            return 0;
        }
    }
    in->Report(kUtError, "Error reading empty SLO section");
    return 1;
}

void
SloShader::WriteEmpty(SloOutputFile* out) const
{
    out->Write("0\n");
}

int
SloShader::ReadSymbols(SloInputFile* in)
{
    if (const char* line = in->ReadLine()) {
        int numSymbols;
        int n = sscanf(line, "%i", &numSymbols);
        if (n == 1) {
            mSymbols.resize(numSymbols);
            for (int i = 0; i < numSymbols; ++i) {
                if (mSymbols[i].Read(in))
                    return 1;
            }
            return 0;
        }
    }
    in->Report(kUtError, "Error reading SLO symbols");
    return 1;
}

void
SloShader::WriteSymbols(SloOutputFile* out) const
{
    int numSymbols = static_cast<int>(mSymbols.size());
    out->Write("%i\n", numSymbols);
    for (int i = 0; i < numSymbols; ++i)
        mSymbols[i].Write(out);
}

// Read an integer from the given input line (ignoring any leading
// whitespace).  If successful, the integer is returned by result parameter
// and the rest of the line is returned.  If unsuccessful, NULL is returned
// and the result parameter is unmodified.
static const char*
ReadInt(const char* line, int* result)
{
    int offset = 0;
    int n = sscanf(line, " %i%n", result, &offset);
    if (n == 1)
        return line + offset;
    else
        return NULL;
}

int
SloShader::ReadArgs(SloInputFile* in)
{
    if (const char* line = in->ReadLine()) {
        int numArgs;
        int n = sscanf(line, "%i", &numArgs);
        if (n == 1) {
            mArgs.resize(numArgs);
            fflush(stdout);
            int argNum = 0;
            const char* line = "";
            while (argNum < numArgs) {
                line = in->ReadLine();
                if (line == NULL) {
                    in->Report(kUtError, "Error reading SLO args");
                    return 1;
                }
                do {
                    line = ReadInt(line, &mArgs[argNum]);
                    if (line)
                        ++argNum;
                } while (line != NULL && argNum < numArgs);
            }
            if (argNum == numArgs)
                return 0;
        }
    }
    in->Report(kUtError, "Error reading SLO args");
    return 1;
}

void
SloShader::WriteArgs(SloOutputFile* out) const
{
    out->Write("%i\n", static_cast<int>(mArgs.size()));
    // Rather writing on argument per line, we write all the arguments for an
    // instruction on one line.
    int argNum = 0;
    for (size_t i = 0; i < mInsts.size(); ++i) {
        int numArgs = mInsts[i].mNumArgs;
        if (numArgs > 0) {
            for (int j = 0; j < numArgs; ++j)
                out->Write("%i ", mArgs[argNum++]);
            out->Write("\n");
        }
    }
}


int
SloShader::ReadInsts(SloInputFile* in)
{
    // Number of instructions was specified in shader info.
    mInsts.resize(mInfo.mNumInsts);
    for (int i = 0; i < mInfo.mNumInsts; ++i) {
        if (mInsts[i].Read(in, &mOpNames))
            return 1;
    }
    return 0;
    in->Report(kUtError, "Error reading SLO instructions");
    return 1;
}

void
SloShader::WriteInsts(SloOutputFile* out) const
{
    assert(mInfo.mNumInsts == static_cast<int>(mInsts.size()) &&
           "Instruction count mismatch");
    int majorSloVersion = static_cast<int>(mHeader.mVersion);
    for (size_t i = 0; i < mInsts.size(); ++i)
        mInsts[i].Write(out, majorSloVersion);
}

