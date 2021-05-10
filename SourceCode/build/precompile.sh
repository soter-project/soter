#!/bin/bash 
sudo apt-get update
sudo apt-get install git bison flex libssl-dev -y
sudo apt-get install libc6-i386 lib32z1 lib32stdc++6 -y
git clone --depth=1 --branch rpi-4.19.y https://github.com/raspberrypi/linux ../linux
