#ifndef YCSB_C_ROCKSDB_DB_H
#define YCSB_C_ROCKSDB_DB_H

#include "ycsb/db.h"
#include <iostream>
#include <string>
#include "ycsb/properties.h"
#include "ycsb/core_workload.h"

#include <rocksdb/db.h>
#include <rocksdb/cache.h>
#include <rocksdb/table.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/options.h>
#include <rocksdb/utilities/options_util.h>
#include <rocksdb/compaction_filter.h>
#include <rocksdb/slice.h>

//#include "env/env_zns.h"

namespace ycsbc {
    class RocksDB : public DB {

    public :
		void Init() {
			std::cerr << "RocksDB Initialization";
		}
		void Close() {}
        RocksDB(const char *dbfilename, utils::Properties &props, bool zns);
        int Read(const std::string &table, const std::string &key,
                 const std::vector<std::string> *fields,
                 std::vector<KVPair> &result);

        int Scan(const std::string &table, const std::string &key,
                 int len, const std::vector<std::string> *fields,
                 std::vector<std::vector<KVPair>> &result);


        int Scan(const std::string &table, const std::string &key, const std::string &max_key,
                 int len, const std::vector<std::string> *fields,
                 std::vector<std::vector<KVPair>> &result);

        int Insert(const std::string &table, const std::string &key,
                   std::vector<KVPair> &values);

        int Update(const std::string &table, const std::string &key,
                   std::vector<KVPair> &values);


        int Delete(const std::string &table, const std::string &key);

        void PrintStats();

        ~RocksDB();

    private:
        rocksdb::DB *db_;
        unsigned noResult;
        std::shared_ptr<rocksdb::Cache> cache_;
        std::shared_ptr<rocksdb::Statistics> dbstats_;
        bool write_sync_;
		ROCKSDB_NAMESPACE::Env* env;
		bool zns;	

        void SetOptions(rocksdb::Options *options, utils::Properties &props, bool zns);
        void SerializeValues(std::vector<KVPair> &kvs, std::string &value);
        void DeSerializeValues(std::string &value, std::vector<KVPair> &kvs);

    };
}


#endif //YCSB_C_ROCKSDB_DB_H
