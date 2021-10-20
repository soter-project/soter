#!/bin/bash 
cd ..
git clone https://github.com/raspberrypi/tools raspberrypi-cross-compile-tools 
dir=$(pwd)
echo PATH=\$PATH:$dir/raspberrypi-cross-compile-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin >> ~/.bashrc 

echo "Please check ~/.bashrc and run 'source ~/.bashrc' "
