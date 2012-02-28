// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ops/OpTypes.h"
#include <ri.h>                 // for RI_CURRNT, etc.
#include <rx.h>
#include <RslPlugin.h>          // for RslFunction
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/**
   Shadeops are implemented by extern C functions that are compiled
   to LLVM IR.  The calling convention is:
   - Floats are passed by value.
   - A triple is passed by reference to a struct of three floats (OpTupleTy).
   - Similarly for matrices (OpMatrix4).
   - Strings are passed by value as C strings (typedefed as OpStringTy).
   - Results are return via an output parameter.
     - We can't rely on returning structs by value that makes the calling
       convention of the generated LLVM code platform dependent.
*/

// We need a dummy symbols that use RSL types to ensure that they appear
// in the serialized bitcode.
RslFunction* g_dummyRslFunction;
RslFunctionTable* g_dummyRslFunctionTable;

// ---------- Convenience functions ----------

// Templated array assignment function
template<typename T>
static void
ArrayAssign(T* dest, int destLen, const T* src, int srcLen) {
    assert(destLen >= 0 && srcLen >= 0 && "No resizable arrays yet");
    assert(destLen == srcLen && "Length mismatch in array assignment");
    memcpy(dest, src, destLen * sizeof(T));
}

// Templated array equalityfunction
template<typename T>
static OpBoolTy
ArrayEQ(T* a, int aLen, T* b, int bLen) {
    assert(aLen >= 0 && bLen >= 0 && "No resizable arrays yet");
    assert(aLen == bLen && "Length mismatch in array assignment");
    for (int i = 0; i < aLen; ++i)
        if (a[i] != b[i])
            return false;
    return true;
}

// Transform a point/vector by a matrix.  The fourth ("homogeneous") component
// of the quad is 1.0 if it's a point, and 0.0 if it's a vector (because
// vectors aren't translated).
static OpVec3 
Transform(const OpMatrix4& mat, const OpVec4& vec) {
    OpVec4 homog(vec[0], vec[1], vec[2], 1.0f);
    return OpVec3(vec * mat.GetCol(0),
                  vec * mat.GetCol(1),
                  vec * mat.GetCol(2)) / (homog * mat.GetCol(3));
}

extern "C" {

// ---------- Forward declarations ----------
float Sign(float a);
void OpSetComp(OpVec3* a, float b, float c);

// ---------- Shadeops ----------
void OpAbs(float* r, float a) { *r = fabsf(a); }

void OpAcos(float* r, float a) { *r = acosf(a); }

void OpAdd_ff(float* r, float a, float b) {
    *r = a + b;
}

void OpAdd_ft(OpVec3* r, float a, const OpVec3& b) { 
    *r = OpVec3(a) + b;
}

void OpAdd_tf(OpVec3* r, const OpVec3& a, float b) { 
    *r = a + OpVec3(b);
}

void OpAdd_tt(OpVec3* r, const OpVec3& a, const OpVec3& b) { 
    *r = a + b;
}

void OpAnd(OpBoolTy* r, OpBoolTy a, OpBoolTy b) { *r = a && b; }

void OpArrayAssign_Fff(float* a, int aLen, float b, float c) 
{
    assert((int)b < aLen && "Array assign index out of range");
    a[(int)b] = c;
}

void OpArrayAssign_Tft(OpVec3* a, int aLen, float b, const OpVec3& c) 
{
    assert((int)b < aLen && "Array assign index out of range");
    a[(int)b] = c;
}

void OpArrayAssign_Mfm(OpMatrix4* a, int aLen, float b, const OpMatrix4& c) 
{
    assert((int) b < aLen && "Array assign index out of range");
    a[(int) b] = c;
}

void OpArrayAssign_Sfs(OpStringTy* a, int aLen, float b, OpStringTy c) 
{ 
    assert((int)b < aLen && "Array assign index out of range");
    a[(int)b] = c;
}

void OpArrayAssignComp(OpVec3* a, int aLen, float b, float c, float d) 
{
    assert((int)b < aLen && "Array assign index out of range");
    OpSetComp(a + (int) b, c, d);
}

void OpArrayAssignMxComp(OpMatrix4* a, int aLen, float b,
                         float c, float d, float e) 
{
    assert((int) b < aLen && "Array assign index out of range") ;
    assert((int) c < 4 && (int) d < 4 && "Matrix index out of range");
    a[(int) b][(int) c][(int) d] = e;
}

void OpArrayRef_Ff(float* r, float* a, int aLen, float b) {
    *r = a[(int) b];
}

void OpArrayRef_Tf(OpVec3* r, OpVec3* a, int aLen, float b) {
    *r = a[(int) b];
}

void OpArrayRef_Mf(OpMatrix4* r, OpMatrix4* a, int aLen, float b) {
    *r = a[(int) b];
}

void OpArrayRef_Sf(OpStringTy* r, OpStringTy* a, int aLen, float b) {
    *r = a[(int) b];
}

void OpAsin(float* r, float a) { *r = asinf(a); }

void OpAssign_ff(float* a, float b) { *a = b; }

void OpAssign_tf(OpVec3* a, float b) { *a = OpVec3(b); }

void OpAssign_tt(OpVec3* a, const OpVec3& b) { *a = b; }

void OpAssign_ss(OpStringTy* a, OpStringTy b) { *a = b; } // pointer assignment

void OpAssign_FF(float* a, int aLen, const float* b, int bLen) {
    ArrayAssign(a, aLen, b, bLen);
}

void OpAssign_TT(OpVec3* a, int aLen, const OpVec3* b, int bLen) {
    ArrayAssign(a, aLen, b, bLen);
}

void OpAssign_MM(OpMatrix4* a, int aLen, const OpMatrix4* b, int bLen) {
    ArrayAssign(a, aLen, b, bLen);
}

void OpAssign_SS(OpStringTy* a, int aLen, OpStringTy* b, int bLen) {
    ArrayAssign(a, aLen, b, bLen);
}

void OpAssignMatrix_f(OpMatrix4* a, float b) { *a = OpMatrix4(b); }

void OpAssignMatrix_m(OpMatrix4* a, const OpMatrix4& b) { *a = b; }

void OpAtan_f(float* r, float a) { *r = atanf(a); }

void OpAtan_ff(float* r, float a, float b) { *r = atan2f(a, b); }

void OpCeil(float* r, float a) { *r = ceilf(a); }

void OpCellNoise_ff(float* r, float a) {
    RxCellNoise(1, &a, 1, r);
}

void OpCellNoise_tf(OpVec3* r, float a) {
    RxCellNoise(1, &a, 3, r->AsFloats());
}

void OpCellNoise_fff(float* r, float a, float b) {
    float in[2] = {a, b};
    RxCellNoise(2, in, 1, r);
}

void OpCellNoise_tff(OpVec3* r, float a, float b) {
    float in[2] = {a, b};
    RxCellNoise(2, in, 3, r->AsFloats());
}

void OpCellNoise_ft(float* r, const OpVec3& a) {
    RxCellNoise(3, const_cast<float*>(a.AsFloats()), 1, r);
}

void OpCellNoise_tt(OpVec3* r, const OpVec3& a) {
    RxCellNoise(3, const_cast<float*>(a.AsFloats()), 3, r->AsFloats());
}

void OpCellNoise_ftf(float* r, const OpVec3& a, float b) {
    float in[4] = {a[0], a[1], a[2], b};
    RxCellNoise(4, in, 1, r);
}

void OpCellNoise_ttf(OpVec3* r, const OpVec3& a, float b) {
    float in[4] = {a[0], a[1], a[2], b};
    RxCellNoise(4, in, 3, r->AsFloats());
}

float OpClamp(float a, float b, float c) { 
    return (a < b) ? b : ((a > c) ? c : a);
}

void OpClamp_fff(float* r, float a, float b, float c) { 
    *r = OpClamp(a,b,c);
}
    
void OpClamp_ttt(OpVec3* r, const OpVec3& a,
                 const OpVec3& b, const OpVec3& c) {
    *r = OpMap3<OpClamp>(a, b, c);
}

void OpColor(OpVec3* r, float a, float b, float c) {
    *r = OpVec3(a, b, c); 
}
    
void OpComp(float* r, OpVec3* a, float b) {
    *r = (*a)[(int) b];
}

void OpCos(float* r, float a) { *r = cosf(a); }

void OpCross(OpVec3* r, const OpVec3& a, const OpVec3& b) {
    *r = a.Cross(b);
}

#define PI 3.1415926536
void OpDegrees(float* r, float a) { *r = a * (180.0f / PI); }

void OpDeterminant(float* r, const OpMatrix4& m) {
    *r = m.Determinant();
}

void OpDistance(float* r, const OpVec3& a, const OpVec3& b) {
    *r = (b-a).Length();
}

float OpDivide(float a, float b) {
    return b == 0.0f ? a : a / b;
}

void OpDivide_ff(float* r, float a, float b) { 
    *r = OpDivide(a, b);
}

void OpDivide_tf(OpVec3* r, const OpVec3& a, float b) {
    if (b == 0.0f)
        *r = a;
    else
        *r = a / b;
}

void OpDivide_tt(OpVec3* r, const OpVec3& a, const OpVec3& b) {
    *r = OpMap2<OpDivide>(a, b);
}

void OpDot(float* r, const OpVec3& a, const OpVec3& b) {
    *r = a * b;
}

void OpErf(float* r, float a) { *r = erf(a); }

void OpErfc(float* r, float a) { *r = erfc(a); }

void OpEQ_ff(OpBoolTy* r, float a, float b) { *r = a == b; }

void OpEQ_tt(OpBoolTy* r, const OpVec3& a, const OpVec3& b) { *r = a == b; }

void OpEQ_mm(OpBoolTy* r, const OpMatrix4& a, const OpMatrix4& b) { *r = a == b; }

void OpEQ_ss(OpBoolTy* r, OpStringTy a, OpStringTy b) {
    *r = !strcmp(a, b);
}

void OpEQ_FF(OpBoolTy* r, float* a, int aLen, float* b, int bLen) {
    *r = ArrayEQ(a, aLen, b, bLen);
}

void OpEQ_TT(OpBoolTy* r, const OpVec3* a, int aLen, const OpVec3* b, int bLen) {
    *r = ArrayEQ(a, aLen, b, bLen);
}

void OpEQ_MM(OpBoolTy* r, const OpMatrix4* a, int aLen,
             const OpMatrix4* b, int bLen) {
    *r = ArrayEQ(a, aLen, b, bLen);
}

void OpEQ_SS(OpBoolTy* r, OpStringTy* a, int aLen, OpStringTy* b, int bLen) {
    assert(aLen >= 0 && bLen >= 0 && "No resizable arrays yet");
    assert(aLen == bLen && "Length mismatch in array assignment");
    for (int i = 0; i < aLen; ++i)
        if (strcmp(a[i], b[i])) {
            *r = false;
            return;
        }
    *r = true;
}

void OpExp(float* r, float a) { *r = expf(a); }

// Note: there is no two-argument overloading with an implicit "Ng" argument.
void OpFaceForward(OpVec3* r, const OpVec3& a, const OpVec3& b, 
                       const OpVec3& c) {
    *r = Sign(-b * c) * a;
}

void OpFloor(float* r, float a) { *r = floorf(a); }

void OpGE(OpBoolTy* r, float a, float b) { *r = a >= b; }

void OpGT(OpBoolTy* r, float a, float b) { *r = a > b; }

// For now, we assume that the residual shader has a $geonormals shadeop if Ng
// is passed to the plugin.  This helps us compile the Worley shader as a
// single partition.  Eventually this should be marked as uncompilable.
// Dependency analysis will be able to hoist it, so it won't impede
// partitioning.
void OpGeoNormals(OpVec3* Ng) { }

void OpInverseSqrt(float* r, float x) { *r = 1/sqrt(x); } // TODO slow

void OpLE(OpBoolTy* r, float a, float b) { *r = a <= b; }

void OpLength(float* r, const OpVec3& a) { *r = a.Length(); }

void OpLog_f(float* r, float a) { *r = logf(a); }

void OpLog_ff(float* r, float a, float b) { *r = logf(a) / logf(b); }

void OpLT(OpBoolTy* r, float a, float b) { *r = a < b; }

void OpMatrix(OpMatrix4* r, float a0, float a1, float a2, float a3,
                    float a4, float a5, float a6, float a7,
                    float a8, float a9, float a10, float a11,
                    float a12, float a13, float a14, float a15) {
    *r = OpMatrix4(a0, a1, a2, a3,
                   a4, a5, a6, a7,
                   a8, a9, a10, a11,
                   a12, a13, a14, a15);
}

float Max(float a, float b) { return a > b ? a : b; }

void OpMax_ff(float* r, float a, float b) { *r = Max(a,b); }

void OpMax_tt(OpVec3* r, const OpVec3& a, const OpVec3& b) {
    *r = OpMap2<Max>(a,b); 
}

float Min(float a, float b) { return a < b ? a : b; }

void OpMin_ff(float* r, float a, float b) { *r = Min(a,b); }

void OpMin_tt(OpVec3* r, const OpVec3& a, const OpVec3& b) {
    *r = OpMap2<Min>(a, b); 
}

float Mix(float a, float b, float c) {
    return a * (1-c) + b * c;
}

void OpMix_fff(float* r, float a, float b, float c) {
    *r = Mix(a, b, c);
}

void OpMix_ttf(OpVec3* r, 
               const OpVec3& a, const OpVec3& b, float c) {
    *r = OpVec3(Mix(a[0], b[0], c),
                Mix(a[1], b[1], c),
                Mix(a[2], b[2], c));
}

void OpMod(float* r, float a, float b) { *r = fmodf(a, b); }

/* XXX The variant with named spaces can't be compiled, so this is disabled.
void OpMTransformMx_mm(OpMatrix4* r, const OpMatrix4& m1, const OpMatrix4& m2) {
    *r = m1.Multiply(m2);
}
*/

void OpMultiply_ff(float* r, float a, float b) { *r = a * b; }

void OpMultiply_tt(OpVec3* r, const OpVec3& a, const OpVec3& b) {
    *r = OpVec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]); 
}


void OpMxComp(float* r, const OpMatrix4& a, float b, float c) {
    *r = a[(int) b][(int) c];
}

void OpMxSetComp(OpMatrix4* a, float b, float c, float d) {
    (*a)[(int) b][(int) c] = d;
}

void OpMxRotate(OpMatrix4* r, 
                const OpMatrix4& m, float angle, const OpVec3& axis) {
    float c = cosf(angle);
    float s = cosf(angle);
    OpVec3 A = axis.Normalized();
    float Ax = A[0], Ay = A[1], Az = A[2];
    OpMatrix4 mx
        (c + (1-c)*Ax*Ax,    (1-c)*Ax*Ay - s*Az, (1-c)*Ax*Az + s*Ay,  .0f,
         (1-c)*Ax*Ay + s*Az, c + (1-c)*Ay*Ay,    (1-c)*Ay*Az - s*Ax,  .0f,
         (1-c)*Ax*Az - s*Ay, (1-c)*Ay*Az + s*Ax, c + (1-c)*Az*Az,     .0f,
         0,                  0.0f,               0.0f,               1.0f);
    *r = mx.Multiply(m);
}

void OpMxScale(OpMatrix4* r, const OpMatrix4& m, const OpVec3& s) {
    *r = OpMatrix4(m[0][0]*s[0], m[0][1]*s[1], m[0][2]*s[2], m[0][3],
                   m[1][0]*s[0], m[1][1]*s[1], m[1][2]*s[2], m[1][3],
                   m[2][0]*s[0], m[2][1]*s[1], m[2][2]*s[2], m[2][3],
                   m[3][0]*s[0], m[3][1]*s[1], m[3][2]*s[2], m[3][3]);
}

void OpMxTranslate(OpMatrix4* r, const OpMatrix4& m, const OpVec3& t) {
    *r = OpMatrix4(m[0][0]+m[0][3]*t[0], m[0][1]+m[0][3]*t[1], 
                   m[0][2]+m[0][3]*t[2], m[0][3], 
                   m[1][0]+m[1][3]*t[0], m[1][1]+m[1][3]*t[1], 
                   m[1][2]+m[1][3]*t[2], m[1][3], 
                   m[2][0]+m[2][3]*t[0], m[2][1]+m[2][3]*t[1], 
                   m[2][2]+m[2][3]*t[2], m[2][3], 
                   m[3][0]+m[3][3]*t[0], m[3][1]+m[3][3]*t[1], 
                   m[3][2]+m[3][3]*t[2], m[3][3]);
}

void OpNE_ff(OpBoolTy* r, float a, float b) { *r = a != b; }

void OpNE_mm(OpBoolTy* r, const OpMatrix4& a, const OpMatrix4& b) { *r = a != b; }

void OpNE_tt(OpBoolTy* r, const OpVec3& a, const OpVec3& b) { *r = a != b; }

void OpNE_ss(OpBoolTy* r, OpStringTy a, OpStringTy b) { *r = strcmp(a, b); }

void OpNE_FF(OpBoolTy* r, float* a, int aLen, float* b, int bLen) {
    OpEQ_FF(r, a, aLen, b, bLen);
    *r = !*r;
}

void OpNE_TT(OpBoolTy* r, const OpVec3* a, int aLen, const OpVec3* b, int bLen) {
    OpEQ_TT(r, a, aLen, b, bLen);
    *r = !*r;
}

void OpNE_MM(OpBoolTy* r, const OpMatrix4* a, int aLen,
             const OpMatrix4* b, int bLen) {
    OpEQ_MM(r, a, aLen, b, bLen);
    *r = !*r;
}

void OpNE_SS(OpBoolTy* r, OpStringTy* a, int aLen, OpStringTy* b, int bLen) {
    OpEQ_SS(r, a, aLen, b, bLen);
    *r = !*r;
}

void OpNegate_f(float* r, float a) { *r = -a; }

void OpNegate_t(OpVec3* r, const OpVec3& a) { *r = -a; }

void OpNoise_ff(float* r, float a) {
    RxNoise(1, &a, 1, r);
}

void OpNoise_tf(OpVec3* r, float a) {
    RxNoise(1, &a, 3, r->AsFloats());
}

void OpNoise_fff(float* r, float a, float b) {
    float in[2] = {a, b};
    RxNoise(2, in, 1, r);
}

void OpNoise_tff(OpVec3* r, float a, float b) {
    float in[2] = {a, b};
    RxNoise(2, in, 3, r->AsFloats());
}

void OpNoise_ft(float* r, const OpVec3& a) {
    RxNoise(3, const_cast<float*>(a.AsFloats()), 1, r);
}

void OpNoise_tt(OpVec3* r, const OpVec3& a) {
    RxNoise(3, const_cast<float*>(a.AsFloats()), 3, r->AsFloats());
}

void OpNoise_ftf(float* r, const OpVec3& a, float b) {
    float in[4] = {a[0], a[1], a[2], b};
    RxNoise(4, in, 1, r);
}

void OpNoise_ttf(OpVec3* r, const OpVec3& a, float b) {
    float in[4] = {a[0], a[1], a[2], b};
    RxNoise(4, in, 3, r->AsFloats());
}

void OpNormalize(OpVec3* r, const OpVec3& a) {
    *r = a.Normalized();
}

void OpOr(OpBoolTy* r, OpBoolTy a, OpBoolTy b) { *r = a || b; }

void OpPNoise_fff(float* r, float a, float p) {
    RxPNoise(1, &a, &p, 1, r);
}

void OpPNoise_tff(OpVec3* r, float a, float p) {
    RxPNoise(1, &a, &p, 3, r->AsFloats());
}

void OpPNoise_fffff(float* r, float a1, float a2, float p1, float p2) {
    float in[2] = {a1, a2};
    float period[2] = {p1, p2};
    RxPNoise(2, in, period, 1, r);
}

void OpPNoise_tffff(OpVec3* r, float a1, float a2, float p1, float p2) {
    float in[2] = {a1, a2};
    float period[2] = {p1, p2};
    RxPNoise(2, in, period, 3, r->AsFloats());
}

void OpPNoise_ftt(float* r, const OpVec3& a, const OpVec3& p) {
    RxPNoise(3, const_cast<float*>(a.AsFloats()), 
             const_cast<float*>(p.AsFloats()), 1, r);
}

void OpPNoise_ttt(OpVec3* r, const OpVec3& a, const OpVec3& p) {
    RxPNoise(3, const_cast<float*>(a.AsFloats()),
             const_cast<float*>(p.AsFloats()), 3, r->AsFloats());
}

void OpPNoise_ftftf(float* r, const OpVec3& a1, float a2, 
                    const OpVec3& p1, float p2) {
    float in[4] = {a1[0], a1[1], a1[2], a2};
    float period[4] = {p1[0], p1[1], p1[2], p2};
    RxPNoise(4, in, period, 1, r);
}

void OpPNoise_ttftf(OpVec3* r, const OpVec3& a1, float a2,
                    const OpVec3& p1, float p2) {
    float in[4] = {a1[0], a1[1], a1[2], a2};
    float period[4] = {p1[0], p1[1], p1[2], p2};
    RxPNoise(4, in, period, 3, r->AsFloats());
}

void OpPoint(OpVec3* r, float a, float b, float c) {
    *r = OpVec3(a, b, c); 
}

void OpPow(float* r, float a, float b) { *r = powf(a, b); }

void OpPrint_f(float a) { 
    printf("1 value:\n  0:%.6f\n", a);
}

void OpPrint_s(OpStringTy a) {
    printf("1 value:\n  0:%s\n", a);
}

void OpPrint_t(const OpVec3& a) {
    printf("1 value:\n  0:%.6f %.6f %.6f\n", a[0], a[1], a[2]);
}

void OpPrint_m(const OpMatrix4& a) {
    printf("1 value:\n  0:"
           "%.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f "
           "%.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f\n",
           a[0][0], a[0][1], a[0][2], a[0][3], 
           a[1][0], a[1][1], a[1][2], a[1][3], 
           a[2][0], a[2][1], a[2][2], a[2][3], 
           a[3][0], a[3][1], a[3][2], a[3][3]);
}

void OpPrintf(OpStringTy, ...); // TODO: special format string


void OpPtLineD(float* r, const OpVec3& p1, const OpVec3& p2,
                const OpVec3& q); // TODO

void OpRadians(float* r, float a) { *r = a * (PI / 180.0f); }

void OpReflect(OpVec3* r, const OpVec3& I, const OpVec3& N) {
    *r = I - 2 * (I*N) * N;
}

void OpRefract(OpVec3* r, 
               const OpVec3& I, const OpVec3& N, float eta) {
    float IdotN = I * N;
    float k = 1.0f - eta*eta*(1.0f - IdotN*IdotN);
    *r = (k < 0) ? OpVec3(0.0f) : eta * I - (eta*IdotN + sqrtf(k)) * N;
}

// Rotate a point Q by angle radians about the axis that passes through the
// points P1 and P2.
void OpRotate(OpVec3* r, const OpVec3& q, float angle, const OpVec3& p1,
              const OpVec3& p2) {
    float c = cosf(angle);
    float s = cosf(angle);
    OpVec3 A = (p2 - p1).Normalized();
    float Ax = A[0], Ay = A[1], Az = A[2];
    OpMatrix3 m(c + (1-c)*Ax*Ax,     (1-c)*Ax*Ay - s*Az,  (1-c)*Ax*Az + s*Ay,
                (1-c)*Ax*Ay + s*Az,  c + (1-c)*Ay*Ay,     (1-c)*Ay*Az - s*Ax,
                (1-c)*Ax*Az - s*Ay,  (1-c)*Ay*Az + s*Ax,  c + (1-c)*Az*Az);
    *r = m * q; // XXX add p1?
}

void OpRound(float* r, float a) { *r = roundf(a); }

void OpScale_ft(OpVec3* r, float a, const OpVec3& b) { 
    *r = a * b;
}

void OpScale_tf(OpVec3* r, const OpVec3& a, float b) {
    *r = a * b;
}

void OpSetComp(OpVec3* a, float b, float c) {
    (*a)[(int) b] = c;
}

void OpSetXComp(OpVec3* a, float b) { (*a)[0] = b; }

void OpSetYComp(OpVec3* a, float b) { (*a)[1] = b; }

void OpSetZComp(OpVec3* a, float b) { (*a)[2] = b; }

float Sign(float a) { 
    return a < 0.0f ? -1.0f : (a > 0.0f ? 1.0f : 0.0f); 
}

void OpSign(float* r, float a) { 
    *r = Sign(a);
}

void OpSin(float* r, float a) { *r = sinf(a); }

void OpSmoothStep(float* r, float min, float max, float value) {
    float t = OpClamp((value - min) / (max - min), 0.0f, 1.0f);
    *r = t * t * (3.0f - 2.0f * t);
}

// TODO: spline

void OpSqrt(float* r, float a) { 
    *r = a < 0.0f ? 0.0f : sqrtf(a); 
}

void OpStep(float* r, float min, float value) {
    *r = value < min ? 0.0f : 1.0f;
}

void OpSubtract_ff(float* r, float a, float b) {
    *r = a - b; 
}

void OpSubtract_ft(OpVec3* r, float a, const OpVec3& b) { 
    *r = OpVec3(a) - b;
}

void OpSubtract_tf(OpVec3* r, const OpVec3& a, float b) { 
    *r = a - OpVec3(b);
}

void OpSubtract_tt(OpVec3* r, const OpVec3& a, const OpVec3& b) { 
    *r = a - b;
}

void OpTan(float* r, float a) { *r = tanf(a); }

/* XXX The variant with named spaces can't be compiled, so these are disabled.
void OpTransformMx_mt(OpVec3* r, const OpMatrix4& m, const OpVec3& t) {
    *r = Transform(m, OpVec4(t, 1.0f));
}

void OpVTransformMx_mt(OpVec3* r, const OpMatrix4& m, const OpVec3& t) {
    *r = Transform(m, OpVec4(t, 0.0f));
}
*/

void OpXComp(float* r, const OpVec3& a) { *r = a[0]; }

void OpYComp(float* r, const OpVec3& a) { *r = a[1]; }

void OpZComp(float* r, const OpVec3& a) { *r = a[2]; }

} // extern "C"
