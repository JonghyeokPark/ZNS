#include <iostream>

#include "rocksdb_db.h"
#include <rocksdb/file_system.h>
#include "coding.h"
#include "env/env_zns.h"

using ROCKSDB_NAMESPACE::BlockBasedTableOptions;
using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
using ROCKSDB_NAMESPACE::CompactionFilter;
using ROCKSDB_NAMESPACE::ConfigOptions;
using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::DBOptions;
using ROCKSDB_NAMESPACE::NewLRUCache;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::Slice;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::Env;

namespace ycsbc {

    RocksDB::RocksDB(const char *dbfilename, utils::Properties &props, bool zns) 
		:noResult(0), cache_(nullptr), dbstats_(nullptr), write_sync_(false), zns(zns) {
        
        //set option
        rocksdb::Options options;
        SetOptions(&options, props, zns);
        
        rocksdb::Status s = rocksdb::DB::Open(options, dbfilename, &db_);

        if(!s.ok()){
            std::cerr<<"Can't open rocksdb "<<dbfilename<<" "<<s.ToString()<<std::endl;
            exit(0);
        }
    }

		
    void RocksDB::SetOptions(rocksdb::Options *options, utils::Properties &props, bool zns) {

		printf("Initializing RocksDB Options from the specified file\n");
		std::string options_file = props.GetProperty(CoreWorkload::OPT_FILE_PATH);
		
		if (zns) {
			//env  = ROCKSDB_NAMESPACE::ZNSEnv("/dev/nvme3n1?be=thrpool");	
			ROCKSDB_NAMESPACE::NewZNSEnv(&env, "/dev/nvme3n1?be=thrpool");
		} else {
			env =  ROCKSDB_NAMESPACE::Env::Default();
		}

		options->env = env;

	    DBOptions db_opts;
	    std::vector<ColumnFamilyDescriptor> cf_descs;

	    if (options_file != "") {
			auto s = LoadOptionsFromFile(options_file, env, &db_opts,
                                   &cf_descs);
	      db_opts.env = env;
	      if (s.ok()) {
	        *options = Options(db_opts, cf_descs[0].options);
	      }
		} else {

        options->create_if_missing = true;
        options->compression = rocksdb::kNoCompression;
        options->max_background_jobs = 2;
        options->max_bytes_for_level_base = 32ul * 1024 * 1024;
        options->write_buffer_size = 32ul * 1024 * 1024;
        options->max_write_buffer_number = 2;
        options->target_file_size_base = 4ul * 1024 * 1024;

        options->level0_file_num_compaction_trigger = 4;
        options->level0_slowdown_writes_trigger = 8;     
        options->level0_stop_writes_trigger = 12;

        options->use_direct_reads = true;
        options->use_direct_io_for_flush_and_compaction = true;

        uint64_t nums = std::stoi(props.GetProperty(CoreWorkload::RECORD_COUNT_PROPERTY));
		uint32_t key_len = std::stoi(props.GetProperty(CoreWorkload::KEY_LENGTH, CoreWorkload::KEY_LENGTH_DEFAULT));
        uint32_t value_len = std::stoi(props.GetProperty(CoreWorkload::FIELD_LENGTH_PROPERTY, CoreWorkload::FIELD_LENGTH_DEFAULT));
        uint32_t cache_size = nums * (key_len + value_len) * 10 / 100; //10%
        if(cache_size < 8 << 20) { 
            cache_size = 8 << 20;
        }
        cache_ = rocksdb::NewLRUCache(cache_size);

        bool statistics = utils::StrToBool(props["dbstatistics"]);
        if(statistics){
            dbstats_ = rocksdb::CreateDBStatistics();
            options->statistics = dbstats_;
        }

        write_sync_ = false;
		}
    }


    int RocksDB::Read(const std::string &table, const std::string &key, const std::vector<std::string> *fields,
                      std::vector<KVPair> &result) {
        std::string value;
        rocksdb::Status s = db_->Get(rocksdb::ReadOptions(),key,&value);
        if(s.ok()) {
            //printf("value:%lu\n",value.size());
            DeSerializeValues(value, result);
            /* printf("get:key:%lu-%s\n",key.size(),key.data());
            for( auto kv : result) {
                printf("get field:key:%lu-%s value:%lu-%s\n",kv.first.size(),kv.first.data(),kv.second.size(),kv.second.data());
            } */
            return DB::kOK;
        }
        if(s.IsNotFound()){
            noResult++;
            //std::cerr<<"read not found:"<<noResult<<std::endl;
            return DB::kOK;
        }else{
            std::cerr<<"read error"<<std::endl;
            exit(0);
        }
    }


    int RocksDB::Scan(const std::string &table, const std::string &key, const std::string &max_key, int len, const std::vector<std::string> *fields,
                      std::vector<std::vector<KVPair>> &result) {
         auto it=db_->NewIterator(rocksdb::ReadOptions());
        it->Seek(key);
        std::string val;
        std::string k;
        for(int i=0;i < len && it->Valid(); i++){
            k = it->key().ToString();
            val = it->value().ToString();
            it->Next();
        } 
        delete it;
        return DB::kOK;
    }

    int RocksDB::Scan(const std::string &table, const std::string &key, int len, const std::vector<std::string> *fields,
                      std::vector<std::vector<KVPair>> &result) {
         auto it=db_->NewIterator(rocksdb::ReadOptions());
        it->Seek(key);
        std::string val;
        std::string k;
        for(int i=0;i < len && it->Valid(); i++){
            k = it->key().ToString();
            val = it->value().ToString();
            it->Next();
        } 
        delete it;
        return DB::kOK;
    }


    int RocksDB::Insert(const std::string &table, const std::string &key,
                        std::vector<KVPair> &values){
        rocksdb::Status s;
        std::string value;
        SerializeValues(values,value);
        rocksdb::WriteOptions write_options = rocksdb::WriteOptions();

        if(write_sync_) {
            write_options.sync = true;
        }

        s = db_->Put(write_options, key, value);

        if(!s.ok()){
            std::cerr<<"insert error\n"<<std::endl;
            exit(0);
        }
       
        return DB::kOK;
    }

    int RocksDB::Update(const std::string &table, const std::string &key, std::vector<KVPair> &values) {
        return Insert(table,key,values);
    }

    int RocksDB::Delete(const std::string &table, const std::string &key) {
        rocksdb::Status s;
        rocksdb::WriteOptions write_options = rocksdb::WriteOptions();
        if(write_sync_) {
            write_options.sync = true;
        }
        s = db_->Delete(write_options,key);
        if(!s.ok()){
            std::cerr<<"Delete error\n"<<std::endl;
            exit(0);
        }
        return DB::kOK;
    }

    void RocksDB::PrintStats() {
        if(noResult) std::cout<<"read not found:"<<noResult<<std::endl;
        std::string stats;
        db_->GetProperty("rocksdb.stats",&stats);
        std::cout<<stats<<std::endl;

        if (dbstats_.get() != nullptr) {
            fprintf(stdout, "STATISTICS:\n%s\n", dbstats_->ToString().c_str());
        }
    }

    RocksDB::~RocksDB() {
        delete db_;
        if (cache_.get() != nullptr) {
            // this will leak, but we're shutting down so nobody cares
            cache_->DisownData();
        }
    }

    void RocksDB::SerializeValues(std::vector<KVPair> &kvs, std::string &value) {
        value.clear();
        PutFixed64(&value, kvs.size());
        for(unsigned int i=0; i < kvs.size(); i++){
            PutFixed64(&value, kvs[i].first.size());
            value.append(kvs[i].first);
            PutFixed64(&value, kvs[i].second.size());
            value.append(kvs[i].second);
        }
    }

    void RocksDB::DeSerializeValues(std::string &value, std::vector<KVPair> &kvs){
        uint64_t offset = 0;
        uint64_t kv_num = 0;
        uint64_t key_size = 0;
        uint64_t value_size = 0;

        kv_num = DecodeFixed64(value.c_str());
        offset += 8;
        for( unsigned int i = 0; i < kv_num; i++){
            ycsbc::DB::KVPair pair;
            key_size = DecodeFixed64(value.c_str() + offset);
            offset += 8;

            pair.first.assign(value.c_str() + offset, key_size);
            offset += key_size;

            value_size = DecodeFixed64(value.c_str() + offset);
            offset += 8;

            pair.second.assign(value.c_str() + offset, value_size);
            offset += value_size;
            kvs.push_back(pair);
        }
    }
}
