// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRVarSet.h"
#include "ir/IRVar.h"
#include "util/UtCast.h"
#include <algorithm>
#include <string.h>

// Union the given set into this one.
void 
IRVarSet::operator+=(const IRVarSet& src)
{
    std::set<IRVar*> dest;
    std::set_union(mVars.begin(), mVars.end(),
                   src.mVars.begin(), src.mVars.end(),
                   std::inserter(dest, dest.begin()));
    mVars.swap(dest);
}

// Intersect this set with the given one.
void 
IRVarSet::Intersect(const IRVarSet& src)
{
    std::set<IRVar*> dest;
    std::set_intersection(mVars.begin(), mVars.end(),
                          src.mVars.begin(), src.mVars.end(),
                          std::inserter(dest, dest.begin()));
    mVars.swap(dest);
}

bool 
OrderByName(IRVar* v1, IRVar* v2)
{
    return strcasecmp(v1->GetShortName(), v2->GetShortName()) < 0;
}

// Copy the variable set to a vector, in sorted order for reproducibility.
void 
IRVarSet::GetSorted(IRVars* vars) const
{
    vars->insert(vars->end(), mVars.begin(), mVars.end());
    std::sort(vars->begin(), vars->end(), OrderByName);
}

std::ostream& 
operator<<(std::ostream& out, const IRVarSet& set)
{
    // We print them in sorted order for unit test baseline stability.
    IRVars vars;
    set.GetSorted(&vars);
    out << "{";
    IRVars::const_iterator it;
    for (it = vars.begin(); it != vars.end(); ++it) {
        if (it != vars.begin()) {
            out << ", ";
        }
        out << (*it)->GetShortName();
    }
    out << "}";
    return out;
}

