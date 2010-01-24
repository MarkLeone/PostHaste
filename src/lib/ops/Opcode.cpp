// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ops/Opcode.h"
#include <assert.h>

const char*
OpcodeName(Opcode opcode)
{
    switch (opcode) {
      case kOpcode_Unknown:
          assert(false && "Cannot get name of 'unknown' opcode.");
          return "unknown";

      case kOpcode_Abs:                return "abs";
      case kOpcode_Acos:               return "acos";
      case kOpcode_Add:                return "_add";
      case kOpcode_Ambient:            return "ambient";
      case kOpcode_And:                return "_and";
      case kOpcode_Area:               return "area";
      case kOpcode_ArrayAssign:        return "_arrayassign";
      case kOpcode_ArrayAssignComp:    return "_arrayassigncomp";
      case kOpcode_ArrayAssignMxComp:  return "_arrayassignmxcomp";
      case kOpcode_ArrayInit:          return "_arrayinit";
      case kOpcode_ArrayLength:        return "arraylength";
      case kOpcode_ArrayRef:           return "_arrayref";
      case kOpcode_Asin:               return "asin";
      case kOpcode_Assign:             return "_assign";
      case kOpcode_AssignMatrix:       return "_assignmatrix";
      case kOpcode_Atan:               return "atan";
      case kOpcode_Atmosphere:         return "atmosphere";
      case kOpcode_Attribute:          return "attribute";
      case kOpcode_Bake3D:             return "bake3d";
      case kOpcode_Break:              return "_break";
      case kOpcode_CTransform:         return "ctransform";
      case kOpcode_CalculateNormal:    return "calculatenormal";
      case kOpcode_CallDSO:            return "calldso";
      case kOpcode_Capacity:           return "capacity";
      case kOpcode_Caustic:            return "caustic";
      case kOpcode_Ceil:               return "ceil";
      case kOpcode_CellNoise:          return "cellnoise";
      case kOpcode_Clamp:              return "clamp";
      case kOpcode_Color:              return "_color";
      case kOpcode_ColorTransform:     return "_colortransform";
      case kOpcode_Comp:               return "comp";
      case kOpcode_Concat:             return "concat";
      case kOpcode_Continue:           return "_continue";
      case kOpcode_Cos:                return "cos";
      case kOpcode_Cross:              return "_cross";
      case kOpcode_Degrees:            return "degrees";
      case kOpcode_Depth:              return "depth";
      case kOpcode_Deriv:              return "Deriv";
      case kOpcode_Determinant:        return "mxdeterminant";
      case kOpcode_Diffuse:            return "diffuse";
      case kOpcode_Displacement:       return "displacement";
      case kOpcode_Distance:           return "distance";
      case kOpcode_Divide:             return "_divide";
      case kOpcode_Dot:                return "_dot";
      case kOpcode_Du:                 return "Du";
      case kOpcode_Dv:                 return "Dv";
      case kOpcode_EQ:                 return "_eq";
      case kOpcode_EnterProc:          return "_enterproc";
      case kOpcode_Environment:        return "environment";
      case kOpcode_Erf:                return "erf";
      case kOpcode_Erfc:               return "erfc";
      case kOpcode_Exp:                return "exp";
      case kOpcode_FaceForward:        return "faceforward";
      case kOpcode_FilterStep:         return "filterstep";
      case kOpcode_Floor:              return "floor";
      case kOpcode_For:                return "_for";
      case kOpcode_Format:             return "format";
      case kOpcode_Fresnel:            return "fresnel";
      case kOpcode_FuncCall:           return "_funccall";
      case kOpcode_GE:                 return "_ge";
      case kOpcode_GT:                 return "_gt";
      case kOpcode_Gather:             return "_gather";
      case kOpcode_GeoNormals:         return "$geonormals";
      case kOpcode_GetLight:           return "getlight";
      case kOpcode_GetLights:          return "getlights";
      case kOpcode_GetShader:          return "getshader";
      case kOpcode_GetShaders:         return "getshaders";
      case kOpcode_GetVar:             return "getvar";
      case kOpcode_HasMethod:          return "hasmethod";
      case kOpcode_If:                 return "_if";
      case kOpcode_Illuminance:        return "_illuminance";
      case kOpcode_Illuminate:         return "_illuminate";
      case kOpcode_Incident:           return "incident";
      case kOpcode_IndirectDiffuse:    return "indirectdiffuse";
      case kOpcode_InverseSqrt:        return "inversesqrt";
      case kOpcode_LE:                 return "_le";
      case kOpcode_LT:                 return "_lt";
      case kOpcode_Length:             return "length";
      case kOpcode_LightSource:        return "lightsource";
      case kOpcode_LightingStart:      return "_lightingstart";
      case kOpcode_Log:                return "log";
      case kOpcode_MTransform:         return "mtransform";
      case kOpcode_MTransformMx:       return "mtransformmx";
      case kOpcode_Match:              return "match";
      case kOpcode_Matrix:             return "_matrix";
      case kOpcode_MatrixTransform:    return "matrixtransform";
      case kOpcode_Max:                return "max";
      case kOpcode_Min:                return "min";
      case kOpcode_Mix:                return "mix";
      case kOpcode_Mod:                return "mod";
      case kOpcode_Multiply:           return "_multiply";
      case kOpcode_MxComp:             return "mxcomp";
      case kOpcode_MxRotate:           return "mxrotate";
      case kOpcode_MxScale:            return "mxscale";
      case kOpcode_MxSetComp:          return "mxsetcomp";
      case kOpcode_MxTranslate:        return "mxtranslate";
      case kOpcode_NE:                 return "_ne";
      case kOpcode_NPreChg:            return "$Nprechg";
      case kOpcode_NTransform:         return "ntransform";
      case kOpcode_NTransformMx:       return "ntransformmx";
      case kOpcode_NeedSurface:        return "_needsurface";
      case kOpcode_Negate:             return "_negate";
      case kOpcode_Noise:              return "noise";
      case kOpcode_Normalize:          return "normalize";
      case kOpcode_Occlusion:          return "occlusion";
      case kOpcode_Opposite:           return "opposite";
      case kOpcode_Option:             return "option";
      case kOpcode_Or:                 return "_or";
      case kOpcode_PNoise:             return "pnoise";
      case kOpcode_PPreChg:            return "$Pprechg";
      case kOpcode_Phong:              return "phong";
      case kOpcode_PhotonMap:          return "photonmap";
      case kOpcode_Point:              return "_point";
      case kOpcode_PointTransform:     return "_pointtransform";
      case kOpcode_Pop:                return "pop";
      case kOpcode_Pow:                return "pow";
      case kOpcode_Print:              return "print";
      case kOpcode_Printf:             return "printf";
      case kOpcode_Push:               return "push";
      case kOpcode_Radians:            return "radians";
      case kOpcode_Random:             return "random";
      case kOpcode_RayInfo:            return "rayinfo";
      case kOpcode_Reflect:            return "reflect";
      case kOpcode_Refract:            return "refract";
      case kOpcode_RendererInfo:       return "rendererinfo";
      case kOpcode_Reserve:            return "reserve";
      case kOpcode_Resize:             return "resize";
      case kOpcode_Return:             return "_return";
      case kOpcode_Rotate:             return "rotate";
      case kOpcode_Round:              return "round";
      case kOpcode_Scale:              return "_scale";
      case kOpcode_SetComp:            return "setcomp";
      case kOpcode_SetXComp:           return "setxcomp";
      case kOpcode_SetYComp:           return "setycomp";
      case kOpcode_SetZComp:           return "setzcomp";
      case kOpcode_ShaderName:         return "shadername";
      case kOpcode_Shadow:             return "shadow";
      case kOpcode_Sign:               return "sign";
      case kOpcode_Sin:                return "sin";
      case kOpcode_SmoothStep:         return "smoothstep";
      case kOpcode_Solar:              return "_solar";
      case kOpcode_Specular:           return "specular";
      case kOpcode_SpecularBrdf:       return "specularbrdf";
      case kOpcode_Spline:             return "spline";
      case kOpcode_Sqrt:               return "sqrt";
      case kOpcode_Step:               return "step";
      case kOpcode_SubSurface:         return "subsurface";
      case kOpcode_Subtract:           return "_subtract";
      case kOpcode_Surface:            return "surface";
      case kOpcode_Tan:                return "tan";
      case kOpcode_Texture3D:          return "texture3d";
      case kOpcode_Texture:            return "texture";
      case kOpcode_TextureInfo:        return "textureinfo";
      case kOpcode_Trace:              return "trace";
      case kOpcode_Transform:          return "transform";
      case kOpcode_TransformMx:        return "transformmx";
      case kOpcode_Transmission:       return "transmission";
      case kOpcode_VTransform:         return "vtransform";
      case kOpcode_VTransformMx:       return "vtransformmx";
      case kOpcode_VectorTransform:	return "_vectortransform";
      case kOpcode_WNoise:             return "wnoise";
      case kOpcode_While:              return "_while";
      case kOpcode_XComp:              return "xcomp";
      case kOpcode_YComp:              return "ycomp";
      case kOpcode_ZComp:              return "zcomp";

      case kOpcode_NumInsts:
          // Fall through to assert.
          break;
    }

    assert(false && "Invalid instruction opcode");
    return "invalid";
}
