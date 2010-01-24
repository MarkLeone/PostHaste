// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_TYPEDEFS_H
#define IR_TYPEDEFS_H

#include "util/UtVector.h"
class IRConst;
class IRInst;
class IRGlobalVar;
class IRLocalVar;
class IRShaderParam;
class IRStmt;
class IRValue;
class IRVar;

typedef UtVector<IRConst*> IRConsts;             ///< IR constants
typedef UtVector<IRInst*> IRInsts;               ///< IR instructions
typedef UtVector<IRShaderParam*> IRShaderParams; ///< IR shader parameters
typedef UtVector<IRStmt*> IRStmts;               ///< IR statements
typedef UtVector<IRValue*> IRValues;             ///< IR values
typedef UtVector<IRLocalVar*> IRLocalVars;       ///< IR locals list
typedef UtVector<IRGlobalVar*> IRGlobalVars;     ///< IR globals
typedef UtVector<IRVar*> IRVars;                 ///< IR variables

#endif // ndef IR_TYPEDEFS_H
