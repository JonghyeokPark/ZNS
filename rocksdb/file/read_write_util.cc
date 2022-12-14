//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "file/read_write_util.h"

#include <sstream>
#include "test_util/sync_point.h"

namespace ROCKSDB_NAMESPACE {

IOStatus NewWritableFile(FileSystem* fs, const std::string& fname,
                         std::unique_ptr<FSWritableFile>* result,
                         const FileOptions& options) {
  IOStatus s = fs->NewWritableFile(fname, options, result, nullptr);
  TEST_KILL_RANDOM("NewWritableFile:0", rocksdb_kill_odds * REDUCE_ODDS2);
  return s;
}

IOStatus NewWritableLeveledFile(Env* env,
		       const std::string& fname,
                       std::unique_ptr<FSWritableFile>* result,
                       const FileOptions& options, int level) {

    Status s = env->NewWritableLeveledFile(fname,
	  reinterpret_cast<std::unique_ptr<rocksdb::WritableFile> *>(result),
	  options, level);

  TEST_KILL_RANDOM("NewWritableLeveledFile:0", rocksdb_kill_odds * REDUCE_ODDS2);

  return *(reinterpret_cast<IOStatus *>(&s));
}

#ifndef NDEBUG
bool IsFileSectorAligned(const size_t off, size_t sector_size) {
  return off % sector_size == 0;
}
#endif  // NDEBUG
}  // namespace ROCKSDB_NAMESPACE
