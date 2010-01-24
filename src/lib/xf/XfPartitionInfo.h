// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef XF_PARTITION_INFO_H
#define XF_PARTITION_INFO_H

class IRShader;

/// Report statistics about partitions constructe by XfPartition.
void XfPartitionInfo(const IRShader* shader, bool printReport);

#endif // ndef XF_PARTITION_INFO_H
