// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_VAR_SET_H
#define IR_VAR_SET_H

#include "ir/IRTypedefs.h"
#include "ir/IRVar.h"
#include "util/UtCast.h"
#include "util/UtVector.h"
#include <iosfwd>
#include <set>
class IRValue;

/// A set of IR variables.
class IRVarSet {
public:
    /// Construct an empty set.
    IRVarSet() { }

    /// Check whether the set is empty.
    bool IsEmpty() const { return mVars.empty(); }

    /// Check whether the specified variable is a member of this set
    /// (based on pointer equality).
    bool Has(const IRVar* var) const {
        return mVars.find(const_cast<IRVar*>(var)) != mVars.end();
    }

    /// If the given value is a variable, add it to this set.
    void operator+=(IRValue* value) {
        if (IRVar* var = UtCast<IRVar*>(value))
            mVars.insert(var); 
    }

    /// Union the given set into this one.
    void operator+=(const IRVarSet& src);

    /// Add a vector of variables or values to this set.
    template<typename T>
    void operator+=(const UtVector<T>& vec) {
        typename UtVector<T>::const_iterator it;
        for (it = vec.begin(); it != vec.end(); ++it)
            *this += *it;
    }

    /// Remove a variable from this set.
    void operator-=(const IRVar* var) { 
        mVars.erase(const_cast<IRVar*>(var)); 
    }

    /// Intersect this set with the given one.
    void Intersect(const IRVarSet& src);

    /// Copy the variable set to a vector, in sorted order for
    /// reproducibility.
    void GetSorted(IRVars* vars) const;

    /// Print a variable set.
    friend std::ostream& operator<<(std::ostream& out, const IRVarSet& set);

private:
    typedef std::set<IRVar*> Vars;
    Vars mVars;
};

#endif // ndef IR_VAR_SET_H
