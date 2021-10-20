#ifndef __CAPABILITY_ENUM_H
#define __CAPABILITY_ENUM_H
typedef enum {
CH = 0,
RM = 1,
DM = 2,
DW = 3,
FP = 4,
CP = 5,
CW = 6,
DC = 7,
DE = 8,
CC = 9,
TP = 10,
KILL = 11,
EX = 12,
NP = 13,
BD = 14,
EXE = 15,
SELFID = 16,
CI = 17,
SysM = 18,
SoftM = 19,
RES = 20,
TIME = 21,
CT = 22,
MATH = 23,
AGNOSTIC = 24,
UNKNOWN = 25,
none_capability = 26
} capability;
#define MAX_CAPABILITY 27
#endif