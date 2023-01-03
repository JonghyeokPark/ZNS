//
// Created by wujy on 11/3/18.
//

#ifndef YCSB_C_ROCKSDB_CONFIG_H
#define YCSB_C_ROCKSDB_CONFIG_H

#include "db.h"
#include "rocksdb/db.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstddef>
#include <string>

using std::string;

namespace ycsbc {
class ConfigRocksDB {
private:
  boost::property_tree::ptree pt_;
  int bloomBits_;
  bool seekCompaction_;
  bool compression_;
  bool directIO_;
  bool noCompaction_;
  int numThreads_;
  int gcThreads_;
  int maxSortedRuns_;
  int sizeRatio_;
  size_t blockCache_;
  size_t gcSize_;
  size_t memtable_;
  size_t maxFileSize_;
  size_t smallThresh_;
  size_t midThresh_;
  bool preheat_;
  int gcLevel_;
  int mergeLevel_;
  bool runGC_;
  bool gcWB_;
  bool tiered_;
  bool levelMerge_;
  bool rangeMerge_;
  bool sep_before_flush_;
  double GCRatio_;
  uint64_t blockWriteSize_;
  bool disableWAL_;
  int memtableNum_;

public:
  ConfigRocksDB(){};
  void init(const string dbConfig) {
    boost::property_tree::ini_parser::read_ini(dbConfig, pt_);
    bloomBits_ = pt_.get<int>("config.bloomBits");
    seekCompaction_ = pt_.get<bool>("config.seekCompaction");
    compression_ = pt_.get<bool>("config.compression");
    directIO_ = pt_.get<bool>("config.directIO");
    blockCache_ = pt_.get<size_t>("config.blockCache");
    sizeRatio_ = pt_.get<int>("config.sizeRatio");
    gcSize_ = pt_.get<size_t>("config.gcSize");
    memtable_ = pt_.get<size_t>("config.memtable");
    maxFileSize_ = pt_.get<size_t>("config.maxFileSize");
    noCompaction_ = pt_.get<bool>("config.noCompaction");
    numThreads_ = pt_.get<int>("config.numThreads");
    smallThresh_ = pt_.get<size_t>("config.smallThresh");
    midThresh_ = pt_.get<size_t>("config.midThresh");
    preheat_ = pt_.get<bool>("config.preheat");
    gcLevel_ = pt_.get<int>("config.gcLevel");
    mergeLevel_ = pt_.get<int>("config.mergeLevel");
    runGC_ = pt_.get<bool>("config.runGC");
    gcWB_ = pt_.get<bool>("config.gcWB");
    tiered_ = pt_.get<bool>("config.tiered");
    levelMerge_ = pt_.get<bool>("config.levelMerge");
    rangeMerge_ = pt_.get<bool>("config.rangeMerge");
    sep_before_flush_ = pt_.get<bool>("config.sepBeforeFlush");
    gcThreads_ = pt_.get<int>("config.gcThreads");
    maxSortedRuns_ = pt_.get<int>("config.maxSortedRuns");
    GCRatio_ = pt_.get<double>("config.gcRatio");
    blockWriteSize_ = pt_.get<uint64_t>("config.blockWriteSize");
    disableWAL_ = pt_.get<bool>("config.disableWAL");
    memtableNum_ = pt_.get<int>("config.memtableNum");
  }

  int getBloomBits() { return bloomBits_; }

  uint64_t getBlockWriteSize() { return blockWriteSize_; }

  int getMaxSortedRuns() { return maxSortedRuns_; }

  int getGCThreads() { return gcThreads_; }

  bool getSeekCompaction() { return seekCompaction_; }

  bool getCompression() { return compression_; }

  bool getDirectIO() { return directIO_; }

  int getNumThreads() { return numThreads_; }

  size_t getBlockCache() { return blockCache_; }

  int getSizeRatio() { return sizeRatio_; }

  size_t getGcSize() { return gcSize_; }

  size_t getMemtable() { return memtable_; }

  size_t getMaxFileSize() { return maxFileSize_; }

  bool getNoCompaction() { return noCompaction_; }

  size_t getSmallThresh() { return smallThresh_; }

  size_t getMidThresh() { return midThresh_; }

  double getGCRatio() { return GCRatio_; }

  bool getPreheat() { return preheat_; }

  int getGCLevel() { return gcLevel_; }

  int getMergeLevel() { return mergeLevel_; }

  bool getRunGC() { return runGC_; }

  bool getGCWB() { return gcWB_; }

  bool getTiered() { return tiered_; }

  bool getLevelMerge() { return levelMerge_; }

  bool getRangeMerge() { return rangeMerge_; }

  bool getSepBeforeFlush() { return sep_before_flush_; }

  bool getDisableWAL() { return disableWAL_; }

  int getMemtableNum() { return memtableNum_; }
};
} // namespace ycsbc

#endif // YCSB_C_ROCKSDB_DB_H
