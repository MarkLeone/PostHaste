// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.


/// Kinds of IR values.
enum IRValueKind {
    kIRConst_Begin,
        // kIRBoolConst,
        kIRNumConst,
        kIRNumArrayConst,
        kIRStringConst,
        kIRStringArrayConst,
    kIRConst_End,
    kIRVar_Begin,
        kIRGlobalVar,
 	kIRLocalVar,
	// kIRMemberVar,
	// kIRMethodParam,
	kIRShaderParam,
    kIRVar_End,
    kIRInst_Begin,
        kIRBasicInst,
        kIRUnknownInst,
/*
        kIRBreakInst,
        kIRContinueInst,
        kIRDsoCall,
        kIRIndexInst,
        kIRLoadInst,
        kIRMethodCall,
        kIRPhiInst,
        kIRReturnInst,
        kIRSelectInst,
        kIRStoreInst,
*/
    kIRInst_End,
};

