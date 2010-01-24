// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef SLO_ENUMS_H
#define SLO_ENUMS_H

/// SLO shader type enum.
enum SloShaderType { 
    kSloShaderNone = -1,
    kSloSurface = 13,
    kSloLight = 14,
    kSloDisplacement = 15,
    kSloVolume = 16,
};

/// SLO type enum.
enum SloType {
    kSloTypeNone = -1,
    kSloPoint = 2,
    kSloColor = 3,
    kSloFloat = 4,
    kSloString = 6,
    kSloLabel = 9,
    kSloCond = 10,
    kSloBool = 11,
    kSloMatrix = 20,
    kSloVector = 21,
    kSloNormal = 22,
    kSloShader = 23,
    kSloVoid = 256,             // TODO: identify SLO void type value.
};

/// SLO storage enum
enum SloStorage {
    kSloStorageNone = -1,
    kSloConstant = 2,
    kSloLocal = 3,
    kSloTemp = 4,
    kSloInput = 5,
    kSloCode = 8,
    kSloGlobal = 10,
    kSloCondTemp = 11,
    kSloOutput = 13,
};

/// SLO detail enum
enum SloDetail {
    kSloDetailNone = -1,
    kSloRareUniform = 1,     // Seen only on temporary used as texture3d result
    kSloVarying = 2,
    kSloUniform = 3,
};

/// Check whether shader type enum is known.
bool SloIsKnown(SloShaderType ty);

/// Check whether type enum is known.
bool SloIsKnown(SloType ty);

/// Check whether storage enum is known.
bool SloIsKnown(SloStorage stor);

/// Check whether detail enum is known.
bool SloIsKnown(SloDetail det);

/// Convert SloShaderType enum to string.
const char* SloShaderTypeToString(SloShaderType ty);

/// Convert SloType enum to string.
const char* SloTypeToString(SloType ty);

/// Convert SloStorage enum to string.
const char* SloStorageToString(SloStorage stor);

/// Convert SloDetail enum to string.
const char* SloDetailToString(SloDetail det);

/// Get the size of the specified type (in words).
unsigned int SloTypeSize(SloType ty);

#endif // ndef SLO_ENUMS_H
