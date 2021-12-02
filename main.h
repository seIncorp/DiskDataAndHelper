#pragma once

#define UNICODE 1
#define _UNICODE 1

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>


#include "fast.h"
#include "bitManipulation.h"

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

} FLAGS;


void helpFunction();
void parsingCommands(int argc, wchar_t* argv[]);
void usingCommands();

void createDiskDrives();
void createDiskDrive();

extern FLAGS flags;