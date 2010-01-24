// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ops/OpInfo.h"
#include <stdlib.h>

// Get the name of the shadeop that implements the specified instruction, if
// any.  Returns NULL if there is no implementation.  The name might require
// mangling to resove overloading (see OpIsOverloaded).
const char*
OpInfo::GetOpName(Opcode opcode)
{
    switch (opcode) {
      case kOpcode_Abs: return "OpAbs";
      case kOpcode_Acos: return "OpAcos";
      case kOpcode_Add: return "OpAdd";
      case kOpcode_And: return "OpAnd";
      case kOpcode_ArrayAssign: return "OpArrayAssign";
      case kOpcode_ArrayAssignComp: return "OpArrayAssignComp";
      case kOpcode_ArrayAssignMxComp: return "OpArrayAssignMxComp";
      case kOpcode_ArrayRef: return "OpArrayRef";
      case kOpcode_Asin: return "OpAsin";
      case kOpcode_Assign: return "OpAssign";
      case kOpcode_AssignMatrix: return "OpAssignMatrix";
      case kOpcode_Atan: return "OpAtan";
      case kOpcode_Ceil: return "OpCeil";
      // Noise is disabled because it hurts performance
      // case kOpcode_CellNoise: return "OpCellNoise";
      case kOpcode_Clamp: return "OpClamp";
      case kOpcode_Comp: return "OpComp";
      case kOpcode_Color: return "OpColor";
      case kOpcode_Cos: return "OpCos";
      case kOpcode_Cross: return "OpCross";
      case kOpcode_Degrees: return "OpDegrees";
      case kOpcode_Determinant: return "OpDeterminant";
      case kOpcode_Distance: return "OpDistance";
      case kOpcode_Divide: return "OpDivide";
      case kOpcode_Dot: return "OpDot";
      case kOpcode_Erf: return "OpErf";
      case kOpcode_Erfc: return "OpErfc";
      case kOpcode_EQ: return "OpEQ";
      case kOpcode_Exp: return "OpExp";
      case kOpcode_FaceForward: return "OpFaceForward";
      case kOpcode_Floor: return "OpFloor";
      case kOpcode_GE: return "OpGE";
      case kOpcode_GT: return "OpGT";
      case kOpcode_GeoNormals: return "OpGeoNormals";
      case kOpcode_InverseSqrt: return "OpInverseSqrt";
      case kOpcode_LE: return "OpLE";
      case kOpcode_LT: return "OpLT";
      case kOpcode_Length: return "OpLength";
      case kOpcode_Log: return "OpLog";
      case kOpcode_Matrix: return "OpMatrix";
      case kOpcode_Max: return "OpMax";
      case kOpcode_Min: return "OpMin";
      case kOpcode_Mix: return "OpMix";
      case kOpcode_Mod: return "OpMod";
      // The variant with named spaces can't be compiled, so this is disabled.
      // case kOpcode_MTransformMx: return "OpMTransformMx";
      case kOpcode_Multiply: return "OpMultiply";
      case kOpcode_MxComp: return "OpMxComp";
      case kOpcode_MxRotate: return "OpMxRotate";
      case kOpcode_MxScale: return "OpMxScale";
      case kOpcode_MxTranslate: return "OpMxTranslate";
      case kOpcode_MxSetComp: return "OpMxSetComp";
      case kOpcode_NE: return "OpNE";
      case kOpcode_Negate: return "OpNegate";
      // Noise is disabled because it hurts performance
      // case kOpcode_Noise: return "OpNoise";
      case kOpcode_Normalize: return "OpNormalize";
      case kOpcode_Or: return "OpOr";
      // Noise is disabled because it hurts performance
      // case kOpcode_PNoise: return "OpPNoise";
      case kOpcode_Point: return "OpPoint";
      case kOpcode_Pow: return "OpPow";
      case kOpcode_Print: return "OpPrint";
      case kOpcode_Radians: return "OpRadians";
      case kOpcode_Reflect: return "OpReflect";
      case kOpcode_Refract: return "OpRefract";
      case kOpcode_Rotate: return "OpRotate";
      case kOpcode_Round: return "OpRound";
      case kOpcode_Scale: return "OpScale";
      case kOpcode_SetComp: return "OpSetComp";
      case kOpcode_SetXComp: return "OpSetXComp";
      case kOpcode_SetYComp: return "OpSetYComp";
      case kOpcode_SetZComp: return "OpSetZComp";
      case kOpcode_Sign: return "OpSign";
      case kOpcode_Sin: return "OpSin";
      case kOpcode_SmoothStep: return "OpSmoothStep";
      case kOpcode_Sqrt: return "OpSqrt";
      case kOpcode_Step: return "OpStep";
      case kOpcode_Subtract: return "OpSubtract";
      case kOpcode_Tan: return "OpTan";
      // The variant with named spaces can't be compiled, so these are disabled.
      // case kOpcode_TransformMx: return "OpTransformMx";
      // case kOpcode_VTransformMx: return "OpVTransformMx";
      case kOpcode_XComp: return "OpXComp";
      case kOpcode_YComp: return "OpYComp";
      case kOpcode_ZComp: return "OpZComp";
      default: return NULL;
    }
}

// An overloaded shadeop name is mangled with suffix denoting the
// argument types, ("f" for float, "t" for triple, "F" for a float array,
// etc).  For example, OpAdd_tt adds two triples.
bool
OpInfo::IsOverloaded(Opcode opcode)
{
    switch (opcode) {
      case kOpcode_Add:
      case kOpcode_ArrayAssign:
      case kOpcode_ArrayRef:
      case kOpcode_Assign:
      case kOpcode_AssignMatrix:
      case kOpcode_Atan:
      case kOpcode_CellNoise:
      case kOpcode_Clamp:
      case kOpcode_Divide:
      case kOpcode_EQ:
      case kOpcode_Log:
      case kOpcode_Max:
      case kOpcode_Min:
      case kOpcode_Mix:
      case kOpcode_MTransform:
      case kOpcode_MTransformMx:
      case kOpcode_Multiply:
      case kOpcode_NE:
      case kOpcode_Negate:
      case kOpcode_Noise:
      case kOpcode_PNoise:
      case kOpcode_Print:
      case kOpcode_Scale:
      case kOpcode_Subtract:
      case kOpcode_Transform:
      case kOpcode_TransformMx:
      case kOpcode_VTransform:
      case kOpcode_VTransformMx:
          return true;
      default:
          return false;
    }
}

// Check whether a shadeop is overloaded by result type.
bool
OpInfo::IsOverloadedByResult(Opcode opcode)
{
    switch (opcode) {
      case kOpcode_Assign:
      case kOpcode_CellNoise:
      case kOpcode_Noise:
      case kOpcode_PNoise:
          return true;
      default:
          return false;
    }
}

// Check whether a shadeop has a void result.  If not, the first argument in
// the SLO instruction is the result.
bool
OpInfo::IsVoid(Opcode opcode)
{
    switch (opcode) {
      case kOpcode_ArrayAssign:
      case kOpcode_ArrayAssignComp:
      case kOpcode_ArrayAssignMxComp:
      case kOpcode_ArrayInit:
      case kOpcode_Fresnel:
      case kOpcode_Resize:
      case kOpcode_Reserve:
      case kOpcode_Push:
      case kOpcode_MxSetComp:
      case kOpcode_Print:
      case kOpcode_Printf:
      case kOpcode_SetComp:
      case kOpcode_SetXComp:
      case kOpcode_SetYComp:
      case kOpcode_SetZComp:
          return true;

      // Special forms also don't have results per se.
      case kOpcode_Break:
      case kOpcode_Continue:
      case kOpcode_EnterProc:
      case kOpcode_For:
      case kOpcode_FuncCall:
      case kOpcode_Gather:
      case kOpcode_Illuminance:
      case kOpcode_Illuminate:
      case kOpcode_LightingStart:
      case kOpcode_NPreChg:
      case kOpcode_NeedSurface:
      case kOpcode_PPreChg:
      case kOpcode_Return:
      case kOpcode_Solar:
      case kOpcode_While:
          return true;

      default:
          return false;
    }
}

// Check whether a shadeop has one or more output parameters.
bool
OpInfo::HasOutput(Opcode opcode)
{
    switch (opcode) {
      case kOpcode_ArrayAssign:
      case kOpcode_ArrayAssignComp:
      case kOpcode_ArrayAssignMxComp:
      case kOpcode_Atmosphere:
      case kOpcode_Attribute:
      case kOpcode_CallDSO:
      case kOpcode_Displacement:
      case kOpcode_Environment:
      case kOpcode_GetVar:
      case kOpcode_Incident:
      case kOpcode_IndirectDiffuse:
      case kOpcode_LightSource:
      case kOpcode_MxSetComp:
      case kOpcode_Occlusion:
      case kOpcode_Opposite:
      case kOpcode_Option:
      case kOpcode_Pop:
      case kOpcode_Push:
      case kOpcode_RayInfo:
      case kOpcode_RendererInfo:
      case kOpcode_Reserve:
      case kOpcode_Resize:
      case kOpcode_SetComp:
      case kOpcode_SetXComp:
      case kOpcode_SetYComp:
      case kOpcode_SetZComp:
      case kOpcode_Surface:
      case kOpcode_Texture3D:
      case kOpcode_TextureInfo:
          return true;
      default:
          return false;
    }
}

// Check whether the ith argument of the specified instruction is an output
// argument.
bool
OpInfo::IsOutput(Opcode opcode, int i)
{
    switch (opcode) {
      case kOpcode_ArrayAssign:
      case kOpcode_ArrayAssignComp:
      case kOpcode_ArrayAssignMxComp:
      case kOpcode_MxSetComp:
      case kOpcode_SetComp:
      case kOpcode_SetXComp:
      case kOpcode_SetYComp:
      case kOpcode_SetZComp:
          return i == 0;

      case kOpcode_Pop:
      case kOpcode_Push:
      case kOpcode_Reserve:
      case kOpcode_Resize:
          return i == 0;

      case kOpcode_Atmosphere:
      case kOpcode_Displacement:
      case kOpcode_LightSource:
      case kOpcode_Surface:
      case kOpcode_Incident:
      case kOpcode_Opposite:
      case kOpcode_GetVar:
          // Note that message passing routines aren't guaranteed to write the
          // output argument!
          return i == 1;

      case kOpcode_Attribute:
      case kOpcode_Option:
      case kOpcode_RayInfo:
      case kOpcode_RendererInfo:
      case kOpcode_TextureInfo:
          // Note that these routines aren't guaranteed to write the output!
          return i == 1;

      // ----- The following have overly conservative answers. -----
      case kOpcode_CallDSO:
          // Any parameter could be written.  Could load DSO and check entry
          // prototype, but even then there's no guarantee.
          return true;

      case kOpcode_Texture3D:
          // Some name/value pairs are options, while the rest are lookups.
          // Options: filterradius, filterscale, maxdepth, maxdist, lerp,
          // coordsystem, cachelifetime, errorcode.
          return i >= 3;

      case kOpcode_Environment:
          // environment("raytrace"): occlusion, opacity
          return i >= 2;

      case kOpcode_IndirectDiffuse:
      case kOpcode_Occlusion:
          // environmentcolor, environmentdir
          return i >= 3;

      default:
          return false;
    }
}

// Check whether the ith argument of the specified instruction is killed
// (i.e. value ignored on input; completely overwritten on output).
bool
OpInfo::KillsArg(Opcode opcode, int i)
{
    // I don't know of any shadeops that unconditionally write their output
    // arguments!  Array assignment and component setters obviously overwrite
    // only part of the output argument value.  Message passing, attribute,
    // option, and info shadeops all conditionally write their outputs.
    return false;
}

