#!/bin/bash

workload="ycsb/workloads/workloada.spec"
dbpath="/tmp/testrocksdb"
#./ycsbc -db basic -threads 4 -P $workload
#./ycsbc -db rocksdb -dbpath $dbpath -threads 4 -P $workload -dbstatistics true -optpath ./ycsb/options.ini
./ycsbc -db zns -dbpath $dbpath -threads 4 -P $workload -dbstatistics true -optpath ./ycsb/options.ini
