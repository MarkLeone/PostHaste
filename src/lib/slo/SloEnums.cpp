// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.


#include "slo/SloEnums.h"
#include <assert.h>

bool SloIsKnown(SloShaderType ty) {
    switch (ty) {
      case kSloShaderNone:
      case kSloSurface:
      case kSloLight:
      case kSloDisplacement:
      case kSloVolume:
          return true;
      default:
          return false;
    }
};

bool SloIsKnown(SloType ty) {
    switch (ty) {
      case kSloTypeNone:
      case kSloPoint:
      case kSloColor:
      case kSloFloat:
      case kSloString:
      case kSloLabel:
      case kSloCond:
      case kSloBool:
      case kSloMatrix:
      case kSloVector:
      case kSloNormal:
      case kSloShader:
          return true;
      case kSloVoid:            // TODO: identify SLO void type value.
      default:
          return false;
    }
};

bool SloIsKnown(SloStorage stor) {
    switch (stor) {
      case kSloStorageNone:
      case kSloConstant:
      case kSloLocal:
      case kSloTemp:
      case kSloInput:
      case kSloCode:
      case kSloGlobal:
      case kSloCondTemp:
      case kSloOutput:
          return true;
      default:
          return false;
    }
};

bool SloIsKnown(SloDetail det) {
    switch (det) {
      case kSloDetailNone:
      case kSloRareUniform:
      case kSloVarying:
      case kSloUniform:
          return true;
      default:
          return false;
    }
};

const char* 
SloShaderTypeToString(SloShaderType ty)
{ 
    switch (ty) {
      case kSloSurface: return "surface";
      case kSloLight: return "light";
      case kSloDisplacement: return "displacement";
      case kSloVolume: return "volume";
      case kSloShaderNone:
          assert(false && "Invalid SloShaderType");
    }
    assert(false && "Unimplemented SloShaderType");
    return "unknown";
};

const char* 
SloTypeToString(SloType ty)
{
    switch (ty) {
      case kSloPoint: return "point";
      case kSloColor: return "color";
      case kSloFloat: return "float";
      case kSloString: return "string";
      case kSloLabel: return "label";
      case kSloCond: return "cond";
      case kSloBool: return "bool";
      case kSloMatrix: return "matrix";
      case kSloVector: return "vector";
      case kSloNormal: return "normal";
      case kSloShader: return "shader";
      case kSloVoid: return "void";
      case kSloTypeNone:
          assert(false && "Invalid SloType");
    }
    assert(false && "Unimplemented SloType");
    return "unknown";
};

const char* 
SloStorageToString(SloStorage stor)
{
    switch (stor) {
      case kSloConstant: return "constant";
      case kSloLocal: return "local";
      case kSloTemp: return "temp";
      case kSloInput: return "input";
      case kSloCode: return "code";
      case kSloGlobal: return "global";
      case kSloCondTemp: return "condtemp";
      case kSloOutput: return "output";
      case kSloStorageNone:
          assert(false && "Invalid SloStorage");
    }
    assert(false && "Unimplemented SloStorage");
    return "unknown";
};

const char* 
SloDetailToString(SloDetail det)
{
    switch (det) {
      case kSloVarying: return "varying";
      case kSloUniform: return "uniform";
      case kSloRareUniform: return "uniform";
      case kSloDetailNone:
          assert(false && "Invalid SloDetail");
    }
    assert(false && "Unimplemented SloDetail");
    return "unknown";
};

// Get the size of the specified type (in words).
unsigned int
TypeSize(SloType ty)
{
    switch (ty) {
      case kSloBool:
      case kSloCond:
      case kSloFloat:
          return 1;
      case kSloColor:
      case kSloPoint:
      case kSloVector:
      case kSloNormal:
          return 3;
      case kSloMatrix:
          return 16;
      case kSloString: 
      case kSloShader: 
          return sizeof(void*) / sizeof(float);
      case kSloLabel:
      case kSloVoid:
      case kSloTypeNone:
          assert(false && "Invalid SloType");
          return 0;
    }
    assert(false && "Unimplemented SloType");
    return 0;
}
