// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef OP_MATRIX4_H
#define OP_MATRIX4_H

#include "ops/OpVec4.h"
#include <assert.h>
#include <iosfwd>

/// 4x4 float matrix.
class OpMatrix4 {
protected:
    OpVec4 v[4];

public:
    /// Construct uninitialized matrix.
    OpMatrix4() { }

    /// Construct identity matrix
    explicit OpMatrix4(float x) { 
        v[0] = OpVec4(  x, .0f, .0f, .0f);
        v[1] = OpVec4(.0f,   x, .0f, .0f);
        v[2] = OpVec4(.0f, .0f,   x, .0f);
        v[3] = OpVec4(.0f, .0f, .0f,   x);
    }

    /// Construct from vectors
    OpMatrix4(const OpVec4& v0, const OpVec4& v1,
             const OpVec4& v2, const OpVec4& v3) {
        v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;
    }

    /// Construct from floats.
    OpMatrix4(float a0, float a1, float a2, float a3,
              float a4, float a5, float a6, float a7,
              float a8, float a9, float a10, float a11,
              float a12, float a13, float a14, float a15) 
    {
        v[0] = OpVec4(a0, a1, a2, a3);
        v[1] = OpVec4(a4, a5, a6, a7);
        v[2] = OpVec4(a8, a9, a10, a11);
        v[3] = OpVec4(a12, a13, a14, a15);
    }

    /// Copy constructor
    OpMatrix4(const OpMatrix4& m) {
        v[0] = m[0]; v[1] = m[1]; v[2] = m[2]; v[3] = m[3];
    }

    /// Assignment.
    OpMatrix4& operator=(const OpMatrix4& m) {
        v[0] = m[0]; v[1] = m[1]; v[2] = m[2]; v[3] = m[3];
        return *this;
    }

    /// Get a row vector.
    const OpVec4& operator[](unsigned int i) const { 
        assert(i < 4 && "Matrix index out of range");
        return v[i]; 
    }

    /// Get a non-const reference to a row vector.
    OpVec4& operator[](unsigned int i) {
        assert(i < 4 && "Matrix index out of range");
        return v[i]; 
    }

    /// Get a column vector.
    OpVec4 GetCol(unsigned int j) const {
        assert(j < 4 && "Matrix index out of range");
        return OpVec4(v[0][j], v[1][j], v[2][j], v[3][j]);
    }

    /// Exact equality
    bool operator==(const OpMatrix4& m) const {
        return v[0] == m[0] && v[1] == m[1] && v[2] == m[2] && v[3] == m[3];
    }

    /// Exact inequality
    bool operator!=(const OpMatrix4& m) const {
        return !(*this == m);
    }

    /// Return transposed matrix.
    OpMatrix4 Transposed() const {
        return OpMatrix4(v[0][0], v[1][0], v[2][0], v[3][0],
                         v[0][1], v[1][1], v[2][1], v[3][1],
                         v[0][2], v[1][2], v[2][2], v[3][2],
                         v[0][3], v[1][3], v[2][3], v[3][3]);
    }

    // Multiply matrices.
    OpMatrix4 Multiply(const OpMatrix4& m) const {
        OpMatrix4 r;
        for (unsigned int i = 0; i < 4; ++i) 
            for (unsigned int j = 0; j < 4; ++j)
                r[i][j] = v[i] * m.GetCol(j);
        return r;
    }

    /// Compute matrix invers using Gauss-Jordan elimination with
    /// partial pivoting.  Based on code by Jean-Francois Doue in:
    ///     "Graphics Gems IV / Edited by Paul S. Heckbert
    ///     Academic Press, 1994, ISBN 0-12-336156-9
    ///     "You are free to use and modify this code in any way 
    ///     you like." (p. xv)
    ///     As modified by J. Nagle, March 1997
    ///     http://www.animats.com/source/graphics/algebra3.h
    OpMatrix4 Inverse() const 
    {
        OpMatrix4 a(*this);    // As a evolves from original mat into identity
        OpMatrix4 b(1.0f);     // b evolves from identity into inverse(a)
        int i, j, i1;

        // Loop over cols of a from left to right, eliminating above and below
        // diag
        for (j=0; j<4; j++) { // Find largest pivot in column j among rows j..3
            i1 = j;           // Row with largest pivot candidate
            for (i=j+1; i<4; i++)
                if (fabs(a[i][j]) > fabs(a[i1][j]))
                    i1 = i;

            // Swap rows i1 and j in a and b to put pivot on diagonal
            OpVec4 tmp = a[i1];
            a[i1] = a[j];
            a[j] = tmp;
            tmp = b[i1];
            b[i1] = b[j];
            b[j] = tmp;

            // Scale row j to have a unit diagonal
            if (a[j][j]==0.) {
                assert(false && "inverse: singular matrix");
                return OpMatrix4(1.0f);
            }
            b[j] /= a[j][j];
            a[j] /= a[j][j];

            // Eliminate off-diagonal elems in col j of a, doing identical ops
            // to b
            for (i=0; i<4; i++)
                if (i!=j) {
                    b[i] -= a[i][j]*b[j];
                    a[i] -= a[i][j]*a[j];
                }
        }
        return b;
    }

    // Public for testing.
    static float Det3x3(float m00, float m01, float m02,
                        float m10, float m11, float m12,
                        float m20, float m21, float m22)
    {
        return m00 * (m11*m22 - m12*m21)
             - m01 * (m10*m22 - m12*m20)
             + m02 * (m10*m21 - m11*m20);
    }

    float Determinant() const
    {
        float m00 = v[0][0], m01 = v[0][1], m02 = v[0][2], m03 = v[0][3],
              m10 = v[1][0], m11 = v[1][1], m12 = v[1][2], m13 = v[1][3],
              m20 = v[2][0], m21 = v[2][1], m22 = v[2][2], m23 = v[2][3],
              m30 = v[3][0], m31 = v[3][1], m32 = v[3][2], m33 = v[3][3];
        float c00 = Det3x3(m11, m12, m13,
                           m21, m22, m23,
                           m31, m32, m33);
        float c01 = Det3x3(m10, m12, m13,
                           m20, m22, m23,
                           m30, m32, m33);
        float c02 = Det3x3(m10, m11, m13,
                           m20, m21, m23,
                           m30, m31, m33);
        float c03 = Det3x3(m10, m11, m12,
                           m20, m21, m22,
                           m30, m31, m32);
        return m00*c00 - m01*c01 + m02*c02 - m03*c03;
    }
};

/// Print a matrix
std::ostream& operator<<(std::ostream& out, const OpMatrix4& m);

#endif // ndef OP_MATRIX4_H
