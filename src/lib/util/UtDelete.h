// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef UT_DELETE_H
#define UT_DELETE_H

/// Delete the elements of a sequence (e.g. std::list or std::vector).
template<typename Seq>
void
UtDeleteSeq(const Seq& seq)
{
    typename Seq::const_iterator it;
    for (it = seq.begin(); it != seq.end(); ++it)
        delete *it;
}

/// Delete the elements of a sequence (e.g. std::list or std::vector), and
/// then delete the sequence itself.
template<typename Seq>
void
UtDeleteSeq(Seq* seq)
{
    if (seq) {
        UtDeleteSeq(*seq);
        delete seq;
    }
}


#endif // ndef UT_DELETE_H
