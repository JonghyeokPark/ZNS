
#include "ycsb/db_factory.h"
#include <string>
#include "ycsb/basic_db.h"
#include "ycsb/rocksdb_db.h"

using ycsbc::DB;
using ycsbc::DBFactory;

DB* DBFactory::CreateDB(utils::Properties &props) {

  if (props["dbname"] == "basic") {
    return new BasicDB;
  } else if (props["dbname"] == "rocksdb") {
	std::string dbpath = props.GetProperty("dbpath","/tmp/test-rocksdb");
	return new RocksDB(dbpath.c_str(), props, false);
  } else if (props["dbname"] == "zns") {
	std::string dbpath = props.GetProperty("dbpath","/tmp/test-zns-rocksdb");
	return new RocksDB(dbpath.c_str(), props, true);
  }

  else {
	return NULL;
  }
}

