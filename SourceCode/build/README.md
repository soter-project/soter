Toolchain & Dependencies
============
Run `pretoolchain.sh`

Run `precompile.sh`

Then you will get two folders under the *parant* of current folder. 

The `raspberrypi-cross-compile-tools` folder is the toolchain for cross-compile. 

The `linux` folder is the source code of raspbian. 

You should copy `fs` under current folder to overwrite those under the `linux`. 

Note: the files provided in this repo is for Linux kernel version **4.4.194**. 
If your kernel version is not **4.4.194**, you should modify the following files manually instead of overwritting:

```
fs/exec.c
fs/open.c
```



Build SOTER for Raspbian
============

This is a copy of a part of Linux kernel source code from 

	https://github.com/raspberrypi/linux 

Branch: rpi-4.19.y 

To compile this source code, copy all the stuff under this repo and replace 
that in the rpi-4.19.y. 
Then follow the instructions in https://www.raspberrypi.org/documentation/linux/kernel/building.md 
to compile. 

Note: if you need a working .config, follow the instructions here: 
    https://www.raspberrypi.org/forums/viewtopic.php?t=195965 

Basically, you need to install the raspbian from official website. 
Then you login to the raspberrypi, and execute the following to commands: 

```
sudo modprobe configs
zcat /proc/config.gz > .config 
```

Then you will get a .config of your current raspbian. 

