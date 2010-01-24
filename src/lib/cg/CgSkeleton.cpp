// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ops/OpTypes.h"
#include <RslPlugin.h>

extern "C" {

// CgIter is a type-generic alternative to RslIter.
struct CgIter {
    float* mData;
    unsigned int* mIncrList;
    bool mIsVarying;
};

// Construct an iterator for the specified RslArg.
void CgGetIter(const RslArg** argv, int argNum, CgIter* it)
{
    *reinterpret_cast<RslFloatIter*>(it) = RslFloatIter(argv[argNum]);
}

// Dereference an iterator.
float* CgDerefIter(CgIter* it)
{
    return it->mData;
}

// Increment an interator.
void CgIncIter(CgIter* it)
{
    it->mData += *it->mIncrList;
    ++it->mIncrList;
}

// Get the number of values for the specified argument.
int CgNumValues(const RslArg** argv, int argNum)
{
    return argv[argNum]->NumValues();
}

// Skeletal declaration of plugin entry point.
PRMANEXPORT int
CgEntryFunc(RslContext* rslContext, int argc, const RslArg** argv)
{
#if 0
    // For example:
    CgIter f, c;
    CgGetIter(argv, 1, &f);
    CgGetIter(argv, 2, &c);

    int n = CgNumValues(argv, 0);
    for (int i = 0; i < n; ++i) 
    {
        float* fp = CgDerefIter(&f);
        OpVec3* cp = (OpVec3*) CgDerefIter(&c);
        CgKernelFunc(fp, cp);
        CgIncIter(&f);
        CgIncIter(&c);
    }
#endif
    return 0;
}

// Skeletal function table.
PRMANEXPORT RslFunctionTable RslPublicFunctions(NULL, NULL, NULL);

// These NULL function pointers are used when instantiating the function table.
RslEntryFunc gCgNullEntryFunc = NULL;
RslVoidFunc gCgNullVoidFunc = NULL;

} // extern "C"
