# soter

## Features

SOTER, the first host-based  malware infection detector tailored for IoT devices.

SOTER features both early and lightweight detections.


## Demos
### Set up virtual devices [https://youtu.be/szKfnbDQ3h0]

[![demo](https://github.com/soter-project/soter/blob/main/1_set_up_virtual_device.png)](https://youtu.be/szKfnbDQ3h0)


### Collect honeypot logs [https://youtu.be/lc2pHEgqWDo]

[![demo](https://github.com/soter-project/soter/blob/main/2_collect_log.png)](https://youtu.be/lc2pHEgqWDo)



## Dataset

Dataset includes follwing collections:

* BinaryStringDataset - includes the binary strings we extracted from the 48,099 malicious binaries.
* ScriptDataset - includes 3,439 malicious Linux  shell  scripts and 9,337 benign firmware scripts.
* OpenWrtLogs - has two parts. The part one includes 352,016 different infection incidents, the part two includes 147,860 different infection incidents. Collected from around 200 high-fidelity software (virtual) IoT devices (as honeypots) on public clouds across the globe at 32 geographically different locations.

The extracting password is: soter

## Source Code

This directory contains the source code of SOTER.


### Hook

Under the ./Source Code/fs directory/ is the hook.

Copy and past those files to your kernel source code under `linux/fs`

Note: This is for linux-4.19.91. 


### Classifier 

Under the ./Source Code/loadable_module/ directory is classifier.

This is the Loadable Kernel Module (LKM). 

Run make to compile it. 

Note: you need proper built system (such as Buildroot) to compile this LKM. 



### Build 

Under the ./Source Code/build/ includes an exmaple of how to build SOTER in Raspbian (Raspberry Pi OS).
