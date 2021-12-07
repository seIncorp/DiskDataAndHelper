#include "main.h"

#include <Fileapi.h>


FLAGS flags = { 0 };
std::vector<LOCAL_DISK_GEOMETRY*> all;
LOCAL_DISK_GEOMETRY* only_one;
wchar_t temp_drive_path[30];


int wmain(int argc, wchar_t* argv[])
{
	/*
		TODO:


		IOCTL:
		- FSCTL_GET_VOLUME_BITMAP



	*/

	parsingCommands(argc, argv);

	
	usingCommands();
	







	// CLOSING HANDLE
	if(flags.can_be_used_flag == 1)
		if (flags.multiple_drives_flag == 1)
			for (int ii = 0; ii < all.size(); ii++)
			{
				if (flags.perf_flag == 1)
					all[ii]->GetDiskPerformanceOFF();

				all[ii]->endDevice();
			}
		else
		{
			if (flags.perf_flag == 1)
				only_one->GetDiskPerformanceOFF();
			
			only_one->endDevice();
		}

	return 0;
}


void helpFunction()
{
	wprintf(L"\n******************************************\n");
	wprintf(L"Commands:\n");
	wprintf(L"\t -H or -h \t= for showing help\n");
	wprintf(L"\t -Dp [name] \t= is used for physical disk\n");
	wprintf(L"\t -Dl [name] \t= is used for logical disk (partition)\n");
	wprintf(L"\t -All \t\t= is used for listing of all logical disks (partitions)\n");
	wprintf(L"\t -att \t\t= with this you will get disk attributes\n");
	wprintf(L"\t -geo \t\t= with this you will get Drive geometry\n");
	wprintf(L"\t -lay \t\t= with this you will get Drive layout\n");
	wprintf(L"\t -perf \t\t= with this you will get Disk performance\n");
	wprintf(L"\t -realD \t\t= with this you will get real disk (it is uesd only for logical disk)\n");
	wprintf(L"\t -volInf \t\t= Volume information (it is uesd only for logical disk)\n");


	wprintf(L"\n\n");
	wprintf(L"Examples:\n");
	wprintf(L"-Dp \\\\.\\PhysicalDrive0\n");
	wprintf(L"-Dl C\n");
	wprintf(L"-Dl C -att -geo -lay -perf -realD -volInf\n");
	wprintf(L"\n******************************************\n");
	wprintf(L"");
	wprintf(L"");
	wprintf(L"");

}

void parsingCommands(int argc, wchar_t* argv[])
{
	for (int aa = 0; aa < argc; aa++)
	{
		if (std::wstring(argv[aa]) == L"-H" || std::wstring(argv[aa]) == L"-h")
		{
			flags.help_flag = 1;
		}

		if (std::wstring(argv[aa]) == L"-Dp")
		{
			// for physical disk
			swprintf(temp_drive_path, 25, L"\\\\.\\%s", argv[aa + 1]);

			flags.Dp_flag = 1;
		}

		if (std::wstring(argv[aa]) == L"-Dl")
		{
			// for logical disk
			swprintf(temp_drive_path, 10, L"\\\\.\\%s:", argv[aa+1]);

			flags.Dl_flag = 1;
		}

		if (std::wstring(argv[aa]) == L"-All")
		{
			flags.All_flag = 1;
			flags.multiple_drives_flag = 1;
		}

		// GetDiskAttributes
		if (std::wstring(argv[aa]) == L"-att")
		{
			flags.att_flag = 1;
		}

		// GetDriveGeometry
		if (std::wstring(argv[aa]) == L"-geo")
		{
			flags.geo_flag = 1;
		}
		 
		// GetDriveLayoutEx
		if (std::wstring(argv[aa]) == L"-lay")
		{
			flags.lay_flag = 1;
		}
		 
		// GetDiskPerformance <-- turn on
		if (std::wstring(argv[aa]) == L"-perf")
		{
			flags.perf_flag = 1;
		}
		
		// GetVolumeInf
		if (std::wstring(argv[aa]) == L"-volInf")
		{
			flags.volInf_flag = 1;
		}

		// GetVolumeRealDisk <<-- only when drive name is used for C:, ...
		if (std::wstring(argv[aa]) == L"-realD")
		{
			flags.realD_flag = 1;
		}

		// GetSTORAGE_PREDICT_FAILURE
		if (std::wstring(argv[aa]) == L"-predFailure")
		{
			flags.predFailure_flag = 1;
		}
	}
}

void usingCommands()
{
	if (flags.help_flag == 1)
	{
		helpFunction();
	}
	else
	{
		if (flags.All_flag == 1)
		{
			createDiskDrives();
		}

		if (flags.Dl_flag == 1 || flags.Dp_flag == 1)
		{
			createDiskDrive();
		}
	}

	if (flags.can_be_used_flag == 1)
	{
		if (flags.multiple_drives_flag == 0)
		{
			if (flags.geo_flag == 1)
			{
				wprintf(L"**Geometry:\n");
				only_one->GetDriveGeometry();

				ee;
			}

			if (flags.att_flag == 1)
			{
				wprintf(L"**Attributes:\n");
				only_one->GetDiskAttributes();
				ee;
			}

			if (flags.lay_flag == 1)
			{
				wprintf(L"**Layout:\n");
				only_one->GetDriveLayoutEx();
				ee;
			}

			if (flags.Dl_flag == 1 && flags.realD_flag == 1)
			{
				wprintf(L"**Real disk:\n");
				only_one->GetVolumeRealDisk();
				ee;
			}

			if (flags.Dl_flag == 1 && flags.volInf_flag == 1)
			{
				wprintf(L"**Volume Information:\n");
				only_one->GetVolumeInf();
				ee;
			}

			if (flags.predFailure_flag == 1)
			{
				wprintf(L"**Predict failure:\n");
				only_one->GetSTORAGE_PREDICT_FAILURE();
				ee;
			}

			if (flags.perf_flag == 1)
			{
				wprintf(L"**Performance:\n");
				only_one->GetDiskPerformance();
				ee;
			}
		}
		else
		{
			for (int ii = 0; ii < all.size(); ii++)
			{
				wprintf(L"[%d - %s] Drive:\n", ii, all[ii]->drive_path);

				if (flags.geo_flag == 1)
				{
					wprintf(L"**Geometry:\n");
					all[ii]->GetDriveGeometry();
					ee;
				}

				if (flags.att_flag == 1)
				{
					wprintf(L"**Attributes:\n");
					all[ii]->GetDiskAttributes();
					ee;
				}

				if (flags.lay_flag == 1)
				{
					wprintf(L"**Layout:\n");
					all[ii]->GetDriveLayoutEx();
					ee;
				}

				if (flags.realD_flag == 1)
				{
					wprintf(L"**Real disk:\n");
					all[ii]->GetVolumeRealDisk();
					ee;
				}

				if (flags.predFailure_flag == 1)
				{
					wprintf(L"**Predict failure:\n");
					all[ii]->GetSTORAGE_PREDICT_FAILURE();
					ee;
				}

				if (flags.perf_flag == 1)
				{
					wprintf(L"**Performance:\n");
					all[ii]->GetDiskPerformance();
					ee;
				}
			}
		}
	}
}

void createDiskDrives()
{
	DWORD tt = GetLogicalDrives();

	std::vector<int> list_partition;

	for (int ii = 0; ii < (sizeof(long) * 8); ii++)
	{
		int bit = (((tt) >> ii) & 1);
		int cc = 0x41 + ii;
		if (bit == 1)
		list_partition.push_back(cc);
	}

	for (int ii = 0; ii < list_partition.size(); ii++)
	{
		wchar_t temp_s[30];
		swprintf(temp_s, 10, L"\\\\.\\%wc:", list_partition[ii]);

		all.push_back(new LOCAL_DISK_GEOMETRY(temp_s));
	}

	for (int ii = 0; ii < all.size(); ii++)
	{
		all[ii]->startDevice();
	}

	flags.can_be_used_flag = 1;
}

void createDiskDrive()
{
	only_one = new LOCAL_DISK_GEOMETRY(temp_drive_path);

	only_one->startDevice();


	flags.can_be_used_flag = 1;
}

