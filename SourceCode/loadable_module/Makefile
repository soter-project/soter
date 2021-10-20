obj-m += raspmonitor_loadable.o

raspmonitor_loadable-objs := rasploadable.o

# Note: 
# The path following `-C` should be the root folder where you have compiled your 
# kernle. 
# E.g., you run `make` at ~/openwrt.15.05.1-compiled/ when you cross compiled kernel.
# Then, you should set the path following `-C` as ~/openwrt.15.05.1-compiled/ 
#
#

#OPENWRT_HOME := ~/Projects/openwrt/openwrt-15.05.1
BUILDROOT_HOME:= /home/hongda/project/buildroot-2020.02.3

# KDIR    := ${OPENWRT_HOME}/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_rt305x/linux-3.18.23/
# KDIR    := ${OPENWRT_HOME}/build_dir/target-arm_cortex-a15+neon-vfpv4_musl_eabi/linux-armvirt_32/linux-5.4.31/
#KDIR    := ${BUILDROOT_HOME}/build_dir/target-arm_cortex-a15+neon-vfpv4_musl-1.1.16_eabi/linux-armvirt/linux-4.4.194/
KDIR    := ${BUILDROOT_HOME}/output/build/linux-4.19.91/
PWD     := $(shell pwd)
# LD      := mipsel-openwrt-linux-ld
# LD      := arm-openwrt-linux-ld
LD      := arm-buildroot-linux-uclibcgnueabi-ld
# PATH	:= $(shell PATH=$PATH:~/mount/stuff/openwrt/staging_dir/toolchain-arm_cortex-a15+neon-vfpv4_gcc-8.4.0_musl_eabi/bin)

# CROSS_COMPILE=mipsel-openwrt-linux-
#CROSS_COMPILE=arm-openwrt-linux-
CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabi-

all:
	#$(MAKE) ARCH=arm -C $(KDIR) M=$(PWD) CROSS_COMPILE=arm-openwrt-linux- modules 
	$(MAKE) ARCH=arm -C $(KDIR) M=$(PWD) CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabi- modules 
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
