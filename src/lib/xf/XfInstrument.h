// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef XF_INSTRUMENT_H
#define XF_INSTRUMENT_H

#include "ir/IRVisitor.h"
class IRShader;
class UtLog;

void XfInstrument(IRShader* shader, UtLog* log, int minPartitionSize);

class XfInstrumentImpl : public IRVisitor<XfInstrumentImpl> {
private:
    int mMinPartitionSize;
    IRShader* mShader;

public:
    XfInstrumentImpl(UtLog* log, int minPartitionSize=1);

    void Instrument(IRShader* shader);
    IRStmt* InstrumentPartition(IRStmt* stmt);

    IRStmt* Walk(IRStmt* stmt);
    bool ShouldCompile(IRStmt* stmt);
    IRStmt* Visit(IRBlock* stmt, int ignored);
    IRStmt* Visit(IRSeq* stmt, int ignored);
    IRStmt* Visit(IRIfStmt* stmt, int ignored);
    IRStmt* Visit(IRForLoop* stmt, int ignored);
    IRStmt* Visit(IRCatchStmt* stmt, int ignored);
    IRStmt* Visit(IRControlStmt* stmt, int ignored);
    IRStmt* Visit(IRGatherLoop* stmt, int ignored);
    IRStmt* Visit(IRIlluminanceLoop* stmt, int ignored);
    IRStmt* Visit(IRIlluminateStmt* stmt, int ignored);
    IRStmt* Visit(IRPluginCall* stmt, int ignored);
};

#endif // ndef XF_INSTRUMENT_H
