# VLDB Lab. ZNS Guide

# Samusng ZNS SSD

## Specification.

||Samusng (PM9731a)| Western Digital*|
|:---|:---:|:---:|
|Capacity|4 TB|3.8 TB|
|Total # of Zones|40,704|3,624|
|Max. open zones|384|14|
|Zone Size|96 MB|1 GB|
|Sector Size|4096|4096|
|Scheduler|mq-deadline|mq-deadline|

*WZS4C8T4TDSP303


## Problems.
Current Linux kernel (> v5.20) provide ZNS (Zoned Namespace) SSD environment, however, this only support 2^n-sized zone.
For Samsung ZNS (`PM9731a`) provide small zone size with 96MB which is not 2^n-sized zone.
This entails mainly two problems: 

1. Linux kernel CAN NOT identify Samsung ZNS SSDs.
2. Libzbd library, which is based on WD's ZNS SSD echo system can not provide offset-zone transaltion.


# ZNS support for Linx (Ubuntu)

1. Remove the condition which disallows non-2^n zone size ZNS drive 
to be updated and use generic method to calculate number of zones
ragher than relying on log and shift based calculation on zone size.
See more detail [patch from Samsung](https://patchwork.kernel.org/project/linux-block/patch/20220506081105.29134-4-p.raghav@samsung.com)

2. Checking if a given sector is aligned to a zone is a common
operation that is performed for zoned devices 
Convert the calculations on zone size to be generic instead of relying on
2^n based logic in the block layer using the helpers wherever possible.
See more detail [patch from Samsung](https://patchwork.kernel.org/project/dm-devel/patch/20220615101920.329421-3-p.raghav@samsung.com/)

```
# Step 0. Prerequisite
sudo apt-get update
sudo apt-get install build-essential libncurses5 libncurses5-dev \
										 bin86 kernel-package libssl-dev bison flex libelf-dev

# Step 1. Load kernel
cd kernel/linux-5.17.8/
uname -a # Check kernel version
sudo cp /boot/config-CURRENT_KERNEL_VERSION ./.config
sudo make menuconfig

# Step 2. Compile kernel
sudo make-kpkg --J 32 --initrd --revision=1.0 kernel_image
sudo dpkg -i linux-image-5.14.18_1.0_amd64.deb
```

# Libzbd 

Fix shift based calculation on zone size.

```
int zbd_report_zones(int fd, off_t ofst, off_t len, enum zbd_report_option ro,
		     struct zbd_zone *zones, unsigned int *nr_zones) {

// ORIGINAL
//	end = ((ofst + len + zone_size_mask) & (~zone_size_mask))
//		>> SECTOR_SHIFT;

// FIXED
  unsigned long long part = ((ofst + len) / zbdi->zone_size);
	if ((ofst+len) % zbdi->zone_size != 0) part++;
	end = (( part * zbdi->zone_size) >> SECTOR_SHIFT);
  
// ORIGINAL
//	ofst = (ofst & (~zone_size_mask)) >> SECTOR_SHIFT;

// FIXED
	part = ((ofst) / zbdi->zone_size);
	ofst = (( part * zbdi->zone_size) >> SECTOR_SHIFT);
}
```

See more detail in the [Official Repository](https://github.com/westerndigitalcorporation/libzbd)

# xZTL

Fix libaray issues 

1. Install xNVMe (v0.4) library
2. Install xZTL Library

```
make lib-only
sudo make install -j
```

See more detail in the [Official Repository](https://github.com/OpenMPDK/xZTL)

# RocksDB

This RocksDB repository added xZTL patch, which supports xZTL environment.
Fix the library issues for supporitng xZTL library (see, Makefile)

```
make static_lib -j
make db_bench DEBUG_LEVEL=0 -j
```

See more detail in the [Official Repository](https://github.com/facebook/rocksdb)



# Contact

- Author. [Jonghyeok Park](https://www.linkedin.com/in/jonghyeokpark/)
- E-mail. akindo19@gmail.com
