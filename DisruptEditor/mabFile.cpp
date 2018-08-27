#include "mabFile.h"

/*
MAB Format (Watch Dogs)
=======================
DWORD id;      // 0x329B
DWORD unk[5];
DWORD file_size;   // file size minus 16
FLOAT anim_length;   // animation length in secs

WORD unk_count1;   // unknown counts
WORD unk_count2;
WORD unk_count3;
WORD unk_count4;
WORD unk_count5;
WORD unk_count6;
WORD unk_count7;
WORD unk_count8;

DWORD data_offsets[11];   // There are 10 data sections here.  Real offset is plus 16.

DWORD unk_count9;   // keytime count?

BYTE unk[5];      // There is a strange list of 5-byte elements here. (unk_count1).

Data sections follow:
Data section 0:      // keytime list of WORDs (unk_count9) - using a factor of 120 ticksPerFrame
Data section 1:      // all unknown
Data section 2:
Data section 3:
Data section 4:
Data section 5:
Data section 6:
Data section 7:
Data section 8:
Data section 9:
Data section 10:
*/

mabFile::mabFile() {}


mabFile::~mabFile() {}
