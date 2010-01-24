// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef OP_VEC4_H
#define OP_VEC4_H

#include "ops/OpVec3.h"
#include <assert.h>
#include <math.h>
#include <iosfwd>

/// Vector of three floats.
class OpVec4 {
protected:
    float n[4];

public:
    /// Construct uninitialized vector.
    OpVec4() { }
    
    /// Construct from one float.
    explicit OpVec4(float x) {
        n[0] = x; n[1] = x; n[2] = x; n[3] = x;
    }

    /// Construct from floats.
    OpVec4(float x, float y, float z, float w) {
        n[0] = x; n[1] = y; n[2] = z; n[3] = w;
    }

    /// Construct from smaller vector.
    OpVec4(const OpVec3& v, float f) {
        n[0] = v[0]; n[1] = v[1]; n[2] = v[2]; n[3] = f;
    }

    /// Construct from vector.
    OpVec4(const OpVec4& v) { 
        n[0] = v[0]; n[1] = v[1]; n[2] = v[2]; n[3] = v[3];
    }

    /// Assignment.
    OpVec4& operator=(const OpVec4& v) {
        n[0] = v[0]; n[1] = v[1]; n[2] = v[2]; n[3] = v[3];
        return *this;
    }

    /// Get reference to vector element.
    float& operator[](unsigned int i) {
        assert(i < 4 && "Vector index out of range");
        return n[i];
    }

    /// Get vector element.
    float operator[](unsigned int i) const {
        assert(i < 4 && "Vector index out of range");
        return n[i];
    }

    /// Exact equality
    bool operator==(const OpVec4& v) const {
        return n[0] == v[0] && n[1] == v[1] && n[2] == v[2] && n[3] == v[3];
    }

    /// Exact inequality
    bool operator!=(const OpVec4& v) const {
        return !(*this == v);
    }

    /// Dot product
    float operator*(const OpVec4& v) const {
        return n[0]*v[0] + n[1]*v[1] + n[2]*v[2] + n[3]*v[3];
    }

    /// Sum vectors
    OpVec4 operator+(const OpVec4& v) const {
        return OpVec4(n[0]+v[0], n[1]+v[1], n[2]+v[2], n[3]+v[3]);
    }

    /// Difference between vectors
    OpVec4 operator-(const OpVec4& v) const {
        return OpVec4(n[0]-v[0], n[1]-v[1], n[2]-v[2], n[3]-v[3]);
    }

    /// Negate vector
    OpVec4 operator-() const {
        return OpVec4(-n[0], -n[1], -n[2], -n[3]);
    }

    /// Scale vector by a float.
    OpVec4 operator*(float f) const {
        return OpVec4(n[0]*f, n[1]*f, n[2]*f, n[3]*f);
    }

    /// Divide vector by a float (using multiply by reciprocal)/
    OpVec4 operator/(float f) const {
        return *this * (1.0f / f);
    }

    /// In-place add.
    void operator+=(const OpVec4& v) {
        n[0] += v[0]; n[1] += v[1]; n[2] += v[2]; n[3] += v[3];
    }

    /// In-place subtract.
    void operator-=(const OpVec4& v) {
        n[0] -= v[0]; n[1] -= v[1]; n[2] -= v[2]; n[3] -= v[3];
    }

    /// In-place multiply.
    void operator*=(float f) {
        n[0] *= f; n[1] *= f; n[2] *= f; n[3] *= f;
    }

    /// In-place divide (using multiply by reciprocal).
    void operator/=(float f) {
        *this *= (1.0f / f);
    }

    /// Get vector as const float array.
    const float* AsFloats() const { return n; }

    /// Get vector as float array.
    float* AsFloats() { return n; }
};

/// Left-scale vector by a float.
inline OpVec4 operator*(float f, const OpVec4& v) { return v * f; }

/// Print a 4-vector
std::ostream& operator<<(std::ostream& out, const OpVec4& v);

#endif // ndef OP_VEC4_H
