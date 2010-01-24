// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef OP_VEC3_H
#define OP_VEC3_H

#include <assert.h>
#include <math.h>
#include <iosfwd>

/// Vector of three floats.
class OpVec3 {
protected:
    float n[3];

public:
    /// Construct uninitialized vector.
    OpVec3() { }
    
    /// Construct from one float.
    explicit OpVec3(float x) {
        n[0] = x; n[1] = x; n[2] = x;
    }

    /// Construct from floats.
    OpVec3(float x, float y, float z) {
        n[0] = x; n[1] = y; n[2] = z;
    }

    /// Construct from array of floats.
    OpVec3(float x[3]) {
        n[0] = x[0]; n[1] = x[1]; n[2] = x[2];
    }

    /// Copy constructor.
    OpVec3(const OpVec3& v) { 
        n[0] = v[0]; n[1] = v[1]; n[2] = v[2];
    }

    /// Assignment.
    OpVec3& operator=(const OpVec3& v) {
        n[0] = v[0]; n[1] = v[1]; n[2] = v[2];
        return *this;
    }

    /// Get reference to vector element.
    float& operator[](unsigned int i) {
        assert(i < 3 && "Vector index out of range");
        return n[i];
    }

    /// Get vector element.
    float operator[](unsigned int i) const {
        assert(i < 3 && "Vector index out of range");
        return n[i];
    }

    /// Exact equality
    bool operator==(const OpVec3& v) const {
        return n[0] == v[0] && n[1] == v[1] && n[2] == v[2];
    }

    /// Exact inequality
    bool operator!=(const OpVec3& v) const {
        return !(*this == v);
    }

    /// Dot product
    float operator*(const OpVec3& v) const {
        return n[0]*v[0] + n[1]*v[1] + n[2]*v[2];
    }

    /// Sum vectors
    OpVec3 operator+(const OpVec3& v) const {
        return OpVec3(n[0]+v[0], n[1]+v[1], n[2]+v[2]);
    }

    /// Difference between vectors
    OpVec3 operator-(const OpVec3& v) const {
        return OpVec3(n[0]-v[0], n[1]-v[1], n[2]-v[2]);
    }

    /// Negate vector
    OpVec3 operator-() const {
        return OpVec3(-n[0], -n[1], -n[2]);
    }

    /// Scale vector by a float.
    OpVec3 operator*(float f) const {
        return OpVec3(n[0]*f, n[1]*f, n[2]*f);
    }

    /// Divide vector by a float (using multiply by reciprocal)/
    OpVec3 operator/(float f) const {
        return *this * (1.0f / f);
    }

    /// Compute length.
    float Length() const {
        const OpVec3& v = *this;
        return sqrtf(v * v);
    }

    /// Compute normalized vector.
    OpVec3 Normalized() const {
        float length = Length();
        return length != 0.0f ? *this / Length() : OpVec3(0.0f);
    }

    /// Compute cross product of two vectors.
    OpVec3 Cross(const OpVec3& v) const {
        return OpVec3(n[1] * v[2] - v[1] * n[2],
                      n[2] * v[0] - v[2] * n[0],
                      n[0] * v[1] - v[0] * n[1]);
    }

    /// Get vector as const float array.
    const float* AsFloats() const { return n; }

    /// Get vector as float array.
    float* AsFloats() { return n; }
};

/// Unary float function.
typedef float (*FloatFunc)(float);

/// Binary float function.
typedef float (*Float2Func)(float, float);

/// Ternary float function.
typedef float (*Float3Func)(float, float, float);

/// Map unary function over a vector.
template<FloatFunc f>
OpVec3 OpMap(const OpVec3& v) {
    return OpVec3(f(v[0]), f(v[1]), f(v[2]));
}

/// Map binary function over two vectors.
template<Float2Func f>
OpVec3 OpMap2(const OpVec3& v1, const OpVec3& v2) {
    return OpVec3(f(v1[0], v2[0]), f(v1[1], v2[1]), f(v1[2], v2[2]));
}

/// Map ternary function over three vectors.
template<Float3Func f>
OpVec3 OpMap3(const OpVec3& v1, const OpVec3& v2, const OpVec3& v3) {
    return OpVec3(f(v1[0], v2[0], v3[0]),
                  f(v1[1], v2[1], v3[0]),
                  f(v1[2], v2[2], v3[0]));
}

/// Left-scale vector by a float.
inline OpVec3 operator*(float f, const OpVec3& v) { return v * f; }

/// Print a 3-vector
std::ostream& operator<<(std::ostream& out, const OpVec3& v);

#endif // ndef OP_VEC3_H
