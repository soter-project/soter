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

* BinaryDataset - includes 48,099 malicious binaries.
* ScriptDataset - includes 3,439 malicious Linux  shell  scripts and 9,337 benign firmware scripts.
* OpenWrtLogs - includes 167,860 successful OpenWrt infection logs. Collected from 163 high-fidelity software (virtual) IoT devices (as honeypots) on public clouds across the globe at 26 geographically different locations for one month.

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
