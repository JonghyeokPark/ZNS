#!/bin/bash

./db_bench --env_uri=xztl:/dev/nvme3n1 --zns=/dev/nvme3n1?be=thrpool --benchmarks=fillrandom \
       --use_direct_reads --key_size=16 --value_size=800 \
       --target_file_size_base=2147483648 \
       --use_direct_io_for_flush_and_compaction \
       --max_bytes_for_level_multiplier=4 --write_buffer_size=2147483648 \
       --target_file_size_multiplier=1 --num=1000000 --threads=2 \
       --max_background_jobs=4 --subcompactions=4

<<"END"
sudo iostat -x 10 >> zns-iostat.out &

./db_bench --fs_uri=zenfs://dev:nvme2n1 --benchmarks=fillrandom \
       --use_direct_reads --key_size=16 --value_size=800 \
       --target_file_size_base=2147483648 \
       --use_direct_io_for_flush_and_compaction \
       --max_bytes_for_level_multiplier=4 --write_buffer_size=2147483648 \
       --target_file_size_multiplier=1 --num=1000000 --threads=32 \
       --max_background_jobs=4 --subcompactions=4 | tee zns-result.txt 


#<<"END"
./db_bench --fs_uri=zenfs://dev:nvme2n1 --benchmarks=fillrandom \
       --use_direct_reads --key_size=16 --value_size=800 \
       --use_direct_io_for_flush_and_compaction \
	   --target_file_size_multiplier=1 --num=10000000 --threads=32 \
       --max_background_jobs=4 --subcompactions=4 

#	   --statistics \
#       --stats_interval_seconds=10 2>&1 | tee result.txt
#END
#--write_buffer_size=2147483648 \
#       --target_file_size_base=2147483648 \
END
