// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef OP_MATRIX3_H
#define OP_MATRIX3_H

#include "ops/OpVec3.h"
#include <assert.h>
#include <iosfwd>

/// 3x3 float matrix.
class OpMatrix3 {
protected:
    OpVec3 v[3];

public:
    /// Construct uninitialized matrix.
    OpMatrix3() { }

    /// Construct identity matrix
    explicit OpMatrix3(float x) { 
        v[0] = OpVec3(  x, .0f, .0f);
        v[1] = OpVec3(.0f,   x, .0f);
        v[2] = OpVec3(.0f, .0f,   x);
    }

    /// Construct from vectors
    OpMatrix3(const OpVec3& v0, const OpVec3& v1, const OpVec3& v2) {
        v[0] = v0; v[1] = v1; v[2] = v2;
    }

    /// Construct from floats.
    OpMatrix3(float a0, float a1, float a2, 
              float a3, float a4, float a5, 
              float a6, float a7, float a8)
    {
        v[0] = OpVec3(a0, a1, a2);
        v[1] = OpVec3(a3, a4, a5);
        v[2] = OpVec3(a6, a7, a8);
    }

    /// Copy constructor
    OpMatrix3(const OpMatrix3& m) {
        v[0] = m[0]; v[1] = m[1]; v[2] = m[2];
    }

    /// Assignment.
    OpMatrix3& operator=(const OpMatrix3& m) {
        v[0] = m[0]; v[1] = m[1]; v[2] = m[2];
        return *this;
    }

    /// Get a row vector.
    const OpVec3& operator[](unsigned int i) const { 
        assert(i < 3 && "Matrix index out of range");
        return v[i]; 
    }

    /// Get a non-const reference to a row vector.
    OpVec3& operator[](unsigned int i) {
        assert(i < 3 && "Matrix index out of range");
        return v[i]; 
    }

    /// Get a column vector.
    OpVec3 GetCol(unsigned int j) const {
        assert(j < 3 && "Matrix index out of range");
        return OpVec3(v[0][j], v[1][j], v[2][j]);
    }

    /// Exact equality
    bool operator==(const OpMatrix3& m) const {
        return v[0] == m[0] && v[1] == m[1] && v[2] == m[2];
    }

    /// Exact inequality
    bool operator!=(const OpMatrix3& m) const {
        return !(*this == m);
    }

    /// Transform vector
    OpVec3 operator*(const OpVec3& vec) const {
        return OpVec3(v[0]*vec, v[1]*vec, v[2]*vec);
    }

    /// Return transposed matrix.
    OpMatrix3 Transposed() const {
        return OpMatrix3(v[0][0], v[1][0], v[2][0],
                         v[0][1], v[1][1], v[2][1],
                         v[0][2], v[1][2], v[2][2]);
    }

    // Multiply matrices.
    OpMatrix3 Multiply(const OpMatrix3& m) const {
        OpMatrix3 r;
        for (unsigned int i = 0; i < 3; ++i) 
            for (unsigned int j = 0; j < 3; ++j)
                r[i][j] = v[i] * m.GetCol(j);
        return r;
    }

    float Determinant() const
    {
        return v[0][0] * (v[1][1]*v[2][2] - v[1][2]*v[2][1])
             - v[0][1] * (v[1][0]*v[2][2] - v[1][2]*v[2][0])
             + v[0][2] * (v[1][0]*v[2][1] - v[1][1]*v[2][0]);
    }
};

/// Print a matrix
std::ostream& operator<<(std::ostream& out, const OpMatrix3& m);

#endif // ndef OP_MATRIX3_H
