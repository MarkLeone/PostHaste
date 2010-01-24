// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_TYPE_H
#define IR_TYPE_H

#include <iosfwd>

/// Type kinds.
enum IRTypeKind {
    kIRVoidTy = 0,
    kIRBoolTy,
    kIRFloatTy,
    kIRPointTy,
    kIRVectorTy,
    kIRNormalTy,
    kIRColorTy,
    kIRMatrixTy,
    kIRStringTy,
    kIRShaderTy,
    kIRArrayTy,
    kIRStructTy,
    kIRNumTypeKinds,
};

/// Convert type kind to string.
const char* IRTypeKindToString(IRTypeKind kind);

/// IRType represents basic types such as float, point, string, etc.
/// Derived classes represent array and struct types.
class IRType {
public:
    /// What kind of type is this?
    IRTypeKind GetKind() const { return mKind; }

    /// Returns true if this type is void.
    bool IsVoid() const { return mKind == kIRVoidTy; }
    /// Returns true if this is a bool.
    bool IsBool() const { return mKind == kIRBoolTy; }
    /// Returns true if this is a float.
    bool IsFloat() const { return mKind == kIRFloatTy; }
    /// Returns true if this is a point.
    bool IsPoint() const { return mKind == kIRPointTy; }
    /// Returns true if this is a vector.
    bool IsVector() const { return mKind == kIRVectorTy; }
    /// Returns true if this is a normal.
    bool IsNormal() const { return mKind == kIRNormalTy; }
    /// Returns true if this is a color.
    bool IsColor() const { return mKind == kIRColorTy; }
    /// Returns true if this is a matrix.
    bool IsMatrix() const { return mKind == kIRMatrixTy; }
    /// Returns true if this is a string.
    bool IsString() const { return mKind == kIRStringTy; }
    /// Returns true if this is a shader.
    bool IsShader() const { return mKind == kIRShaderTy; }
    /// Returns true if this is a array.
    bool IsArray() const { return mKind == kIRArrayTy; }
    /// Returns true if this is a struct.
    bool IsStruct() const { return mKind == kIRStructTy; }

    /// Returns true if this is a triple (point, vector, normal, or color).
    bool IsTriple() const 
    { 
        return IsPoint() || IsVector() || IsNormal() || IsColor();
    }

    /// Returns true if this is a numeric type (float, triple, or
    /// matrix).  Returns false for arrays.
    bool IsNumeric() const 
    {
        switch (mKind) {
          case kIRFloatTy:
          case kIRPointTy:
          case kIRVectorTy:
          case kIRNormalTy:
          case kIRColorTy:
          case kIRMatrixTy:
              return true;
          default:
              return false;
        }
    }

    /// Get the size (in words) of this type.
    unsigned int GetSize() const;

    /// Check if a type is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRType* type) { 
        return true;  // Every IRType is an instance of this class.
    }

protected:
    /// Types are constructed only by the IRTypes factory.
    friend class IRTypes;
    friend class IRStaticTypes;
  
    /// Construct an IRType.
    IRType(IRTypeKind kind) : mKind(kind) { }

private:
    IRTypeKind mKind;
};

/// Print a type.
std::ostream& operator<<(std::ostream& out, const IRType& ty);

#endif // ndef IR_TYPE_H
