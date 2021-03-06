#pragma once

#define UNICODE 1
#define _UNICODE 1

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <initguid.h>
#include <guiddef.h>
#include <fileapi.h>
#include <ntddscsi.h>

#include <array>
#include <sstream>


#include <iostream>
#include <comdef.h>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")








//DEFINE_GUID(PARTITION_BASIC_DATA_GUID, 0xC12A7328L, 0xF81F, 0x11D2, 0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B)
//DEFINE_GUID(GUID_BUS_TYPE_PCMCIA,      0x09343630L, 0xaf9f, 0x11d0, 0x92, 0x9f, 0x00, 0xc0, 0x4f, 0xc3, 0x40, 0xb1);
DEFINE_GUID(PARTITION_BASIC_DATA_GUID, 0xEBD0A0A2L, 0xb9e5, 0x4433, 0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7);
DEFINE_GUID(PARTITION_ENTRY_UNUSED_GUID, 0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
DEFINE_GUID(PARTITION_SYSTEM_GUID, 0xc12a7328L, 0xf81f, 0x11d2, 0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b);
DEFINE_GUID(PARTITION_MSFT_RESERVED_GUID, 0xe3c9e316L, 0x0b5c, 0x4db8, 0x81, 0x7d, 0xf9, 0x2d, 0xf0, 0x02, 0x15, 0xae);
DEFINE_GUID(PARTITION_LDM_METADATA_GUID, 0x5808c8aaL, 0x7e8f, 0x42e0, 0x85, 0xd2, 0xe1, 0xe9, 0x04, 0x34, 0xcf, 0xb3);
DEFINE_GUID(PARTITION_LDM_DATA_GUID, 0xaf9b60a0L, 0x1431, 0x4f62, 0xbc, 0x68, 0x33, 0x11, 0x71, 0x4a, 0x69, 0xad);
DEFINE_GUID(PARTITION_MSFT_RECOVERY_GUID, 0xde94bba4L, 0x06d1, 0x4d40, 0xa1, 0x6a, 0xbf, 0xd5, 0x01, 0x79, 0xd6, 0xac);




#include "fast.h"
#include "bitManipulation.h"

#include "ata_data.h"
#include "disk_data.h"

#include "disk_geometry.h"


typedef struct
{
	int can_be_used_flag;
	int multiple_drives_flag;

	int help_flag;
	int Dp_flag;
	int Dl_flag;
	int All_flag;
	int att_flag;
	int geo_flag;
	int lay_flag;
	int realD_flag;
	int perf_flag;
	int volInf_flag;
	int predFailure_flag;
	int ATA_IDD_flag;
	int DCO_flag;
	int HPA_data_flag;

} FLAGS;


void helpFunction();
void parsingCommands(int argc, wchar_t* argv[]);
void usingCommands();

void createDiskDrives();
void createDiskDrive();

extern FLAGS flags;



